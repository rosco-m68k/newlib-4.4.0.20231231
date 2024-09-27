;------------------------------------------------------------
;                                  ___ ___ _
;  ___ ___ ___ ___ ___       _____|  _| . | |_
; |  _| . |_ -|  _| . |     |     | . | . | '_|
; |_| |___|___|___|___|_____|_|_|_|___|___|_,_|
;                     |_____|       firmware v2
;------------------------------------------------------------
; Copyright (c)2020-2021 Ross Bamford and contributors
; See top-level LICENSE.md for licence information.
;
; Stubs for calling SD functions in the EFP from C,
; using the BBSD prototypes from stage 1.
;------------------------------------------------------------
    include "rosco_m68k_public.inc"

ATA_check_support::
    move.l  #15,D0
    trap    #13
    cmp.l   #$1234FEDC,D0
    beq.s   .OK
    move.l  #0,D0
    bra.s   .DONE
.OK
    move.l  #1,D0
.DONE
    rts

ATA_init::
    movem.l A0-A1/D1,-(A7)
    move.l  #16,D0
    move.l  (16,A7),D1
    move.l  (20,A7),A1
    trap    #13
    movem.l (A7)+,A0-A1/D1
    rts

ATA_read_sectors::
    movem.l A0-A2/D1-D2,-(A7)
    move.l  #17,D0
    move.l  (24,A7),A2
    move.l  (28,A7),D1
    move.l  (32,A7),D2
    move.l  (36,A7),A1
    trap    #13
    movem.l (A7)+,A0-A2/D1-D2
    rts

ATA_write_sectors::
    movem.l A0-A2/D1,-(A7)
    move.l  #18,D0
    move.l  (24,A7),A2
    move.l  (28,A7),D1
    move.l  (32,A7),D2
    move.l  (36,A7),A1
    trap    #13
    movem.l (A7)+,A0-A2/D1
    rts

ATA_ident::
    movem.l A0-A2/D1,-(A7)
    move.l  #19,D0
    move.l  (24,A7),A2
    move.l  (28,A7),A1
    trap    #13
    movem.l (A7)+,A0-A2/D1
    rts
