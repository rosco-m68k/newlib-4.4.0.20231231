;------------------------------------------------------------
;                                  ___ ___ _   
;  ___ ___ ___ ___ ___       _____|  _| . | |_ 
; |  _| . |_ -|  _| . |     |     | . | . | '_|
; |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
;                     |_____|         libraries
;------------------------------------------------------------
; Copyright (c)2023 Ross Bamford
; See top-level LICENSE.md for licence information.
;
; interrupt for buffered DUART for debugger
;
; This is **not** general purpose, it's specifically wired to
; yank the breakpoint trap when Ctrl-C is received...
;
; The bits that do that are commented (there's one for each
; UART).
;
; See comments in gdb-support.c about potentially simplifying
; this whole piece...
;------------------------------------------------------------

        section .text
        align 2
; DUART register offsets
DUART_MRA       equ     $00
DUART_CSRA      equ     $02
DUART_SRA       equ     $02
DUART_CRA       equ     $04
DUART_RBA       equ     $06
DUART_IMR       equ     $0a
DUART_ISR       equ     $0a
DUART_MRB       equ     $10
DUART_SRB       equ     $12
DUART_CSRB      equ     $12
DUART_CRB       equ     $14
DUART_RBB       equ     $16

; DUART interrupt vector address
VECADDR         equ     $45<<2


; C callable - void _duart_install_interrupt(CHAR_DEVICE *device, RingBuffer *uart_a, RingBuffer *uart_b)
;
; If you don't want to install for both UARTs, pass NULL for either RingBuffer pointer.
;
_duart_install_interrupt::
        movem.l A0-A1/D0,-(A7)

        move.w  SR,D0                           ; Save SR
        or.w    #$0700,SR                       ; No interrupts for a bit

        move.l  16(A7),A0                       ; Get BASEADDR structure
        move.l  (A0),A0                         ; Get base address

        move.l  20(A7),A1                       ; Get RingBuffer A pointer
        move.l  A1,RINGBUF_A                    ; Store it
        beq.s   .uartb                          ; Skip setup if it's NULL

        ; Ensure UART A is set up just like we like it...
        move.b  #$88,DUART_CSRA(A0)             ; 115K2
        move.b  #$10,DUART_CRA(A0)              ; Reset to MR1A
        move.b  #$13,DUART_MRA(A0)              ; Ensure No RTS, RxRDY, Char, No parity, 8 bits
        move.b  #$07,DUART_MRA(A0)              ; (Normal, No TX CTS/RTS, 1 stop bit)

.uartb
        move.l  24(A7),A1                       ; Get RingBuffer B pointer
        move.l  A1,RINGBUF_B                    ; Dereference and store it
        beq.s   .done                           ; Skip setup if it's NULL

        ; Ensure UART B is set up just like we like it...
        move.b  #$88,DUART_CSRB(A0)             ; 115K2
        ;move.b  #$BB,DUART_CSRB(A0)             ; 9600
        move.b  #$10,DUART_CRB(A0)              ; Reset to MR1B
        move.b  #$13,DUART_MRB(A0)              ; Ensure No RTS, RxRDY, Char, No parity, 8 bits
        move.b  #$07,DUART_MRB(A0)              ; (Normal, No TX CTS/RTS, 1 stop bit)
        ;move.b  #$93,DUART_MRB(A0)              ; RTS, RxRDY, Char, No parity, 8 bits
        ;move.b  #$37,DUART_MRB(A0)              ; (Normal, TX CTS/RTS, 1 stop bit)

.done
        move.l  A0,BASEADDR                     ; Store BASEADDR base pointer
        move.l  VECADDR,CHAIN                   ; Store existing handler
        move.l  #HANDLER,VECADDR                ; And install new one

        move.b  #$28,DUART_IMR(A0)              ; Enable RXRDY/RXFULL interrupt and keep counter going
        move.w  D0,SR                           ; Re-enable interrupts

        movem.l (A7)+,A0-A1/D0
        rts


; C callable - void remove_interrupt(void)
;
_duart_remove_interrupt::
        movem.l D0/A0-A1,-(A7)

        move.w  SR,D0                           ; Save SR
        or.w    #$0700,SR                       ; No interrupts for a bit

        move.l  CHAIN,VECADDR                   ; Restore original handler
        move.l  BASEADDR,A0                     ; Get BASEADDR structure
        move.b  #$08,DUART_IMR(A0)              ; Just keep counter going

        move.w  D0,SR                           ; Re-enable interrupts
        movem.l (A7)+,D0/A0-A1
        rts


; The interrupt handler (also chains to the original handler)
HANDLER:
        movem.l D0-D1/A0-A2,-(A7)
        clr.w   INTR_FLAG                       ; Clear the CTRL-C interrupt flag
        move.l  BASEADDR,A0                     ; Load BASEADDR pointer
        move.l  #duart_buffer_char,A1           ; Buffer routine in A1
        move.l  RINGBUF_A,A2                    ; UART A RingBuffer in A2
        cmp.l   #0,A2                           ; Is the pointer NULL?
        beq.s   .uartB                          ; Skip to DUART B if so...

;; UART A
.loopA
        move.b  DUART_ISR(A0),D0
        btst    #1,D0                           ; Check if ready bit is set
        beq.s   .uartB                          ; Just bail now if not (and go check UART B)

        move.b  DUART_SRA(A0),D0                ; Check if error bits are set
        and.b   #$F0,D0
        beq.s   .contA                          ; Continue if not...
        bsr.s   .error                          ; ... else branch error subroutine

.contA
        move.b  DUART_RBA(A0),D0                ; Grab character from A receive buffer
        jsr     (A1)                            ; Call duart_buffer_char

        bra.s   .loopA                          ; And continue testing...

; .error is a subroutine, used by both A and B
.error
        move.b  D0,D1                           ; Every day I'm shufflin' (registers)
        btst    #4,D1                           ; Overrun error?
        beq.s   .notoverrun        
        move.b  #$40,DUART_CRA(A0)              ; Reset overrun error status

.notoverrun
        rts
        
;; UART B
.uartB
        move.l  RINGBUF_B,A2                    ; UART B RingBuffer in A2
        cmp.l   #0,A2                           ; Is the pointer NULL?
        beq.w   .chain                          ; Skip to chained handler if so...

.loopB
        move.b  DUART_ISR(A0),D0
        btst    #5,D0                           ; Check if ready bit is set
        beq.s   .chain                          ; Just bail now if not (and go check timer tick)

        move.b  DUART_SRB(A0),D0                ; Check if error bits are set
        and.b   #$F0,D0
        beq.s   .contB                          ; Continue if not...
        bsr.s   .error                          ; ... else branch error subroutine

.contB        
        move.b  DUART_RBB(A0),D0                ; Grab character from B receive buffer

        ; DEBUGGER SPECIFIC
        cmp.b   #3,D0                           ; Did we get a ctrl-c?
        bne.s   .do_buffer_b                    ; Continue with buffering if not...
        
        ; This is about to get a bit hairy, stay with me...
        ;
        ; This handler, when done, chains a return to the original DUART handler, to keep
        ; the timer tick and heartbeat flash working.
        ;
        ; What this code does is set up a fake exception frame on the stack, such that 
        ; when the `rte` at the end of the chained handler is hit, it will "return to"
        ; the debugger's `_catchException` handler, with everything set up as if a NMI
        ; has been triggered.
        ;
        ; That handler will then interpret that "NMI" as a SIGINT, and communicate back
        ; to the gdb client that we've been interrupted, with frames etc set up correctly
        ; to reflect the place in the calling code where _this_ DUART interrupt was 
        ; triggered.
        ;
        tst.w   INTR_FLAG                       ; Have we already set up an interrupt frame?
        bne.s   .loopB                          ; Just continue looping if so

        move.l  (A7)+,TEMP_REGS+0               ; Unstack the saved registers for a sec
        move.l  (A7)+,TEMP_REGS+4
        move.l  (A7)+,TEMP_REGS+8
        move.l  (A7)+,TEMP_REGS+12
        move.l  (A7)+,TEMP_REGS+16

        move.w  #$7c,6(A7)                      ; Fiddle the vector offset in the calling frame, make it look like NMI (translates to SIGINT)
        move.w  #$114,-(A7)                     ; Create a new exception frame, with reason code 276 (user vec 0x45)...
        move.l  #_catchException,-(A7)          ; Returning to _catchException...
        move.w  sr,-(A7)                        ; With whatever SR we have right now...

        move.l  TEMP_REGS+16,-(A7)              ; Restack those saved registers
        move.l  TEMP_REGS+12,-(A7)
        move.l  TEMP_REGS+8,-(A7)
        move.l  TEMP_REGS+4,-(A7)
        move.l  TEMP_REGS+0,-(A7)

        move.w  #1,INTR_FLAG                    ; Set the interrupt flag, so we don't set up another fake frame
                                                ; (this is in case the user is sitting on CTRL-C).

        bra.s   .loopB                          ; And go back to looping - don't bother buffering the ctrl-c
        ; /DEBUGGER SPECIFIC

.do_buffer_b:
        jsr     (A1)                            ; Call duart_buffer_char

        bra.w   .loopB                          ; And continue testing...

.chain
        movem.l (A7)+,D0-D1/A0-A2               ; Restore regs...
        move.l  CHAIN,-(A7)                     ; And "return to" the original ISR
        rts


        section .bss,bss
        align 4
RINGBUF_A   ds.l        1                       ; Ringbuffer for UART A
RINGBUF_B   ds.l        1                       ; Ringbuffer for UART B
BASEADDR    ds.l        1                       ; DUART base address from CHAR_DEVICE struct     
CHAIN       ds.l        1                       ; Chained ISR (timer tick probably)
TEMP_REGS   ds.l        20                      ; Temp register storage for HANDLER
INTR_FLAG   ds.w        2                       ; Interrupt set flag for HANDLER
