@ =============================================================================
@ io.s - Input/Output functions for the SubC language, written in ARM
@        assembly language
@ =============================================================================

          .data
          .align  4
io_pars:  .word   1                       @ file handle = stdout
          .word   0                       @ points to message text
          .word   0                       @ # of chars to write

io_nl:    .asciz  "\r\n"                  @ CR, LF

          .equ    ANGEL_EXIT,  0x18       @ exit program
          .equ    ANGEL_CALL,  0x123456   @ syscall number
          .equ    ANGEL_WRITE, 0x05       @ write-to-file
          .equ    PARAM_TEXT,  0x04       @ offset in params for pointer-to-text
          .equ    PARAM_LEN,   0x08       @ offset in params for text-length

          .global says, sayn, sayl

          .text
          .align  4
@ ===================================================================
@ Measure the number of character in the string at [R0].  Return
@ answer in R0.  (This is a leaf subroutine, so no need to
@ save/restore LR or FP)
@ ===================================================================
io_len:   MOV   R1, #0          @ counter
io_10:    LDRB  R2, [R0], #1    @ load current byte
          CMP   R2, #0          @ sentinel zero?
          BEQ   io_20
          ADD   R1, R1, #1      @ one more char*
          B     io_10
io_20:    MOV   R0, R1          @ move length into R0
          BX    LR

@ ===================================================================
@ The SubC compiler, for the call: i = says(msg); will emit the
@ code:   LDR R0, <msg-reg-or-off>
@         BL  says
@ Note that 'says' uses only registers {R0-R3} which are "scratch",
@ so no need to save or restore any registers here
@ ===================================================================
says: PUSH  {LR}                  @ because we call a subroutine
      PUSH  {R0}                  @ save R0 (pointer to message)
      BL    io_len                @ how long is message? answer in R0

      LDR   R1, =io_pars          @ parameter block
      STR   R0, [R1,#PARAM_LEN]   @ update params - length
      POP   {R0}                  @ restore R0 (pointer to message)
      STR   R0, [R1,#PARAM_TEXT]  @ update params - message

      MOV   R0, #ANGEL_WRITE      @ Write-to-File

      SWI   #ANGEL_CALL           @ syscall
      MOV   R0, #0                @ always return 0
      POP   {LR}
      BX    LR

@ ===================================================================
@ int sayn(int n) - display 'n' as an 8-digit hex integer.  Returns 0
@
@ Suppose n = R0 = 0x7ABC1234.  Start by rotating R0 by 28 bits into
@ R1, which will then contain 0xABC12347.  Now mask off the low 8
@ bits to obtain R1 = 0x00000007.  Convert 0-9 to Ascii '0'-'9', or
@ 0xA-0xF to Ascii 'A'-'F'.
@
@ Then repeat, with a rotate by 24 bits, etc
@ ===================================================================
io_buf: .space  9                 @ 8 hex digits + 8 trailing '\0'
        .align  4

sayn:   PUSH  {LR}                @ save
        LDR   R2, =io_buf
        LDR   R3, =28             @ shift amount

io_30:  CMP   R3, #0              @ done?
        BLT   io_40               @ yes

        ROR   R1, R0, R3          @ eg: 7ABC1234 -> ABC12347
        SUB   R3, #4              @ eg: 28       -> 24
        AND   R1, R1, #0x0000000F @ eg:          -> 00000007
        CMP   R1, #9              @ eg: compare 7 and 9
        ADDLE R1, R1, #'0'        @     '7'
        ADDGT R1, R1, #'A' - 0xA  @ eg: 0xA -> 'A'
        STRB  R1, [R2], #1
        B     io_30

io_40:  LDR   R0, =io_buf
        BL    says
        MOV   R0, #0              @ always returns 0
        POP   {LR}                @ restore
        BX    LR

@ ===================================================================
@ int sayl() - display a newline.  Returns 0.
@ ===================================================================
sayl: PUSH  {LR}                  @ because we call a subroutine
      LDR   R0, =io_nl
      BL    says
      POP   {LR}                  @ restore
      BX    LR                    @ return