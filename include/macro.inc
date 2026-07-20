.macro glabel label
    .global \label
    \label:
.endm

.macro .def #
.endm

.macro li reg, num
.if \num < 0x8000 && \num > 0
  ori \reg, $0, \num & 0xFFFF
.elseif \num > 0xFFFF && (\num & 0xFFFF) != 0
  /* big constant with nonzero low half: raw-shift high part (NOT %hi,
     which carry-rounds when low >= 0x8000) + ori. Without this branch
     the low half was silently DROPPED (caught by func_80053B40's
     0x86186187 magic divisor, 2026-07-19). */
  lui \reg, (\num >> 16) & 0xFFFF
  ori \reg, \reg, \num & 0xFFFF
.elseif \num > 0xFFFF
  lui \reg, %hi(\num)
.elseif \num > 0
  ori \reg, $0, \num & 0xFFFF
.elseif \num == -0x8000
  addiu \reg, $0, \num & 0xFFFF
.elseif \num > -0x8000
  addiu \reg, $0, \num
.elseif (\num & 0xFFFF) != 0
  lui \reg, (\num >> 16) & 0xFFFF
  ori \reg, \reg, \num & 0xFFFF
.else
  lui \reg, %hi(\num)
.endif
.endm

.macro move a, b
    addu \a, \b, $zero
.endm
.macro nonmatching name, size
.endm
.macro enddlabel name
.endm
.macro alabel name
.endm
