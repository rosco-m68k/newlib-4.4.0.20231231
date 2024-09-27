;------------------------------------------------------------
;                                  ___ ___ _
;  ___ ___ ___ ___ ___       _____|  _| . | |_
; |  _| . |_ -|  _| . |     |     | . | . | '_|
; |_| |___|___|___|___|_____|_|_|_|___|___|_,_|
;                     |_____|           stdlibs
;------------------------------------------------------------
; Copyright (c)2020-2024 Ross Bamford & Contributors
; See rosco_m68k LICENSE.md for licence information.
;
; This is the initialization code. The loader jumps into this code after the
; "program" is received via serial.
;
; The first section is position independent, but will be loaded by the
; firmware loader at $00040000. This section copies the rest of the loaded 
; code to $2000, and then jumps to it.
;
; The second section (after RELOCATED_START) is executed next, based at $1000.
; This just calls out to __kinit to initialize .data and .bss, and then does a
; jump straight into kmain (the user program).
;
; All of this is depending on a bit of linker magic - see 
; hugerom_rosco_m68k_program.ld to see how that works.
;
; GCC global constructors/destructors (with attributes and 
; __cxa_atexit) are supported here, too.
;------------------------------------------------------------
    include "rosco_m68k_public.inc"

    section .reloc

; NOTE: Loaded at $00040000 but init code is position-independent.

_start::                                ; position-independent load addr
    lea.l   _start(PC),A0               ; PC-rel source addr (load addr)
    lea.l   _reloc,A1                   ; absolute dest addr (run addr)
    move.l  #_postreloc_end,D0          ; init section absolute end addr
    sub.l   A1,D0                       ; subtract dest addr for init length
    lsr.l   #2,D0                       ; convert to long words
    subq.l  #1,D0                       ; subtract 1 for dbra
.INIT_LOOP:
    move.l  (A0)+,(A1)+                 ; copy long word from source to dest
    dbra    D0,.INIT_LOOP               ; loop until end of postinit

    jmp     POSTRELOC                   ; jump to copied postinit at run addr

    section .postreloc

POSTRELOC:                              ; running from copied run addr location
    move.l  #_data_end,D0               ; absolute data end addr from linker
    sub.l   A1,D0                       ; subtract dest addr for code+data length
    lsr.l   #2,D0                       ; convert to long words
    move.l  D0,D1                       ; copy for outer loop
    swap    D1                          ; swap for 64K chunk count
    bra.b   .COPY_START                 ; branch to loop test first

.COPY_LOOP:
    move.l  (A0)+,(A1)+                 ; copy long word from source to dest
.COPY_START
    dbra    D0,.COPY_LOOP               ; inner loop
    dbra    D1,.COPY_LOOP               ; outer loop

_enter::                                ; entrypoint at copied run addr location
    move.l  SDB_MEMSIZE,A7              ; reset stack to top of memory
 
    jsr     _bss_init                   ; prepare C environment - clear .bss
    jsr     _init                       ;                       - call global ctors

    move.l  #_fini,-(A7)                ; Set up _fini with atexit
    jsr     atexit

    ifd ROSCO_M68K_DEBUG
    jsr     _start_debugger             ; Initialize debugging on UART 1
    move.l  #_cleanup_debugger,(A7)     ; Make sure we cleanup debugging at exit
    jsr     atexit
    endif

    move.l  #0,(A7)                     ; Stack 0 argc
    move.l  #NULL_ARRAY,-(A7)           ; Stack empty argv
    lea.l   main,A0
    jsr     (A0)                        ; Fly user program, Fly!

.POSTMAIN:
    move.l  D0,-(A7)                    ; main return value to stack
    jsr     exit                        ; call exit to do atexit stuff
                                        ; Noreturn, calls through to our _exit, but newlib
                                        ; code expects a return address so jsr anyway...


_bss_init:
    move.l  #_bss_end,D0
    sub.l   #_bss_start,D0
    lsr.l   #2,D0
    move.l  #_bss_start,A0
    bra.s   .loopstart

.loop:
    clr.l   (A0)+

.loopstart:
    dbra    d0,.loop
    rts

    align  4
    section .bss,bss

NULL_ARRAY      ds.l    1
