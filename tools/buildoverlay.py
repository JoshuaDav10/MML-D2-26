# Build script for building elfs for chunks/overlays and building said chunks from them and combining the the chunks into the whole bin archive file that originally contained them
# ```
# CHUNK HEADER (offsets are relative to chunk's location)
# 0x0: ??? Type? (It seems that if 0, this is a code overlay loaded into executable game memory)
# 0x4: Size of chunk
# 0x8: ???
# 0xC: Load address if type 0
# 0x10-0x40: ???
# 0x40-0x800: Chunk filename, as a null terminated string
# 0x800-(0x800+chunk size): file contents
# Chunk size, aligned up to 0x800: next chunk, starting with the header
# ```

# directory structure:
# asm/{ARCHIVE_FILE_NAME_WITHOUT_EXT}/{CHUNK_FILE_NAME_WITH_EXT}/*_header.s -> chunk header dump
# asm/{ARCHIVE_FILE_NAME_WITHOUT_EXT}/{CHUNK_FILE_NAME_WITH_EXT}/data/*.s -> data disassembly
# asm/{ARCHIVE_FILE_NAME_WITHOUT_EXT}/{CHUNK_FILE_NAME_WITH_EXT}/./*.s -> code disassembly
# src/{ARCHIVE_FILE_NAME_WITHOUT_EXT}/{CHUNK_FILE_NAME_WITH_EXT}/*.c -> decompiled code
# assets/{ARCHIVE_FILE_NAME_WITHOUT_EXT}/{CHUNK_FILE_NAME_WITH_EXT}/*.bin -> binary asset dumps

import hashlib
import os, sys, json, yaml, struct, subprocess, shutil, glob, re

VERSION = "us"
CROSS = "mipsel-elf-"
AS = f"{CROSS}as"
LD = f"{CROSS}ld"
CPP = os.environ.get("CPP", "cpp" if shutil.which("cpp") else f"{CROSS}cpp")
OBJCOPY = f"{CROSS}objcopy"
CC = "./bin/cc1-27"
AS_FLAGS        = "-Iinclude -march=r3000 -mtune=r3000 -no-pad-sections -O1 -G0"
# NOTE: this is a SECOND copy of the compiler flags, independent of the Makefile's
# CC_FLAGS. Changing one does not change the other — that cost real time on
# 2026-07-26. Keep them in sync.
# -mel: cc1-27 defaults to BIG-endian for unaligned access (lwl/lwr, swl/swr get
# swapped byte offsets). Needed by eve19's align-1 CHECK_BLOCK struct copies.
CC_FLAGS        = "-mcpu=3000 -quiet -G0 -w -O2 -funsigned-char -fpeephole -ffunction-cse -fpcc-struct-return -fcommon -fverbose-asm -fgnu-linker -mgas -msoft-float -gcoff -mel"
CPP_FLAGS       = "-Iinclude -undef -Wall -lang-c -fno-builtin -Dmips -D__GNUC__=2 -D__OPTIMIZE__ -D__mips__ -D__mips -Dpsx -D__psx__ -D__psx -D_PSYQ -D__EXTENSIONS__ -D_MIPSEL -D_LANGUAGE_C -DLANGUAGE_C -DHACKS"

ASM_DIR         = "asm"
SRC_DIR         = "src"
ASSETS_DIR      = "assets"
INCLUDE_DIR     = "include"
BUILD_DIR       = "build"
CONFIG_DIR      = "config"
TOOLS_DIR       = "tools"

MASPSX = "python3 tools/maspx/maspsx.py --no-macro-inc --expand-div"
PYPATCHASM = "tools/patchasm.py"

build_log = open(f"logs/build_{sys.argv[1]}.log", "w")

def run_cmd(cmd, what):
    rc = os.system(cmd)
    if rc != 0:
        print(f"buildoverlay: failed ({what}): {cmd}", file=sys.stderr)
        sys.exit(1)

def curated_syms_path(parent_archive_file_name, chunk_file_name):
    return f"{CONFIG_DIR}/overlay/splat.{VERSION}.{parent_archive_file_name}/syms.{VERSION}.{chunk_file_name}.txt"

def curated_syms_ld_arg(parent_archive_file_name, chunk_file_name):
    syms_path = curated_syms_path(parent_archive_file_name, chunk_file_name)
    if not os.path.exists(syms_path):
        return ""
    lines = []
    with open(syms_path, "r") as f:
        for line in f:
            line = line.split("//", 1)[0].strip()
            if line:
                lines.append(line)
    if not lines:
        return ""
    ld_path = f"{BUILD_DIR}/{parent_archive_file_name}.{chunk_file_name}.syms.ld"
    os.makedirs(os.path.dirname(ld_path), exist_ok=True)
    with open(ld_path, "w") as f:
        f.write("\n".join(lines) + "\n")
    return f"-T {ld_path} "

def list_src_files(parent_archive_file_name, chunk_file_name):
    files = []
    if os.path.exists(os.path.join(ASM_DIR, parent_archive_file_name, chunk_file_name)):
        for dir in [os.path.join(ASM_DIR, parent_archive_file_name, chunk_file_name)]:
            files += [os.path.join(dir, file) for file in os.listdir(dir) if file.endswith(".s")]
    if os.path.exists(os.path.join(ASM_DIR, parent_archive_file_name, chunk_file_name, "data")):
        for dir in [os.path.join(ASM_DIR, parent_archive_file_name, chunk_file_name, "data")]:
            files += [os.path.join(dir, file) for file in os.listdir(dir) if file.endswith(".s")]
    if os.path.exists(os.path.join(SRC_DIR, parent_archive_file_name, chunk_file_name)):
        for dir in [os.path.join(SRC_DIR, parent_archive_file_name, chunk_file_name)]:
            files += [os.path.join(dir, file) for file in os.listdir(dir) if file.endswith(".c")]
    if os.path.exists(os.path.join(ASSETS_DIR, parent_archive_file_name, chunk_file_name)):
        for dir in [os.path.join(ASSETS_DIR, parent_archive_file_name, chunk_file_name)]:
            files += [os.path.join(dir, file) for file in os.listdir(dir) if file.endswith(".bin")]
    return files

def list_o_files(parent_archive_file_name, chunk_file_name):
    files = list_src_files(parent_archive_file_name, chunk_file_name)
    for i in range(len(files)):
        files[i] = BUILD_DIR + "/" + files[i] + ".o"
        if not os.path.exists(os.path.dirname(files[i])):
            os.makedirs(os.path.dirname(files[i]))
    return files

def link_overlay(parent_archive_file_name, chunk_file_name, elf):
    syms_arg = curated_syms_ld_arg(parent_archive_file_name, chunk_file_name)
    cmd = (
        f"{LD} -o {elf} -Map {BUILD_DIR}/{parent_archive_file_name}.{chunk_file_name}.map "
        f"-T ./{parent_archive_file_name}.{chunk_file_name}.ld "
        f"-T {CONFIG_DIR}/overlay/splat.{VERSION}.{parent_archive_file_name}/undefined_syms_auto.{VERSION}.{chunk_file_name}.txt "
        f"-T {CONFIG_DIR}/overlay/splat.{VERSION}.{parent_archive_file_name}/undefined_funcs_auto.{VERSION}.{chunk_file_name}.txt "
        f"{syms_arg}"
        f"-T {BUILD_DIR}/generated.rock_neo.syms.txt --no-check-sections -nostdlib -s"
    )
    run_cmd(cmd, f"ld {parent_archive_file_name}/{chunk_file_name}")

# def generate_rock_neo_syms_txt():
#     rock_neo_elf = f"{BUILD_DIR}/rock_neo.elf.unstripped"
#     rock_neo_syms_txt = f"{BUILD_DIR}/generated.rock_neo.syms.txt"

#     elf_file = open(rock_neo_elf, "rb")
#     elf = elftools.elf.elffile.ELFFile(elf_file)
#     syms = elf.get_section_by_name(".symtab")
#     with open(rock_neo_syms_txt, "w") as f:
#         for sym in syms.iter_symbols():
#             if sym.entry.st_value >= 0x80010000 and sym.entry.st_value < 0x80200000 and " " not in sym.name and sym.name != "":
#                 f.write(f"{sym.name} = 0x{sym.entry.st_value:08x};\n")
#         f.write("""
# SUPPORT_STG_LOAD_ADDRESS = 0x801F6000;
# SUPPORT_EBD_LOAD_ADDRESS = 0x801F2000;
# SUPPORT_PROGBIN_LOAD_ADDRESS = 0x801D8000;
# STAGE_STG_LOAD_ADDRESS = 0x80194000;
# STAGE_EBD_LOAD_ADDRESS2 = 0x80190000;
# STAGE_EBD_LOAD_ADDRESS1 = 0x8016C000;
# STAGE_MDT_LOAD_ADDRESS = 0x80164000;
# STAGE_IDX_LOAD_ADDRESS = 0x8015C000;
# MSG_LOAD_ADDRESS = 0x80153000;
# STAGE_HED_LOAD_ADDRESS = 0x8013A000;
# STAGE_PROGBIN_LOAD_ADDRESS = 0x80100000;
# SHL_PROGBIN_LOAD_ADDRESS = 0x800D8800;
# """)

def assemble_s_file(s_file, o_file):
    run_cmd(f"{AS} {AS_FLAGS} -o {o_file} {s_file}", f"as {s_file}")
    build_log.write(f"as {s_file} -> {o_file}\n")

def compile_c_file(c_file, o_file):
    run_cmd(
        f"{CPP} {CPP_FLAGS} {c_file} | {CC} {CC_FLAGS} | {MASPSX} | python3 {PYPATCHASM} | {AS} {AS_FLAGS} -o {o_file}",
        f"cc {c_file}",
    )
    build_log.write(f"cc {c_file} -> {o_file}\n")

def compile_asset_file(bin_file, o_file):
    run_cmd(f"{LD} -r -b binary -o {o_file} {bin_file}", f"bin {bin_file}")
    build_log.write(f"bin {bin_file} -> {o_file}\n")

src_files_hash_cache = {}

def get_src_files_hash(parent_archive_file_name, chunk_file_name):
    src_files = list_src_files(parent_archive_file_name, chunk_file_name)
    for src_file in src_files:
        with open(src_file, "rb") as f:
            src_files_hash_cache[src_file] = hashlib.sha256(f.read()).hexdigest()
    src_files_hash_cache["rock_neo.elf"] = hashlib.sha256(open(f"{BUILD_DIR}/rock_neo.elf", "rb").read()).hexdigest()

def build_hash_cache(parent_archive_file_name, chunk_file_name):
    json_path = f"{BUILD_DIR}/{parent_archive_file_name}.{chunk_file_name}.src_hash.json"
    get_src_files_hash(parent_archive_file_name, chunk_file_name)
    with open(json_path, "w") as f:
        json.dump(src_files_hash_cache, f)

def build_chunk(parent_archive_file_name, chunk_file_name):
    elf = f"build/{parent_archive_file_name}.{chunk_file_name}.elf"
    if os.path.exists(elf):
        os.remove(elf)
    global src_files_hash_cache
    json_path = f"{BUILD_DIR}/{parent_archive_file_name}.{chunk_file_name}.src_hash.json"
    if os.path.exists(json_path):
        with open(json_path, "r") as f:
            src_files_hash_cache = json.load(f)
    need_to_link = ("rock_neo.elf" not in src_files_hash_cache or src_files_hash_cache["rock_neo.elf"] != hashlib.sha256(open(f"{BUILD_DIR}/rock_neo.elf", "rb").read()).hexdigest())
    for o_file in list_o_files(parent_archive_file_name, chunk_file_name):
        s_file = o_file[:-2].replace(BUILD_DIR + "/", "")
        if s_file.endswith(".s") and (s_file not in src_files_hash_cache or src_files_hash_cache[s_file] != hashlib.sha256(open(s_file, "rb").read()).hexdigest()):
            assemble_s_file(s_file, o_file)
            need_to_link = True
        elif s_file.endswith(".c") and (s_file not in src_files_hash_cache or src_files_hash_cache[s_file] != hashlib.sha256(open(s_file, "rb").read()).hexdigest()):
            compile_c_file(s_file, o_file)
            need_to_link = True
        elif s_file.endswith(".bin") and (s_file not in src_files_hash_cache or src_files_hash_cache[s_file] != hashlib.sha256(open(s_file, "rb").read()).hexdigest()):
            compile_asset_file(s_file, o_file)
            need_to_link = True
    if need_to_link:
        link_overlay(parent_archive_file_name, chunk_file_name, elf)
        build_log.write(f"ld {elf}\n")
    return need_to_link

def binarize_chunk(parent_archive_file_name, chunk_file_name):
    elf = f"build/{parent_archive_file_name}.{chunk_file_name}.elf"
    run_cmd(f"{OBJCOPY} -O binary {elf} {elf}.bin", f"objcopy {parent_archive_file_name}/{chunk_file_name}")
    build_log.write(f"objcopy {elf} -> {elf}.bin\n")

def get_chunk_list(parent_archive_filename):
    json_path = f"config/overlay/splat.{VERSION}.{parent_archive_filename}/build.json"
    build_log.write(json_path + "\n")
    with open(json_path, "r") as f:
        data = json.load(f)
    build_log.write(str(data) + "\n")
    return data["overlays"]

def get_chunk_file_names(parent_archive_filename):
    folder = f"asm/{parent_archive_filename}"
    return [subdir for subdir in os.listdir(folder) if os.path.isdir(os.path.join(folder, subdir))]
def align_up(val, align):
    if (val % align) == 0:
        return val
    return (val + align - 1) & ~(align - 1)

def get_next_offset(file, offset, chunk_size, unk, chunk_type):
    if ("ST05_00E.BIN" in file) and (offset - chunk_size) == 0x89800:
        # hacky workaround
        return align_up(offset, 0x800) + 0x800
    elif "ST04B.BIN" in file and (offset - chunk_size - 0x800) == 0x43000:
        return align_up(offset, 0x800) + 0x800
    elif "ST08_01.BIN" in file and (offset - chunk_size- 0x800) == 0x8B800:
        return align_up(offset, 0x800) + 0x800
    elif "ST14.BIN" in file and (offset - chunk_size- 0x800) == 0x4B800:
        return align_up(offset, 0x800) + 0x800
    elif "ST08_02.BIN" in file and (offset - chunk_size- 0x800) == 0x89800:
        return align_up(offset, 0x800) + 0x800
    elif "ST0F_03.BIN" in file and (offset - chunk_size- 0x800) == 0x69800:
        return align_up(offset, 0x800) + 0x800
    elif "ST05_02.BIN" in file and (offset - chunk_size- 0x800) == 0x72800:
        return align_up(offset, 0x800) + 0x800
    elif "ST00.BIN" in file and (offset - chunk_size- 0x800) in [0x0009E000]:
        return align_up(offset, 0x800) + 0x800
    elif chunk_type == 5 and unk in [0x1820, 0x1220]:
        return align_up(offset, 0x800) + 0x800
    elif chunk_type == 4:
        return align_up(offset - chunk_size, 0x800)
    else:
        return align_up(offset, 0x800)
def build_overlay(parent_archive_filename):
    chunk_file_names = get_chunk_list(parent_archive_filename)
    configured_chunks = []
    need_to_rebuild = False
    for chunk_file_name, _, _, _, _ in chunk_file_names:
        if not os.path.exists(f"config/overlay/splat.{VERSION}.{parent_archive_filename}/{chunk_file_name}.yaml"):
            continue
        configured_chunks.append(chunk_file_name)
        if build_chunk(parent_archive_filename, chunk_file_name):
            need_to_rebuild = True
            binarize_chunk(parent_archive_filename, chunk_file_name)
            build_hash_cache(parent_archive_filename, chunk_file_name)
    if need_to_rebuild:
        for chunk_file_name in configured_chunks:
            elf_bin = f"{BUILD_DIR}/{parent_archive_filename}.{chunk_file_name}.elf.bin"
            if not os.path.exists(elf_bin):
                print(
                    f"buildoverlay: missing {elf_bin} for {parent_archive_filename}/{chunk_file_name}",
                    file=sys.stderr,
                )
                sys.exit(1)
        # TEMP: instead of rebuilding entire bin files, let's just take the existing bin file and emplace the progbins into them (for now)
        shutil.copyfile(f"disks/{VERSION}/CDDATA/DAT/{parent_archive_filename}.BIN", f"{BUILD_DIR}/{parent_archive_filename}.BIN")
        archive_bytes = bytearray(open(f"{BUILD_DIR}/{parent_archive_filename}.BIN", "rb").read())
        for chunk_file_name, chunk_offset, chunk_type, chunk_size, unk in chunk_file_names:
            if not os.path.exists(f"config/overlay/splat.{VERSION}.{parent_archive_filename}/{chunk_file_name}.yaml"):
                continue
            elf_bin = f"{BUILD_DIR}/{parent_archive_filename}.{chunk_file_name}.elf.bin"
            with open(elf_bin, "rb") as chunk:
                archive_bytes[chunk_offset: chunk_offset + os.path.getsize(elf_bin)] = chunk.read()
        with open(f"{BUILD_DIR}/{parent_archive_filename}.BIN", "wb") as f:
            f.write(archive_bytes)
        build_log.write(f"build/{parent_archive_filename}.BIN\n")

    

def main():
    # if "rock_neo.elf" not in src_files_hash_cache or src_files_hash_cache["rock_neo.elf"] != hashlib.sha256(open(f"{BUILD_DIR}/rock_neo.elf", "rb").read()).hexdigest():
    #     generate_rock_neo_syms_txt()
    parent_archive_file_name = sys.argv[1].replace(f"splat.{VERSION}.", "")
    try:
        build_overlay(parent_archive_file_name)
    except Exception as e:
        build_log.write(str(e) + "\n")
        sys.exit(1)


if __name__ == "__main__":
    main()
