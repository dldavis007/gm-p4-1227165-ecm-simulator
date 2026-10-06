	;*******************************************************
	;* BUA/9340 P/N 16059335 8/1/85 P
	;* SS BY BUA/1728 P/N
	;*
	;* ECM P/N 1227165
	;*
	;* MY86 YB L98 MD8 NM8 VE1 G44
	;*
	;* GM P4 computer system
	;*
	;* Calibration Tables
	;*
	;* Deduced fm CAMS Disassembly of EPROM Code
	;*
	;* From 1986 Y CAR, (BUA, VETTE)
	;*
	;* L98, 8 85 - 86 160 BAUD ONLY
	;* LB9, F 86 160 BAUD ONLY
	;*
	;* LB9, F 87 - 88 160 & 8192 BAUD
	;* L98, 8 87 - 88 160 & 8192 BAUD
	;*
	;* DATA ON PIN E
	;*
	;* The ECM is uni-directiona in 160 Baud when the ECM
	;* is not in the ALDL mode. When the ECM is in the ALDL
	;* Mode, (10k). The ECM will xmit a normal 25 item ALDL
	;* list at 160 baud.
	;*
	;* The ECM will listen for 50 MSEC after completion of
	;* the 160 baud ALDL list. for an inboud Test device
	;* originated 8192 (polling) message.
	;*
	;* If the 8192 polling message is accepted, 8192
	;* commications will continue till power down,
	;*
	;*
	;* CALIB = $C000 - $C800
	;* IEEE CK SUM =
	;*
	;*******************************************************
		ORG $C000
LC000: 	FDB $25E5 	; EPROM ID code
LC002: 	FDB $0979 	; Date Code
LC004: 	FDB $02EE 	; Sequence Number
					;
;LC006: 	FDB $9708 	; Ck Sum of Addr's $C008 - $FFFF (SUM of all bytes)
LC006: 	FDB $973A 	; Ck Sum of Addr's $C008 - $FFFF (SUM of all bytes) C6AE B6 instead of 84
LC008: 	FCB $32 	; Pgm Match Byte, $32 = 1986 Prod.
					; -Set $AA For bypass of Cksum
					;
LC009: 	FCB 0 		; Num of Cyl, 8 = 0000 0000, $00
					; 6 = 1100 0000, $C0
					; 4 = 1000 0000, $80
					; 3 = 0110 0000, $60
					;
LC00A: 	FDB $CA21 	; Cust ID WORD #1
LC00C: 	FDB $C1A1 	; Cust ID WORD #2
					;
					;
LC00E: 	FDB 3277 	; 66Hz, VATS Ck Value, Fail if VATS L.T.
LC010: 	FDB 1638 	; 33hZ, VATS Ck Value, Fail if VATS G.T.
					;
LC012: 	FDB $0385 	; If Ign off => this, turn off MPU
					;----------------------------------------
LC014: 	FCB $B4 	; 1011 0100 Air Fuel Opt Word 1, ($B4)
					;
					; b7 = Use Filter for Air flow
					; b6 = Use TCC for Shft Lamp Cnt'l
					; b5 = Req Clsed Lp for Can Purge
					; b4 = VATS Enable
					;
					; b3 =
					; b2 = Analog MAF Meter in use (vs FM DIGITAL)
					; b1 = Single Fire Mode
					; b0 = Manual Xmission
					;
					;----------------------------------------
LC015: 	FCB $04 	; 0000 0100 2nd AIr Flow Mode Word, ($04)
					;
					; b7 =
					; b6 =
					; b5 =
					; b4 =
					;
					; b3 =
					; b2 =
					; b1 = 1 = 2 AIR valves, 0 = 1 valve
					; b0 =
					;
					;----------------------------------------
LC016: 	FCB $62 	; 3rd AIr Flow Mode Word, 0110 0010
					;
					; b7 =
					; b6 = Calc Base Inj PW, not Tbl
					; b5 = Int Reset when B.Lrn cell Chnge
					; b4 =
					;
					; b3 =
					; b2 =
					; b1 = Reset Corr/init of Auto Enrich
					; b0 =
					;
					;----------------------------------------
LC017: 	FCB $00 	; 4th AIr Flow Mode Word, 0000 0000
					;
					; b7 = Use TCC out to Cnt'l A/C Clutch
					; b6 = Use Pwr Steer Pressure Sw
					; b5 = N.O. Cooling Fan Req input
					; b4 = TCC lk'ed Hwy Mode Spark Ck On.
					;
					; b3 = If CCP Chg, force Int to 128
					; b2 = 4th Gear Hwy Mode Spark Adv on
					; b1 =
					; b0 = Use Ld Val for BLM not dispflow
					;----------------------------------------
		;*******************************************************
		;* Spark param'a, (EST)
		;* TYPE 32 ECM P/N 122
		;*******************************************************
LC018: 	FDB 3277 	; Start up RPM
LC01A: 	FCB 8 		; 12.5 mS LP'S RPM must be up, (8 Lp's)
LC01B: 	FCB 32 		; If Diff Ld Val > Use Max Dwell
LC01C: 	FCB 17 		; Advance Ref, (5.9 Deg), (Val/2.844)
					; Initial advance -- 6 deg
LC01D: 	FCB 13 		; 198.28 usec Time Corr to S.A.
					; - (Val * 15.26) (spark delay)
LC01E: 	FDB 0119 	; 42 DEG Max S.A., (Rel to lead)
LC020: 	FDB 65525 	; 3.5 Deg Max S.A. Retard
LC022: 	FDB 57 		; Fixed S.A. For Diag, (20 Deg)
LC024: 	FCB 23 		; 8 Deg Add S.A. for ALDL
					;
					; >> STALL SAVER PARAMS <<
					; NUM = ARG/12.5 For all RPM args
					;
LC025: 	FCB 32 		; Stall Saver Lo lmt, (400 RPM)
LC026: 	FCB 36 		; Stall Saver Lmt, (450 RPM)
LC027: 	FCB 44 		; Stall Saver Lmt, (550 RPM)
LC028: 	FCB 44 		; If RPM > Disable Stall Saver S.A.
LC029: 	FCB 2 		; Add To SABLND ****
					;
					; Hot Restart Spark Params
					;
LC02A: 	FCB 113 	; If Cool > 54.7C (129 F) DISABLE HOT RESTART,
					; Num = (temp C +40) * (256/192)
					;
LC02B: 	FCB 28 		; HOT RETARD, (9.8 Deg)
					; Num = temp * (256/192)
					;
LC02C: 	FCB 113 	; DISABLE IF COOL => 54.7c
					; Num = (temp C +40) * (256/192)
					;
					; Disable cold S.A.
					;
LC02D: 	FCB 27 		; If cool diff fm start > 20c
					; Num = temp C * (256/192)
					;----------------------------------------------
					; MAIN SPARK ADV vs LD vs RPM
					;
					; Dissassemby of BUA
					; 09-03-1994, 13:39:24
					;
					;
					; 12 x 12
					;
					; TBL = SPK * (256/90)
					;----------------------------------------------
		ORG $C02E
LC02E: 	FCB 0 		; Min RPM Val
		FCB 32 		; Min LD VALUE Value
		FCB 12 		; COL'S/ROW
					;----------------------------------------------
					; 400 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 57 		; 20.0 32.0
		FCB 57 		; 20.0 48.0
		FCB 57 		; 20.0 64.0
		FCB 57 		; 20.0 80.0
		FCB 57 		; 20.0 96.0
		FCB 57 		; 20.0 112.0
		FCB 57 		; 20.0 128.0
		FCB 57 		; 20.0 144.0
		FCB 57 		; 20.0 160.0
		FCB 57 		; 20.0 176.0
		FCB 57 		; 20.0 192.0
		FCB 57 		; 20.0 208.0
					;----------------------------------------------
					; 600 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 57 		; 20.0 32.0
		FCB 57 		; 20.0 48.0
		FCB 57 		; 20.0 64.0
		FCB 57 		; 20.0 80.0
		FCB 57 		; 20.0 96.0
		FCB 57 		; 20.0 112.0
		FCB 57 		; 20.0 128.0
		FCB 57 		; 20.0 144.0
		FCB 57 		; 20.0 160.0
		FCB 57 		; 20.0 176.0
		FCB 57 		; 20.0 192.0
		FCB 57 		; 20.0 208.0
					;----------------------------------------------
					; 800 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 77 		; 27.1 32.0
		FCB 77 		; 27.1 48.0
		FCB 77 		; 27.1 64.0
		FCB 77 		; 27.1 80.0
		FCB 77 		; 27.1 96.0
		FCB 70 		; 24.6 112.0
		FCB 65 		; 22.9 128.0
		FCB 65 		; 22.9 144.0
		FCB 63 		; 22.2 160.0
		FCB 59 		; 20.7 176.0
		FCB 59 		; 20.7 192.0
		FCB 59 		; 20.7 208.0
					;----------------------------------------------
					; 1000 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 80 		; 28.1 32.0
		FCB 80 		; 28.1 48.0
		FCB 80 		; 28.1 64.0
		FCB 80 		; 28.1 80.0
		FCB 80 		; 28.1 96.0
		FCB 73 		; 25.7 112.0
		FCB 65 		; 22.9 128.0
		FCB 65 		; 22.9 144.0
		FCB 65 		; 22.9 160.0
		FCB 59 		; 20.7 176.0
		FCB 59 		; 20.7 192.0
		FCB 59 		; 20.7 208.0
					;----------------------------------------------
					; 1200 RPM
					;
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 85 		; 29.9 32.0
		FCB 85 		; 29.9 48.0
		FCB 85 		; 29.9 64.0
		FCB 85 		; 29.9 80.0
		FCB 85 		; 29.9 96.0
		FCB 78 		; 27.4 112.0
		FCB 70 		; 24.6 128.0
		FCB 70 		; 24.6 144.0
		FCB 70 		; 24.6 160.0
		FCB 60 		; 21.1 176.0
		FCB 60 		; 21.1 192.0
		FCB 60 		; 21.1 208.0
					;----------------------------------------------
					; 1400 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 88 		; 30.9 32.0
		FCB 88 		; 30.9 48.0
		FCB 88 		; 30.9 64.0
		FCB 88 		; 30.9 80.0
		FCB 88 		; 30.9 96.0
		FCB 81 		; 28.5 112.0
		FCB 73 		; 25.7 128.0
		FCB 73 		; 25.7 144.0
		FCB 73 		; 25.7 160.0
		FCB 65 		; 22.9 176.0
		FCB 65 		; 22.9 192.0
		FCB 65 		; 22.9 208.0
					;----------------------------------------------
					; 1600 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 102 	; 35.9 32.0
		FCB 102 	; 35.9 48.0
		FCB 101 	; 35.5 64.0
		FCB 98 		; 34.5 80.0
		FCB 96 		; 33.8 96.0
		FCB 94 		; 33.1 112.0
		FCB 91 		; 32.0 128.0
		FCB 88 		; 30.9 144.0
		FCB 77 		; 27.1 160.0
		FCB 70 		; 24.6 176.0
		FCB 70 		; 24.6 192.0
		FCB 70 		; 24.6 208.0
					;----------------------------------------------
					; 1800 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 106 	; 37.3 32.0
		FCB 106 	; 37.3 48.0
		FCB 105 	; 36.9 64.0
		FCB 102 	; 35.9 80.0
		FCB 100 	; 35.2 96.0
		FCB 98 		; 34.5 112.0
		FCB 93 		; 32.7 128.0
		FCB 91 		; 32.0 144.0
		FCB 81 		; 28.5 160.0
		FCB 73 		; 25.7 176.0
		FCB 73 		; 25.7 192.0
		FCB 73 		; 25.7 208.0
					;----------------------------------------------
					; 2000 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 109 	; 38.3 32.0
		FCB 109 	; 38.3 48.0
		FCB 108 	; 38.0 64.0
		FCB 104 	; 36.6 80.0
		FCB 102 	; 35.9 96.0
		FCB 99 		; 34.8 112.0
		FCB 95 		; 33.4 128.0
		FCB 93 		; 32.7 144.0
		FCB 85 		; 29.9 160.0
		FCB 75 		; 26.4 176.0
		FCB 75 		; 26.4 192.0
		FCB 75 		; 26.4 208.0
					;----------------------------------------------
					; 2200 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 112 	; 39.4 32.0
		FCB 112 	; 39.4 48.0
		FCB 111 	; 39.0 64.0
		FCB 105 	; 36.9 80.0
		FCB 102 	; 35.9 96.0
		FCB 102 	; 35.9 112.0
		FCB 98 		; 34.5 128.0
		FCB 94 		; 33.1 144.0
		FCB 88 		; 30.9 160.0
		FCB 78 		; 27.4 176.0
		FCB 78 		; 27.4 192.0
		FCB 78 		; 27.4 208.0
					;----------------------------------------------
					; 2400 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 116 	; 40.8 32.0
		FCB 116 	; 40.8 48.0
		FCB 114 	; 40.1 64.0
		FCB 108 	; 38.0 80.0
		FCB 105 	; 36.9 96.0
		FCB 102 	; 35.9 112.0
		FCB 100 	; 35.2 128.0
		FCB 97 		; 34.1 144.0
		FCB 92 		; 32.3 160.0
		FCB 81 		; 28.5 176.0
		FCB 81 		; 28.5 192.0
		FCB 81 		; 28.5 208.0
					;----------------------------------------------
					; 2800 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 120 	; 42.2 32.0
		FCB 120 	; 42.2 48.0
		FCB 116 	; 40.8 64.0
		FCB 115 	; 40.4 80.0
		FCB 108 	; 38.0 96.0
		FCB 108 	; 38.0 112.0
		FCB 100 	; 35.2 128.0
		FCB 97 		; 34.1 144.0
		FCB 94 		; 33.1 160.0
		FCB 85 		; 29.9 176.0
		FCB 85 		; 29.9 192.0
		FCB 85 		; 29.9 208.0
					;----------------------------------------------
					; 3200 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 134 	; 47.1 32.0
		FCB 134 	; 47.1 48.0
		FCB 125 	; 44.0 64.0
		FCB 122 	; 42.9 80.0
		FCB 108 	; 38.0 96.0
		FCB 108 	; 38.0 112.0
		FCB 100 	; 35.2 128.0
		FCB 97 		; 34.1 144.0
		FCB 94 		; 33.1 160.0
		FCB 85 		; 29.9 176.0
		FCB 85 		; 29.9 192.0
		FCB 85 		; 29.9 208.0
					;----------------------------------------------
					; 3600 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 134 	; 47.1 32.0
		FCB 134 	; 47.1 48.0
		FCB 129 	; 45.4 64.0
		FCB 129 	; 45.4 80.0
		FCB 114 	; 40.1 96.0
		FCB 108 	; 38.0 112.0
		FCB 101 	; 35.5 128.0
		FCB 97 		; 34.1 144.0
		FCB 94 		; 33.1 160.0
		FCB 85 		; 29.9 176.0
		FCB 85 		; 29.9 192.0
		FCB 85 		; 29.9 208.0
					;----------------------------------------------
					; 4000 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 134 	; 47.1 32.0
		FCB 134 	; 47.1 48.0
		FCB 129 	; 45.4 64.0
		FCB 129 	; 45.4 80.0
		FCB 129 	; 45.4 96.0
		FCB 117 	; 41.1 112.0
		FCB 106 	; 37.3 128.0
		FCB 97 		; 34.1 144.0
		FCB 94 		; 33.1 160.0
		FCB 85 		; 29.9 176.0
		FCB 85 		; 29.9 192.0
		FCB 85 		; 29.9 208.0
					;----------------------------------------------
					; 4400 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 134 	; 47.1 32.0
		FCB 134 	; 47.1 48.0
		FCB 129 	; 45.4 64.0
		FCB 129 	; 45.4 80.0
		FCB 129 	; 45.4 96.0
		FCB 117 	; 41.1 112.0
		FCB 106 	; 37.3 128.0
		FCB 106 	; 37.3 144.0
		FCB 94 		; 33.1 160.0
		FCB 87 		; 30.6 176.0
		FCB 87 		; 30.6 192.0
		FCB 87 		; 30.6 208.0
					;----------------------------------------------
					; 4800 RPM
					; SPK ADV LD VAL
					;----------------------------------------------
		FCB 134 	; 47.1 32.0
		FCB 134 	; 47.1 48.0
		FCB 129 	; 45.4 64.0
		FCB 129 	; 45.4 80.0
		FCB 129 	; 45.4 96.0
		FCB 117 	; 41.1 112.0
		FCB 106 	; 37.3 128.0
		FCB 106 	; 37.3 144.0
		FCB 94 		; 33.1 160.0
		FCB 92 		; 32.3 176.0
		FCB 92 		; 32.3 192.0
		FCB 92 		; 32.3 208.0
					;----------------------------------------------
					;----------------------------------------------
					; COOLANT ADVANCE CORRECTION vs LOAD VAL vs COO
					;
					; Dissassemby of BUA, blocks = 12
					; 02-05-1995, 11:02:07
					;
					; TBL = (SPK ADV + BIAS) * (256/90 )
					;----------------------------------------------
		ORG $C0FD 	;
					;
LC0FD: 	FCB 57 		; 20 DEG SA, COOL TBL BIAS
					;
LC0FE: 	FCB 0 		; Ld selector 0 = LV8
					; 4 = LVALT
					;
LC0FF: 	FCB 32 		; Min COOL Val, (-16 C)
		FCB 32 		; Min LD VAL Value
		FCB 9 		; COL'S/ROW
					;----------------------------------------------
					; -16 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 81 		; 8.5 32.0
		FCB 81 		; 8.5 48.0
		FCB 81 		; 8.5 64.0
		FCB 81 		; 8.5 80.0
		FCB 81 		; 8.5 96.0
		FCB 81 		; 8.5 112.0
		FCB 81 		; 8.5 128.0
		FCB 85 		; 9.9 144.0
		FCB 85 		; 9.9 160.0
					;----------------------------------------------
					; -4 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 81 		; 8.5 32.0
		FCB 81 		; 8.5 48.0
		FCB 81 		; 8.5 64.0
		FCB 81 		; 8.5 80.0
		FCB 81 		; 8.5 96.0
		FCB 81 		; 8.5 112.0
		FCB 81 		; 8.5 128.0
		FCB 85 		; 9.9 144.0
		FCB 85 		; 9.9 160.0
					;----------------------------------------------
					; 8 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 68 		; 3.9 32.0
		FCB 68 		; 3.9 48.0
		FCB 68 		; 3.9 64.0
		FCB 68 		; 3.9 80.0
		FCB 68 		; 3.9 96.0
		FCB 68 		; 3.9 112.0
		FCB 68 		; 3.9 128.0
		FCB 68 		; 3.9 144.0
		FCB 68 		; 3.9 160.0
					;----------------------------------------------
					; 20 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 57 		; 0.0 80.0
		FCB 57 		; 0.0 96.0
		FCB 57 		; 0.0 112.0
		FCB 57 		; 0.0 128.0
		FCB 71 		; 5.0 144.0
		FCB 71 		; 5.0 160.0
					;----------------------------------------------
					; 32 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 57 		; 0.0 80.0
		FCB 57 		; 0.0 96.0
		FCB 57 		; 0.0 112.0
		FCB 57 		; 0.0 128.0
		FCB 71 		; 5.0 144.0
		FCB 71 		; 5.0 160.0
					;----------------------------------------------
					; 44 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 57 		; 0.0 80.0
		FCB 57 		; 0.0 96.0
		FCB 57 		; 0.0 112.0
		FCB 57 		; 0.0 128.0
		FCB 71 		; 5.0 144.0
		FCB 71 		; 5.0 160.0
					;----------------------------------------------
					; 56 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 57 		; 0.0 80.0
		FCB 57 		; 0.0 96.0
		FCB 57 		; 0.0 112.0
		FCB 57 		; 0.0 128.0
		FCB 57 		; 0.0 144.0
		FCB 57 		; 0.0 160.0
					;----------------------------------------------
					; 68 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 57 		; 0.0 80.0
		FCB 57 		; 0.0 96.0
		FCB 57 		; 0.0 112.0
		FCB 57 		; 0.0 128.0
		FCB 57 		; 0.0 144.0
		FCB 57 		; 0.0 160.0
					;----------------------------------------------
					; 80 Deg c COOL
					;
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 57 		; 0.0 80.0
		FCB 57 		; 0.0 96.0
		FCB 57 		; 0.0 112.0
		FCB 57 		; 0.0 128.0
		FCB 57 		; 0.0 144.0
		FCB 57 		; 0.0 160.0
					;----------------------------------------------
					; 92 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 57 		; 0.0 80.0
		FCB 57 		; 0.0 96.0
		FCB 57 		; 0.0 112.0
		FCB 57 		; 0.0 128.0
		FCB 57 		; 0.0 144.0
		FCB 57 		; 0.0 160.0
					;----------------------------------------------
					; 104 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 57 		; 0.0 80.0
		FCB 57 		; 0.0 96.0
		FCB 57 		; 0.0 112.0
		FCB 57 		; 0.0 128.0
		FCB 57 		; 0.0 144.0
		FCB 57 		; 0.0 160.0
					;----------------------------------------------
					; 116 Deg c COOL
					; spk adv LV8
					;----------------------------------------------
		FCB 57 		; 0.0 32.0
		FCB 57 		; 0.0 48.0
		FCB 57 		; 0.0 64.0
		FCB 51 		; -2.1 80.0
		FCB 46 		; -3.8 96.0
		FCB 40 		; -5.9 112.0
		FCB 40 		; -5.9 128.0
		FCB 40 		; -5.9 144.0
		FCB 40 		; -5.9 160.0
					;----------------------------------------------
					;----------------------------------------------
					; HIGHWAY MODE SPARK CONTROL
					; ADVANCE vs LD8 LOAD
					;
					; TYPE 32, BUA
					;----------------------------------------------
		ORG $C16E
LC16E: 	FCB 133 	; If Coolant <= 60c, (140f), disable
					; hi-way mode spark
					;
LC16F: 	FCB 100 	; If LV8 > 100 disable hi-way mode spark
					;
LC170: 	FCB 254 	; If RPM is less than 6350 RPM then
					; disable hi-way mode spark
					;
LC171: 	FCB 10 		; Enable hi-way mode spark if > 10 Sec
					;------------------------------------------------------
					; HIGHWAY MODE SPARK ADVANCE vs LOAD VALUE
					;
					;
					; TBL = SA * (256/90)
					;----------------------------------------------
					; DEG SA LD VAL
					;----------------------------------
LC172: 	FCB 28 		; 10 32
		FCB 28 		; 10 48
		FCB 28 		; 10 64
		FCB 28 		; 10 80
		FCB 28 		; 10 96
		FCB 23 		; 10 112
		FCB 8 		; 3 128
		FCB 0 		; 0 144
					;----------------------------------------------
					;----------------------------------------------
					; START UP SPARK ADVANCE vs COOLANT TEMP
					;
					;
					; TBL = SA deg * (256/90)
					;----------------------------------------------
		ORG $C17A 	; DEG SPK Deg C COOL
					;----------------------------------
LC17A: 	FCB 28 		; 10 -40
LC17B: 	FCB 28 		; 10 -28
LC17C: 	FCB 28 		; 10 -16
LC17D: 	FCB 28 		; 10 -4
LC17E: 	FCB 28 		; 10 8
LC17F: 	FCB 28 		; 10 20
LC180: 	FCB 28 		; 10 32
LC181: 	FCB 28 		; 10 44
LC182: 	FCB 28 		; 10 56
LC183: 	FCB 28 		; 10 68
LC184: 	FCB 28 		; 10 80
LC185: 	FCB 28 		; 10 92
LC186: 	FCB 28 		; 10 104
LC187: 	FCB 28 		; 10 116
					;----------------------------------------------
					;----------------------------------------------
					; START UP SPARK ADVANCE DECAY DELAY vs STARTUP COOL
					;
					; Dissassemby of BUA, LINES = 14
					; 09-14-1994, 10:09:39
					;
					; Table value = INJECTS
					;
					; TBL = VAL/LC188
					;----------------------------------------------
		ORG $C188 	;
					;
LC188: FDB 0256 	; Mult for tbl,
					; (NUM = ARG * 256)
					;
					; DECAY DLY DEG c S/U COOL
					;----------------------------------
LC18A: 	FCB 100 	; 25,600 -40
		FCB 100 	; 25,600 -28
		FCB 100 	; 25,600 -16
		FCB 100 	; 25,600 -4
		FCB 100 	; 25,600 8
		FCB 100 	; 25,600 20
		FCB 100 	; 25,600 32
		FCB 100 	; 25,600 44
		FCB 100 	; 25,600 56
		FCB 80 		; 25,600 68
		FCB 40 		; 25,600 80
		FCB 40 		; 25,600 92
		FCB 40 		; 25,600 104
		FCB 40 		; 25,600 116
					;----------------------------------------------
					;----------------------------------------------
					; START UP SPARK ADVANCE DECAY TIME vs STARTUP COOL
					;
					;
					; TBL = INJECTS
					;----------------------------------------------
		ORG $C198 	;INJ'S DEG c S/U COOL
					;----------------------------------
LC198: 	FCB 1 		; -40
		FCB 1 		; -28
		FCB 1 		; -16
		FCB 1 		; -4
		FCB 1 		; 8
		FCB 1 		; 20
		FCB 1 		; 32
		FCB 1 		; 44
		FCB 1 		; 56
		FCB 1 		; 68
		FCB 1 		; 80
		FCB 1 		; 92
		FCB 1 		; 104
		FCB 1 		; 116
					;----------------------------------------------
					;----------------------------------------------
		FCB 0
		FCB 0
		FCB 0
		FCB 0
					;----------------------------------------------
		;***********************************************
		;* PROM TEST WORD 1 FOR ERR CODE 51
		;* Set ERR code 51 if N.E. ($55)
		;***********************************************
		ORG $C1AA
LC1AA: 	FCB $55 	; EPROM TEST WD 1
					;----------------------------------------------
					; STARTUP SPK ADV DECAY VS STARTUP COOLANT
					;
					; TBL =
					;----------------------------------------------
		ORG $C1AB 	; Deg c COOL
					;----------------------------------
LC1AB: 	FCB 128 	; 1.4 Degrees -40
		FCB 128 	; -28
		FCB 128 	; -16
		FCB 128 	; -4
		FCB 128 	; 8
		FCB 128 	; 20
		FCB 128 	; 32
		FCB 128 	; 44
		FCB 128 	; 56
		FCB 128 	; 68
		FCB 128 	; 80
		FCB 128 	; 92
		FCB 128 	; 104
		FCB 255 	; 2.8 116
					;----------------------------------------------
LC1B9: 	FCB 52 		; If filtered RPM/12.5 = > 65O RPM then KNOCK is on
					; Table value = Arg * (256/12.5)
LC1BA: 	FCB 2 		; If filtered MPH = > 2 than Then enble KNOCK
LC1BB: 	FCB 114 	; 25.3 deg Max allowable KNOCK Retard when not in WOT
					; Table value = Arg * (256/45)
LC1BC: 	FCB 142 	; If COOLANT < 66.5C THEN DISABLE KNOCK
					; Table value = (Arg+40) * (256/192)
LC1BD: 	FCB 53 		; If delta COOL since start up G.T. arg ENABLE KNOCK
					; Table value = Arg * (256/192)
					;----------------------------------------------
					; KNOCK ATACK RATE vs RPM
					;
					;
					; TBL = ATTACK RATE IN (Deg/msec) /0.0225
					;----------------------------------------------
		ORG $C1BE 	; Deg RPM
					;----------------------------------
LC1BE: 	FCB 7 		; 0.158 400
		FCB 8 		; 0.180 1200
		FCB 13 		; 0.293 2000
		FCB 16 		; 0.360 3200
		FCB 16 		; 0.360 4800
					;----------------------------------------------
					;----------------------------------------------
					; KNOCK PCT. RECOVERY RATE vs RPM
					;
					; Table values attack rate in PCT.
					;
					; RECOVERY/SEC * 256/500
					;----------------------------------------------
		ORG $C1C3 	; PCT RPM
					;----------------------------------
LC1C3: 	FCB 20 		; 39.0 400
		FCB 20 		; 39.0 1200
		FCB 26 		; 50.8 2000
		FCB 31 		; 60.5 3200
		FCB 31 		; 60.5 4800
					;----------------------------------------------
					;----------------------------------------------
					; MAX KNOCK RETARD WHEN IN WOT vs RPM
					;
					; Dissassemby of BUA, LINES = 8
					; 09-14-1994, 10:27:25
					;
					; TBL = 5.688 * DEG RETARD
					;----------------------------------------------
		ORG $C1C8 			; DEG RETARD RPM
					;----------------------------------
LC1C8: 	FCB 108 	; 19 800
LC1C9: 	FCB 77 		; 14 1600
LC1CA: 	FCB 65 		; 11 2400
LC1CB: 	FCB 77 		; 14 3200
LC1CC: 	FCB 51 		; 9 4000
LC1CD: 	FCB 34 		; 6 4800
LC1CE: 	FCB 34 		; 6 5600
LC1CF: 	FCB 34 		; 6 6400
					;----------------------------------------------
		;***********************************************
		;* >>> END OF IGNITION TABLES <<<
		;*
		;***********************************************
		;***********************************************
		;* >>>DIAGNOSTIC PARAMS <<<
		;*
		;* BUA, TYPE 32, (MY86)
		;* General Params.
		;***********************************************
		ORG $C1D0 	;
					;-----------------------------------
LC1D0: 	FCB $F7 	; 1111 0111, Mask For Mal Funct flg 1
					; b 0 Disables ERR Recognition
					;
					; b 0 = ERR Code 23 MAT Sensor low
					; b 1 = ERR Code 22 TPS low
					; b 2 = ERR Code 21 TPS High
					; b 3 = ERR Code 16 Not used
					;
					; b 4 = ERR Code 15 Cool Sensor Low Temp
					; b 5 = ERR Code 14 Cool Sensor Hi Temp
					; b 6 = ERR Code 13 Oxy Sensor
					; b 7 = ERR Code 12 No Ref pulse (Dist)
					;------------------------------------
LC1D1: 	FCB $CE 	; 1100 1110, Mask for ERR flag 2
					;
					; b 0 = ERR Code 35 M/A
					; b 1 = ERR Code 34 MAF Sensor low
					; b 2 = ERR Code 33 MAF Sensor high
					; b 3 = ERR Code 32 EGR Diag
					;
					; b 4 = ERR Code 31
					; b 5 = ERR Code 26
					; b 6 = ERR Code 25 MAT Sensor High
					; b 7 = ERR Code 24 VSS
					;------------------------------------
LC1D2: 	FCB $FF 	; 1111 1111, Mask For ERR flag 3
					;
					; b 0 = ERR Code 51 Prom Error
					; b 1 = ERR Code 46 VATS Fail
					; b 2 = ERR Code 45 o2 Sensor Rich
					; b 3 = ERR Code 44 o2 Sensor Lean
					;
					; b 4 = ERR Code 43 KNOCK Fail
					; b 5 = ERR Code 42 SPK Monitor error
					; b 6 = ERR Code 41 CYL Select error
					; b 7 = ERR Code 36 Burn off Diag.
					;------------------------------------
LC1D3: 	FCB $E0 	; 1110 0000, Mask for ERR flg 4
					;
					; b 0 = ERR Code 63
					; b 1 = ERR Code 62
					; b 2 = ERR Code 61
					; b 3 = ERR Code 56
					;
					; b 4 = ERR Code 55 ADU Error
					; b 5 = ERR Code 54 Fuel pump Voltage
					; b 6 = ERR Code 53 Over voltage
					; b 7 = ERR Code 52 Missing Cal Pack
					;------------------------------------
LC1D4: 	FCB $00 	; 0000 0000, Mask for ERR flg 5
					;
					; b 0 = ERR Code
					; b 1 = ERR Code
					; b 2 = ERR Code
					; b 3 = ERR Code
					;
					; b 4 = ERR Code
					; b 5 = ERR Code
					; b 6 = ERR Code
					; b 7 = ERR Code
					;--------------------------------------
					;------------------------------------
LC1D5: 	FCB 50 		; 50, SUCESSIVE PWR UP'S W/O ERROR
					;
LC1D6: 	FCB 10 		; 1 Sec ERR funct LOGGING FILTER CONSTANT #1
					; Num + Arg * 10
					;
LC1D7: 	FCB 50 		; 5 Sec ERR funct LOGGING FILTER CONSTANT #2
					; Num = Arg * 10
					;
LC1D8: 	FCB 100 	; 10 Sec ERR funct LOGGING FILTER CONSTANT #3
					; Num = Arg * 10
					;
LC1D9: 	FCB 120 	; 12 Sec ERR funct LOGGING FILTER CONSTANT #4
					; Num = Arg * 10
					;
LC1DA: 	FCB 0 		; Ck ENG LAMP BULB TST TIME
					;---------------------------------
					;---------------------------------------------
					; ERR # 13 Params
					; >> o2 sensor <<
					;---------------------------------------------
		ORG $C1DB
LC1DB: 	FCB 146 	; Coolant lo limit. >= 70C, (157f)
					; CALIB = (deg c - 40) * (256/192)
LC1DC: 	FCB 60 		; If eng run time < 30 Sec, disable ERR 13
					; CALIB = sec / 2
LC1DD: 	FCB 79 		; If o2 volts <= 350 Mvdc, ERR 13
					; CALIB = vdc * 226
LC1DE: 	FCB 124 	; If o2 volts > 548 Mvdc, ERR 13
					; CALIB = vdc * 226
LC1DF: 	FCB 13 		; 5% TPS Min for ERR 13
					; CALIB = TPS * 2.56
LC1E0: 	FCB 30 		; 15 Sec TPS Min time limit for ERR 13
					; CALIB = Sec / 2
					;--------------------------------------------------------
					; ERR # 14 Params
					; >> Cool sensor Hi <<
					;--------------------------------------------------------
		ORG $C1E1
LC1E1: 	FCB 227 	; If cool 130c, (266f) < then Disable #14
					; CALIB = (deg c +40) * 256/192
					;--------------------------------------------------------
					; ERR # 14/15 Params
					; >> Cool sensor Hi/Lo <<
					;--------------------------------------------------------
		ORG $C1E2
LC1E2: 	FCB 135 	; 61c, (142f), Cool Temp for default #14/15
					; CALIB = (deg c +40) * (256/192)
LC1E3: FDB 0000 	; If Eng run 0 Sec Disable #15
					;--------------------------------------------------------
					; ERR # 15 Params
					; >> Cool sensor Low <<
					;--------------------------------------------------------
		ORG $C1E5
LC1E5: 	FCB 250 	; IF TPS A/D <= disable ERR #15
					; TBL4 ADCOOL4K
					;--------------------------------------------------------
					; ERR #21 Params
					; >> TPS sensor Hi <<
					;--------------------------------------------------------
		ORG $C1E6
LC1E6: 	FCB 246 	; IF TPS A/D > set ERR #21
LC1E7: 	FCB 128 	; If TPS <= 50% disable #21
LC1E8: 	FCB 20 		; 20 Sec's Time req for ERR #21
LC1E9: 	FCB 12 		; If Air Flow >= 12 gm?sec, disable ERR #21
					;--------------------------------------------------------
					; ERR # 21/22 Params
					; >> TPS sensor Hi/Lo <<
					;--------------------------------------------------------
		ORG $C1EA
LC1EA: 	FCB 48 		; Use as default A/D value for
					; TPS if ERR #21
					;--------------------------------------------------------
					; ERR # 22 Params
					; >> TPS sensor Lo <<
					;--------------------------------------------------------
		ORG $C1EB
LC1EB: 	FCB 12 		; If TPS A/D >= 12 then disable ERR #22
					;--------------------------------------------------------
					; ERR # 23 Params
					; >> MAT sensor lo <<
					;--------------------------------------------------------
		ORG $C1EC
LC1EC: 	FCB 4 		; If MAT < Enable ERR 23
					; TBL3, A/D MAT INV
LC1ED: 	FCB 120 	: 12 Sec req for ERR #23
					;--------------------------------------------------------
					; ERR # 23/25 Params
					; >> MAT sensor lo/Hi <<
					;--------------------------------------------------------
		ORG $C1EE
LC1EE: 	FDB 0240 	: if Eng Run time <= 120 SEC															
					; disable ERR #23 & ERR #25
LC1F0: 	FCB 1 		; 1 MPH MIN TO ENABLE ERR 23/25
LC1F1: 	FCB 29 		; If ERR #25 Use as Default
					; for MAT, (Deg C)
					;TBL3, ADMATINV
					;--------------------------------------------------------
					; ERR # 24 Params
					; >> Vss Sensor <<
					;--------------------------------------------------------
		ORG $C1F2
LC1F2: 	FCB 3 		; If > 3 MPH then disable ERR 24
LC1F3: 	FCB 40 		; If <= 1000 RPM, Disable ERR #24
					; TBL1, NTRPM
LC1F4: 	FCB 240 	; If RPM > 6000 then disable ERR 24
					; TBL1, NTRPM
LC1F5: 	FCB 5 		; If => 2% TPS then Disable ERR 24
					; CALIB = Arg * 2.56, (1.93%)
LC1F6: 	FCB 26 		; If LV8 >= 26, Disable ERR 24
LC1F7: 	FCB 2 		; 2 Sec's TIMER for ERR 24
					;--------------------------------------------------------
					; ERR # 25 Params
					; >> MAT sensor Hi <<
					;--------------------------------------------------------
		ORG $C1F8
LC1F8: 	FCB 243 	; If MAT > then Enable ERR 25
					; TBL3, ADMATINV
LC1F9: 	FCB 120 	; 12 Sec's req for ERR 25
					; CALIB = Sec's * 10
					;--------------------------------------------------------
					; ERR # 32 Params
					; >> EGR Diag <<
					;--------------------------------------------------------
		ORG $C1FA
LC1FA: 	FCB 94 		; If COOL >= 30.5c, (87f), Skip
					; Strt up ERR #32
					; CALIB = (deg c + 40) * (256/19)
LC1FB: 	FCB 128 	; If LV8 > 128 then Skip #32
LC1FC: 	FCB 173 	; If Cool < 89.8c, (194f),
					; Disable ERR #32
					; CALIB = (deg c + 40) * (256/192)
LC1FD: 	FCB 133 	; If EGR DC < 51.9%, skip ERR #32
					; CALIB = EGR DC * 2.56
LC1FE: 	FCB 77 		; If TPS > 30%than Skip #32
					; CALIB = Arg * 2.56
LC1FF: 	FCB 15 		; If TPS < 5.8%, Skip ERR #32
					; CALIB = Arg * 2.56
LC200: 	FDB 2560 	; Log ERR #32 if tmr > 2560 CNT'S
LC202: 	FCB 1 		; Inc ERR #32 tmr this amt
					; every 100 Msec.
					;--------------------------------------------------
					; ERR # 33 Params
					; >> MAF Sensor Hi <<
					;--------------------------------------------------
		ORG $C203
LC203: 	FCB 38 		; If TPS >= 14.8%, set ERR #34
					; CALIB = Arg * 2.56
LC204: 	FCB 45 		; If Air Flow <= 45 gms/sec
					; disable ERR #33
LC205: 	FCB 96 		; If > 0.6 Sec, Set code #33
					; CALIB + Arg * 160
LC206: 	FCB 8 		; 800 Msec ERR 34 Min time req.
					; CALIB = Arg * 10
LC207: 	FCB 128 	; If RPM > Disable #33
					; RPM/25
LC208: 	FCB 100 	; 10.0 VDC PUMP VDC FOR ERR 33 ENABLE
					;--------------------------------------------------------
					; ERR # 34 Params
					; >> MAF Sensor Lo <<
					;--------------------------------------------------------
LC209: 	FCB 40 		; 6.4 SEC'S, MIN TIME W/O SIGNAL, (IF FM DIG MAF)
					; CALIB = Arg/6.25
LC20A: 	FDB 55 		; IF Analog CTS * 7 or PP2DLT < 55
					; Set ERR 34.
LC20C: 	FCB 2 		; ERR #43, 0.2 Sec Min time req.
					; CALIB + Arg * 10
LC20D: 	FCB 24 		; If RPM > 600, Ck ERR #34
LC20E: 	FCB 16 		; If TPS > 6.25%, Ck ERR #34
					; CALIB = Arg * 2.56,
LC20F: 	FCB 0 		; If LV8 >= 0, Ck ERR #34
LC210: 	FCB 130 	; If LV8 <= 130, Ck ERR #34
LC211: 	FDB 85 		; IF Analog CTS * 7 or
					; PP2DLT <, enab ERR #34
					;--------------------------------------------------------
					; ERR # 33/34 Params
					; >> MAF Sensor Hi/Lo <<
					;--------------------------------------------------------
LC213: 	FCB 167 	; If TPS > 65.2%, then use for MAF Default
					; CALIB = Arg * 2.56
LC214: 	FCB 21 		; Gms/sec Scale factor
					; gms/sec/IAC Count
					; CALIB = Arg * 256
LC215: 	FDB 1024 	; 4 Gms/Sec default air flow offset
					;CALIB = Arg * 256
					;--------------------------------------------
					; DEFAULT AIR FLOW OFFSET PER PCT TPS VS RPM
					; Value = Gms/Sec * 100 pct, (Offset)
					;
					; TBL = Arg * 100
					;--------------------------------------------
		ORG $C217
LC217: 	FCB 8 		; Table Value
					;
					; Gms/Sec RPM
					;------------------------
		FCB 30 		; 0.30 400
		FCB 90 		; 0.90 800
		FCB 110 	; 1.10 1200
		FCB 135 	; 1.35 1600
		FCB 150 	; 1.50 2000
		FCB 175 	; 1.75 2400
		FCB 210 	; 2.10 3200
		FCB 225 	; 2.25 4000
		FCB 230 	; 2.30 4800
					;--------------------------------------------
					;--------------------------------------------------------
					; ERR # 36 Params
					; >> MAF Burn off Diag <<
					;--------------------------------------------------------
LC221: 	FCB 3 		; 0.3 Second Dly prior to Burn off
					; CAL = Arg * 10
LC222: 	FCB 6 		; 6 fails req for ERR #36 If HLM air meter
LC223: 	FCB 95 		; 1.90 VDC, Fail B/O test if HLM A/D GT 1.90 VDC
LC224: 	FCB 20 		; Fail B/O test if A/D < 400 mvdc
					;--------------------------------------------------------
					; ERR # 41 Params
					; >> Cyl Sel error <<
					;--------------------------------------------------------
LC225: 	FCB 0 		; Fuel mode
					; 24 = TBI
					; 16 = 4 Cyl PFI
					; 8 = 6 Cyl PFI
					; 0 = 8 Cyl PFI
					;--------------------------------------------------------
					; ERR # 42 Params
					; >> EST monitor error <<
					;--------------------------------------------------------
LC226: 	FCB 4 		; RPM, tbl1,ntrpm
					;TBL1 NTRPM
LC227: 	FCB 2 		;If SPK toggled, (PA1 cnt's)
					; >= 2 times, prior to eng run set ERR #42
LC228: 	FCB 64 		;1048.6 msec ERR 42 treshold,
					;--------------------------------------------------------
					; ERR # 43 Params
					; >> KNOCK fail <<
					;--------------------------------------------------------
LC229: 	FCB 235 	; If KNOCK low time > 3.67 Sec,Enab ERR #43
					; CALIB = Arg * 64
LC22A: 	FCB 45 		; If ERR 43, then KNOCK retard 7.9 Deg
					; CALIB = Deg * 256/45
LC22B: 	FCB 57 		; Add 10 Deg Sprk Adv for #43 test
					; CALIB = Deg * 256/45
LC22C: 	FCB 173 	; 90C, (194) MIN COOL TO ENABLE ERR 43
					; CALIB = (Deg C + 40) * 256/192
LC22D: 	FCB 170 	; If LV < 170, Disable ERR 43
					;--------------------------------------------------------
					; ERR # 44 Params
					; >> o2 Sensor lean <<
					;--------------------------------------------------------
LC22E: 	FCB 45 		; If 02 Sensor >= 0.19v, then disable #44
					; CALIB = Arg * 226
LC22F: 	FCB 20 		; 20 Seconds to set #44
					;--------------------------------------------------------
					; ERR # 45 Params
					; >> o2 Sensor rich <<
					;--------------------------------------------------------
LC230: 	FCB 158 	; If o2 Sensor >= 0.699v, disable ERR #45
					; CALIB = Arg * 226
LC231: 	FCB 50 		; 50 Seconds to set ERR
LC232: 	FCB 5 		; If TPS > 1.95%, enab ERR #45
					; CALIB = Arg * 2.56
LC233: 	FCB 00 		; If TPS < 0%, enab ERR #45
					; CALIB = TPS% * 2.56
					;--------------------------------------------------------
					; ERR # 54 Params
					; >> Fuel Pump voltage <<
					; CALIB = Arg * 10
					;--------------------------------------------------------
LC234: 	FCB 15 		; 1.5 SEC'S MIN FOR ERR 54
LC235: 	FCB 20 		; 2.0 VDC MIN PUMP VOLTS FOR ERR #54
LC236: 	FCB 20 		; 2.O VDC MIN PUMP VOLTS FOR ERR 54
LC237: 	FCB 20 		; 2.O VDC MIN IGN VOLTAGE FOR ERR 54
					;----------------------------------------------------------
		;*******************************************************
		;* >>> EGR QUAILIFICATIONS <<<
		;*
		;* BUA, ECM TYPE $32
		;*
		;*******************************************************
		ORG $C238
LC238: 	FCB 5 		; Enable EGR if TPS > 1.95%, (If EGR DC <> 0)
					; CALIB = Arg * 2.56
LC239: 	FCB 8 		; Enable EGR if TPS > 3.13%, (if EGR DC = 0)
					; CALIB = Arg * 2.56
					;
LC23A: 	FCB 30 		;If MAT < then disable EGR
					;-----------------------------------------------------
					;-----------------------------------------------------
					; EGR Duty CYC vs LV8 & RPM
					;
					; Tbl Val(DC) = CALIB * (256/100)
					;-----------------------------------------------------
		ORG $C23B
LC23B: 	FCB 32 		; Min Min Val for RPM
		FCB 32 		; LV8 Min
		FCB 9 		; 9 Lines in table
					;----------------------------------
					; 800 RPM
					;----------------------------------
		FCB 255 	; LV 32, DC =99.6%
		FCB 255 	; LV 48
		FCB 255 	; LV 64
		FCB 255 	; LV 80
		FCB 255 	; LV 96
		FCB 255 	; LV 112
		FCB 255 	; LV 128
		FCB 255 	; LV 144
		FCB 255 	; LV 160
					;--------------------------------
					; 1200 RPM
					;---------------------------------
		FCB 255 	;LV 32
		FCB 255 	;LV 48
		FCB 255 	;LV 64
		FCB 255 	;LV 80
		FCB 255 	;LV 96
		FCB 255 	;LV 112
		FCB 255 	;LV 128
		FCB 255 	;LV 144
		FCB 255 	;LV 160
					;---------------------------------
					; 1600 RPM
					;---------------------------------
		FCB 255 	;LV 32
		FCB 255 	;LV 48
		FCB 255 	;LV 64
		FCB 255 	;LV 80
		FCB 255 	;LV 96
		FCB 255 	;LV 112
		FCB 255 	;LV 128
		FCB 255 	;LV 144
		FCB 255 	;LV 160
					;---------------------------------
					; 2000 RPM
					;---------------------------------
		FCB 255 	;LV 32
		FCB 255 	;LV 48
		FCB 255 	;LV 64
		FCB 255 	;LV 80
		FCB 255 	;LV 96
		FCB 255 	;LV 112
		FCB 255 	;LV 128
		FCB 255 	;LV 144
		FCB 255 	;LV 160
					;------------------------------------------------------
					;------------------------------------------------------
					; EGR DUTY CYCLE MULTIPLIER (0-2) vs COOLANT
					; Gain vs Coolant Temp.
					;
					; TABLE VAL = Mult * 128
					;------------------------------------------------------
		ORG $C262
					;------------------------------------------------------
					;
					; MULT COOL
					;------------------------------------------------------
LC262: 	FCB 0 		; 0 8
		FCB 0 		; 0 20
		FCB 0 		; 0 32
		FCB 0 		; 0 44
		FCB 128 	; 1 56
		FCB 128 	; 1 68
		FCB 128 	; 1 80
					;------------------------------------------------------
		;*******************************************************
		;*******************************************************
		;* >>>> Can Purge Param's <<<<
		;*
		;*******************************************************
		ORG $C269
LC269: 	FCB 80 		; If Eng time > 80 sec then disable CCP
LC26A: 	FCB 147 	; Disable CCP if Cool < 70.3c, (158.4f)
					; cal = (Deg c + 40) * 256/192
					;----------------------
					; CCP off to on Params
					;----------------------
LC26B: 	FCB 16 		; Enable CCP if >= 5 MPH
					; CALIB = Arg * (16/5)
LC26C: 	FCB 10 		; If TPS > = 3.9% then enable CCP
					; CALIB = Arg * 2.56
LC26D: 	FCB 0 		; If Air flow > 0 then enable CCP
					;----------------------
					; CCP on to off params
					;
					;----------------------
LC26E: 	FCB 10 		; If < 3.12 MPH Disable CCP
					; CALIB = Arg * (16/5)
LC26F: 	FCB 5 		; If TPS < 1.95% then Disable purge
					; CALIB = Arg * 2.56
LC270: 	FCB 0 		; If Flow > 0 don't disable CCP
					; Gms/Sec
LC271: 	FCB 32 		; Filter Coef for CCP D.C.
					;------------------------------------------------------
					;------------------------------------------------------
					; CCP Duty Cycle vs MAF, Gms/Sec
					;
					; Table Value = %DC * 2.56
					;------------------------------------------------------
		ORG $C272
LC272: 	FCB 8 		; 9 Lines in tbl
					;------------------------------------------------------
					;
					; %DC AIR FLOW, Gms/Sec
					;------------------------------------------------------
		FCB 16 		; 6 0
		FCB 32 		; 13 4
		FCB 48 		; 19 8
		FCB 80 		; 31 12
		FCB 160 	; 63 16
		FCB 208 	; 81 20
		FCB 255 	; 100 24
		FCB 255 	; 100 28
		FCB 255 	; 100 32
					;------------------------------------------------------
					;------------------------------------------------------
					; CCP Duty Cycle Gain Vs Load Value
					;
					; Tble Value = Factor * 128
					;------------------------------------------------------
;		ORG $C27B
;LC27B:	FCB 8 		; 9 LINE TBL
					;--------------------------------------------------
					;------------------------------------------------------
					;
					; GAIN LD VAL
					;------------------------------------------------------
LC27C: 	FCB 128 	; 1 32
		FCB 128 	; 1 64
		FCB 128 	; 1 96
		FCB 128 	; 1 128
		FCB 128 	; 1 160
		FCB 128 	; 1 192
		FCB 128 	; 1 224
		FCB 128 	; 1 256
					;------------------------------------------------------
		;*******************************************************
		;*******************************************************
		;* TRANSMISSION CONTROL DATA & PARAM's
		;* ECM TYPE $32
		;* 700R4
		;*******************************************************
		ORG $C284
LC284: 	FCB 255 	; SPEED FILTER COEF, (0.996)
					; CALIB = ARG * 256
					;
LC285: 	FCB 120 	; 50C, (122F), COOL TRESH FOR LOCK/UP SHFT ENAB
					; CALIB = (ARG+40) * 256/192
					;
LC286: 	FCB 41 		; 1st GEAR DIRECT N/V RATIO
LC287: 	FCB 35 		; 1st GEAR O.D. N/V RATIO
					;
LC288: 	FCB 30 		; MIN 4th GEAR N/V RATIO
LC289: 	FCB 35 		; MAX 4th GEAR N/V RATIO
					;
LC28A: 	FCB 8 		; 0.8 SEC, 4TH GEAR N/V TIMER
					; CALIB = ARG * 10, (8/10 SEC)
					;
LC28B: 	FCB 255 	; LOOK AHEAD 1ST GEAR TPS, (100%)
					;
LC28C: 	FCB 0 		; UP SHIFT/LOCK DELAY
					; CALIB = ARG * 10, (SEC)
					;
LC28D: 	FCB 0 		; 1ST GEAR DELAY TIMER
					; CALIB = ARG * 10, (SEC)
					;
LC28E: 	FCB 3 		; HYST FOR %TCC
					; CALIB = ARG * 2.56
					;
LC28F: 	FCB 32 		; RD SPEED COAST, (32 MPH)
					;
LC290: 	FCB 8 		; LOW MPH COAST TPS = 3.125%
					; CALIB = ARG * 2.56, (TPS %)
					;
LC291: 	FCB 0 		; HI MPH COAST TPS
					; CALIB = ARG * 10, (TPS %)
					;
LC292: 	FCB 255 	; KICK DN PREVENT (SHFTER SW)
					; CALIB = ARG / 25, (6375 RPM)
					;
LC293: 	FCB 128 	; KICK DN/UNLOCK TPS
					; CALIB = ARG/25, (3200 RPM)
					;
LC294: 	FCB 128 	; KICK DN/UNLOCK TPS PREV'NT RPM
					; CALIB = ARG/25, (3200 RPM)
					;
LC295: 	FCB 60 		; MIN VSS, (60 MPH) FOR FORCE UPSHIFT
					;
LC296: 	FCB 0 		; 0 SEC'S, HI TO LO GEAR LOCK
					; CALIB = ARG * 10, (SEC'S)
					;
LC297: 	FCB 26 		; BIAS TO LOCK ENAB THRESH, (26 MPH)
					;
LC298: 	FCB 255 	; NO PASSBY L/U IF TPS < 99.6%
					; CALIB = ARG * 2.56
					;
LC299: 	FCB 28 		; NO PASS BY L/U IF =< 28 MPH
					;
					; PASS BY RPH HYST PAIR
					;
LC29A: 	FCB 32 		; NO PASS BY L/U IF > 800 RPM
LC29B: 	FCB 255 	; NO PASS BY L/U IF RPM < 6375
					; CAL = ARG/25
					;
LC29C: 	FDB 65535 	; 819 SEC DLY PRIOR TO PASSBY/LOCKUP
					; CALIB = ARG * 80
					;
LC29E: 	FDB 0240 	; KEEP TCC LK'ED UP FOR PASSBY 30 SEC
					; CAL = ARG * 80
					;------------------------------------------------------
					; MANUAL - 1ST GEAR DNSHFT
					; AUTO TRANS - NON 4TH GEAR UNLOCK
					;
					; % OF FULL LOAD * 2.56
					; 12 ENTRIES
					;------------------------------------------------------
		ORG $C2A0
LC2A0: 	FCB 14 		; 14 MPH, UNLOCK 1ST(MAN), LO GR (AUTO)
					;------------------------------------------------------
					;
					; %FULL LD MPH
					;------------------------------------------------------
LC2A1: 	FCB 59 		; 23 12
		FCB 72 		; 28 20
		FCB 92 		; 35 28
		FCB 128 	; 50 36
		FCB 166 	; 65 44
		FCB 218 	; 85 52
		FCB 236 	; 92 60
		FCB 255 	; 100 68
		FCB 255 	; 100 76
		FCB 255 	; 100 84
		FCB 255 	; 100 92
		FCB 255 	; 100 100
					;------------------------------------------------------
					;------------------------------------------------------
					; Manual - 1st gear Up shift
					; Auto Trans - Non 4th Gear lock
					;
					; % Of Full Load * 2.56
					;------------------------------------------------------
		ORG $C2AD
LC2AD: 	FCB 15 		; UNLOCK AT 15 MPH 1ST MAN & LOW GRS MAN
					;------------------------------------------------------
					;
					; %FULL LD MPH
					;------------------------------------------------------
LC2AE: 	FCB 51 		; 20 12
		FCB 59 		; 23 20
		FCB 67 		; 26 28
		FCB 87 		; 34 36
		FCB 115 	; 45 44
		FCB 166 	; 65 52
		FCB 210 	; 82 60
		FCB 210 	; 82 68
		FCB 210 	; 82 76
		FCB 210 	; 82 84
		FCB 210 	; 82 92
		FCB 210 	; 82 100
					;--------------------------------------------------
LC2BA: 	FCB 14 		; Unlock 14 MPH Hi gr Auto & Non 1st Man
					;------------------------------------------------------
					; MANUAL - 1ST GEAR DNSHIFT
					; AUTO TRANS - 4TH GEAR UNLOCK
					;
					; % OF FULL LOAD * 2.56
					;------------------------------------------------------
		ORG $C2BB
					;------------------------------------------------------
					;
					; %FULL LD MPH
					;------------------------------------------------------
LC2BB: 	FCB 255 	; 100 12
		FCB 113 	; 44 20
		FCB 141 	; 55 28
		FCB 172 	; 67 36
		FCB 200 	; 78 44
		FCB 225 	; 88 52
		FCB 255 	; 100 60
		FCB 255 	; 100 68
		FCB 255 	; 100 76
		FCB 255 	; 100 84
		FCB 255 	; 100 92
		FCB 255 	; 100 100
					;---------------------------------------------------
					;-----------------------------------------------------
					; MANUAL - 4TH GEAR UP SHIFT
					; AUTO TRANS - 4TH GEAR LOCK
					;
					; TBLE VAL = PCT FULL LD * (256/100)
					;-----------------------------------------------------
		ORG $C2C7
LC2C7: 	FCB 15 		; 15 MPH MIN LK HI GEAR AUTO & NON 1ST MAN
					;------------------------------------------------------
					;
					; %TPS MPH
					;------------------------------------------------------
LC2C8: 	FCB 00 		; 0 12
		FCB 77 		; 30 20
		FCB 105 	; 41 28
		FCB 136 	; 53 36
		FCB 174 	; 68 44
		FCB 215 	; 84 52
		FCB 255 	;100 60
		FCB 255 	;100 68
		FCB 255 	;100 76
		FCB 255 	;100 84
		FCB 255 	;100 92
		FCB 255 	;100 100
		;******************************************************
		;******************************************************
		;* Check CARS LIGHT TABLE & CALIB'S
		;*
		;******************************************************
		ORG $C2D4
LC2D4: 	FCB 120 	; 50C, (122F) COOL TEMP FOR ENG LITE ENABLE
					; CALIB = (TEMP C + 40) * (256/192)
LC2D5: 	FCB 40 		; 40 MPH LIMIT FOR LITE ENAB
LC2D6: 	FCB 00 		; 0% TPS MIN LIMIT FOR LITE
LC2D7: 	FCB 00 		; 0 LV8 MIN FOR LITE ON
					;
					; 2 line tbl
					;
LC2D8: 	FCB 56 		; 1400 RPM/25 Thres for lite
LC2D9: 	FCB 208 	; 5200 RPM/25, force lite on
					;-----------------------------------------------------
					; SHIFT Lite on Table, TPS VS RPM
					;
					;
					; % TPS * (256/100)
					;-----------------------------------------------------
;		EQU $02DA
					;-----------------------------------------------------
					;
					; %TPS RPM
					;-----------------------------------------------------
LC2DA: 	FCB 13 		; 5 800
		FCB 46 		; 18 1600
		FCB 118 	; 46 2400
		FCB 192 	; 75 3200
		FCB 192 	; 75 4000
		FCB 255 	;100 4800
		FCB 255 	;100 5600
					;-----------------------------------------------------
					;
					; 2 line tbl
					;
LC2E1: 	FCB 52 		; Lite off, 625 RPM/25
LC2E2: 	FCB 204 	; Force lite Off, 5100 RPM/25
					;-----------------------------------------------------
					; SHIFT Lite Off Table, %TPS vs RPM
					;
					; Table Val = % TPS * (256/100)
					;-----------------------------------------------------
;		EQU $02E3
					;-----------------------------------------------------
					;
					; %TPS RPM
					;-----------------------------------------------------
LC2E3: 	FCB 18 		; 7 800
		FCB 51 		; 20 1600
		FCB 123 	; 48 2400
		FCB 197 	; 77 3200
		FCB 197 	; 77 4000
		FCB 255 	;100 4800
		FCB 255 	;100 5600
					;-----------------------------------------------------
LC2EA: 	FCB 2 		; E Lite on delay Mult
					; CALIB = Arg
					;-----------------------------------------------------
					; Eng Lite on Modifier Delay vs TPS
					;
					;
					; Tbl Val = Sec * 10/delay mult
					;----------------------------------------------------
					;
					; SEC'S %TPS
					;----------------------------------------------------
LC2EB: 	FCB 150 	; 30 0.0
		FCB 150 	; 30 12.5
		FCB 150 	; 30 25.0
		FCB 150 	; 30 37.5
		FCB 150 	; 30 50.0
		;*******************************************************
		;********************************************************
		;* >>> Cooling Fan Tables & Params <<<
		;*
		;* TYPE $32 ECM
		;********************************************************
LC2F0: 	FCB 35 		; Fan D.C. = if MPH > 35 & A/C Press Hi
LC2F1: 	FCB 11 		; Use A/C If > 11 MPH
LC2F2: 	FCB 75 		; 15 Sec, Min Fan On Time
					; Cal = Arg * 5
					;
					;
					;
LC2F3: 	FCB 255 	; Fan 99.6% D.C. If A/C Pres ^ & MPH <
					; Cal = Arg * 2.56
					;
					; FAN 1 ON 107c
					; Cal = (deg c + 40) * (256/192)
					;
LC2F4: 	FCB 197 	; Fan D.C. = tbl if Cool >= 107c, (226f)
					; & A/C on & FAN OFF
					;
					; FAN 2 OFF, 105c
					;
LC2F5: 	FCB 193 	; Fan D.C. = tbl if Cool >= 104.7c, (220.5f)
					; & A/C on & FAN on
					;
					; FAN 2 ON, 115c
					; Cal = (deg c + 40) * (256/192)
					;
LC2F6: 	FCB 207 	; Fan D.C. = tbl if Cool >= 115.2c, (239.5f)
					; & A/C off & FAN off
					;
					; FAN 1 OFF 110c
					;
LC2F7: 	FCB 200 	; Fan D.C. = tbl if Cool >= 110c, (230f)
					; A/C off & FAN on
					; Cal = (Arg + 40) * 256/192, (Deg C)
					;---------------------------------------------
					; Fan Duty Cycle D.C. vs Coolant Temp
					;
					; Tbl Val = %D.C. * (256/100)
					;---------------------------------------------
LC2F8: 	FCB 255 	;100% DC, 80 Deg C
		FCB 255 	; 92
		FCB 255 	; 104
		FCB 255 	; 116
		FCB 255 	; 128
		FCB 255 	; 140
		FCB 255 	; 152 C
		;***************************************************
		;***************************************************
		;* >>> Air Injection Managment Tables & Param's <<<
		;*
		;***************************************************
LC2FF: 	FCB 00 		; Max Air to ports if in Pwr Enrich
					; If In WOT >= then divert Air, (Sec)
					;
LC300: 	FCB 10 		; Divert if If RPM Cont > ____ > 10 SEC
					; Cal = Arg * 10, (1 Sec)
					;
LC301: 	FCB 170 	; If Filtered o2 > 0.752 vdc for ___
					; Then divert
					; Cal = Arg * 226
LC302: 	FCB 56 		; If Filtered o2 0.247 vdc & in closed loop
					; > Then divert
					; Cal = Arg * 226
					;
LC303: 	FDB 200 		; 20 sec Rich or Lean o2 Divert timer
					; Cal = Arg * 10
					;
LC305: 	FCB 160 	; Divert if cont > 4000 RPM & time >
					;
LC306: 	FCB 10 		; Enable air to conv If > 1 SEC since
					; last neg delta LV8 > is <
					;
LC307: 	FCB 25 		; If LV8 < 25 then Divert
LC308: 	FCB 100 	; If LV8 > 100 & MPH > Divert Air
LC309: 	FCB 60 		; If > 60 MPH & LV8 > then divert
					;
LC30A: 	FCB 64 		; Enable air to conv if in open lp
					; & 100 Msec Drop in LV8 < 64
					;
LC30B: 	FCB 73 		; If Cool < 14c, (58.5f) then divert
					; Cal = (Arg + 40) * 256/192
LC30C: 	FCB 254 	; If in Open Lp => 25.4 Sec after
					; closed loop then Divert
					; Cal = Arg * 10
		;***************************************************
					;--------------------------------------------------
					; >>> Delta TPS Accel Enrichment <<<
					; Tables & params
					;--------------------------------------------------
LC30D: 	FCB 32 		; 0.125, Trans TPS filter coef.
					; Cal = Arg * 256
					;
LC30E: 	FCB 10 		; 3.9% Min Diff TPS for Pwr Enrichment
					; Cal = Arg * 2.56
					;
LC30F: 	FCB 10 		; 0 Async Accel Enrich if Neg Diff TPS > 3.9%
					; Cal = msec * 2.56
					;
LC310: 	FDB 0111 	; 1.69 msec Min Async Pulse Width
					; Cal = msec * 65.536
					;
LC312: 	FDB 0364 	; 5.5 msec Max Async Pulse Width
					; Cal = msec * 65.536, (5.55 Msec)
LC314: 	FCB 2 		; Cal of Injects in fuel limiting
					;--------------------------------------------------
					; ACCEL ENRICHMENT FACTOR % BPW INJ (0-4)
					;
					;
					; TBL = FACTOR * 64
					;--------------------------------------------------
LC315: 	FCB 32 		; 0.5, 1 A.E. Pulses
		FCB 32 		; 0.5 2
		FCB 32 		; 0.5 3
		FCB 32 		; 0.5 4
		FCB 32 		; 0.5 5
		FCB 32 		; 0.5 6
		FCB 32 		; 0.5 7
		FCB 32 		; 0.5 8
					;--------------------------------------------------
					;-------------------------------------
					; Number of Pulses Coolant
					;
					;
					; TBL = Number of Pulses
					;--------------------------------------
LC31D: 	FCB 8 		; Num of Entries in table
					;--------------------------------------------------
					;
					; PULSES Deg c COOL
					;--------------------------------------------------
		FCB 12 		; 12 -40
		FCB 12 		; 12 -16
		FCB 12 		; 12 8
		FCB 12 		; 12 32
		FCB 12 		; 12 56
		FCB 12 		; 12 80
		FCB 12 		; 12 104
		FCB 12 		; 12 128
		FCB 12 		; 12 152
					;--------------------------------------------------
					;-------------------------------------
					; Async Factor vs COOLANT
					; 8 LINE TBL, BUA TYOE $32 ECM
					;
					; TBL = Factor * 128
					;--------------------------------------
LC327: 	FCB 8 		; NUM OF ENTRIES IN TABLE
					;
					; MULT Deg C
					;-------------------------
		FCB 128 	; 1.00, -40
		FCB 128 	; 1.00 -16
		FCB 128 	; 1.00 8
		FCB 128 	; 1.00 32
		FCB 128 	; 1.00 56
		FCB 96 		; 0.75 80
		FCB 64 		; 0.50 104
		FCB 64 		; 0.50 128
		FCB 64 		; 0.50 152 C
					;--------------------------------------------------
					;--------------------------------------------------
					; FUEL Limiting Factor vs Coolant
					;
					; Bits 0 - 3 Are after Next Inject
					; Bits 4 - 7 Trigger To Next inject
					;--------------------------------------------------
LC331: 	FCB 217 	; 217 -40 Deg C
		FCB 217 	; 217 -16
		FCB 217 	; 217 8
		FCB 217 	; 217 32
		FCB 217 	; 217 56
		FCB 169 	; 169 80
		FCB 169 	; 169 104
		FCB 169 	; 169 128 C
					;--------------------------------------------------
					;-------------------------------------
					; Differental Accel Enrichment
					; Tables & param's
					;--------------------------------------
LC339: 	FCB 1 		; Loops between Filtering of Ld Vals, (sec)
LC33A: 	FCB 32 		; Transient Ld Val Filter coef, (0.125)
LC33B: 	FCB 128 	; Init Ld Val Filter constant
LC33C: 	FCB 20 		; Min Delta LV8 for Acel enrich
					;------------------------------------------------------
					; LV8 AE FACTOR vs DELTA LV8
					;
					; DELTA LV8 ABOVE MINIMUM (LC33C)
					;
					; TBL = Mult * 128
					;------------------------------------------------------
LC33D: 	FCB 4 		; Num of lines in table -1
					;------------------------------------------------------
					;
					; MULT Diff ld VAL
					;------------------------------------------------------
		FCB 32 		; 0.25 0
		FCB 32 		; 0.25 64
		FCB 40 		; 0.31 128
		FCB 52 		; 0.40 192
		FCB 52 		; 0.40 256
					;------------------------------------------------------
					;------------------------------------------------------
					; Coolant Multilpier vs Coolant Temp.
					;
					;
					; Tbl Value = Mult * 128
					;------------------------------------------------------
LC343: 	FCB 8 		; Num of lines in table -1
					;------------------------------------------------------
					;
					; MULT COOL Deg C
					;------------------------------------------------------
		FCB 45 		; 0.35 -40 Deg C
		FCB 45 		; 0.35 -16
		FCB 45 		; 0.35 8
		FCB 45 		; 0.35 32
		FCB 19 		; 0.14 56
		FCB 6 		; 0.04 80
		FCB 0 		; 0,00 104
		FCB 0 		; 0.00 128
		FCB 0 		; 0,00 152
					;------------------------------------------------------
					;------------------------------------------------------
					; ACCEL ENRICH DECAY FACTOR VS COOLANT TEMP.
					;
					; TABLE VALUE + DIFF LD VAL PCT'AGE PER INJECT
					;
					; TBL VAL = %CHG * (256/100)
					;------------------------------------------------------
LC34D: 	FCB 8 		; Num of lines in table -1
					;------------------------------------------------------
					;
					; %CHG COOL Deg C
					;------------------------------------------------------
		FCB 64 		; 25 -40 Deg C
		FCB 64 		; 25 -16
		FCB 44 		; 17 8
		FCB 44 		; 17 32
		FCB 64 		; 25 56
		FCB 102 	; 40 80
		FCB 128 	; 50 104
		FCB 153 	; 60 128
		FCB 153 	; 60 152
		;*******************************************************
		;*******************************************************
		;* >>> Cranking Tables & Param's <<<
		;*
		;*******************************************************
LC357: 	FDB $0000 	; DRP VAL If Crank RPM < then use this
LC359: 	FCB 128 	; Crank RPM Coef, (0.5)
					; CALIB = Arg * 256
LC35A: 	FCB 0 		; IF FF CNTR > 0 & enr running then
					; skip crank fuel logic
					;------------------------------------------------------
					; CRANK FUEL MULTIPLIER vs RPM
					;
					;
					; TBL = Mult * 256
					;------------------------------------------------------
		ORG $C35B
					;------------------------------------------------------
					;
					; MULT RPM
					;------------------------------------------------------
LC35B: 	FCB 0 		; 0 0
		FCB 0 		; 0 50
		FCB 0 		; 0 100
		FCB 0 		; 0 150
		FCB 0 		; 0 200
		FCB 0 		; 0 250
		FCB 0 		; 0 300
		FCB 0 		; 0 350
		FCB 0 		; 0 400
		FCB 0 		; 0 450
		FCB 0 		; 0 500
		FCB 0 		; 0 550
		FCB 0 		; 0 600
		FCB 0 		; 0 650
		FCB 0 		; 0 700
		FCB 0 		; 0 750
		FCB 0 		; 0 800
					;------------------------------------------------------
					;------------------------------------------------------
					; CRANK FUEL vs COOLANT Temp.
					;
					;
					; Table = Msec * (65.536 * 256) / LC36C
					;------------------------------------------------------
LC36C: FDB 6554 	; Scale Factor for Max Crk P.W.
					;------------------------------------------------------
					;
					; MSEC PW COOL Deg c
					;------------------------------------------------------
LC36E: 	FCB 230 	; 89.8 -40
		FCB 205 	; 80.0 -28
		FCB 77 		; 30.1 -16
		FCB 51 		; 19.9 -4
		FCB 33 		; 12.9 8
		FCB 23 		; 8.9 20
		FCB 20 		; 7.8 32
		FCB 19 		; 7.4 44
		FCB 18 		; 7.0 56
		FCB 17 		; 6.6 68
		FCB 14 		; 5.5 80
		FCB 12 		; 4.7 92
		FCB 16 		; 6.3 104
		FCB 20 		; 7.8 115
					;------------------------------------------------------
LC37C: FDB 0032 	; 32 Sec Min eng run time to reset
LC37E: 	FCB 197 	; Min TPS 76.9% limit to reset pls ctr
					;------------------------------------------------------
					;
					; CRANK FUEL PW MULT vs DRP's
					;
					; Tbl = Mult * 256
					;------------------------------------------------------
		ORG $C37F
					;------------------------------------------------------
					;
					; MULT DRP'S
					;------------------------------------------------------
LC37F: 	FCB 255 	; 1 0
		FCB 255 	; 1 8
		FCB 255 	; 1 16
		FCB 255 	; 1 24
		FCB 255 	; 1 32
		FCB 255 	; 1 40
		FCB 255 	; 1 48
		FCB 255 	; 1 56
		FCB 255 	; 1 64
		FCB 255 	; 1 72
		FCB 255 	; 1 80
		FCB 255 	; 1 88
		FCB 255 	; 1 96
		FCB 255 	; 1 104
		FCB 255 	; 1 112
		FCB 255 	; 1 120
		FCB 255 	; 1 128
					;------------------------------------------------------
					;------------------------------------------------------
					; Crank Fuel PW Mult vs TPS %
					;
					; Table = Mult * 64
					;------------------------------------------------------
LC390: 	FCB 8 		;Num of lines in table -1
					;------------------------------------------------------
					;
					; MULT %TPS
					;------------------------------------------------------
		FCB 64 		; 1.000 0.0
		FCB 81 		; 1.265 12.5
		FCB 81 		; 1.265 25.0
		FCB 89 		; 1.390 37.5
		FCB 97 		; 1.515 50.0
		FCB 106 	; 1.656 62.5
		FCB 115 	; 1.796 75.0
		FCB 0 		; 0.000 87.5 Un-flood
		FCB 0 		; 0.000 100
					;------------------------------------------------------
		;*******************************************************
		;* >>> Fuel/Air Tables & Params <<<
		;*
		;*******************************************************
LC39A: 	FCB 112 	; Throt Hi - Throt Lo
LC39B: 	FCB 35 		; 13.7% Min TPS Throttle Posit
					; CALIB = Arg * 2.56
LC39C: 	FCB 1 		; Filt coef Low TPS, (0.0039)
LC39D: 	FCB 16 		; Cool Temp Coef, (0.0625)
					; CALIB = Arg * 256
		;********************
		;* o2 SENSOR PARAMS
		;********************
LC39E: 	FCB 240 	; o2 Sens 12.5 Msec COEF, (0.9375)
					; CALIB = Arg * 256
					;
LC39F: 	FCB 5 		; o2 Sens 100 Msec coef, (0.0195)
					; CALIB = Arg * 256
					;
LC3A0: 	FCB 102 	; 12.5 msec o2 Filter init val
					; WHEN ENG NOT RUNNING, (0.398 VDC)
					; CALIB = Arg * 256
					;
					; Fuel Output Param's
					;
LC3A1: 	FDB 0111 	; 1.693 msec Min Base PW
					; CALIB = Arg * 65.536
LC3A3: 	FDB 0111 	; 1.693 msec Default Pulse Width
					; CALIB = Arg * 65.536
LC3A5: 	FCB 255 	; SHUT OFF ALL FUEL if > 255 MPH
					; CALIB = Arg = Table
LC3A6: 	FDB 0098 	; SHUT OFF ALL FUEL IF => RPM
					; & MPH =>
					; CALIB = Arg 65536 * 120/(Arg * Num Cyl)
LC3A8: 	FCB 254 	; ENAB FUEL IF < 254 MPH
LC3A9: 	FDB 103 	; ENAB FUEL IF RPM <
					; CALIB = Arg $FFFF * 120/Arg * Num Cyl
					;------------------------------------------------------
					; Inj Offset Vs Batt Volts
					; Measured At Pump
					;
					; TBL = Msec * 32.768 Msec
					;------------------------------------------------------
		ORG $C3AB
					;------------------------------------------------------
					;
					; Msec ADDER BAT VDC
					;------------------------------------------------------
LC3AB: 	FCB 21 		; 6.90 0.0
		FCB 21 		; 6.90 1.6
		FCB 21 		; 6.90 3.2
		FCB 227 	; 2.89 4.8
		FCB 227 	; 2.89 6.4
		FCB 95 		; 1.20 8.0
		FCB 59 		; 9.6
		FCB 41 		; 11.2
		FCB 30 		; 12.8
		FCB 21 		; 0.336 14.4
		FCB 16 		; 16.0
		FCB 11 		; 17.6
		FCB 10 		; 19.2
		FCB 8 		; 20.8
		FCB 7 		; 0.152 22.4
		FCB 6 		; 24.0
		FCB 5 		; 25.5 VDC
					;------------------------------------------------------
					;------------------------------------------------------
					; Inj Offset for Small PW's vs Base PW
					;
					; TBL = Msec * 65.536 Msec
					;------------------------------------------------------
LC3BC: 	FCB 18 		; 0.198 msec 0.488 msec
		FCB 15 		; 0.153 0.732
		FCB 13 		; 0.122 0.976
		FCB 10 		; 0.076 1.220
		FCB 8 		; 0.046 1.460
		FCB 5 		; 0.015 1.780
		FCB 3 		; 0.000 2.197
		FCB 1 		; 0.000 2.440
		FCB 0 		; 0.000 2.685
		FCB 0 		; 0.000 2.929
		FCB 0 		; 0.000 3.170
		FCB 0 		; 0.000 3.410
		FCB 0 		; 0.000 3.660
		FCB 0 		; 0.000 3.900
					;------------------------------------------------------
		;*******************************************************
		;* >>> Open Loop Fuel Tables & Param's <<<
		;*
		;* ORG $S3CB
		;*******************************************************
		ORG $C3CB
		
LC3CB: 	FDB 0445 	; Stoch Ratio  <<<<<<<<<<<<< dld this seems wrong LDD will offset to word C3CA
					; CALIB = 6553.6/Arg, (14.727:1)
LC3CD: 	FDB 0735 	; Table LU Multiplier
					; 65536/(7235) * 5)
					;------------------------------------------------------
					; INJ FLOW RATE
					; 23#/HR INJECTORS
					;
					; Double Fire #/h = 10158.8/cal val
					; Single Fire #/h = 20317.6/cal val
					;------------------------------------------------------
LC3CF: 	FDB 0441 	; Double Fire Prod of Inj Flow rate
					; cal = Sec/gm * (256 * 5)
					;
					; 0.344/sec/g,
					; (1/0.344/sec/g) = 2.902 g/sec
					; 2.902 g/sec * 3.6 = 10.448 kg/hr, (23#)
					;
LC3D1: 	FDB 0882 	; Single Fire Prod of Inj Flow rate
					; cal = Sec/gm * (256 * 10)
					; 0.344/sec/g, 2.902 g/sec * 3.6 = 10.448 kg/hr, (23#)
					;------------------------------------------------------
					;------------------------------------------------------
					; OPEN LOOP A/F Pct Chg vs LD VALUE
					; TYPE $32 ECM MY 86
					;
					; Dissassemby of BUA LINES = 17
					; 09-23-1994, 09:32:07
					;
					; TBL = 2.56 * PCT CHG TO AFR
					;------------------------------------------------------
		ORG $C3D3
					;------------------------------------------------------
					;
					; PCT CHG TO AFR LD VAL
					;------------------------------------------------------
LC3D3: 	FCB 0 		; 0.0 0
LC3D4: 	FCB 0 		; 0.0 16
LC3D5: 	FCB 0 		; 0.0 32
LC3D6: 	FCB 0 		; 0.0 48
LC3D7: 	FCB 0 		; 0.0 64
LC3D8: 	FCB 3 		; 1.2 80
LC3D9: 	FCB 8 		; 3.1 96
LC3DA: 	FCB 8 		; 3.1 112
LC3DB: 	FCB 13 		; 5.1 128
LC3DC: 	FCB 18 		; 7.0 144
LC3DD: 	FCB 23 		; 9.0 160
LC3DE: 	FCB 23 		; 9.0 176
LC3DF: 	FCB 23 		; 9.0 192
LC3E0: 	FCB 23 		; 9.0 208
LC3E1: 	FCB 23 		; 9.0 224
LC3E2: 	FCB 23 		; 9.0 240
LC3E3: 	FCB 23 		; 9.0 256
					;--------------------------------------------------
					;--------------------------------------------------
					; START UP ENRICH vs COOL
					;
					; Dissassemby of BUA, LINES = 14
					; 09-23-1994, 09:26:54
					;
					; TBL = 2.56 * PCT CHG
					;--------------------------------------------------
		ORG $C3E4
					;--------------------------------------------------
					;
					; PCT CHG Deg c COOL
					;--------------------------------------------------
LC3E4: 	FCB 235 	; 91.8 -40.0
LC3E5: 	FCB 215 	; 84.0 -28.0
LC3E6: 	FCB 194 	; 75.8 -16.0
LC3E7: 	FCB 171 	; 66.8 -4.0
LC3E8: 	FCB 115 	; 44.9 8.0
LC3E9: 	FCB 36 		; 14.1 20.0
LC3EA: 	FCB 36 		; 14.1 32.0
LC3EB: 	FCB 36 		; 14.1 44.0
LC3EC: 	FCB 36 		; 14.1 56.0
LC3ED: 	FCB 26 		; 10.2 68.0
LC3EE: 	FCB 26 		; 10.2 80.0
LC3EF: 	FCB 26 		; 10.2 92.0
LC3F0: 	FCB 26 		; 10.2 104.0
LC3F1: 	FCB 26 		; 10.2 116.0
					;------------------------------------------------------
					;------------------------------------------------------
					; Start up Enrich Decay vs Cool Temp.
					;
					; Tbl Val = Num of Injects
					;------------------------------------------------------
LC3F2: 	FDB 512 	;Scale factor for Table, (2)
LC3F4: 	FCB 175 	; -40,
		FCB 158 	; -28
		FCB 140 	; -16
		FCB 140 	; - 4
		FCB 140 	; 8
		FCB 127 	; 20
		FCB 120 	; 32
		FCB 100 	; 44
		FCB 80 		; 56
		FCB 73 		; 68
		FCB 67 		; 80
		FCB 67 		; 92
		FCB 67 		; 104
		FCB 67 		; 116
					;------------------------------------------------------
					;------------------------------------------------------
					; Start up Enrichment Decay Rep Rate Vs Start
					; up Coolant Temp
					;
					; Tbl Val = Num of Injects
					;------------------------------------------------------
LC402: 	FCB 23 		; -40 Deg C
		FCB 23 		; -28. Injects
		FCB 23 		; -16
		FCB 23 		; -4
		FCB 23 		; 8
		FCB 21 		; 20
		FCB 20 		; 32
		FCB 16 		; 44
		FCB 13 		; 56
		FCB 12 		; 68
		FCB 11 		; 80
		FCB 11 		; 92
		FCB 11 		; 104
		FCB 11 		; 116
					;------------------------------------------------------
					;------------------------------------------------------
					; Start up Enrich Decay amt Vs Start up Cool Temp
					;
					; Tbl Val = Pct Chg * 655.36
					;------------------------------------------------------
LC410: 	FCB 252 	; -40
		FCB 252 	; -28
		FCB 252 	; -16
		FCB 252 	; -4
		FCB 252 	; 8
		FCB 252 	; 20
		FCB 252 	; 32
		FCB 151 	; 44
		FCB 151 	; 56
		FCB 151 	; 68
		FCB 151 	; 80
		FCB 151 	; 92
		FCB 151 	; 104
		FCB 151 	; 116
					;--------------------------------------------------
					;--------------------------------------------------
					; OPEN LOOP A/F % CHANGE VS COOL TEMP
					;
					; Dissassemby of BUA, LINES = 14
					; 09-23-1994, 09:36:15
					;
					; TBL = 2.56 * %CHG TO AFR + BIAS
					;--------------------------------------------------
		ORG $C41E
LC41E: 	FCB 221 	; Open Lp AFR Bias to Set up Lean Lmt
					;
					; AFR = 20:1 EXAMPLE
					; 14.7 * 256 = 189
					;--------------------------------------------------
					;
					; %CHG TO AFR Deg c COOL
					;--------------------------------------------------
LC41F: 	FCB 89 		; 34.8 -40.0
LC420: 	FCB 68 		; 26.6 -28.0
LC421: 	FCB 64 		; 25.0 -16.0
LC422: 	FCB 53 		; 20.7 -4.0
LC423: 	FCB 49 		; 19.1 8.0
LC424: 	FCB 35 		; 13.7 20.0
LC425: 	FCB 25 		; 9.8 32.0
LC426: 	FCB 25 		; 9.8 44.0
LC427: 	FCB 25 		; 9.8 56.0
LC428: 	FCB 25 		; 9.8 68.0
LC429: 	FCB 25 		; 9.8 80.0
LC42A: 	FCB 25 		; 9.8 92.0
LC42B: 	FCB 25 		; 9.8 104.0
LC42C: 	FCB 25 		; 9.8 116.0
					;--------------------------------------------------
					;------------------------------------------------------
					; Dissassemby of BUA.BIN
					; Base fuel injection PW's vs LOAD
					;
					; 07-22-1989 12:28:30
					;
					; BPINJ/5 = 89 * INJ BC * LV8 / ( 1024 * LV SCALAR)
					;
					; TBL = Msec * (65536/5)
					; TBL = 13.1072 * PW Msec
					;------------------------------------------------------
LC42D: 	FCB 0 		; Row min, RPM
		FCB 0 		; Min LV8 Value
		FCB 17 		; Num ROWS/BLK
					;----------------------------------------------------
					; 0 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 400 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 800 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 1200 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 1600 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 2000 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 2400 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 2800 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 3200 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 3600 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 4000 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 4400 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 4800 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 5200 RPM Msec. LOAD
					;-----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 5600 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 6000 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
					; 6400 RPM Msec. LOAD
					;----------------------------------------------------
		FCB 0 		; 0.0 LD = 0
		FCB 8 		; 0.6 LD = 16
		FCB 15 		; 1.1 LD = 32
		FCB 23 		; 1.8 LD = 48
		FCB 31 		; 2.4 LD = 64
		FCB 38 		; 2.9 LD = 80
		FCB 46 		; 3.5 LD = 96
		FCB 54 		; 4.1 LD = 112
		FCB 61 		; 4.7 LD = 128
		FCB 69 		; 5.3 LD = 144
		FCB 77 		; 5.9 LD = 160
		FCB 84 		; 6.4 LD = 176
		FCB 92 		; 7.0 LD = 192
		FCB 100 	; 7.6 LD = 208
		FCB 107 	; 8.2 LD = 224
		FCB 115 	; 8.8 LD = 240
		FCB 123 	; 9.4 LD = 256
					;----------------------------------------------------
		;***************************************************
		;* Closed Loop QUAL'S
		;*
		;*
		;***************************************************
LC551: 	FCB 107 	; 40.7c, (104.4f) Min Temp for Closed Loop
					; CALIB = (deg c + 40) * (256/192)
					;
LC552: 	FCB 147 	; Use Hot C Loop timer If Cool T >= 70.3c, (158.5f)
					; CALIB = (deg c + 40) * (256/192)
					;
LC553: 	FCB 73 		; Use Cold C Loop timer if Cool T <= 14.7c, (58.5f)
					; CALIB = (deg c + 40) * (256/192)
					;
		FCB 0 		; ? ? ?
LC555: 	FCB $AA 	; Diag Prom test word 2
					; Set Error 51 if N.E. $AA
LC556: 	FCB 150 	; 75 SEC, Cold Closed Lp Timer
					; (SEC * 2 Sec)
LC557: 	FCB 103 	; 51.4 SEC, Warm Closed Lp Timer,
					; (SEC * 2 Sec)
LC558: 	FCB 25 		; 12.5 SEC, Hot Closed Lp Timer
					; Arg, (SEC * 2)
					;
					; o2 RDY HYST PAIR
					; (CAL = vdc * 2260
					;
LC559: 	FCB 158 	; If 02 Volts > 0.699 VDC then o2 ready
LC55A: 	FCB 45 		; If o2 < 0.199 VDC then o2 is ready
					;
LC55B: 	FCB 50 		; 10 Sec for o2 Max/Min
					; CALIB = (SEC/5 Sec)
					;------------------------------------------------------
					; Closed Loop Fuel Cnt'l Param's
					;
					;------------------------------------------------------
					;
					; CLOSED Lp INTAGRATOR LIMITS
					;
LC55C: 	FCB 40 		; Closed Lp Corr, Min INT Value
LC55D: 	FCB 180 	; Closed Lp Corr, Max INT Value
LC55E: 	FCB 3 		; Clsed Lp Idle TPS 1.17% Gain Factor
					; CALIB = tps * 2.56
LC55F: 	FCB 16 		; 0.2 sec Added Corr To Int Dly At Idle
					; CALIB = Arg * 80
LC560: 	FCB 160 	; EECC Clsd Lp Gain factor
					; CALIB = Arg * 256, (0.625 Mult)
LC561: 	FCB 12 		; Diff Value to Make up Rich/Lean Window
					; for Fast o2, CALIB = Arg * 226, (.053 VDC)
LC562: 	FCB 23 		; Diff Value to Make up Rich/Lean Window
					; When Air div, CALIB = Arg * 226, (.102 VDC)
LC563: 	FCB 4 		; Min Error To Implement Int.
					; CALIB = Arg * 226, (.017 VDC)
LC564: 	FCB 224 	; Positive Error Gain Factor
					; CALIB = Arg * 226, (.991 VDC)
LC565: 	FCB 154 	; o2 Filter Constant when Coolant Tmp Low
LC566: 	FCB 96 		; Cool Int Offset Int Gain
					; Cal = (Arg + 40) * 256/196, (42.8 C)
					;------------------------------------------------------
					; Upper 0 error for Slow 02 Vs Air flow
					;
					; Table value = Volts * 226
					;------------------------------------------------------
LC567: 	FCB 140 	; 0 Gms/Sec
		FCB 148 	; 8
		FCB 152 	; 16
		FCB 152 	; 24
		FCB 148 	; 32
		FCB 144 	; 40
		FCB 132 	; 48
		FCB 128 	; 56
		FCB 125 	; 64
					;------------------------------------------------------
					;------------------------------------------------------
					; Lower 0 error for Slow 02 Vs Air flow
					;
					; Table value = Volts * 226
					;------------------------------------------------------
LC570: 	FCB 120 	; 0 Gms/Sec
		FCB 128 	; 8
		FCB 132 	; 16
		FCB 132 	; 24
		FCB 128 	; 32
		FCB 124 	; 40
		FCB 112 	; 48
		FCB 109 	; 56
		FCB 106 	; 64
					;------------------------------------------------------
					;------------------------------------------------------
					; Fast o2 Rich/lean Treshold vs Air Flow
					;
					; Table value = Volts * 226
					; ORG $C579
					;------------------------------------------------------
LC579: 	FCB 130 	; 0 Gms/sec
		FCB 138 	; 8
		FCB 142 	; 16
		FCB 142 	; 24
		FCB 138 	; 32
		FCB 134 	; 40
		FCB 122 	; 48
		FCB 120 	; 56
		FCB 118 	; 64
					;------------------------------------------------------
					;------------------------------------------------------
					; Integrator Delay Vs Air flow
					;
					; TBL = Sec's * 80
					;------------------------------------------------------
LC582: 	FCB 32 		; 0 Gms/Sec
		FCB 28 		; 8
		FCB 24 		; 16
		FCB 22 		; 24
		FCB 20 		; 32
		FCB 18 		; 40
		FCB 16 		; 48
		FCB 16 		; 56
		FCB 12 		; 64
					;------------------------------------------------------
					;------------------------------------------------------
					; Slow o2 Filter Coef Vs Air flow
					;
					; TBL = Mult * 256
					;
					;------------------------------------------------------
LC58B: 	FCB 16 		; 0 Gms/Sec
		FCB 24 		; 8
		FCB 32 		; 16
		FCB 36 		; 24
		FCB 40 		; 32
		FCB 44 		; 40
		FCB 44 		; 48
		FCB 44 		; 56
		FCB 44 		; 64
					;------------------------------------------------------
					;------------------------------------------------------
					; o2 Error Reduction Vs Air flow
					;
					; Table value = Mult (0-1) * 256
					;------------------------------------------------------
LC594: 	FCB 200 	; 0 Gms/Sec
		FCB 224 	; 8
		FCB 236 	; 16
		FCB 244 	; 24
		FCB 248 	; 32
		FCB 255 	; 40
		FCB 255 	; 48
		FCB 255 	; 56
		FCB 255 	; 64
					;------------------------------------------------------
					;------------------------------------------------------
					; Proportional Step Width Offset vs RPM
					;
					; Table value = Sec's * 80
					;------------------------------------------------------
LC59D: 	FCB 20 		; 400 RPM
		FCB 12 		; 800
		FCB 8 		; 1200
		FCB 4 		; 1600
		FCB 3 		; 2000
		FCB 2 		; 2400
					;------------------------------------------------------
					;------------------------------------------------------
					; Proportional Step Size Vs Error
					;
					; TBL = BIN STEPS
					;------------------------------------------------------
LC5A3: 	FCB 3 		; 3 0 Error counts
		FCB 3 		; 3 8
		FCB 3 		; 3 16
		FCB 2 		; 2 24
		FCB 2 		; 2 32
		FCB 2 		; 2 40
		FCB 3 		; 3 48
		FCB 4 		; 4 56
		FCB 7 		; 7 64
		FCB 10 		; 10 72
		FCB 12 		; 12 80
		FCB 16 		; 16 88
					;------------------------------------------------------
					;------------------------------------------------------
					; PROPORTIONAL STEP WIDTH Vs Error
					;
					;
					; Table value = Sec's * 80
					;------------------------------------------------------
LC5AF: 	FCB 4 		; 0 Error (BIN)
		FCB 4 		; 8
		FCB 4 		; 16
		FCB 4 		; 24
		FCB 4 		; 32
		FCB 8 		; 40
		FCB 12 		; 48
		FCB 24 		; 56
		FCB 32 		; 64
		FCB 64 		; 72
		FCB 128 	; 80
		FCB 240 	; 88
					;------------------------------------------------------
					;------------------------------------------------------
					; Rich/Lean Offset Vs Coolant Temp
					;
					;
					; TBL = BIN VAL
					;------------------------------------------------------
LC5BB: 	FCB 0 		; 0 -40 Deg C
		FCB 0 		; 0 -28
		FCB 0 		; 0 -16
		FCB 0 		; 0 -4
		FCB 0 		; 0 8
		FCB 16 		; 16 20
		FCB 16 		; 16 32
		FCB 16 		; 16 44
		FCB 16 		; 16 56
		FCB 16 		; 16 68
		FCB 16 		; 16 80
		FCB 0 		; 0 92
		FCB 0 		; 0 104
		FCB 0 		; 0 116
					;------------------------------------------------------
					;------------------------------------------------------
					; Integrator Delay Vs Error
					;
					; TBL = Multiplier (0 - 255)
					;------------------------------------------------------
LC5C9: 	FCB 255 	; 0 Error
		FCB 255 	; 8
		FCB 255 	; 16
		FCB 255 	; 24
		FCB 128 	; 32
		FCB 68 		; 40
		FCB 64 		; 48
		FCB 64 		; 56
		FCB 64 		; 64
		FCB 64 		; 72
		FCB 64 		; 80
		FCB 64 		; 88
					;------------------------------------------------------
		;*******************************************************
		;* >>> Block Learn Tables & Param's <<<
		;*
		;*
		;*******************************************************
LC5D5: 	FCB 28 		; BLM Cell Boundry, (700 RPM)
LC5D6: 	FCB 48 		; BLM Cell Boundry (1200 RPM)
LC5D7: 	FCB 80 		; BLM Cell Boundry, (2000 RPM)
					;
LC5D8: 	FCB 12 		; BLM Flow Cell Boundry, (12 Gms/Sec)
LC5D9: 	FCB 22 		; BLM Flow Cell Boundry, (22 Gms/Sec)
LC5DA: 	FCB 34 		; BLM Flow Cell Boundry, (34 Gms/Sec)
					;
LC5DB: 	FCB 4 		; BLM Hysteresis, (100 RPM RPM)
					;
LC5DC: 	FCB 2 		; BLM Hysteresis
					; CALIB = Arg, (2 * 25 Gms/Sec)
LC5DD: 	FCB 120 	; If Coolant <= Disable BLM update
					; CALIB = (Arg + 40) * 256/192, (50 C)
LC5DE: 	FCB 240 	; If Coolant >= Disable BLM update 140c, 284f
					; CALIB = (Arg + 40) * 256/192, (140 C)
LC5DF: 	FCB 0 		; If Ld Val < Disable BLM Update
LC5E0: 	FCB 28 		; BLM Update Rate
					; CALIB = Arg * 20 + 1, (450 MSec)
					; (50 Msec/bit)
LC5E1: 	FCB 2 		; 2, BLM Mult Update Amt
LC5E2: 	FCB 160 	; Max Allowable BLM
LC5E3: 	FCB 108 	; Min Allowable BLM
LC5E4: 	FCB 5 		; If Clsd Lp int > 5, Enab BLM Update
LC5E5: 	FCB 5 		; If Clsd Lp int < 5, Enab BLM Update
		;*****************************************
		;* Memory Stay Alive Tables & Param's
		;*
		;* $C5E6
		;****************************************
LC5E6: 	FCB 118 	; Low Lmt for SAM Cell 0
LC5E7: 	FCB 150 	; High Lmt for SAM Cell 0
					;
LC5E8: 	FCB 118 	; Low Lmt for other SAM Cell's
LC5E9: 	FCB 150 	; High Lmt for other SAM Cell's
					;
LC5EA: 	FCB 0 		; Sam Cell A Number, Idle
LC5EB: 	FCB 9 		; SAM Cell B Number, Non Idle
					;
LC5EC: 	FCB 75 		; Stop Cell O update when time up, 15 SEC'D
					; CALIB = Arg * 5, (75/5 Sec's)
					;
LC5ED: 	FCB 171 	; If Temp < 88c Then Skip SAM Update
					; CALIB = (Arg + 40) * 256/192, (88.25 C)
					;
LC5EE: 	FCB 187 	; If Temp > 100c Then Skip SAM Update
					; CALIB = (Arg + 40) * 256/192, (100.25 C)
					;
LC5EF: 	FCB 16 		; SAM A Filter Coef.
					; CALIB = Arg * 256
					;
LC5F0: 	FCB 24 		; SAM B Filter Coef.
					; CALIB = Arg * 256
		;*******************************************************
		;* Decel Enleanment Param's
		;*
		;* $C5F1
		;*******************************************************
LC5F1: 	FCB 40 		; Enab Dcel Int Reset If Ld val <=
					; CALIB = Arg
					;
LC5F2: 	FCB 28 		; Enab Dcel Int Reset If RPM >
					; CALIB = Arg/25, (700 RPM)
					;
LC5F3: 	FCB 255 	; TPS Filter Coef
					; CALIB = Arg * 255
					;
LC5F4: 	FCB 240 	; Dif TPS% Threshold for Decle Enlean
					; CALIB = Arg * 2.56, (93%)
					;
LC5F5: 	FCB 245 	; Diff Ld Val Thresh Of Decel Enlean
					; CALIB = Arg
					;
LC5F6: 	FCB 0 		; Decel Enlean Time To maint.
					; CALIB = Arg * 80
					;
LC5F7: 	FCB 0 		; Decel Enlean T/F Default
					; CALIB = Arg
					;
LC5F8: 	FCB 128 	; lmt Fuel factor
					; CALIB = Arg * 128, (0.9375)
					;
LC5F9: 	FCB 128 	; lmt Fuel factor
					; CALIB = Arg * 128, (0.9375)
		;*******************************************************
		;* Decl Fuel cut off Tables & Param's
		;*
		;*******************************************************
LC5FA: 	FCB 48 		; Decel Fuel C/O if > 1200 RPM Upper Hyst value
					; CALIB = Arg/25
					;
LC5FB: 	FCB 36 		; Remain In C/O if > 900 RPM
					; Lower hyst value
					; CALIB = Arg/25
					;
LC5FC: 	FCB 30 		; Enab C/O if Ld Val < 30
					; Lower hyst value
					;
LC5FD: 	FCB 40 		; Stay in C/O if Ld Val <
					; Upper Hyst value
					;
LC5FE: 	FCB 7 		; Max RPM Decel In 12.5 Msec
					; CALIB + Arg/12.5, (87.5 RPM)
					;
LC5FF: 	FCB 20 		; C/O Cint Min time Lmt
					; CALIB = Arg * 80, (0.25 Sec)
					;
LC600: 	FCB 8 		; If TPS < then Enab C/O
					; CALIB = Arg * 2.56, (3.125%)
					;
LC601: 	FCB 0 		; If Cool Temp < then Disab C/O
					; CALIB = (Arg + 40) * 256, (Deg C)
					;
LC602: 	FCB 15 		; If VSS <= Disable C/O
					; CALIB = Arg, (15 MPH)
					;
LC603: 	FCB 8 		; 3.125% TPS, C/O Stall Saver TPS Default
					; CALIB = ARG * 2.56, (3.125%)
					;
LC604: 	FCB 30 		; 375 msec, C/O Stall Saver T/F TPS Duration
					; CALIB = Arg * 80,
					;
LC605: 	FDB 0256 	; 3.9 Msec, C/O Stall Saver PW
					; CALIB = Arg * 65.536
					;
LC607: 	FCB 1 		; CALIB of C/O Stall Saver Acel Enrich Pulses
					;
LC608: 	FDB 0511 	; 6.38 Sec, Min Time Between Consec DEFCO'S
					; CALIB = Arg * 80,
					;
LC60A: 	FCB 10 		; 4%TPS, DFCO T/F TPS Default
					; CALIB = Arg *2.56
		;*******************************************************
		;* PWR ENRICHMENT TABLES & PARAM'S
		;*
		;* MY86 TYPE 32 ECM
		;*******************************************************
LC60B: 	FCB 50 		; IF Ld VAL > 50 ENAB PWR ENRICH
					; CAL = VAL
					;
LC60C: 	FCB 10 		; Ld VAL HYST FOR PWR ENRICH.
					; CAL = VAL
					;
LC60D: 	FCB 16 		; TPS HYST. DISABLE PWR ENRICH IF TPS <
					; CAL = VAL * 2.56, (6.25%)
					;
					;
					;-------------------------------------------------
					; Pwr Enrichment TPS Treshold vs RPM
					;
					; BUA, TYPE $32, ECM P/N 1227165
					;
					; TBL = TPS% * 2.56
					;-------------------------------------------------
		ORG $C60E
					;-------------------------------------------------
					;
					; %TPS RPM
					;-------------------------------------------------
LC60E: 	FCB 180 	; 70 400
		FCB 180 	; 70 1200
		FCB 180 	; 70 2000
		FCB 180 	; 70 3200
		FCB 180 	; 70 4800
					;-------------------------------------------------
					;-------------------------------------------------
					; Pwr Enrichment Air/Fuel Pct Change
					; vs Coolant Temperature
					;
					; TBL = Pct Change * 2.56
					;-------------------------------------------------
LC613: 	FCB 8 		; 9 Line table
					;------------------------------------------------------
					;
					; %CHG Deg c COOL
					;------------------------------------------------------
		FCB 140 	; 54.68 -40 C
		FCB 125 	; 48.82 -16
		FCB 112 	; 43.75 8
		FCB 86 		; 33.59 32
		FCB 58 		; 22.66 56 (132.8f)
		FCB 58 		; 22.66 80
		FCB 58 		; 22.66 104
		FCB 58 		; 22.66 128
		FCB 58 		; 22.66 152 (305f)
					;-------------------------------------------------------
					;------------------------------------------------------
					; Pwr Enrichment Air/Fuel Pct Change vs RPM
					;
					; TBL = (Pct Change * 1.28) + 128
					;
					;------------------------------------------------------
LC61D: 	FCB 128 	;O% O RPM
		FCB 128 	; 400
		FCB 128 	; 800
		FCB 128 	; 1200
		FCB 128 	; 1600
		FCB 128 	; 2000
		FCB 128 	; 2400
		FCB 128 	; 2800
		FCB 128 	; 3200
		FCB 128 	; 3600
		FCB 128 	; 4000
		FCB 128 	; 4400
		FCB 128 	; 4800
		FCB 128 	; 5200
		FCB 128 	; 5600
		FCB 128 	; 6000
		FCB 128 	; 6400 RPM
					;------------------------------------------------------
		;*******************************************************
		;*******************************************************
		;* >>> IAC Tables & Params <<<
		;*
		;*******************************************************
LC62E: 	FCB 128 	; RPM 12.5 Msec Filter Coef, (0.5)
					; CALIB = Arg * 256
LC62F: 	FCB 144 	; Idle Spd Start up park posit.
					; CALIB = Arg
LC630: 	FCB 32 		; Steps Added to Warm Park posit if
					; hot restart spark retard is active.
LC631: 	FCB 10 		; Steps Added to Warm Park down if
					; A/C on during start up.
LC632: 	FCB 5 		; Add Steps for Fan Anticipate
					; CALIB = Arg/2
LC633: 	FCB 5 		;Add Offset for Cold Engine
					;when cold spark has expired
					;
					; Cmd Speed Definitions
					;
LC634: 	FCB 4 		; IAC offset added for pk/neut
					; CALIB = Arg/12.5, (50 RPM)
LC635: 	FCB 160 	; Idle Speed Strt up Dly Time
					; CALIB = Arg/6.25, (1000 Sec)
LC636: 	FCB 0 		; IAC Offset Cmd Spd Added
					; CALIB = Num/12.5, (RPM)
					;--------------------------------------------------
					; IAC TARGET RPM vs coolant Temp.
					;
					; Dissassemby of BUA, LINES = 17
					; 09-23-1994, 10:07:16
					;
					; TBL = .08 * RPM/12.5
					;--------------------------------------------------
		ORG $C637
					;--------------------------------------------------
					;
					; RPM/12.5 Deg c COOL
					;--------------------------------------------------
LC637: 	FCB 84 		; 1050 -40
LC638: 	FCB 84 		; 1050 -28
LC639: 	FCB 84 		; 1050 -16
LC63A: 	FCB 84 		; 1050 -4
LC63B: 	FCB 84 		; 1050 8
LC63C: 	FCB 76 		; 950 20
LC63D: 	FCB 72 		; 900 32
LC63E: 	FCB 60 		; 750 44
LC63F: 	FCB 48 		; 600 56
LC640: 	FCB 48 		; 600 68
LC641: 	FCB 44 		; 550 80
LC642: 	FCB 44 		; 550 92
LC643: 	FCB 44 		; 550 104
LC644: 	FCB 44 		; 550 116
LC645: 	FCB 44 		; 550 128
LC646: 	FCB 44 		; 550 140
LC647: 	FCB 44 		; 550 152
					;--------------------------------------------------
					;--------------------------
					; CMD Speed Cnt'l Deadband
					;--------------------------
LC648: 	FCB 6 		; Max RPM Error for deadband in Pk/Neut
					; CALIB = Arg/12.5, (75 RPM)
LC649: 	FCB 4 		; Max RPM Error for deadband in drive
					; CALIB = Arg/12.5, (50 RPM)
LC64A: 	FCB 2 		; Max RPM error for A/C & Min Motor Posit.
					; Learning Dead Band, CALIB = Arg * 12.5
					; (2.5 RPM)
		;**********************************
		;* PID ENABLE CONDITIONS
		;**********************************
LC64B: 	FCB 3 		; Enab T/F if TPS% >
					; CALIB = Arg * 2.56, (1.17%)
LC64C: 	FCB 0 		; No store of new TPS if Diif
					; fm old is <
LC64D: 	FCB 8 		; No Decay till TPS >=
					; CALIB = Arg * 2.56, (3.125%)
LC64E: 	FCB 6 		; Enab C/L PID if MPH <=
					; CALIB = Arg * 16/5, (1.8 MPH)
LC64F: 	FCB 4 		; 50 RPM, Min under spd error to enab PID
					; Durring Xsission dly, CALIB = Arg/12.5
					;-------------------------
					; Proportional Gains
					;
					; ORG $C650
					;-------------------------
LC650: 	FCB 32 		; Undr Spd Error Proprt Gain
					; CALIB = Arg * 256 * 12.5
LC651: 	FCB 32 		; Over spd Error Proport Gain
LC652: 	FCB 255 	; Min Under spd err to enab
					; High Prop gain addition
LC653: 	FCB 0 		; High undr spd error prop gain add.
					;----------------------
					; Derivitve Gains
					;
					; ORG $C654
					;----------------------
LC654: 	FCB 16 		; Low gain for deriv term when RPM
					; rate inc
					; CALIB =(steps/rpm/sec) * 256 * 15.625
LC655: 	FCB 64 		; Steps Gain deric term when RPM Rate inc
					; CALIB =(steps/rpm/sec) * 256 * 15.625
LC656: 	FCB 22 		; 343 RPM/Sec Min dec RPM rate to enable
					; hi gain for direv term,
					; CALIB = (rpm/sec)/15.625
LC657: 	FCB 255 	; Steps
					; CALIB =(steps/rpm/sec) * 256 * 15.625
LC658: 	FCB 4 		; 50 RPM min umderspd error to enab hi
					; proportional & hi deriv gain additions
					;------------------------
					; Integral Gains
					;
					; ORG $C659
					;-----------------------
LC659: 	FCB 40 		; PK/NEUT intigrator gain if RPM
					; error& rate out of dead bamds
					; CALIB =(steps/rpm/sec) * 256 * 15.625
LC65A: 	FCB 40 		;
					; CALIB =(steps/rpm/sec) * 256 * 15.625
LC65B: 	FCB 12 		; Pk/Neut, max RPM rate to intigrate w/KISITGN
					; CALIB = (RPM/SEC)/15.625, (187.5 RPM/SEC)
LC65C: 	FCB 12 		; Drive, max RPM rate to intigrate w/KISITGD
					; CALIB = (RPM/SEC)/15.625, (187.5 RPM/SEC)
					;------------------------------
					; Pk/Neut Scalling Gain
					;
					; ORG $C65D
					;-------------------------------
LC65D: 	FCB 255 	; Pk/Neut, Mult for prop & deriv terms.
					; CALIB = arg * 256
					;------------------------------
					; TPS Follower gain's
					;
					; ORG $C65E
					;------------------------------
LC65E: 	FCB 5 		; 31.25 msec between T/F steps in PK/NUET
					; CALIB =msec/6.25
LC65F: 	FCB 4 		; 25 msec between T/F steps in drive
					; CALIB =msec/6.25
LC660: 	FCB 10 		; 1.953 msec/RPM addition vs MPH between T/F steps
					; in Drive
					; CALIB =msec * (32/6.25)
LC661: 	FCB 16 		; 100 msec addition to time between T/F steps
					; if in open loop IAC.
					; CALIB =msec/6.25
LC662: 	FCB 192 	; 1.5 steps/% TPS throt follower slope gain
LC663: 	FCB 75 		; 75 Steps T.F. max steps in drive.
LC664: 	FCB 255 	;Pk/Neut T.F. mult to T.F. steps
					; (NEUT STEPS)/(DRIVE STEPS)
					;----------------------------
					;----------------------------
					; A/C Learning Gain
					;
					;-----------------------------
LC665: 	FCB 20 		; STEPS, A/C DEFAULT
LC666: 	FCB 32 		; STEPS, MAX A/C LEARN STEPS
LC667: 	FCB 1 		; STEPS, MIN A/C LEARN STEPS
LC668: 	FCB 5 		; STEPS, MAX PLUS A/C LEARN PWER OFF/ON CYCLE
LC669: 	FCB 20 		; 400 MSEC MIN RPM ERR DEAD BAND TIME BEFORE
					; LEARN ENABLE/DISABLE
LC66A: 	FCB 128 	; STEPS, P/N NEUT GAIN FOR A/C LEARNED VAL
					; (DRIVE STEPS)/(NEUT STEPS)
					;-----------------------------
					;
					; STEPPER MOTOR PARAMS'S
					;
LC66B: 	FCB 32 		; QUANTIZER GAIN TO CONVERT ALGO OUT FROM LINEAR
					; MOTOR GAINS TO STEPS
					; CALIB = GAIN * 256
					;
LC66C: 	FCB 8 		; IDLE SPD INVERSE QUANT GAIN TO MAKE
					; FARCTIONAL STPS
					;----------------------------------------------
					; A/C Clutch Tables & Param's
					;
					;
					;----------------------------------------------
LC66D: 	FCB 255 	; IF TPS GT 99% DISABLE A/C
					; (MUST BE 99-16 TO RE-ENABLE)
					;
LC66E: 	FCB 255 	; IF PLUS DIFF TPS GT 99%DISABLE A/C
LC66F: 	FCB 0 		; DISABLE A/C CLUTCH 0 SEC AFTER IF DIFF TPS
LC670: 	FCB 1 		; 1 SEC DELAY FOR CLUTCH ENGAGE
LC671: 	FCB 1 		; 1 SEC DELAY FOR CLUTCH DIS-ENGAGE
LC672: 	FCB 255 	; IF COOL GT 150 Deg c, DIS-ENGAGE CLUTCH
					;-------------------------------
					; Transitional Delays
					;
					;--------------------------------
LC673: 	FCB 40 		; DISABLE CLSD LP PID FOR 800 Msec
					; AFTER OPN LP DISABLE
					;
LC674: 	FCB 30 		; DISABLE CLSD LP PID FOR 600 Msec
					; AFTER TPS FOLLOWER DISABLE
					;
LC675: 	FCB 16 		; DISABLE CLSD LP PID FOR 320 Msec
					; AFTER NEUT DRIVE SHIFT
					;
LC676: 	FCB 20 		; DISABLE CLSD LP PID FOR 400 Msec
					; AFTER FAN ON
					;
LC677: 	FCB 20 		; DISABLE CLSD LP PID FOR 1000 Msec
					; AFTER EXPIRATION OF COLD ENGINE SPARK
					;------------------------------
					; Low Integrator Gains
					;
					;------------------------------
LC678: 	FCB 40 		; 40 STEPS PK/NEUT LO INT GAIN IF
					; RPM ERR IN DEADBAND
					; CALB = (NUM * 128 * 12.5/.05)
					;
LC679: 	FCB 8 		; 40 STEPS, DRIVE LO INT GAIN IF
					; RPM ERR IN DEADBAND
					; CALB = (NUM * 128 * 12.5/.05)
					;
LC67A: 	FCB 0 		; IF RPM ERROR LT 0 DISABLE & CLR INTEGRAL
					; CALIB = RPM/12.5
					;
					;---------------------------
					; Motor Reset Param's
					;
					; ORG $C67B
					;---------------------------
LC67B: 	FCB 145 	; MAX IAC STEPS POSSIBLE
					;
					; RPM Rate Deadband
					;
LC67C: 	FCB 16 		;__ RPM/SEC MIN DIRVITIVE RATE
					; CALIB = RPM/SEC/15.625
					;
					; Clutch Anticipate
					;
					;
					; SKIP Pwr Steer Press Sw READ
					; HYST PAIR
					;
LC67D: 	FCB 255 	; SKIP PS SW READ IF 100% TPS, UPPER VAL
LC67E: 	FCB 255 	; SKIP PS SW READ IF 100% TPS, LOWER VAL
LC67F: 	FCB 8 		; SKIP PWR STEER PRESS SW READ IF => 8 MPH
					;
					; Pwr Steering Anticipate
					;
LC680: 	FCB 0 		; PWR STEER ANTIC STEPS
LC681: 	FCB 0 		; PWR STEER ANTIC STEPS if A/C on
					;
					; MANUAL PARAM'S
					;
LC682: 	FCB 0 		; FLAG, '1' IF MANUAL VECH SELECTED ELSE '0'
LC683: 	FCB 5 		; 5 STEPS ADDED TO T/F IN DRIVE &
					; VECH IS MOVING (MPH>
					;
					; PID HOT SPARK RETARD PARAM'S
					;
LC684: 	FCB 25 		;
LC685: 	FCB 24 		; ADD 300 RPM SPEED OFFSET IF HOT SPARK
					; RETARD IS ACTIVE
					;------------------------------------------------------
					; IAC Mult (0-1) vs Coolant temp
					;
					;
					; TABLE VALUE = MULT * 256
					;------------------------------------------------------
LC686: 	FCB 8 		; 9 Lines in Tbl.
					;------------------------------------------------------
					;
					; MULT DEG c COOL
					;------------------------------------------------------
		FCB 204 	; 0.80 -40
		FCB 204 	; 0.80 -16
		FCB 230 	; 0.90 8
		FCB 255 	; 1.00 32
		FCB 255 	; 1.00 56
		FCB 255 	; 1.00 80
		FCB 255 	; 1.00 104
		FCB 255 	; 1.00 128
		FCB 255 	; 1.00 152
					;------------------------------------------------------
					;------------------------------------------------------
					; Warm Park Posit vs Coolant Temp
					;
					; TABLE = STEPS
					;
					;
					;------------------------------------------------------
		ORG $C690
LC690: 	FCB 8 		; 9 Lines in Tbl
					;------------------------------------------------------
					;
					; DEG c COOL
					;------------------------------------------------------
		FCB 145 	; -40
		FCB 145 	; -16
		FCB 113 	; 8
		FCB 90 		; 32
		FCB 80 		; 56
		FCB 65 		; 80
		FCB 50 		; 104
		FCB 50 		; 128
		FCB 50 		; 152
					;------------------------------------------------------
		;*******************************************************
		;******************************************************
		;* Mass Air Flow Tables & Param's
		;*
		;* DIGITAL MASS FLOW SENSOR
		;*
		;* Freq range 34 - 150 hZ
		;* Period 927 - 437 counts
		;* Range 1490 counts
		;*
		;* table 1 0 - 511 counts, 0 - 22 gms/sec
		;* table 2 512 - 767 counts, 22 - 48
		;* table 3 768 - 1023 counts, 48 - 82
		;* table 4 1024 - 1279 counts, 82 - 135
		;* table 5 1280 - 1535 counts, 135 - 206
		;* table 6 1536 - 1791 counts, 206 - 255
		;*
		;*
		;*******************************************************
		ORG $C69A
LC69A: 	FCB 80 		; Ld VAR FOR SCALING LV8 SCALE FACTORS
					; CAL = arg + 64, (16)
LC69B: 	FCB 10 		; 1 Sec MAF BURN OFF TIME.
LC69C: 	FCB 50 		; 5 SEC MAF BURNOFF DELAY TIME FROM IGN OFF
		ORG $C69D
					;----------------------------------------
					; Mass Air Flow TABLE 1
					;
					; TBL = gms/Sec * 11.1
					;
					;----------------------------------------
LC69D: 	FCB 23 		; TABLE SCALAR
LC69E: 	FCB 8 		; 9 LINE TBL
					;----------------------------------------
					; gms/SeC BIN VDC #/HR
					;----------------------------------------
		FCB 0 		; 0.0 0 0.00 0
		FCB 36 		; 3.2 64 0.18 25
		FCB 50 		; 4.5 128 0.37 35
		FCB 69 		; 6.2 192 0.55 48
		FCB 93 		; 8.4 256 0.73 65
		FCB 121 	; 10.9 320 0.91 84
		FCB 155 	; 14.0 384 1.10 108
		FCB 197 	; 17.7 448 1.28 137
		FCB 248 	; 22.3 512 1.46 172
					;----------------------------------------
					;----------------------------------------
					; Mass Air Flow TABLE 2
					;
					; TBL = gms/Sec * 5.33
					;
					;----------------------------------------
LC6A8: 	FCB 48 		; TABLE SCALAR
LC6A9: 	FCB 8 		; LINE TBL
					;----------------------------------------
					; gms/SeC BIN VDC #/HR
					;----------------------------------------
		FCB 119 	; 22.3 512 1.46 172
		FCB 133 	; 25.0 544 1.55 193
		FCB 147 	; 27.6 576 1.65 213
		FCB 163 	; 30.6 608 1.74 236
		FCB 182 	; 34.1 640 1.83 264					<<<<<<<<<< This is different in BUA 86 vette ($84 or 132).  This value seems correct
		FCB 198 	; 37.1 672 1.92 287
		FCB 217 	; 40.7 704 2.01 314
		FCB 237 	; 44.5 736 2.10 343
		FCB 254 	; 47.7 768 2.19 368
					;----------------------------------------
					;----------------------------------------
					; Mass Air Flow TABLE 3
					;
					; TBL = gms/Sec * 3.08
					;----------------------------------------
LC6B3: 	FCB 83 		; TABLE SCALAR
LC6B4: 	FCB 8 		; 9 LINE TBL
					;----------------------------------------
					; gms/SeC BIN VDC #/HR
					;----------------------------------------
		FCB 147 	; 47.7 768 2.19 368
		FCB 158 	; 51.2 800 2.29 395
		FCB 170 	; 55.1 832 2.38 425
		FCB 182 	; 59.0 864 2.47 455
		FCB 195 	; 63.2 896 2.56 488
		FCB 209 	; 67.8 928 2.65 523
		FCB 223 	; 72.3 960 2.74 558
		FCB 238 	; 77.2 992 2.83 596
		FCB 253 	; 82.0 1024 2.93 633
					;----------------------------------------
					;----------------------------------------
					; Mass Air Flow TABLE 4
					;
					;
					; TBL = 1.90 * gms/Sec
					;----------------------------------------
LC6BE: 	FCB 135 	; TABLE SCALAR
LC6BF: 	FCB 8 		; 9 LINE TBL
					;----------------------------------------
					; gms/SeC BIN VDC #/HR
					;----------------------------------------
		FCB 156 	; 82.3 1024 2.93 635
		FCB 166 	; 87.5 1056 3.02 676
		FCB 176 	; 92.8 1088 3.11 716
		FCB 187 	; 98.6 1120 3.20 761
		FCB 198 	; 104.4 1152 3.29 806
		FCB 210 	; 110.7 1184 3.38 855
		FCB 222 	; 117.1 1216 3.47 903
		FCB 237 	; 125.0 1248 3.57 965
		FCB 255 	; 134.5 1280 3.66 1038
					;----------------------------------------
					;----------------------------------------
					; Mass Air Flow TABLE 5
					;
					;
					; TBL = 1.24 * gms/Sec
					;----------------------------------------
LC6C9: 	FCB 207 	; TABLE SCALAR
LC6CA: 	FCB 8 		; 9 LINE TBL
					;----------------------------------------
					; gms/SeC BIN VDC #/HR
					;----------------------------------------
		FCB 166 	; 134.2 1280 3.66 1036
		FCB 176 	; 142.3 1312 3.75 1098
		FCB 186 	; 150.4 1344 3.84 1161
		FCB 196 	; 158.5 1376 3.93 1223
		FCB 207 	; 167.4 1408 4.02 1292
		FCB 219 	; 177.1 1440 4.11 1367
		FCB 230 	; 186.0 1472 4.21 1435
		FCB 242 	; 195.7 1504 4.30 1510
		FCB 255 	; 206.2 1536 4.39 1591
					;----------------------------------------
					;----------------------------------------
					; Mass Air Flow TABLE 6
					;
					; TBL = 1.00 * gms/SEC
					;
					;----------------------------------------
LC6D4: 	FCB 255 	; TBL SCALAR
LC6D5: 	FCB 16 		; 16 LINE TBL
					;----------------------------------------
					; gms/SeC BIN VDC #/HR
					;----------------------------------------
		FCB 207 	; 206.2 1536 4.39 1591
		FCB 213 	; 212.2 1552 4.43 1637
		FCB 219 	; 218.1 1568 4.48 1684
		FCB 226 	; 225.1 1584 4.53 1737
		FCB 233 	; 232.1 1600 4.57 1791
		FCB 240 	; 239.1 1616 4.62 1845
		FCB 248 	; 247.0 1632 4.66 1906
		FCB 255 	; 254.0 1648 4.71 1960
		FCB 255 	; 254.0 1664 4.75 1960
		FCB 255 	; 254.0 1680 4.80 1960
		FCB 255 	; 254.0 1696 4.85 1960
		FCB 255 	; 254.0 1712 4.89 1960
		FCB 255 	; 254.0 1728 4.94 1960
		FCB 255 	; 254.0 1744 4.98 1960
		FCB 255 	; 254.0 1760 5.03 1960
		FCB 255 	; 254.0 1776 5.07 1960
		FCB 255 	; 254.0 1792 5.12 1960
					;----------------------------------------
					;-------------------------------------------------------
					; Mass Flow Filter & Limits
					;
					;
					;-------------------------------------------------------
		ORG $C6E7
LC6E7: 	FCB 32 		; Mult for sliding filter
LC6E8: 	FDB 2048 	; Min Out of MAF, (32 Hz)
					; CALIB = 65536/hZ
LC6EA: 	FDB 0768 	; Min Allowable Flow, (3 gms/Sec)
					; CALIB = (gm/sec) * 256
					;--------------------------------------------------
					; Max Air Flow vs RPM
					;
					; 17 LINE, 0 -> 6400 RPM
					;
					; TBL = Grams/Sec
					;---------------------------------------------------
		ORG $C6EC
					; gms/sec RPM
					;---------------------------
LC6EC: 	FCB 23 		; 23 0
		FCB 23 		; 23 400
		FCB 30 		; 30 800
		FCB 48 		; 48 1200
		FCB 68 		; 68 1600
		FCB 89 		; 89 2000
		FCB 111 	; 111 2400
		FCB 141 	; 141 2800
		FCB 170 	; 170 3200
		FCB 200 	; 200 3600
		FCB 220 	; 220 4000
		FCB 236 	; 236 4400
		FCB 245 	; 245 4800
		FCB 247 	; 247 5200
		FCB 247 	; 247 5600
		FCB 247 	; 247 6000
		FCB 255 	; 255 6400
					;---------------------------------------------------
		;**************************************************
		;* >>> Serial Data Tables & Params <<<
		;*
		;* MY 86 TYPE $32, ECM P/N 1227165
		;***************************************************
		ORG $C6FD
LC6FD: 	FCB 4 		; Dis String length, (Bytes)
					;--------------------------------------------------
					; DIS Addresses, Xmit
					;
					;--------------------------------------------------
LC6FE: 	FDB $C009 	; Num O cyl Addr
		FDB $011A 	; Running total of fuel supplied Addr
		FDB $011E 	; Running total of Dist. Traveled Addr
		FDB $C70C 	; Gal's/Sec Scale Factor Addr
		FDB $0000 	;
		FDB $0000 	;
		FDB $0000 	;
					;------------------------------------------------------
					;------------------------------------------------------
					; DISPLAY Fuel Rate Scale Factors
					;
					;------------------------------------------------------
LC70C: 	FCB 122 	; Gals/Hr Inj flow rate
					; CALIB = Arg * 32, (3.8125 G/Hr)
		;*******************************************************
		;* 160 BAUD DIAGNOSTICS
		;* Table Of Addr's vs DATA
		;*
		;*******************************************************
		ORG $C70D
LC70D: 	FDB $C000 	; LOC 2, PROM ID (16 bits)
		FDB $C001 	; LOC 3, MSB
		FDB $002C 	; LOC 4, IAC Present Posit.
		FDB $005D 	; LOC 5, Coolant temp, (A/D)
		FDB $0065 	; LOC 6, Filtered MPH
		FDB $0112 	; LOC 7, EGR D/C
		FDB $0057 	; LOC 8, RPM
		FDB $0081 	; LOC 9, TPS (A/D)
		FDB $00C6 	; LOC 10, Base FI Pulse Width
		FDB $006F 	; LOC 11, Filtered o2 Sig
		FDB $0005 	; LOC 12, Err Flg #1, (Sensors0
					;---------------------------------
					; Bit 0 = ERR 23 MAT SENSOR LOW
					; 1 = ERR 22 TPS LOW
					; 2 = ERR 21 TPS HIGH
					; 3 = ERR 16 NOT USED
					;
					; 4 = ERR 15 COOL SENSOR LOW TEMP.
					; 5 = ERR 14 COOL SENSOR HIGH TEMP.
					; 6 = ERR 13 O2 SENSOR
					; 7 = ERR 12 NO REF PULSES
					;---------------------------------
					;
		FDB $0006 	; LOC 13, Err Flg #2
					;
					; Bit 0 = ERR 35 NOT USED
					; 1 = ERR 34 MAF SENSOR LOW
					; 2 = ERR 33 MAF SENSOR HIGH
					; 3 = ERR 32 EGR DIAG.
					;
					; 4 = ERR 31 NOT USED
					; 5 = ERR 26 NOT USED
					; 6 = ERR 25 MAT SENSOR HIGH
					; 7 = ERR 24 VSS
					;---------------------------------
		FDB $0007 	; LOC 14, Err Flg #3
					;
					; Bit 0 = ERR 51 PROM ERROR
					; 1 = ERR 46 VATS FAILED
					; 2 = ERR 45 O2 SENSOR RICH
					; 3 = ERR 44 O2 SENSOR LEAN
					;
					; 4 = ERR 43 ESC FAILURE
					; 5 = ERR 42 EST ERROR
					; 6 = ERR 41 CYL SELECT ERROR
					; 7 = ERR 36 BURNOFF DIAG.
					;---------------------------------
		FDB $0008 	; L0C 15, Err Flg #4
					;
					; Bit 0 = ERR 63 NOT USED
					; 1 = ERR 62 NOT USED
					; 2 = ERR 61 NOT USED
					; 3 = ERR 56 NOT USED
					;
					; 4 = ERR 55 NOT USED
					; 5 = ERR 54 FUEL PUMP VOLTAGE
					; 6 = ERR 53 OVER VOLTAGE
					; 7 = ERR 52 CAL PAC MISSING
					;---------------------------------
		FDB $0044 	; 16, MAF Flow
		FDB $0060 	; 17, MAT
		FDB $0037 	; 18, MCU Stat Flf, (Xmission etc)
		FDB $00A1 	; 19, ECU PA3 Counter
		FDB $00C0 	; 20, BLM Mult.
		FDB $00F1 	; 21, Rich/Lean Counter
		FDB $00EA 	; 22, Decell Fuel Gms/Sec
		FDB $00EB 	; 23, LSB
		FDB $00D5 	; 24, Base PW (Last Inj), 16 bits
		FDB $00D6 	; 25, LSB
		;*******************************************************
		;*******************************************************
		;* MODE 0, REVERT TO NORMAL MODE.
		;*
		;* ALDL DEVICE MUST REQUEST MODE 0 BY
		;* XMITING THE FOLLOWING MSG TO
		;* THE ECM:
		;*
		;* MSG ID = $80
		;* MSG LEN 1+85 = $56
		;* MODE = $00
		;* CKSUM = $29
		;*
		;* THE ECM WILL RESPOND WITH:
		;*
		;* MSG ID = $80
		;* MSG LEN 64+85 = $95
		;* MODE = $00
		;* CKSUM = $cc
		;*
		;*******************************************************
		ORG $C73D
LC73D: 	FDB $0000 	; Next Msg entry address
					;
		FCB $80 	; DEVICE ID Code
					;
		FCB $00 	; Option Flag Word
					;
		FCB 1 		; Output Msg Length
		FCB 1 		; Input Msg Length
					;
		FDB $016F 	; Addr of output cnt'l block, (1 Byte long)
		FDB $0133 	; Addr of input cnt'l block, (29 bytes long)
					;------------------------------------------------------
					; ADDR'S OF DIFFERENT MODE BLOCKS
					;
					; Table Of Addr's vs DATA
					;
					;------------------------------------------------------
		ORG $C747
LC747: 	FDB $C73D 	; MSG CODE $80 Mode 0 Entries
		FDB $C751 	; MSG CODE $80 Mode 1 Entries
		FDB $C7D9 	; Msg Code $80 Mode 2 Entries
		FDB $C7E3 	; Msg Code $80 Mode 3 Entries
		FDB $C7ED 	; Msg Code $80 Mode 4 Entries
					;------------------------------------------------------
		;*******************************************************
		;* MODE 1 FIXED DATA DATA STREAM
		;* ALDL DEV MUST REQUEST MODE 1 BY
		;* XMITING THE FOLLOWING MSG TO
		;* THE ECM:
		;*
		;* MSG ID = $80
		;* MSG LEN 1+85 = $56
		;* MODE = $01
		;* CKSUM = $29
		;*
		;* THE ECM WILL RESPOND WITH:
		;*
		;* MSG ID = $80
		;* MSG LEN 64+85 = $95
		;* MODE = $01
		;* DATA BYTE 1 = $XX
		;* .
		;* .
		;* BATA BYTE 63 = $XX
		;* CKSUM = $cc
		;*
		;*******************************************************
LC751: 	FDB $0000 	; Nxt Msg Entry Addr
					;
		FCB $80 	; DEVICE ID Code
					;
		FCB $80 	; RAM/ROM FLAG
					;
		FCB 64 		; Output Msg Lenght
		FCB 1 		; Input Msg Length
					;
		FDB $016F 	; Addr of Output Cnt'l Block
		FDB $0133 	; Addr of Input Cnt'l Block
					;
LC75B: 	FDB $C000 	;1, PROM ID MSB
		FDB $C001 	;2, PROM ID LSB
					;
		FDB $0005 	;3, Err Flg #1
					;
					; Bit 0 = ERR 23 MAT SENSOR LOW
					; 1 = ERR 22 TPS LOW
					; 2 = ERR 21 TPS HIGH
					; 3 = ERR 16 NOT USED
					;
					; 4 = ERR 15 COOL SENSOR LOW TEMP.
					; 5 = ERR 14 COOL SENSOR HIGH TEMP.
					; 6 = ERR 13 O2 SENSOR
					; 7 = ERR 12 NO REF PULSES
					;---------------------------------
		FDB $0006 	;4, Err Flg #2
					;
					; Bit 0 = ERR 35 NOT USED
					; 1 = ERR 34 MAF SENSOR LOW
					; 2 = ERR 33 MAF SENSOR HIGH
					; 3 = ERR 32 EGR DIAG.
					;
					; 4 = ERR 31 NOT USED
					; 5 = ERR 26 NOT USED
					; 6 = ERR 25 MAT SENSOR HIGH
					; 7 = ERR 24 VSS
					;---------------------------------
		FDB $0007 	;5, Err Flg #3
					;
					; Bit 0 = ERR 51 PROM ERROR
					; 1 = ERR 46 VATS FAILED
					; 2 = ERR 45 O2 SENSOR RICH
					; 3 = ERR 44 O2 SENSOR LEAN
					;
					; 4 = ERR 43 ESC FAILURE
					; 5 = ERR 42 EST ERROR
					; 6 = ERR 41 CYL SELECT ERROR
					; 7 = ERR 36 BURNOFF DIAG.
					;---------------------------------
		FDB $0008 	;6, Err Flg #4
					;
					; Bit 0 = ERR 63 NOT USED
					; 1 = ERR 62 NOT USED
					; 2 = ERR 61 NOT USED
					; 3 = ERR 56 NOT USED
					;
					; 4 = ERR 55 NOT USED
					; 5 = ERR 54 FUEL PUMP VOLTAGE
					; 6 = ERR 53 OVER VOLTAGE
					; 7 = ERR 52 CAL PAC MISSING
					;---------------------------------
		FDB $0009 	;7, Err Flg #5
					;
					; Bit 0 = NOT USED
					; 1 = NOT USED
					; 2 = NOT USED
					; 3 = NOT USED
					;
					; 4 = NOT USED
					; 5 = ERR 66 NOT USED
					; 6 = ERR 65 NOT USED
					; 7 = ERR 64 NOT USED
					;---------------------------------
					;
		FDB $005D 	;8, Coolant, A/D
		FDB $005F 	;9, Strt up Coolant, Deg C
		FDB $0081 	;10, TPS, A/D
		FDB $0057 	;11, RPM, Var
					;
		FDB $0095 	;12, Ref Period
		FDB $0096 	;13, LSB
					;
		FDB $0065 	;14, MPH Var for Disp.
		FDB $0066 	;15, LSB
					;
		FDB $008F 	;16, N/V Ratio
		FDB $006F 	;17, o2 Filtered
		FDB $00F1 	;18, ALDL Rich/Lean Counter
		FDB $00C6 	;19, Base PW Correction
		FDB $00C0 	;20, Blk Learn, Scaled
		FDB $00BF 	;21, Blk Learn
		FDB $00C9 	;22, Clsd/Lp Int.
		FDB $002C 	;23, IAC Present Posit.
		FDB $0101 	;24, IAC Step Dir Cmd
		FDB $0102 	;25, Requested Idle Spd.
		FDB $0063 	;26, Filtered Ld Val
		FDB $0064 	;27, LSB
		FDB $0061 	;28, Raw Ld Val
		FDB $0060 	;29, MAT Val
		FDB $012B 	;30, MAT Val, A/D
		FDB $0112 	;31, EGR D.C.
		FDB $0113 	;32, Can Prg D.C.
		FDB $00F4 	;33, Fan D.C.
		FDB $007E 	;34, Batt Vlts, A/D
		FDB $007F 	;35, Pump VDC
					;
		FDB $00EA 	;36, Fuel Gms/Sec, 16 bits
		FDB $00EB 	;37, LSB
					;
		FDB $00B6 	;38, UNLIMITED AIRFLOW, (MAF Diag)
					;
		FDB $0115 	;39, Tot Sprk Adv rel to TDC
		FDB $0116 	;40, LSB
					;
		FDB $009D 	;41, Sprk Adv rel to TDC
		FDB $009E 	;42, LSB
					;
		FDB $00A1 	;43, ECU Cnt'r
		FDB $00A5 	;44, Knock Retard
					;
		FDB $00D5 	;45, Base PW, Last Inject
		FDB $00D6 	;46, LSB
					;
		FDB $00CE 	;47, Total Air/Fuel Val
		FDB $00CF 	;48, LSB
					;
		FDB $011A 	;49, Run tot Fuel Consumed
		FDB $011B 	;50, LSB
					;
		FDB $011E 	;51, Run tot Dist Run, (.005mi/bit)
					;
		FDB $001A 	;52, Eng Run Time, Sec
		FDB $001B 	;53, LSB
					;
					;---------------------------------
		FDB $0035 	;54, Minor Lp mode word #2
					;
					; Bit 0 = 1= OVERDRIVE ON
					; 0= OVERDRIVE OFF
					; 1 = ERR 14 or 15 THIS STARTUP
					; 2 = REF PULSES OCCURRED (6.25 MSEC CHECK)
					; 3 = 1= ALDL MODE, 8192 LOCKED IN, & MODE 4
					;
					; 4 = DIAGNOSTIC SWITCH IN DIAGNOSTIC POSITION
					; 5 = DIAGNOSTIC SWITCH IN ALDL POSITION
					; 6 = HIGH BAT. VOLT. , DISABLE SOLENOID DISCRETS
					; 7 = SHIFT LIGHT (1= ON, 0= OFF)
					;---------------------------------
		FDB $0037 	;55, MPU Status wd, (Fan, P.S. etc)
					;
					; Bit 0 = PARK/NEUTRAL MODE
					; 1 = NOT IN THIRD GEAR
					; 2 = OVERDRIVE REQUEST
					; 3 = EXCESSIVE POWER STEER PRES.
					; , CLUTCH ANTICIPTE
					;
					; 4 = EGR DIAGNOSTIC SWITCH CLOSED
					; 5 = TCC LOCKED
					; 6 = FAN REQUEST BIT
					; 7 = 0 = A/C REQUEST
					;---------------------------------
		FDB $002E 	;56, Mode wd
					;
					; Bit 0 = PARK/NEUTRAL (0 = DRIVE)
					; 1 = 3rd GEAR
					; 2 = 4th GEAR
					; 3 = POWER STEERING ( 1 = CRAMP)
					;
					; 4 = READ BUT NOT USED
					; 5 = EGR DIAGNOSTIC
					; 6 = FAN REQUEST <=> ACHP (A/C HI PRESSURE)
					; 7 =AIR CONDITIONER ( 0 = A/CREQUESTED)
					;---------------------------------
		FDB $0034 	;57, Minor Lp wd #1
					;
					; Bit 0 = ADVANCE FLAG ( 0= ADV. , 1= RTD)
					; 1 = CHK ENGINE LIGHT DELAY FLAG
					; 2 = INTERRUPT SERVICE EXECUTION EXCEED 6.25 MSEC
					; 3 = FAN ON DISABLED BY PID
					;
					; 4 = TCC ROAD SPEED 1st PULSE FLAG
					; 5 = A/C CLUTCH FLAG ( 0= A/C CLUTCH ON)
					; 6 = BYPASS CHECK ENABLE
					; 7 = ENGINE RUNNING FLAG ( 1= RUNNING)
					;
					;---------------------------------
		FDB $0001 	;58, Non Vol Mem Mode wd
					;
					; Bit 0 = O2 SENSOR READY
					; 1 = CLOSED LOOP TIMER TIMED OUT
					; 2 = NOT USED
					; 3 = IMPROPER SHUTDOWN
					;
					; 4 = NOT USED
					; 5 = IAC KICKDOWN ENABLED
					; 6 = KWARM KICKDOWN ENABLED
					; 7 = ERR 42 FAILED
					;
					;---------------------------------
		FDB $0004 	;59, MANUAL Xmission Mode wd
					;
					; Bit 0 = OVERDRIVE ON
					; 1 = 1st GEAR DIRECT
					; 2 = 1st GEAR LOOK-AHEAD OK
					; 3 = 4th GEAR
					;
					; 4 = 1st GEAR OVERDRIVE
					; 5 = DOWNSHIFT/OFF REQUEST
					; 6 = UPSHIFT/ON REQUEST
					; 7 = OVERDRIVE ACTIVE
					;
					;---------------------------------
		FDB $003C 	;60, ALCL Mode Wd.
					;
					; Bit 0 = ALDL RESET
					; 1 = NOT USED
					; 2 = FIELD SERVICE MODE
					; 3 = FIRST C/L PASS THRU FIELD SERVICE MODE DONE
					; (1= DONE)
					;
					; 4 = FIELD SERVICE. O2 TRANSITION
					; 5 = ONE SECOND FLAG (SYMMETRICAL)
					; 6 = 200 MSEC TOGGLE BIT FOR 2.5 HZ FSM FLASH RATE
					; 7 = PULLUP RESISTOR FOR COOL
					;---------------------------------
		FDB $003E 	;61, Mjr Lp mode wd
					;
					; Bit 0 = 100 MSEC, OLD CCP PURGE ON FLAG (0= OFF)
					; 1 = AIR CONTROLLED, 0= AIR DIVERTED
					; 2 = AIR SWITCHED TO PORT
					; 3 = NOT USED
					;
					; 4 = SKIP BURNOFF DUE TO > 17 VOLTS THIS STARTUP
					; 5 = D.E. QSEC
					; 6 = BURN OFF AIR METER
					; 7 = DECEL ENLEANMENT
					;
					;---------------------------------
		FDB $0046 	;62, Serial Data Mode Word
					;
					; Bit 0 = EXPECTING FIRST 160 BAUD INTERRUPT
					; 1 = EXPECTING SECOND 160 BAUD INTERRUPT
					; 2 = IN 8192 MODE
					; 3 = LOCKED IN 8192 MODE
					;
					; 4 = NOT USED
					; 5 = NOT USED
					; 6 = NOT USED
					; 7 = NOT USED
					;
					;---------------------------------
		FDB $0044 	;63, Flg Word Fuel/Air Mode word
					;
					; Bit 0 = NOT USED
					; 1 = LEARN CONTROL FLAG
					; (1= ENABLE STORE, 0= DISABLE)
					; 2 = NOT USED
					; 3 = NOT USED
					;
					; 4 = VSS FAILURE
					; 5 = EECC SLOW O2 RICH/LEAN FLAG
					; 6 = RICH/LEAN FLAG (1= RICH , 0= LEAN)
					; 7 = CLOSED LOOP FLAG (1= C/L , 0= O/L)
					;---------------------------------
					;-----------------------------------------------------
		;*******************************************************
		;* MODE 2 SELECTABLE MEMORY DUMP
		;*
		;* ALDL DEV MUST REQUEST MODE 2 BY
		;* XMITING THE FOLLOWING MSG TO
		;* THE ECM:
		;*
		;* MSG ID = $80
		;* MSG LEN 3+85 = $58
		;* MODE = $02
		;* START ADD MSB = $aa
		;* START ADD LSB = $aa
		;* CKSUM = $cc
		;*
		;* THE ECM WILL RESPOND WITH:
		;*
		;* MSG ID = $80
		;* MSG LEN 65+85 = $96
		;* MODE = $03
		;* ADD CONT = $dd
		;* .
		;* .
		;* ADD (+63) CONT = $dd
		;* CKSUM = $cc
		;*
		;*******************************************************
		ORG $C7D9
LC7D9: 	FDB $0000 	; Nxt Msg Entry Addr
		FCB $80 	; DEVICE Code
		FCB $40 	; Option Flag Word
		FCB $40 	; ????
		FCB 3 		; Input Msg Length
		FDB $0150 	; Addr of Output Cnt'l Block
		FDB $0133 	; Addr of Input Cnt'l Block
					;------------------------------------------------------
		;*******************************************************
		;* MODE 3 RAM DUMP 0 to 8 ADDRESSES
		;*
		;* ALDL DEV MUST REQUEST MODE 3 BY
		;* XMITING THE FOLLOWING MSG TO
		;* THE ECM:
		;*
		;* MSG ID = $80
		;* MSG LEN 2n+85 = $58
		;* MODE = $03
		;* ADD MSB = $aa
		;* ADD LSB = $aa
		;* .
		;* .
		;* ADD MSB = $aa
		;* ADD LSB = $aa
		;* CKSUM = $cc
		;*
		;* THE ECM WILL RESPOND WITH:
		;*
		;* MSG ID = $80
		;* MSG LEN n+1+85 = $nn
		;* MODE = $03
		;* ADD CONT = $dd
		;* ADD CONT = $dd
		;* .
		;* .
		;* ADD CONT = $dd
		;* ADD CONT = $dd
		;* CKSUM = $cc
		;*
		;*
		;*******************************************************
		ORG $C7E3
LC7E3: 	FDB $0000 	; Nxt Msg Entry Addr
					;
		FCB $80 	; DEVICE ID Code
					;
		FCB $40 	; RAM/ROM Flag Word
					;
		FCB 9 		; Output Msg Length
		FCB 17 		; Input Msg Length
		FDB $0150 	; Addr of Output Cnt'l Bock
		FDB $0133 	; Addr of Input Cnt'l Block
					;------------------------------------------------------
		;*******************************************************
		;* MODE 4 MODIFIER OP'S
		;*
		;* MODE 4 WILL MODIFY ECM OPERATION
		;* & XMIT THE CONTENTS OF RAM,
		;* (0 -> 8 LOC'S) AS REQUESTED
		;*
		;* ALDL DEV MUST REQUEST MODE 4 BY
		;* XMITING THE FOLLOWING MSG TO
		;* THE ECM:
		;*
		;* MSG ID = $80
		;* MSG LEN 2n+1+10+85 = $nn
		;* MODE = $04
		;* CNTL WD 1 = $cc
		;* .
		;* .
		;* CNTL WD 10 = $cc
		;* ADD 1 MSB = $aa
		;* ADD 1 LSB = $aa
		;* .
		;* .
		;* ADD N MSB = $aa
		;* ADD N LSB = $aa
		;* CKSUM = $cc
		;*
		;* THE ECM WILL RESPOND WITH:
		;*
		;* MSG ID = $80
		;* MSG LEN n+1+85 = $nn
		;* MODE = $04
		;* ADD CONT = $dd
		;* .
		;* .
		;* ADD CONT = $dd
		;* CKSUM = $cc
		;*
		;*
		;*
		;*******************************************************
		ORG $C7ED
LC7ED: 	FDB $0000 	; Nxt Msg Entry Addr
					;
		FCB $80 	; DEVICE IDCode
					;
		FCB $40 	; RAM/ROM Flag Word
					;
		FCB 9 		; Output Msg Length
		FCB 27 		; Input Msg Length
					;
		FDB $0150 	; Addr of Output Cnt'l Bock
		FDB $0133 	; Addr of Input Cnt'l Block
					;--------------------------------------------------
		ORG $C7F7
LC7F7: 	FCB 30 		; If no Comm >= 30 Sec Force mode 0
					; CALIB = SEC'S
L07F8: 	FCB $0 		;
L07F9: 	FCB $0 		;
L07FA: 	FCB $0 		;
L07FB: 	FCB $0 		;
L07FC: 	FCB $0 		;
L07FD: 	FCB $0 		;
L07FE: 	FCB $0 		;
L07FF: 	FCB $0 		;
		;*******************************************************
		;* END OF CALIB TABLES
		;*******************************************************
		;***************************************************
		;* BUA/9340 P/N 16059335 8/1/85 P
		;* SS BY BUA/1728 P/N 1227165
		;* REV 8/1/95, WS
		;*
		;* ECM P/N 1227165
		;*
		;* MY86 YB L98 MD8 NM8 VE1 G44
		;*
		;* GM P4 computer system
		;*
		;* RAM Locations $0000 - $00FF
		;* NON VOL RAM Locations
		;*
		;* MPU Status Reg's $3FFA
		;* MPU Control Reg's $3FFC
		;*
		;***************************************************
		ORG $C800
LC800: 	LDS #$01FF 				; Set Up Stack
		JSR LF3B5 				; 11 Usec Delay
		LDAA #$08 				; 0000 1000, Bit 3
		STAA $5000 				; Set Log RAM to MPU
		LDX #$0A 				;
		BRCLR $03 $08 LC815 	; Mode Wd3, ERR 51
								; ... else
		LDX #$02 				; 0000 0010
LC815: 	STX $3FFC 				; MCU CSR
								;
								; Int RAM 3FC0 - 3FFA, 58 BYTES ?
								;
		LDX #$3FC0 				;
		CLRA 					; Clr D Reg
		CLRB 					;
LC81D: 	STD 0,X 				; CLR RAM
		INX 					;
		INX 					;
		CPX #$3FFA 				; Done ?
		BNE LC81D 				; Loop if not
								; ... else
		;***************************************************
		;* Set up MPU Devices
		;*
		;***************************************************
								;--------------------------------------------------
								; BIT 0 = SFO
								; BIT 1 = SF1
								; SFO SF1 Frq Hz
								; 0 0 542,288
								; 1 0 262,144
								; 0 1 131,072
								; 1 1 65,536
								;
								; BIT 2 = SPO
								; BIT 3 = SP1
								; SPO SP1 Frq Hz
								; 0 0 542,288
								; 1 0 1,048,576
								; 0 1 1,572,864
								; 1 1 2,097,152
								; BIT 4 =
								; BIT 5 =
								;
								; BIT 6 = 0 = SI,
								; 1 = PRP for Serial Input
								;
								; BIT 7 = Ready Bit
								; 0 = Init TX/Rx Bits
								; 1 = Xmiter Ready For Next Byte
								; Serial Cnt'l set this to 1
								;
		LDAA #$8C 				; 1000 1100
		STAA $4001 				; Set Prescaler Serial Port Val
								;----------------------------------------------------------
								;----------------------------------------------------------
								; DDR REGESTER
								;
								; 1 = INPUT
								; 0 = OUTPUT
								;
		LDAA #$8F 				; 1000 1111,
		STAA $4003 				; Set DDR Direction
								;----------------------------------------------------------
								;----------------------------------------------------------
								; BIT 0 = Input is Sync fm RX
								; BIT 1 = Output to DO 1 or Tmr Cnt'l PW mod
								; BIT 2 = Output to DO 2
								; BIT 3 = Output to DO 3
								;
								; BIT 4 = SSO
								; BIT 5 = SS1
								; SSO SS1 BAUD RATE
								; 1 0 8192
								; 0 1 1024
								; 1 1 256
								;
								; BIT 6 = Match enab Bit
								; BIT 7 = Not Used
								;
		LDAA #$90 				;1001 0000
		STAA $4004 				;Set Baud = 8192
								;----------------------------------------------------------
								;
								; Test A/D Operation
								;
		LDAA #$B0 				; A/D Ch TST Ch.
		JSR LF1C3 				; To A/D routine
								;
								; Clr RAM ?
								; RAM $01B2 - $002D, 185 BYTES
								;
		LDX #$01B2 				; Last Used RAM Loc
LC83D: 	CLR 0,X 				; Clear RAM
		DEX 					; Bump Addr Down
		CPX #$002D 				; Done ?
		BNE LC83D 				; If Not Loop
								;
								; Enable SXR Chip
								; Par I/O
								;
		LDAA $4004 				; Get CSR
		ORAA #$08 				; Set b3
		STAA $4004 				; Wr Back
								;
								; Serial I/O (SPI) Init
								;
		LDX #$4002 				; Output Data Latch
		BSET 0,X 04	 			; SELECT FMD
		LDAA #$04 				; b2, EST ENABLE
		STAA $30 				; Save Status to NON VOL RAM
		JSR LF1F9 				; Xmit to Serial Data
		BCLR 0,X 04 				; De-select SPI
		STAA $49 				; FACTORY TST FMD BYTE 1
								;-------------------------------------
								;
								; Check Socketed device
								;
		LDX #$C008 				;
		LDD #$3FF8 				; B Cnt'r Last H.U LF32E 	; To Ck SUM
		JSR LF32E				; To CHKSUM
		LDAA LC008 				; Get Stored CK SUM
		CMPA #$AA 				; IF its AN $AA Bypass
		BEQ LC879 				; Xit & Clr ER #51 Flg
								; ... else
		CMPA #$32 				; Ck for correct EPROM
		BNE LC87E 				; If Not exit w/error
								; ... else
		CPY LC006 				; CK CK SUM VAL
		BNE LC87E 				; Bad Ck Sum, Xit w/Error
								; ... else
LC879: 	BCLR $47 $10 			; Clr Er #51 Flg
		BRA LC881 				; Exit Ck Sum test
LC87E: 	BSET $47 $10 			; Set Err, (#51)
LC881: 	LDX #$FB1A 				; Init MPU
		BRCLR $03 $08 LC88B 	; Mode Wd3, ERR 51
								; ... else
		LDX #$FB12 				; 64,274
LC88B: 	STX $3FFC 				; MCU CSR
								;
								; A/D BATTERY VOLTAGE
								;
		LDAA #$10 				; Set Up For Batt Volts, A/D CH1
		JSR LF1BE 				; To A/D
		STAA $7E 				; Batt Volts, (A/D)
		CMPA #100 				; 10 VDC
		BCC LC8E2 				; IF LT 10 VDC, EXIT via HU CHECK
								; ... else
		BSET $40 $20 			; set B5, ERR 54A
								; ... Force Default for Pump Volts
								;
								; CK FUEL PUMP VOLTAGE
								;
		JSR LE816 				; To Pump Subroutine
		LDAA $7F 				; Pump VDC, (A/D)
		CMPA #160 				; 16.0 VDC
		BCS LC8E2 				; IF GT 16 VDC. EXIT via HU CHECK
								; ... else
								;
								; CK DIAG MODE STATUS
								; (IF DIAG V gt 0.8V & lt 2V, SET FACT TEST)
								;
		LDAA #$70 				; A/D Ch 7 DIAG SW STATUS
		JSR LF1BE 				; To A/D Subroutine
		CMPA #100 				; 2.0 VDC
		BCC LC8E2 				; IF GT 2.0 VDC, EXIT via HU CHECK
								; ... se
		CMPA #40 				; 800 Mvdc
		BCS LC8E2 				; IF LT 800 mvdc, EXIT via HU CHECK
								; ... else
		BSET $47 $80  			; Set b7, FACTORY Tst MODE. <---***
		BSET $31 $04  			; Enable SPK
								;--------------------------------------------------
								;
								; DO GM IEEE SUM, $C000 --> $FFFF
								; $4000 IN LENGTH
								;
		LDX #$C000 				; START OF EPROM
		LDD #$4000 				; LENGTH OF EPROM
		JSR LF32E 				; To Ck Sum Subroutine
		STY $0173 				; IEEE CK SUM
								;
								; INIT IAC & IGN OFF TIMER
								;
		LDAA #204 				;
		STAA $0179 				; IAC MOT
		STAA $32 				; IGN TIMER
								;
								;
								;
		LDAA $49 				; FACTORY TST FMD BYTE 1
		ANDA #$03 				; b1 & b0,
		BNE LC8DF 				; BR IF NOT b0 & b1
								; ... else
		LDX #$0000 				;
		LDAA #$AA 				;
LC8D7: 	STAA 0,X 				;
		INX 					;
		CPX #$002E 				;
		BNE LC8D7 				;
								; ... else
LC8DF: 	JMP LC9E3 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; CK IF HEADS UP PRESENT
								;
								;--------------------------------------------------
LC8E2: 	LDX $5800 				; H.U. ROM Addr
		CPX #$7E58 				; HU ID CODE
		BNE LC8ED 				; BR IF NO HEADS UP
								; ... else
		JSR $5812 				; H.U. ROM Addr
								;--------------------------------------------------
LC8ED: 	JSR LF3A7 				; Call RAM ERR Wd's Ck Sum
		CPD $18 				; Ck Sum OF Err Words
		BEQ LC90E 				; BR IF Z
								; ... else
								;--------------------------------------------------
								; Init RAM (Non Vol)
								;
								;--------------------------------------------------
								;
								; CLR RAM, $002D -> $0000
								;
		LDX #$002D 				; POINT TO TOP ADDRESS
LC8F8: 	CLR 0,X 				; CLEAR MEM LOCATION
		DEX 					; DECR ADDRESS POINTER
		BNE LC8F8 				; TILL DONE
								; ... else
								;
								; DO ERR WD CKSUM
								;
		JSR LF3A7 				; Call NON VOL RAM ERR Wd's Ck Sum
		STD $18 				; Save Ck Sum OF Err Words
								;
								; INIT BLM CELLS
								;
		JSR LF434 				; To INIT BLM Init.
								;
								; INIT IAC
								;
		LDAA LC62F 				; 144 IAC Idle Spd Start up park posit.
		STAA $2C 				; Save Strt up IAC Posit
		LDAA #$40 				; b6
		STAA $3D 				; Set NV RAM Bad Flg
								;--------------------------------------------------
								;--------------------------------------------------
								;
								;
								;--------------------------------------------------
LC90E: 	LDAA $47 				; MD WD
		BITA #$10 				; b4
		BEQ LC93B 				; BR IF NOT b4,
		LDAA $03 				; Get MD WD3
		ORAA #$08 				; Set Err b3, ERR 51
		STAA $03 				;
		JSR LF498 				; Else Lite Ck Eng Lamp
		BSET $07 $01 			; Err #3 Flg word
		LDAA $47 				;
		ORAA #$10 				; Set b4
		STAA $47 				;
		JSR LF3A7 				; ERR Wd's Ck Sum
		STD $18 				; Save Ck Sum OF Err Words
		LDAA $47 				;
		BMI LC93B 				; BR IF
								;
								; CK DIAG SW STATUS
								;
		LDAA #$70 				; A/D Ch 7 DIAG SW STATUS
		JSR LF1BE 				; To A/D Subroutine
		CMPA #40 				;
		BCS LC93B 				; BR IF A/D RESULT L.T. 88 MVDC
		;*******************************************************
		;* WAIT HERE FOR INTERUPT
		;*
		;*******************************************************
LC938: 	SWI 					;
		BRA LC938 				;
		;*******************************************************
LC93B: 	BSET $3C $80 			; SET b7
		JSR LF3B6 				; TO SEG 6, Log RAM to MCU, Cool A/D
		BRSET $01 $08 LC96A 	; BR IF b3,
								; ... else
								;--------------------------------------------------
								; LK UP Start up Enrich vs Cool Temp
								;
								;
								; Tbl Val = Pct Chg * (256/100)
								;--------------------------------------------------
		LDAA $5B 				; COOLANT
		CMPA #208 				; 115c
		BLS LC94D 				; BR IF COOL GT 115c
								; ... else
		LDAA #208 				; USE 115c MAX FOR LK UP
LC94D: 	PSHA 					;			
		LDX #$C3E4 				; INDEX START UP ENRICH Tbl
		JSR LF2C6 				; Call 2d Lk Up, (No Offset)
		CLRB 					;
		STD $0E 				; SAVE START UP ENRICH %CHG
								;--------------------------------------------------
								;--------------------------------------------------
								; LK UP Opn Lp AFR % Chg vs Cool Temp.
								;
								; Tbl = Pct Chg * 2.56 + AF BIAS
								;--------------------------------------------------
		PULA 					; GET COOL VAL FM STX
		PSHA 					;
		LDX #$C41F 				; INDEX UP Opn Lp AFR % Chg Tbl
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		STAA $CD 				; Cool Temp, AFR Ratio
								;--------------------------------------------------
								;--------------------------------------------------
								; LK UP Strt Up Spark Adv vs Cool Temp.
								;
								; TBL = SA deg * (256/90)
								;--------------------------------------------------
		PULA 					; GET COOL VAL FM STX
		LDX #$C17A 				; Strt Up Spark Adv Tbl
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		STAA $13 				; Spark Adv (START UP)
								;--------------------------------------------------
								;--------------------------------------------------
								;
								;
								;
								;--------------------------------------------------
LC96A: 	LDAA $5D 				; RESTART COOLANT
		BCLR $3B $10 			; CLR b4
		CMPA LC02C 				; Hot Restart Hi Lmt, (54c)
		BCS LC97A 				; BR IF RESTART COOL LT 54c
								; ... else
		LDAB $01 				;
		ORAB #$40 				; SET b6
		STAB $01 				;
LC97A: 	CMPA LC1FA 				; ERR # 32 Params, 31c QUAL
		BCC LC990 				; BR IF COOL G.T. 31c
								; ... else
		LDX $1A 				; Eng Run Time (sec)
		BNE LC990 				; BR IF N/Z
								; ... else
		JSR LF1E0 				;
		BITA #$20 				;
		BNE LC990 				; BR IF b5
								; ... else
		LDAA $41 				; DIAGNOSTIC MODE WD 3
		ORAA #$20 				; SET b5, EGR DIAG SW BAS AT START UP,
								; ... ERR 32
		STAA $41 				; DIAGNOSTIC MODE WD 3
LC990: 	LDAA LC3A0 				; o2 filter init val
		STAA $6F 				; FILT o2 VDC
		STAA $71 				; FILT o2 VDC
		STAA $73 				; FILT o2 VDC
								;-------------------------------------------------
								;
								; GET TPS A/D & PROCESS
								;
		LDAA #$50 				; A/D Ch 5, (TPS Voltage)
		JSR LF1BE 				; To A/D Subroutine
		STAA $81 				; TPS, (A/D)
		LDAA LC39B 				; Min TPS Posit
		STAA $86 				; Fltered low TPS, (A/D)
		JSR LF35A 				; To TPS Load Subroutine
		LDAA $7E 				; Batt Volts Value, (A/D)
		JSR LDE72 				; TO MAJOR LOOP SUNBOUTINE, Seg E
								; W/BATTERY A/D VALUE
		LDD $3FC8 				; SPK Period
		STD $B2 				; OLD PA2 CNT'R
		LDD LC3CB 				; 445d, Get Stoch ratio
		STD $CE 				; AFR
		LDAA #08 				;
		BITA 01 				; b0
		BNE LC9C0 				; BR IF b0
								; ... else
		DECA 					;
		STAA $04 				; Xmish Mode word
LC9C0: 	JSR LFA58 				;
		LDAA #$04 				;
		JSR LF4A8 				;
		LDAA #$0E 				;
		STAA $00 				;
		LDAA #128 				;
		STAA $C0 				; Blk Lrn Mult, Scaled 1
		STAA $C6 				; Base PW Cls Lp Corr <----- *****
		STAA $C9 				; Cls/Lp Int, Add Fuel if > 128
		LDAA #$19 				;
		STAA $018A 				; LOCATION OF NEXT ENTRY IN EPROM TBL
		LDAA #160 				;
		STAA $68 				;
		LDAA LC62F 				; 144, IDLE SPD START UP PARK POSIT.
		STAA $010C 				;
								;
								;
								;
LC9E3: 	LDAA $4005 				; Free run Up Cnt'r
		ADDA #$02 				; ADD 2 CNT'S
		STAA $4006 				; Match Reg
		LDX #$4007 				; index TX/Rx CSR
		BSET 0,X 01 			; SET b0
		JMP LCCC6 				;
		;*******************************************************
		;*******************************************************
		;* IRQ1 SERVICE ROUTINE, CALLED FROM INT AT LFF2
		;* TYPE $32, ECM P/N
		;*
		;* Executed every IRQ1
		;* All Minor Loop done here:
		;*
		;* Air/Fuel
		;* SPK
		;* IAC
		;* Etc.
		;*******************************************************
LC9F4: 	LDX #$4007 				; index TX/Rx CSR
								;
								; CK SCI RX STATUS, (8192 BAUD)
								;
		BRSET 1,X $01 LCA67 	; BR IF b0
								; ... else
		BRCLR 0,X $20 LCA0A 	; BR IF b5, If RX Int Not enab
								; ... else
		BRCLR 1,X $20 LCA2B 	; BR IF NOT b5, If not RX Int
								; ... else
		BSET $46 $08 			; Set 8192 Baud
		JSR LFA91 				; SCI RX SUBROUTINE
		RTI
								;
								; CK SCI XMIT STATUS, (8192 BAUD)
								;
LCA0A: 	BRCLR 0,X $80 LCA16 	; If Xmit not enab.
								; ... else
		BRCLR 1,X $80 LCA2B 	; IF not Xmit Int.
								; ... else
		JSR LFBBB 				; Xmit Handler
		RTI 					;
LCA16: 	BRCLR 0,X $40 LCA2B 	; If SCI Xmit Complete not Enab
								; ... else
		BRCLR 1,X $40 LCA2B 	; If SCI Xmit not Complete
								;
								; Disable SCI Xmit Via SXR
								;
		LDAA $4004 				; Par I/O CSR
		ANDA #$F7 				; Disable Xmit Via SXR
		STAA $4004 				; Par I/O CSR
								;
								; Enab RX Int's
								;
		LDAA #$27 				; Enab RX Int's
		STAA $4007 				; TX/Rx CSR
LCA2B: 	RTI			
								;-------------------------------------------------
								;-------------------------------------------------
LCA2C: 	LDAA $018C 				; UP CNTS VAL WHEN 6.25 MSEC INT
		ADDA #143 				;
		STAA $4006 				;
		BCLR $46 $01 			; Clr 1st Int Expected Flg
		JSR LF880 				; TO 160 BAUD SERIAL
		RTI
								;-------------------------------------------------
								;-------------------------------------------------
LCA3B: 	LDAA $018C 				; UP CNTS VAL WHEN 6.25 MSEC INT
		ADDA #205
		STAA $4006 				;
		BCLR $46 $02 			; Clr 2nd Expected Int Flg
		LDAA #$04 				; Set Ser Data Line Hi
		JSR LF4A8 				;
		BRCLR $35 $20 LCA66 	; BR IF NOT b5, (SW IN DIAG MODE)
								; ... else
		LDAA $018A 				; LOCATION OF NEXT ENTRY IN EPROM TBL
		CMPA #25 				;
		BNE LCA66 				;
								; ... else
		LDAA $0189 				; ALCL NUM OF BITS TO TX
		BNE LCA66 				; BR IF NZ
								; ... else
		LDAA #$09 				; 450 Msec
		STAA $012E 				; 50 Msec TIMER TO LOOK FOR 8192 BAUD
								;
		BSET $46 4 				; SET b2
		JSR LFA7A 				; Init 8192 Baud Ser Reg's
LCA66: 	RTI 					;
								;--------------------------------------------------
								;---------------------------------------------
								; 6.25 Msec Int Logic
								;
								;---------------------------------------------
LCA67: 	BRSET $46 $01 LCA2C 	; BR IF b0, 1st Serial Data Input
								; ... else
		BRSET $46 $02 LCA3B 	; BR IF b1, 2nd Serial Data Input
								; ... else
		BRCLR $47 $80 LCA76 	; BR IF b3,
								; ... else
		JMP LFCA2 				; To Factory test routine		<<<<<<<<<< This is FCA2 in BUA-1728 dld (table before it is bigger)
								;-------------------------------------------------
LCA76: 	BRSET $34 $04 LCA86 	; If tmg error already flaged
								; ... else
		BRCLR $36 $04 LCA86 	; IF NOT b2, (TMG ERROR CK FLG)
								; ... else
		LDAA $00 				; Fm NON VOL RAM
		STAA $0119 				; Save Counter
								;
		BSET $34 $04 			; SET b2, Tmg error
LCA86: 	BSET $36 $04 			; SET b2, (FLG FOR TMG ERR CHK)
								;
		LDAB $4006 				; <----- ????
		STAB $018C 				; UP CNTS VAL WHEN 6.25 MSEC INT
								;
		BRCLR $46 4 LCAA4 		; If Not in 8192 Baud mode
								; ... else
		ADDB #205 				; Set Up for 6.25 Ms Int.
		BRSET $46 8 LCAB3 		; If locked in 8192 Baud Mode
								; ... else
		DEC $012E 				; 50 Msec TIMER TI LOOK FOR 8192 BAUD
		BNE LCAB3 				; BR IF Z
								; ... else
		BCLR $46 $04 			; Disable 8192 Baud mode
		LDAB $4006 				; <----- ????
LCAA4: 	ADDB #11 				; Set up for 160 Ms Int, (160 Baud)
		BSET $46 $03 			; SET b0 & b1
		LDAA #$FB 				;
		JSR LF49A 				; Set Serial line Low
		LDAA #$01 				;
		STAA $4007 				; Disable 8192 Baud Int's
LCAB3: 	STAB $4006 				; Save 160 Int time
		CLI 					; CLR & RESTORE INTERUPTS
								;
								; TEST FOR VALID STX
								;
		TSX 					; Save STX pointer
		LDS #$01FF 				; RE-SET USR STX
		CPX #$01F7 				; Ck STX ptr VS Home Addr
		BEQ LCAC3 				; BR IF STX AT $01F7
								; ... else
		BSET $33 $01 			; SET b0, STACK FAIL
LCAC3: 	JSR LDEA4 				; Ck Vss Every 6.25 Ms
		LDD $3FFA 				; Get MCU Status Reg.
		STAA $A0 				; Old MPU status reg
								;
								; Set Ref Pulse, If it Happened in this Int period
								;
		LDAB $35 				; MNR LOOP MD WD 2
		BITA #$08 				; b3, (DRP DURRING LAST 6.25 msec)
		BEQ LCB15 				; BR IF NOT b3
								; ... else
		ORAB #$04 				; SET b2
		PSHB 					; SAVE MD WD TO STX
								;---------------------------------------
								; Cold Start Inj Elimination
								;
								;---------------------------------------
		LDX $1A 				; Eng Run Time (sec) Fm NON VOL RAM
		CPX LC37C 				; 32 Sec Min eng run time to reset
		BHI LCAEA 				; BR IF GT 32 Sec THRESH
								; ... else
		LDAB $82 				; TPS Ld Axis Var
		CMPB LC37E 				; 77%, TPS Value
		BHI LCAEA 				; BR IF TPS LT 77%, (If Yes clear)
								; ... else
		LDAB $0118 				; GET the REF COUNTER
		INCB 					; INCR REF COUNT
		BPL LCAEB 				; BR
								; ... else
		BRA LCAEE 				;
LCAEA: 	CLRB 					; RESET COUNTER
LCAEB: 	STAB $0118 				; SAVE NEW REF COUNTER
LCAEE: 	PULB 					; Restore Mode Word
		BRCLR $30 $04 LCB07 	; BR IF NOT b2
								; ... else
		BRSET $9D $80 LCB07 	; BR IF b7
								; ... else
		LDAA $012A 				;
		ADDA LC029 				; 2, SPK BLEND, Add to ????
		BCC LCB04 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX LMT
		BCLR $36 $20 			; CLR b5, BLEND SPARK
LCB04: 	STAA $012A 				;
LCB07: 	BRSET $3B $02 LCB15 	; BR IF b1
								; ... else
		BRSET $34 $80 LCB15 	; BR IF b7, ENG RUNNING
								; ... else
		BSET $3B $02 			; SET b1
		CLR $0472 				; CLR TMR
LCB15: 	LDAA $33 				;
		BITA #$10 				; b4, 1 = IGN OFF
		BEQ LCB1D 				; BR IF NOT b4, Ign OFF
								; ... else
		ANDB #$FB 				; CLR b2, (DRP LAST 6.25 Msec)
LCB1D: 	STAB $35 				; Reset Ref Pulse Indicator
								;--------------------------------------------------
								; CHECK VATS
								;
								; TYPE #32 ECM P/N 1227165
								;--------------------------------------------------
		BRSET $03 $20 LCB5A 	; BR IF b5, VATS AREADY PASSED
								; ... else
								;
								; CK IF VATS ENABLED
								;
		LDAA LC014 				; A/F OPTION WD #1
		BITA #$10 				; b4, VATS ON
		BEQ LCB4F 				; BR If NOT VATS ENABLED, (ERR 46)
								; ... else
		BSET $07 $02 			; Set b1, Err 3 Flg word, (VATS Fail)
		JSR LF3A7 				; UPDATE ERR Wd's Ck Sum
		STD $18 				; Save Ck Sum OF Err Words
								;--------------------------------------------------
								;
								; CK VATS FREQ, (PERIOD) WINDOW (33 -> 66Hz)
								; BUA, (MY 86 TYPE 32 ECM)
								;
		LDD $3FF8 				; PA1
		CLV 					; DELAY, (clr over flow flg)
		SUBD $0121 				; Old VATS PERIOD
		BEQ LCB5A 				; If No PULSE
								; ... else
		LDX $3FF8 				; PA1
		STX $0121 				; Old VATS
		PSHB 					;
		PSHA 					;
								;
								; CK VATS FREQUENCY LIMITS
								;
		SUBD LC00E 				; 3277d, VATS Max Limit, 66Hz
		PULA 					;
		PULB 					;
		BHI LCB5A 				; BR IF CNT'R LT VATS Max Limit
								; ... else
		SUBD LC010 				; 1638d, VATS Min Value, 33hz
		BCS LCB5A 				; BR IF CNT'R GT VATS Max Limit
								; ... else
LCB4F: 	BSET $03 $20 			; SET b5, Mode Wd3, VATS OK
		BCLR $07 $02 			; CLR b1, Err #3 Flg word< (VATS OK)
		JSR LF3A7 				; UPDATE ERR Wd's Ck Sum
		STD $18 				; Save Ck Sum OF Err Words
								;------------------------------------------------------
LCB5A: 	LDX $3FC0 				; Last 2 Ref Periods
		STX $95 				; Current Minor LP Ref Period
								;--------------------------------------------------
		;***************************************************
		;* CK ALDL
		;*
		;* BUA, TYPE $32, ECM P/N 1227165
		;***************************************************
		ORG $CB5F
		BRSET $46 $08 LCB67 	; BR If 8192 Mode in locked Mode
								; ... else
		BRCLR $35 $20 LCBB4 	; BR If Not b5, DIAG SW IN in ALDL Mode
								; ... else
								;------------------------------
								; LOOK FOR ALDL MODE 4
								; (USE SERIAL DATA+1 ICB)
								;
								; (BUA TYPE 32)
								;
								;------------------------------
LCB67: 	LDAA $0151 				; SERIAL DATA+1 ICB,
		CMPA #$04 				; CHK FOR Mode 4
		BNE LCBA4 				; NOT IN Mode 4
								; ... else
		BRSET $35 $08 LCBB4 	; BR IF b3, exit (PRIOR MODE 4)
								; ... else
		BSET $35 $08 			; Set Mode 4 Flg <----****
								;------------------------------
								; b6, RESET SYSTEM ERRORS
								; (USE ALDL CWD 1, ICB+4)
								; WD 3, b6
								;------------------------------
		LDAA $0154 				; SERIAL DATA+4 ICB, ND 4 CNT'L WD 3, b6
		BITA #$40 				; b6
		BEQ LCB90 				; BR IF NOT b6 (CK IAC)
								; ... else
								;
								; Clear Error Flag words
								;
		CLR $05 				; Err #1 Flg word
		CLR $06 				; Err #2 Flg word
		CLR $07 				; Err #3 Flg word
		CLR $08 				; Err #4 Flg word
		CLR $09 				; Err #5 Flg Word
		JSR LF3A7 				; ERR Wd's Ck Sum
		STD $18 				; Save Ck Sum OF Err Words
								;-------------------------------
								;-------------------------------
								; b5, RESET IAC TO MAX (255)
								; WD 3, b5
								;-------------------------------
LCB90: 	BITA #$20 				; b5, MD 4 CNT'L WD 3, b5
		BEQ LCB9B 				; BR IF NOT b5,
								; ... else
		BSET $F3 $04 			; SET b2,
		LDAB #255 				; MAX VALUE
		STAB $2C 				; IAC Present Posit
								;-------------------------------
								; b4, RESET BLM'S
								; WD 3, b4
								;-------------------------------
LCB9B: 	BITA #$10 				; b4, MD 4 CNT'L WD 3, b4
		BEQ LCBB4 				; BR IF NOT b3, EXIT
								; ... else
		JSR LF434 				; Go INIT BLM Init.
		BRA LCBB4 				; EXIT
								;--------------------------------------------------
								; CK IF IN ALDL MODE 4
								; IF NOT IN MODE 4, CLR FLAGS
								;
								;
								;--------------------------------------------------
LCBA4: 	BRCLR $35 $08 LCBB4 	; BR IF NOT b3, ALDL MODE 4, EXIT
								; ... else
		BCLR $35 $08 			; CLR b3, (MODE 4 FLG)
		BCLR $36 $01 			; CLR b0, (SKP ERR 42 FOR STALL SVR BYPASS)
		BCLR $3F $01 			; CLR b0
		CLR $B4 				; SPK Fd Bk counter, (Err #42)
								;--------------------------------------------------
LCBB4: 	LDAA $00 				; Minor Lp Counter
		INCA 					; Inc Minor Lp Counter
		CMPA #160 				; If Loop Cnt 160 (16)
		BNE LCBDA 				; If not 160, (16)
								; ... else
		LDAB $3C 				; ALCL MOD WD
		EORB #$20 				; b5, Toggle 1 Sec Flg
		STAB $3C 				; ALCL MOD WD
								;--------------------------------------------------
								; ENG RUN TIME COUNTER
								;
								; Do not Inc time if eng not running
								; Time in Sec's = 80 x .0125
								;--------------------------------------------------
		LDAB $34 				; Get Mode wd 1
		BPL LCBCA 				; BR IF ENG NOT RUNNING
								; ... else
		LDX $1A 				; Eng Run Time (sec)
		INX 					; INC RUN TIME
		STX $1A 				; Save Eng Run Time (sec)
								;---------------------------------------------
								;---------------------------------------------
								; If no Comm >= 30 Sec Force mode 0
								; (TM'R VAL AT C7F7)
								;---------------------------------------------
LCBCA: 	LDAA $0171 				; SERIAL DATA TIME TO FORCE MODE 0
		INCA 					; Incr Ser Data time out timer
		STAA $0171 				; NEW Ser DATA TIME VAL
		CMPA LC7F7 				; 30 SEC,
		BLS LCBD9 				; BR TIME GT 30 SEC
								; ... else
		CLR $0151 				; SERIAL DATA+1 ICB, (STATUS)
								;
								; FORCE MODE 0
								;
LCBD9: 	CLRA 					; Clr Minor loop counter
LCBDA: 	STAA $00 				; Save Minor Lp Count
		SEI 					; TURN OFF INTERUPTS
		JSR LF1E0 				; Reset the FMD chip
		STAA $2E 				; Save Results
		JSR LF1E5 				; Read other, (Byte 2)
		STAA $2F 				; Save 2nd FMD Byte
		CLI 					; CLEAR & RESTORE INTERUPTS
		LDX #$D000 				; Set PW word to zero D.C.
		LDAA $2E 				; Get results
		COMA 					; Comp FMD bytes
		ANDA #$DF 				; CLR b6
		LDAB $37 				; Get prior Value, MCU INPUT STATUS WD
		ANDB #$20 				; CLR ALL EXCEPT b5, (TCC LOCKED)
		ABA 					; Result to A reg
								;----------------------
								; CK AFR MODE WD 4
								; Norm Open Fan set
								;----------------------
		LDAB LC017 				; Get Fuel/Air Mode Wd 4
		BITB #$20 				; b5, Norm Open Fan Req ?
		BEQ LCBFE 				; BR IF not b5,
								; ... else
		EORA #$40 				; If so Invert Bit 6
LCBFE: 	ROLB 					;
		BMI LCC03 				; If PWR STEER SEL
								; ... else
		ANDA #$F7 				; CLR b3, EXCESS PWR STEER ACTIVITY
LCC03: 	STAA $37 				; SAVE TO MCU INPUT STATUS WD
								;
								; CK A/C QUALS
								;
		LDAA $82 				; TPS Ld Axis Var
		CMPA LC67D 				; 100% TPS,
		BCS LCC21 				; BR IF TPS LT THRESH
								; ... else
		CMPA LC67E 				; 100% TPS
		BCC LCC21 				; BR IF TPS GT THRESH, (TO IAC LOOP)
								; ... else
		LDAA $65 				; Vss
		CMPA LC67F 				; 8 MPH
		BHI LCC21 				; SKIP PS SW READ IF > 8 MPH
								;
								; PWR STEER SW
								;
		LDAA $2E 				;
		COMA 					; INVER
		ANDA #$08 				; MASK, FOR b3, PWR STEER
		ORAA $37 				; MCU INPUT STATUS WD
		STAA $37 				; MCU INPUT STATUS WD
								;------------------------------------------------------
								; Status Word $37, (RAM)
								; Bit
								; 0 1 = in Pk or Nuet
								; 1 1 = Not in 3rd Gear or O.D.
								; 3 1 = Pwr Steering Pw Sw, (Hi Load)
								;
								; 4 1 = EGR Diag closed
								; 5 1 = TCC
								; 6 1 = Cooling Fan Request
								; 7 1 = A/C Compressor not On
								;-------------------------------------------------------
LCC21: 	JSR LF4DF 				; Do IAC 6.25 Ms Loop
		LDAA #$50 				; A/D Ch 5, TPS Voltage)
		JSR LF1BE 				; To A/D subroutine
		STAA $81 				; Save TPS, (A/D)
		JSR LF35A 				; To TPS Load Subroutine
								;
								; Transient TPS Filter
								;
		LDX $DD 				; Transient TPS
		LDAA $82 				; TPS Ld Axis Var
		LDAB LC30D 				; 0.125, Trans TPS filter coef.
		JSR LF250 				; FILTER ROUTINE
		STD $DD 				; Transient TPS
								;
								; KEEP TRACK of SERIAL DATA & AIRFLOW HERE
								;
		LDAB $B0 				; MAF Hi TMR (100Ms)
		LDAA $34 				; Get mode wd 1
		BMI LCC65 				; IRQ ?
		BRSET $33 $10 LCC62 	; BR IF b4, IGN OFF
								; ... else
		LDAA $7F 				; Pump Volts, (A/D)
		CMPA LC208 				; 10 vdc, MIN FOR ERR 34
		BLS LCC62 				; BR IF VOLTAGE GT 10 V
								; ... else
		CMPB LC205 				; 600 Msec MIN QUAL TO SET HI MAF ERR 33
		BLS LCC58 				; BR IF MAF HI TIME G.T. 600 Msec
								; ... else
		LDAA $3F 				;
		ORAA #$80 				; SET b7, (MAF HI ERROR)
		STAA $3F 				;
		BRA LCC65 				;
LCC58: 	LDAA $B6 				; Get Air Flow value
		CMPA LC204 				; If Air Flow <= 45 gms/sec disable ERR 33
		BLS LCC62 				; BR IF AIR FLOW G.T. 45 gms/S
								; ... else
		INCB 					; INCR HI MAF TMR
		BRA LCC63 				;
LCC62: 	CLRB 					; CLR ERR 33, MAF HI TMR
LCC63: 	STAB $B0 				; ERR 33, MAP Hi TMR, (100Ms)
								;
								; CK EPROM ID BYTE
								;
LCC65: 	LDAB LC1AA 				; Get EPROM ID
		CMPB #$55 				; Is it $55
		BNE LCC95 				; If not do BAD EPROM err
		LDAB LC555 				;
		CMPB #$AA 				; Is It $AA ?
		BNE LCC95 				; If not do BAD EPROM err
								; ... else
								;-------------------------------------
								;On Even minor Loops Do:
								; 1. Spark
								; 2. O2 Sensor read
								; 3. Road Speed, Vss
								;--------------------------------------
		LDAA $00 				; Get Minor Lp counter
		RORA 					;
		BCC LCC7B 				;
								; ... else
LCC78: 	JMP LD6D1 				; Do Air Fuel On Odd Loops
LCC7B: 	BCLR $3B $08 			; CLR b3, MODE 4 BYPASS FUEL
								;--------------------------------------------------
								; CK IF IN MODE 4 <----****
								;
								;
								;--------------------------------------------------
		BRCLR $35 $08 LCC87 	; BR IF NOT b3, MODE 4
								; ... else
		LDAA $0154 				; MD 4 CNTL WD 1
		BMI LCC8C 				;
								; ... else
								;-------------
								; TOGGLE COP
								;-------------
LCC87: 	JSR LF21A 				; Toggle COP 2 Each 6.25 Ms
								; SPK Edge Constrant
		BRA LCC92 				;
								;--------------------------------------------------
LCC8C: 	BSET $3B $08 			; SET b3
		BSET $36 $01 			; SET b0, SKP ERR 42 FPR STALL ..
								; SVR SPK BYPASS
LCC92: 	JMP LCD05 				; Minor Lp
								;
								; Bad EPROM Set Error Routine
								;
								;
LCC95: 	BSET $4E 1 				; Err #51, NEW ERR 2
		LDAA $34 				;
		ANDA #$7F 				; CLR b7, Eng Run Flg
		STAA $34 				;
		LDAA $3C 				; Get COP 2 Flag
		ORAA #$80 				; SET b8
		STAA $3C 				;
								;-------------------------------------------
								;If Bad EPROM Disable SPK, (even Loops)
								; Don't tog COP 2, (Odd Loops)
								;--------------------------------------------
		LDAA $00 				; Cont if EPROM bad
		RORA 					;
		BCS LCC78 				;
								; ... else
		JMP LD306 				; Go Disable SPK
		;***************************************************
		;***************************************************
		;* MAJOR LOOP SUBROUTINE
		;*
		;* Execute on 100 Msec
		;*
		;*
		;*
		;***************************************************
LCCAC: 	LDAA $00 		; Get Loop Cnt
		RORA 			;
		BCC LCCB6 		; EXEC MJR LOOP , SKIP COP
						; ... else
		LDAA #$FF 		; Set COP hi
		STAA $400B 		; Arm COP1
LCCB6: 	LDAB $00 		; Get Loop Cnt
		ANDB #$0F 		; 0000 1111
		LDX #$CCE4 		; Point to Seg Addr Table
		ASLB 			;
		ABX 			; ADJ ADDRESS POINTER FOR PROPER SUB ADDR
		LDX 0,X 		; Get SEG ADDR
		JSR 0,X 		; Jump to SEG Addr pointed
		BCLR $36 $04 	; CLR b2, TIMIMG ERR CK FLG
						;-------------------------------------------------
LCCC6: 	CLI 			; CLEAR & RESTORE INTERUPTS
		LDX $0110 		;
		BRA LCCCF 		;
						;--------------------------
						; Refill RAM
						;--------------------------
LCCCC: 	LDX $0110 		; Get RAM Addr.
LCCCF: 	SEI 			; TURN OFF INTERUPTS
		LDD 0,X 		;
		STD 0,X 		; REFILL HERE
		CLI 			; CLEAR & RESTORE INTERUPTS
		DEX 			; DECR ADDR POINTER
		DEX 			; (TEWICE)
		STX $0110 		; Save At RAM Addr
		BPL LCCCC 		; TILL DONE
						; ...
		LDX #$01FE 		; Top Of RAM ?
		STX $0110 		; Save RAM Addr here
		BRA LCCCC 		; Loop till Done
						;-----------------------------------
						; MAJOR LOOP SEGMENT TABLE
						; TYPE $8D, 122
						;-----------------------------------
LCCE4: 	FDB $CD04 		; SEG 0, Do nothing, exit w/RTS
		FDB $EDA3 		; SEG 1, Output Bit Sig', (TCC, CCP etc)
		FDB $E07F 		; SEG 2, Vss Calc
		FDB $EA28 		; SEG 3, Misc 100 Msec
		FDB $F8F9 		; SEG 4, Log RAM to H.U.
		FDB $ECEA 		; SEG 5, A/C, Clsd Lp & Cool FAN
		FDB $F3B6 		; SEG 6, Log RAM to MCU, Cool A/D
		FDB $EA95 		; SEG 7, L.U. Coolant var's
		FDB $CD04 		; SEG 8, Do Nothing, exit w/RTS
		FDB $E34E 		; SEG 9, Inj Air Managment
		FDB $EB3A 		; SEG A, Lk UP Man Air temp Var's
		FDB $DEE5 		; SEG B, EGR
		FDB $E75D 		; SEG C, Can Purge
		FDB $EF04 		; SEG D, Diagnostics
		FDB $DE6D 		; SEG E, TCC (A/D voltage convert)
		FDB $EC23 		; SEG F, Fuel/Air Major Lp
LCD04: 	RTS
		;*******************************************************
		;*******************************************************
		;* KNOCK (A Minor loop routne)
		;*
		;* EXECUTED EVERY OTHER IRQ1
		;*
		;* All Ninor Lp functions Related to SPK & Rd Spd
		;*******************************************************
LCD05: 	LDAA $35 				; MD WD 2
		ANDA #$04 				; CLR b2, (DIAG SW IN DIAG MODE)
		ORAA $40 				; DIAGNOSTIC MODE WD 2
		STAA $40 				; DIAGNOSTIC MODE WD 2
								;-------------------------------------------
								; LK UP Slow o2 Filter Coef Vs Air flow
								;
								; TBL = Mult * 256
								;-------------------------------------------
		LDAA $D2 				; Ld Val Limited AIR FLOW for Lk Up
		LDX #$C58B 				; TBL
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		PSHA 					; FILT COEF TO STX
		LDAA $5B 				; COOLANT
		LDAB LC565 				; Get cold o2 filter coef
		MUL 					;
		ADDA $5B 				; COOLANT
		BCC LCD22 				; IF NO OVERFLOW
								; ... else
		LDAA #255 				; MAX VAL
LCD22: 	PULB 					; GET SLOW o2 vs FLOW RESULT FM STX
		MUL 					;
		TAB 					;
		LDAA $6F 				; FILT o2 VDC
		LDX $71 				; Mnr lp slo flt
		JSR LF250 				; LAG FILTER ROUTINE
		STD $71 				; SAVE FILTERED A/D o2 RESULT
								;
								; SPK 12.5 Msec Logic
								;
		BRCLR $35 $04 LCD4B 	; Skp to LCD54 if no ref
								; - in last 12.5 msec
		LDD $3FC8 				; Get SPK Period
		SUBD $B2 				; OLD PA2 CNT'R, Sub Value after
								; Lst SPK Period
		TSTA 					;
		BNE LCD3F 				;
								; ... else
		CMPB LC228 				;
		BLS LCD47 				;
								; ... else
LCD3F: 	INC $B4 				; SPK Fd Bk counter, (Err #42)
		BNE LCD47 				; BR IF NZ
								; ... else
		DEC $B4 				; SPK Fd Bk counter, (Err #42)
LCD47: 	ADDD $B2 				; OLD PA2 CNT'R, SPK Period
		STD $B2 				; OLD PA2 CNT'R, SPK Period
LCD4B: 	LDAA $35 				; MINOR LP MD WD 2
		LDAB $34 				;
		BMI LCD5F 				;
								; ... else
		LDX $95 				; Current Minor LP Ref PERIOD
		CPX LC018 				; Ck Start up RPM Lmt
		BCS LCD5F 				;
								; ... else
		ANDA #$FB 				; CLEAR b2, (DRP LAST 6.25 Msec)
		STAA $35 				;
		CLRA 					;
		BRA LCD8B 				;
LCD5F: 	BITA #$04 				; b2
		BEQ LCDC1 				; BR IF NOT b2
								; ... else
		ANDA #$FB 				; CLR b2, (DRP LAST 6.25 Msec)
		STAA $35 				;
								; ... and
								;-------------------------
								; CALC DYNAMIC DWELL
								; = (DYNAMIC DWELL/8) +1
								;-------------------------
		LDD $99 				; DYNAMIC DWELL
		LSRD 					; Div by 8
		LSRD 					;
		LSRD 					;
		COMA 					;
		COMB 					;
		ADDD $99 				; Dynamic Dwell
		BPL LCD74 				;
								; .... else
		CLRA 					; USE ZERO
		CLRB 					;
LCD74: 	STD $99 				; Dynamic Dwell
								;-------------------------
		CLR $9F 				; Num Of Minor Loops between Ref Pulse
		LDAA $34 				;
		BMI LCDDD 				; BR IF
								; ... else
		LDAB $33 				;
		BITB #$08 				; b3, 2nd DRP (PERIOD VALID)
		BEQ LCDD0 				; BR IF NOT b3
								; .... else
		LDAA $93 				; Cont'r for Eng Strt Up
		CMPA LC01A 				; Num OF 12.5 Ms lps reguired
								; for RPM to be abover limit
		BCC LCD8F 				; BR IF
								; else
		INCA 					;
LCD8B: 	STAA $93 				; Cntr for Eng Strt Up
		BRA LCDD4 				;
LCD8F: 	LDAA $34 				;
		ORAA #$80 				; SET b7, ENGINE RUNNING FLAG
		STAA $34 				;
		BCLR $3B $02 			; CLR b1
								;
								; CK IF SINGLE FIRE MODE SEL
								;
		LDAA LC014 				; AFR MD WD 1, 1011 0100
		BITA #$02 				; b1, SINGLE FIRE MODE SEL
		BEQ LCDA5 				; BR IF NOT b1. If MAF in use
								; .... else
		BSET $39 $80 			; SET b7, (IN SINGLE FIRE)
		BCLR $39 $50 			; CLR b4 & b6, (TGL SF & PW = 0)
								;---------------------------
								; MAF, (Hot Wire) Routines
								;
								;---------------------------
LCDA5: 	CLR $B0 				; MAP Hi Cnt'r, (100 Ms)
		BRSET $33 $10 LCDB9 	; BR IF b4, (1 = IGN OFF)
								; ... else
								;
		LDX $EF 				; Diff Val to calc Air Flow
		CPX LC211 				; IF Analog VALS * 7 or
								; PP2DLT < 85, enab ERR #34
		BCC LCDB9 				;
								; ... else
		LDAA $3F 				;
		ORAA #$40 				; SET b6, ERR #34
		STAA $3F 				;
LCDB9: 	LDAA $01 				;
		ORAA #$08 				; SET b3
		STAA $01 				;
		BRA LCDDD 				;
LCDC1: 	LDAA $9F 				; Num Of Minor Lp's between DRP'S
		CMPA #23 				; 23 DRP'S ?
		BCS LCDD7 				; BR IF L.T. 23 DRP'S
								; else
		LDX #$FFFF 				;
		STX $3FC0 				; Ref period Cnt'r,
								;
								; WAIT FOR INTERUPT
								;
LCDCD: 	SWI 					; LOOP HERE TILL INTERUPT
		BRA LCDCD 				;
								;--------------------------------------------------
								;
								;
								;--------------------------------------------------
LCDD0: 	ORAB #$08 				; SET b3, 2nd DRP, (PERIOD VALID)
		STAB $33 				; MINOR LP MD WD 1
								;
LCDD4: 	JMP LD306 				;
LCDD7: 	TSTB 					;
		BPL LCDD4 				;
		INCA 					; BUMP UP COUNTER
		STAA $9F 				; NUM Of MINOR LOOPS BETWEEN DRP'S
LCDDD: 	LDAA #0 				;
		STAA $400C 				;
		BSR LCDE6 				;
		BRA LCDF6 				;
								;
								; CALC RPM VAL
								;
LCDE6: 	LDX #0149 				;
		LDAA LC009 				; Get Num of Cyl's, (0 = 8 CYL)
		BNE LCDF2 				; BR IF NZ
								; ... else
		LDD $95 				; Current Minor LP Ref Period fm MPU
		BRA LCDF5 				;
LCDF2: 	JSR LF266 				; MUL 8 X 16 SUROUTINE
LCDF5: 	RTS 					;
								;---------------------------------------------------
LCDF6: 	STD $0468 				; IM RAM, NUM OF 65.5Khz cnts between DRPs
								; between DRPs
								;-------------------------------------
								; RPM, RPM P, RPM/25 & RPM/12.5
								;
								; Q = (153.6/RPM RER) * 2 ^8
								;
								; PER = 1/65.5 Khz per bit
								;--------------------------------------
		PSHB 					;
		PSHA 					;
		ASLD 					; SET DIVIDE OPER (x2)
		PSHB 					;
		PSHA			
								;
		PULX 					;
		LDD #0307 				; 15 * (512/25) or 153.6 * 2
		JSR LF215 				; TO F DIV SUBTOUTINE
		PSHB 					;
		PSHA 					;
		CMPA #96 				; 2400 RPM
		BLS LCE14 				;
								; ... else
								;
								; ROUND Q & ADJ to HI RANGE (2400-4800)
								;
		ADDD #16512 			;
		BCC LCE1B 				; IF IN HI RANGE RPM/25
								; ... else
		LDAA #255 				; If > 4800 RPM limit to 4800, (255d)
		BRA LCE1B 				;
LCE14: 	ASLD 					; x2, (RPM.12.5)
		SUBD #8064 				;
		BCC LCE1B 				;
								; ... else
		CLRA 					; Else clear RPM
								;--------------------------------
								; (-400 RPM ($20) + 128 to round)
								; 0 CTS = 400 RPM
								;--------------------------------
LCE1B: 	STAA $56 				; RPM/25
		PULA 					;
		PULB					;
								;
		PSHB 					;
		PSHA 					;
		ADDD #128 				; Round off
		BCC LCE28 				; if no overflow
								; ... else
		LDAA #255 				; limit to 255
LCE28: 	STAA $57 				; RPM/25
								;-----------------------
								; MAKE UN-FILT RPM/12.5
								;-----------------------
		PULA 					;
		PULB 					;
		ASLD 					; x2
		BCS LCE34 				;IF OVERFLOW LIMIT TO 255, (3187 RPM)
								; ... else
		ADDD #128 				;
		BCC LCE36 				;
								; ... else
LCE34: 	LDAA #255 				; USE MAX VAL, 3187 RPM
LCE36: 	CLRB 					;
		LDX $58 				; RPM/12.5, (UN-FILT)
		BEQ LCE41 				; IF 0, EXIT
								; ... else
		LDAB LC62E 				; RPM 12.5 Msec IAC Filter Coef, (0.5)
		JSR LF250 				; FILTER ROUTINE
LCE41: 	STD $58 				; FILT RPM/12.5
								;-------------------------
								;
		BRCLR $01 $80 LCE4C 	; BR IF NOT b7
								; ... else
		BCLR $36 $03 			; b0 & b1, SKIP ERR 43.. & IN STALL SVR
		BRA LCEB3
LCE4C: 	CMPA LC025 				; Stall Svr Lmt, (400 RPM)
		BCS LCE5D 				;
								; ... else
		CMPA LC026 				; Stall Svr Lmt, (450 RPM)
		BCC LCE81 				;
								; ... else
		LDAB $5A 				;
		CMPB LC027 				; Stall Svr Lmt, (550 RPM)
		BLS LCE81 				;
								; ... else
LCE5D: 	BRSET $36 $02 LCE7E 	; b1, IN STALL SVR BYPASS SPK
		CLR $B4 				; SPK Fd Bk counter, (Err #42)
		LDD $3FFC 				; CPU CR
		ANDB #$EF 				; CLR b4
		JSR LF3B5 				; 11 Usec Delay
		STD $3FFC 				; CPU CR
		BCLR $30 $04 			; CLR b2, EST ENABLE BIT, OFF
		SEI 					; TURN OFF INTERUPTS
		JSR LF1E5 				; FMD SERIAL BYTE READ
		CLI 					; CLR & RESTORE INTERUPTS
		LDD $3FC8 				; Get SPK Period, (PA2 CNT'R)
		STD $B2 				; OLD PA2 CNT'R, SPK Period
		LDAA $58 				; RPM/12.5
								;
LCE7E: 	BSET $36 $03 			; SET b1 & b0, STALL SAVER SPK FLAG
								;
								; NO STALL SAVER SA
								;
LCE81: 	BRCLR $36 $02 LCEB3 	; BR IF NOT b1, IN STALL SVR SPARK
								; ... else
								;
								; Disable Stall SVR Lmt, (550 RPM)
								;
		CMPA LC028 				; STALL SVR RPM THRESH
		BLS LCEB3 				; BR IF RPM GT 550 RPM
								; ... else
		BCLR $36 $03 			; Clear Stall SVR SPARK FLAG, b0 & b1
		INC $B4 				; EST FB CNT'R, PREVENT ERR 42
								;
								; ZERO VAL'S
								; EST DWELL, EST PULSE WIDTH
								;
		CLRA 					;
		CLRB 					; ZERO VAL'S
		STD $3FDC 				; TO SPK Dwell Cnt'r
		JSR LF3B5 				; 11 Usec Delay
		STD $3FE6 				; SPK Dwell Cnt'r
		JSR LF3B5 				; 11 Usec Delay
		STD $3FE8 				; CURRENT EST FALL - LAST EST FALL
		JSR LF3B5 				; 11 Usec Delay
		STD $3FF6 				; EST FALL CNT'R
								; (Time From Ref to Fire IGN Cnt'r)
		JSR LF3B5 				; 11 Usec Delay
		LDX $3FEC 				; B Cnt'r last ref
		JSR LF3B5 				; 11 Usec Delay
		STX $3FE4 				; B Cnt'r Bgn nxt Dwell
		;*****************
		;* Compute Dwell
		;*****************
								;
								; IF LOAD CHANGE DURRING LAST
								; 25 msec > 10 Kpa SET DWELL
								; to $FFxx & Br to LCEC8 ...
								;
LCEB3: 	LDAA #255 				;
		LDAB $63 				; FILTERED LD VALUE
		SUBB $62 				; NORMILIZED LOAD, 25 Msec OLD
		BCS LCEC0 				; BR IF OLD G.T. NEW
								; ... else
		CMPB LC01B 				; Ld Val vs Dwell Limit
		BCC LCECB 				; br if G.T.
								; ... else
								;----------------------------------------
								; IF DELTA RFPER > DYNAMIC DWELL
								; set DYNAMIC DWELL = -2 * DELTA REFPER
								;-----------------------------------------
LCEC0: 	LDD $97 				; OLD REF PER
		SUBD $95 				; CURRENT MNR LOOP DRP PERIOD
		ASLD 					; DIV by 2
		SUBD $99 				; DYNAMIC DWELL
		BMI LCECD 				;
								; ... else
		ADDD $99 				; DYNAMIC DWELL
LCECB: 	STD $99 				; DYNAMIC DWELL
								;------------------------------
								; Limit DYNAMIC DWELL to
								; REF PERIOD/8, (11 DEG FOR V8)
								;------------------------------
LCECD: 	LDD $95 				; Current Minor LP Ref Period Fm MPU
		LSRD 					; 	DIV BY 8			
		LSRD 					;			
		LSRD 					;			
		SUBD $99 				; Dynamic Dwell
		BCC LCEDA 				; Br if DYNAMIC DWELL > REFPER/8
								; ... else
								;
								; IF DYNAMIC DWELL > REFPER/8 set DYNAMIC DWELL to REFPER/8
								;
		ADDD $99 				; Dynamic Dwell
		STD $99 				; SAVE NEW Dynamic Dwell
LCEDA: 	LDD $95 				; Current Minor LP Ref Period fm MPU
		STD $97 				; Last Minor LP Ref Period fm MPU
		LSRD 					; /2
		SUBD #229 				; 3.49 Msec, (15.25 Msec/bit)
		BCC LCEE9 				; BR IF D G.T. 3.5 Msec,
								; ... else
		ADDD #0308 				; ADD 4.697 Msec
		BRA LCEF9 				; EXIT via BATTERY COMP
LCEE9: 	LSRD 					;			
		SUBD #0295 				; 4.5 Msec
		BCS LCEF4 				; BR IF D L.T. 4.5 Msec
								; ... else
		ADDD #0382 				; ADD 5.825 Msec
		BRA LCEF9 				; EXIT via BATTERY COMP
								;---------------------------
								; D = 1/4 * (D + 23.3 msec)
								;---------------------------
LCEF4: 	ADDD #1527 				; 23.3 Msec
		LSRD 					; /4
		LSRD 					;
		;********************************
		;* BATTERY VOLTAGE COMPENSATION
		;*
		;********************************
LCEF9: 	PSHB 					;
		PSHA 					;
		LDAA #124 				; 12.4 VDC
		SUBA $7E 				; Batt Volts Value, (A/D)
		BCC LCF02 				; BR IF BAT V L.T. 12.4
								; ... else
		CLRA 					; NEGITIVE RESULT
								;
								; LOW BATTERY DWELL
								; If Batt < 12.4 VDC
								;
LCF02: 	LDAB #04 				;
		MUL 					; 630 usec/Volt
		TSX 					;
		ADDD 0,X 				;
		PULX 					;
		ADDD $99 				; Dynamic Dwell
		STD $9B 				; Dwell Cnts to MPU
		LDD $95 				; Current Minor LP Ref Period fm MPU
		SUBD #39 				;
		SUBD $9B 				; Dwell Cnts to MPU
		BCC LCF1A 				; BR IF NO UNDERFLOW
								; ... else
		ADDD $9B 				; Dwell Cnts to MPU, (15.25 Msec/BIT)
		STD $9B 				; Dwell Cnts to MPU, (15.25 Msec/BIT)
								;
								; CK PASSBY Lk Up QUALS
								;
								; (DISABELD BY QUAL VAL'S)
								;
LCF1A: 	LDX $0125 				; TCC LK UP PASS BY TMR, (SEC * 80)
		BNE LCF5A 				; BR IF NZ
								; ... else
		LDAA $82 				; TPS Ld Axis Var
		CMPA LC298 				; No PASSBY Lk Up if TPS < 99.6%
		BCS LCF44 				; BR IF TPS L.T. 99.6%
								; ... else
		LDAA $65 				;
		CMPA LC299 				; No PASS BY Lk Up if RPM < 6375
		BLS LCF44 				; BR IF RPM G.T. 6375
								; ... else
		CMPA LC29A 				; No PASS BY Lk Up if > 800 RPM
		BHI LCF44 				; BR IF RPM L.T. 800
								; ... else
		LDAA $57 				; RPM/25
		CMPA LC29B 				; NO PASS BY LK UP IF RPM < 6375
		BCS LCF44 				;
								; ... else
		LDX $0123 				; DLY TMR
		CPX LC29C 				; 819 Sec DLY prior to PASBY LOCKUP
		BHI LCF4C 				;
								; ... else
		INX 					; INCR TCC LK UP PASS BY TMR
		BRA LCF47 				;
LCF44: 	LDX #0 					; CLR DLY TMR
LCF47: 	STX $0123 				; DLY TMR
		BRA LCF85 				;
LCF4C: 	LDX LC29E 				; KEEP TCC LK'ED UP FOR PASSBY 30 SEC
		STX $0125 				; TCC LK'ED UP TMR
		LDX #0 					; CLR DLY TMR
		STX $0123 				;
		BRA LCF60 				;
LCF5A: 	DEX 					; DEC TMR
		STX $0125 				; TCC LK'ED UP TMR
		BEQ LCF7F 				;
LCF60: 	LDAA $7E 				; Batt Volts Value, (A/D)
		CMPA #171 				; 17.1 VDC
		BCC LCF85 				; BR IF BATT GT 17.1 VDC
								; ... else
								;
								; CK AFR MD WD 1 & 4
								;
		LDX #$C014 				; AFR MODE WD POINTER
		BRSET 3,X $80 LCF85 	; BR IF USE TCC OUT FOR A/C CN'T, (C017)
								; ... else
		BRSET 0,X $40 LCF85 	; BR IF USE PWR STEER SW SEL
								; ... else
								;
		LDX #$DFFF 				; 57,343
		STX $3FD6 				; TCC PW Cnt'r
		BSET $37 $20 			; SET b5, MCU INPUT STATUS WD, TCC
		BSET $36 $08 			; SET b3, TCC LOCKED FOR PASS BY NOISE
		BRA LCF85 				; EXIT TO UP MAIN SPK LK UP
LCF7F: 	BSET $04 $81 			; SET b7 & b0, Xmission Mode word
								; OD ACTIVE, OD ON
		BCLR $36 $08 			; CLR b3, TCC LOCKED FOR PASS BY NOISE
		;***************************************************
		;* LK UP MAIN SPARK ADV vs LD vs RPM
		;*
		;* TBL = SPK * (256/90)
		;***************************************************
LCF85: 	LDX #$C02E 				; POINT TO MIN SPK TBL
		LDAA $01 				;
		ASLA 					;
		BMI LCFA7 				; Br IF WARM Kickdown enabled
								; ... else
		LDAB $5B 				; GET FILT COOLANT
		CMPB LC02C 				; START UP COOLANT
		BCC LCF9D 				; BR IF COOLANT GT 26c
								; ... else
		SUBB $5F 				; START UP COOL
		BCS LCFA3 				; BR IF COOL LT $5F
								; ... else
		CMPB LC02D 				; Cool Diff Lmt Fm Tbl, (20c)
		BCS LCFA3 				;
								; ... else
LCF9D: 	LDAA $3B 				;
		ORAA #$40 				; SET b6, WARM KICK Dn REQ
		STAA $3B 				;
LCFA3: 	LDAB #208 				; Max LOAD AXIS ON 1st WARM PASS
		BRA LCFAF 				;
LCFA7: 	LDAB $63 				; filtered ld value
		CMPB #208 				; 208 gms/SEC MAX FOR LK UP
		BLS LCFAF 				; BR IF LD VAL GT 208
								; ... else
		LDAB #208 				; USE MAX LD VAL
LCFAF: 	LDAA $56 				; RPM/25
		JSR LF27C 				; 3d Lk Up Routine
		STAA $045D 				; SAVE HU SPK VAL
		PSHA 					; SAVE MAIN SPK TO STX
		;***************************************************
		;* LK UP COOL COMP SPK ADV vs LD vs COOL
		;*
		;***************************************************
		LDAA $5B 				; COOLANT
		CMPA #208 				; Limit COOL DEG FL to 115c
		BLS LCFC0 				; BR IF COOL LT 115c
								; ... else
		LDAA #208 				; USE 115c AS MAX LK UP VAL
LCFC0: 	LDX #$C0FE 				; POINT TO COOL COMP SPK LD SEL BYTE
		LDAB $63 				; FILTERED LD VALUE
		TST 0,X 				; CK LD SELECTOR BYTE (0 = LD VAL)
		BNE LCFD1 				; BR IF NZ
								; ... else
		CMPB #160 				; MAX LD VAL FOR LK UP
		BLS LCFD4 				; IF LD LT 160
								; ... else
		LDAB #160 				; USE MAX LD VAL FOR LK UP
		BRA LCFD4 				;
LCFD1: 	LSRB 					; LD VAL/2
		ADDB #32 				; MIN LD VAL FOR LK UP
LCFD4: 	INX 					; POINT TO TBL
		JSR LF27C 				; 3d Lk Up Routine
		STAA $045E 				; SAVE HU COOL COMP SPK RESULT
		PSHA 					; SAVECOOL COMP SPK TO STX
		;***************************************************
								;
								; CK
								;
		LDAB LC017 				; AFR MD WD 4
		LDAA $37 				; MCU INPUT STATUS WD
		BITB #$10 				; b4, EGR DIAG CLOSED
		BEQ LCFE9 				; BR IF NOT b4
								; ... else
		BITA #$20 				; b5, N.O. Fan Req
		BEQ LD017 				; BR IF NOT b5
								; ... else
LCFE9: 	BITB #$04 				; b2, 4th gr Hwy Mode Sprk Adv on
		BEQ LCFF4 				; BR IF NOT b2
								; ... else
		LDAA $8F 				; N/V ratio
		CMPA LC288 				; Min 4th gr Val
		BHI LD017 				;
								; ... else
LCFF4: 	LDAA $63 				; FILTERED LD VALUE
		CMPA LC16F 				; If LV8 > 100 disable hi-way mode spark
		BHI LD017 				; BE IF LD VAL LT 100
								; ... else
		LDAB $5B 				; COOLANT
		CMPB LC16E 				; If Coolant <= 59.8c, (139.5f), disable
		BLS LD017 				; BR IF COOL GT THRESH
								; ... else
		LDAB $56 				; RPM/25
		CMPB LC170 				; If RPM is less than 6350 RPM, disable
		BCS LD017 				;
								; ... else
		LDAB $A6 				; Hiway Mode Spk Tmr
		CMPB LC171 				; Enable hi-way mode spark if > 10 Sec
		BHI LD01D 				; BR IF TMR LT 10 SEC
								; ... else
		LDAA $00 				; GET MJR LOOP CNTR
		BNE LD018 				;
								; ... else
		INCB 					; INCR TMR
		BRA LD018 				;
LD017: 	CLRB 					; CLR TMR
LD018: 	STAB $A6 				; Hiway Mode Spk Tmr
		CLRA 					;
		BRA LD02B 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; LK UP HIGHWAY MODE SPARK ADVANCE vs LV8 (load)
								;
								; TABLE VALUE = SA * (256/90)
								;--------------------------------------------------
LD01D: 	CMPA #144 				; MAX LD VAL FOR LK UP
		BLS LD023 				; BR IF LD VAL LT 144
								; ... else
		LDAA #144 				; MAX LD VAL FOR LK UP
LD023: 	LDAB #32 				; MIN LD VAL FOR LK UP
		LDX #$C172 				; ADDR OF TBL
		JSR LF2B9 				; 2D Lk Up
LD02B: 	LDAB $3A 				; HU DISPLAY MD WD
		ANDB #$EF 				; CLR b4
		STAA $045F 				; SAVE HU HIGHWAY MODE SPARK ADVANCE
		PSHA 					; SAVE HIGHWAY MODE SPARK ADVANCE TO STX
		BEQ LD037 				; BR IF Z
								; ... else
		ORAB #$10 				; SET b4
LD037: 	STAB $3A 				; HU DISPLAY MD WD
								;--------------------------------------------------
								;--------------------------------------------------
								; HOT RE-START RETARD
								;--------------------------------------------------
		LDAA $01 				;
		BITA #$20 				; b5
		BNE LD04D 				; BR IF b5, (if IAC kick dn enab)
								; ... else
		LDAB $5F 				; START UP COOL THRESH
		CMPB LC02A 				; Low Lmt for Hot Restart, (54c)
		BCS LD04D 				;
								; ... else
		LDAB $1B 				;
		SUBB LC684 				; 25d, Hot Sprk Retard
		BCS LD054 				;
								; ... else
LD04D: 	CLRB 					;
		ORAA #$20 				; SET b5, IAC KICK DN ENABLE FLG
		STAA $01 				;
		BRA LD057 				;
LD054: 	LDAB LC02B 				; Hot Restard Lmt fm Tbl, (9.8 Deg)
LD057: 	LDAA $3A 				; HU DISPLAY MD WD
		ANDA #$DF 				; CLEAR b5
		STAB $0460 				;
		PSHB 					;
		BEQ LD063 				;
		ORAA #$20 				; SET b5
LD063: 	STAA $3A 				; HU DISPLAY MD WD
		CLRB 					;
		LDAA $35 				; MNR LOOP MD WD 2
		BITA #$10 				; b4, DIAG SW IN DIAG MODE
		BNE LD088 				; BR IF b4
		BRCLR $30 $04 LD088 	; BR IF NOT b2
								; ... else
		TSX 					;
		LDAA 0,X			
		BNE LD088 				;
		LDAA $02 				; Mode Wd1
		BMI LD088 				;
		RORA 					;
		BCC LD08B 				;
		LDAA $5B 				; COOLANT
		CMPA LC22C 				;
		BLS LD08B 				;
		LDAB LC22B 				;
		BRA LD08B 				;
LD088: 	BCLR $02 $01 			; CLR b0, ERR 43B TEST IN WORK
LD08B: 	PSHB 					;
		CLRB 					;
		LDAA $35 				; MNR LP MD WD 2
		BITA #$20 				; b4, DIAG SW IN DIAG MODE
		BEQ LD096 				; BR IF NOT b5
								; ... else
		;***************************************************
		;* SUM THE SPARK ADVANCES
		;*
		;***************************************************
		LDAB LC024 				; Added S.A. for ALDL Mode
LD096: 	LDX #0 					; ZERO OUT SPK ADV ACCUM
		ABX 					;
		PULB 					; GET ___ SPK
		ABX 					; + KNOCK TEST SA
		PULA 					; GET ___ SPK
		PULB 					;
		ABX 					;
		PULB 					; GET ___ SPK
		ABX 					;
		PULB 					; GET ___ SPK
		ABX 					;
		LDAB $13 				; START UP SPARK ADVANCE
		ABX 					;
		PSHA 					;
		PSHX 					; SAVE PARTIAL SUMMED SPK ADV TO STX
		PULA 					;
		PULB 					;
		TSX 					;
		SUBB LC0FD 				; SUB OFF 20 DEG, COOL COMP SPK BIAS
		SBCA #0 				;
		SUBB 0,X 				;
		SBCA #$0000 			;
		INS 					; RESET STX
		STD $0115 				; FINAL SPK ADV VALUE
								;-------------------------------------------------
								; CK IF HEADS UP CONNECTED
								; MOD SPARK
								;-------------------------------------------------
		LDX $33 				; MINOR LOOP MD WD 1
		BPL LD0BF 				; BR IF NO HEADS UP ON LINE
								; ... else
		JSR $5809 				; TO HEADS UP
								;
								;
								; GET INITAL LEAD FM CAL
								;
LD0BF: 	SUBB LC01C 				; IGN LEAD Value
		SBCA #0			
		STD $9D 				; SPK ADV REL TO REF, (Unlim)
		LDD LC01E 				; MAX S.A. Limit, Aprox 40 Deg
		SUBD $9D 				; SPK ADV REL TO REF, (Unlim)
		BGT LD0D1 				; BR IF SPK G.T SPK LMT
								; ... else
		ADDD $9D 				; SPK ADV REL TO REF, (Unlim)
		STD $9D 				; SPK ADV REL TO REF, (Unlim)
LD0D1: 	JSR LFA1E 				; TPS ACCEL ENR ??
		LDD $3FCA 				; Counter #3, 16.5 Khz
		PSHB 					;
		PSHA 					;
		SUBD $A1 				;
		TSTA 					;
		BEQ LD0E0 				;
								; ... else
		LDAB #255 				; USE MAX VAL
LD0E0: 	PULX 					;
		PSHB 					;
		STX $A1 				;
		CLRA 					;
		BRSET $03 $80 LD105 	; BR IF b7, KNOCK ENBLED BY DIFF COOL
								; ... else
		LDAB $5B 				; COOLANT
		SUBB $5F 				; START UP COOL
		BLS LD0F9 				; BR IF COOL G.T. START UP COOL
								; ... else
		CMPB LC1BD 				;
		BLS LD0F9 				;
		BSET $03 $80 			; SET b7, KNOCK ENBLED BY DIFF COOL
		BRA LD105 				;
								;
								; CK KNOCK CONTROL QUAL'S
								;
LD0F9: 	LDAB $5B 				; COOLANT
		CMPB LC1BC 				; If COOLANT < 67c, DISABLE KNOCK
		BCC LD105 				; BR IF COOL GT 67c
								; ... else
		BCLR $40 $80 			; CLR b7, KNK ENABLED (BY COOLANT)
		BRA LD157 				;
LD105: 	BSET $40 $80 			; SET b7, KNK ENABLED (BY COOLANT)
								;
		LDAB $58 				; FILT RPM/12/5
		CMPB LC1B9 				; If RPM = > 650 RPM KNOCK is ENABLED
		BCC LD116 				; BR IF RPM G.T. 650
								; ... else
								;
								; CK ERR #43 STATUS
								;
		LDAB $65 				;
		CMPB LC1BA 				; If ERR 43, then KNOCK retard 8 Deg
		BCS LD157 				;
								; ... else
LD116: 	LDAA LC22A 				; If ERR 43, then KNOCK retard 8 Deg
		LDAB $02 				; MD WD 2
		BMI LD157 				;
								; ... else
								;
								; CK BATTERY
								;
		LDAB $7E 				; Batt Volts Value, (A/D)
		CMPB #93 				; 9.3 VDC
		BCS LD157 				; BR IF BATTERY L.T. 9.3 VDC
								; ... else
		LDAB $3B 				;
		BITB #$20 				; b5
		BNE LD157 				; BR IF b5
								; ... else
								;--------------------------------------------------
								; KNOCK ATACK RATE vs RPM
								;
								; TBL = ATTACK RATE IN Deg/MSEC /0.0225,
								; 400 - 4800 RPM
								;-------------------------------------------------
		LDAA $56 				; RPM/25
		LSRA 					; RPM/25/4			
		LSRA 					;			
		LDX #$C1BE 				; Tbl Addr,
								; - Diff Cool Temp G.T. Enab KNOCK
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		PULB 					; GET KNOCK CNT FM STX
		PSHB 					;
		MUL 					; KNOCK CNT * ATACK RATE
		ASLD 					; MULT * 2
		ADDA $A5 				; SUM TO KNOCK RETARD
		BCC LD13D 				; .... else
								; ... else
		LDAA #255 				; USE MAX VALUE
LD13D: 	PSHA 					;
		LDAA LC1BB 				; 25.3 deg Max KNOCK Retard, (not in WOT)
		LDAB $43 				; AF MODE WD
		BITB #$20 				; b5, PWR ENR IS ACTIVE
		BEQ LD152 				; BR IF NOT b5
								; .. else
								;---------------------------------------------
								; Lk Up MAX KNOCK RETARD WHEN IN WOT vs RPM
								; TBL = Deg * (256/45) 800 - 6400 RPM
								;--------------------------------------------
		LDX #$C1C8 				; Point to tbl
		LDAA $57 				; RPM/25
		LSRA 					; RPM * 2
		LDAB #16 				; Offset for Lk Up
		JSR LF2B9 				; 2d Lk Up
LD152: 	PULB 					;
		CBA 					; COMP LK'ED UP RESULT to B reg
		BCS LD157 				; BR IF B G.T. K'ED UP RESULT
								; .. else
		TBA 					;
								;
								; RETARD
								;
LD157: 	STAA $A5 				; Retard for knock
		LSRA 					; RETARD/2
		PSHA 					;
		LDAB $33 				; MINOR LOOP MD WD 1
		BPL LD164 				;
								; .. else
		LDAB $4B 				; HU SPK FLAGS
		RORB 					;
		BCS LD16D 				;
								; .. else
LD164: 	LDD $9D 				; SPK ADV Rel to Ref, (Unlim)
		TSX 					;
		SUBB 0,X 				;
		SBCA #0 				;
		STD $9D 				;
LD16D: 	PULA 					;
		LDAA $02 				; MB WD 2
		PULB 					;
		TSTB 					;
		BEQ LD180 				;
								; ... else
		BITA #$01 				;
		BEQ LD17C 				;
								; ... else
		ANDA #$FE 				;
		ORAA #$40 				;
LD17C: 	ANDA #$7F 				; CLEAR b7, ERR 43 TEST FAIL THIS START
		STAA $02 				; MD WD 2
								;---------------------------------
								; DIAG 20 Deg SPK ADV
								;---------------------------------
LD180: 	LDAA $41 					; CK IF IN DIAG, DIAGNOSTIC MODE WD 3
		BPL LD189 				;
								;
		LDD LC022 				; 20 Deg, Fixed S.A. For diagnostics
		STD $9D 				; Spk Adv Rel to Ref, (Unlim)
								;---------------------------------
								; LIMIT SPARK RETARD
								;---------------------------------
LD189: 	LDX LC020 				; MAX Spk. RETARD lIMIT FM tBL
		CPX $9D 				; SPK ADV REL TO Ref, (Unlim)
		BLT LD192 				;
								; ... else
		STX $9D 				; USE MAX LMT FOR Spk RTD
								;------------------------------------------------
								;--------------------------------------------------
								; CK IF IN MODE 4 < ----*****
								;
								; CNT'L WD ..,b3 1 = CONTROLING SPK
								; CNT'L WD ..,b5 1 =
								;
								;
								;--------------------------------------------------
LD192: 	BRCLR $35 $08 LD1C8 	; BR IF NOT b3, MODE 4
								; ... else
								;
								; CK MODE 4 CNT'L CWD 5, b3
								;
		LDAA $0158 				; ALDL FUNCT MOD ENABLE WD
		BITA #$08 				; b3
		BEQ LD1C8 				; Br IF NOT b3, NOT DOING SPL, (EXIT)
								; ... else
								;
								; CK IF ALDL MODIFING SPARK
								; (MODE 4 CWD 8)
								;
		LDAB $015B 				; ALDL SPK ADV, (MODE 4 WD 8, b4)
		BITA #$10 				; b4
		BNE LD1B0 				; Br IF b4, CONTROLLING Spk
								; ... else
		BITA #$20 				; b5
		BEQ LD1AD 				; Br NOT b5, IF ADV SPARK
								; ... else
		LDAA #128 				;
		NEGB 					;
		BRA LD1C4 				; Br IF IN RETARD SA
LD1AD: 	CLRA 					;			
		BRA LD1C4 				; EXIT W/NO SPK MOD
								;
								; CK IF ADD or RETARD SPK
								;
LD1B0: 	BITA #$20 				; b5
		BNE LD1BD 				; BR IF b5, SUBTRACT SPARK ADVANCE
								; ... else
								;
								; ADD SPARK ADVANCE
								;
		LDD $9D 				; SPK ADV REL TO REF, (Unlim)
		ADDB $015B 				; ALDL SPK ADV
		ADCA #00 				; 40 deg ???
		BRA LD1C4 				; GO SAVE SPK ADV
								;
								; SUBTRACT SPARK ADVANCE
								;
LD1BD: 	LDD $9D 				; Spk Adv Rel to Ref, (Unlim)
		SUBB $015B 				; ALDL SPK ADV
		SBCA #$0 				;
LD1C4: 	STD $9D 				; Spk Adv Rel to Ref, (Unlim)
		BRA LD1DB 				;
								;--------------------------------------------------
								;--------------------------------------------------
LD1C8: 	BRSET $41 $80 LD1DB 	; BR IF b7, IN DIAGNOSTICS
								; ... else
		LDX $9D 				; Spk Adv Rel to Ref, (Unlim)
		BMI LD1DB 				; BR IF RETARD
		PSHX 					;
		TSX 					;
		LDAA $012A 				; SPK ADV BLEND MULT
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		PULX 					;
		STD $9D 				; Spk Adv Rel to Ref, (Unlim)
LD1DB: 	LDD $9D 				; Spk Adv Rel to Ref, (Unlim)
		BMI LD1E7 				;
		LDAA $34 				;
		ANDA #$FE 				; CLR b0, IF ADVANCE CLR ADV/RETARD FL
		STAA $34 				;
		BRA LD1EE 				;
LD1E7: 	LDAA $34 				;
		ORAA #$01 				; SET b0, IF RETARD, (ADV/RETARD FLG)
		STAA $34 				;
		NEGB 					;
LD1EE: 	PSHB 					;
								;---------------------------------------------
								; CK HEADS UP
								; MAKE SPK ADV REL TO TDC
								;
								;---------------------------------------------
		LDAA $33 				; CK IF HEADS UP ON LINE
		BPL LD1F6 				; BR IF NO HEADS UP
								; ... else
		JSR $580F 				; TO HEADS UP <---------------
		;*******************************************************
		;*******************************************************
		;* SPARK OUT ROUTINE
		;*
		;* EXECUTED EVERY OTHER 1RQI
		;* ALL MNR LOOP SPARK FUNCTIONS and Vss
		;*
		;*
		;*******************************************************
LD1F6: 	PULA 					;
		TSX 					;
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		PULX 					;
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDAB $34 				; FLAG
		RORB 					;
		BCS LD20C 				; TO SPK OUT ROUTINE
								; ... else
		CLRA 					;
		CLRB 					;
		SUBD 0,X 				;
		PULX 					;
		BRA LD20E 				;
LD20B: 	RTS 					;
								;--------------------------------------------------
								;--------------------------------------------------
								; SPARK OUTPUT ROUTINE
								;
								; BUA, TYPE $32 ECM P/N 1227165
								;--------------------------------------------------
LD20C: 	PULB 					;
		PULB 					;
LD20E: 	SUBB LC01D 				; Time Corr for Spark, Aprox 200 usec
		SBCA #0			
		PSHB 					;
		PSHA 					;
		TSX 					;
								;------------------------
								; LMT SPK ADV INCREASE TO
								; DRP PERIOD/16
								;------------------------
		CLRA 					;
		CLRB 					;
		SUBD $95 				; Current Minor LP Ref Period fm MPU
		LSRD 					; TIME/16
		LSRD 					;
		LSRD 					;
		LSRD 					;
		ORAA #$F0 				; MASK FOR MSB
		ADDD $3FF6 				; Time Fm Ref to Fire IGN Cnt'r
		SUBD 0,X 				;
		BMI LD243 				;
								; ... else
		ADDD 0,X 				;
		STD 0,X 				;
		ASLD 					;
		LDD $95 				; Current Minor LP Ref Period
		BCC LD23B 				;
								; ... else
		COMA 					;
		COMB 					;
		ADDD #1 				;
		SUBD 0,X 				;
		BCC LD23F 				;
								; ... else
		BRA LD243 				;
LD23B: 	SUBD 0,X 				;
		BCC LD243 				;
LD23F: 	ADDD 0,X 				;
		STD 0,X 				;
LD243: 	LDD 0,X 				;
		SUBD $3FF6 				; TIME Fm Ref to Fire IGN Cnt'r
		BSR LD20B 				; SHORT DELAY
		STD $3FE8 				; CURRENT SPK Fall Cnt'r
		BSR LD20B 				; SHORT DELAY
		ADDD $3FDC 				; SPK Dwell period cnt'r
		BSR LD20B 				; SHORT DELAY
		SUBD $9B 				; Dwell Cnt's to CPU
		STD $3FE6 				; SPK Dwell Cnt'r
		BSR LD20B 				; SHORT DELAY
		LDD $9B 				; Dwell Cnts to MPU
		STD $3FDC 				; SPK Dwell period cnt'r
		BSR LD20B 				; SHORT DELAY
		PULA 					;
		PULB 					;
		STD $3FF6 				; Time Fm Ref to Fire IGN Cnt'r
		LDAA $33 				; MINOR LOOP MD WD 1
		BITA #$08 				; b3, 1st DRP BIT = 1 IF 1ST DRP NOW
		BEQ LD2A4 				; BR IF NOT b3, 1st DRP
		BITA #$10 				;
		BNE LD295 				; BR IF b4
								; ... else
								;
								; CK IF ERR 42
								; SPK MON EANBLED
								;
		LDAB LC1D2 				; Mask For ERR flag 3
		BITB #$20 				; b5, SKP Monitor error
		BEQ LD295 				; BR IF NOT b5
								; ... else
		LDAB $34 				;
		ASLB 					;
		BMI LD295 				;
								; ... else
		ASLA 					;
		BMI LD28A 				;
								; ... else
		LDAB $B4 				; SPK Fd Bk counter, (Err #42)
		CLR $B4 				; SPK Fd Bk counter, (Err #42)
		CMPB LC227 				; ERR #42 Param, If SPK toggled,
								; (PA1 cnt's) >= 2 times,
								; prior to eng run set ERR #42
		BLS LD292 				; BR IF LT 2 CNT'S
								; ... else
LD28A: 	BSET $33 $40 			; SET b6, LOCK IN ERR 42B
		LDD $3FFC 				; CPU CR
		BRA LD2D7 				;
LD292: 	BSET $34 $40 			; BYPASS CHECK ENABLED
LD295: 	LDX $3FEC 				; B Cnt'r last ref
		LDAB $33 				; MINOR LOOP MD WD 1
		ANDB #$F7 				; CLR b3, 1st DRP BIT, (VALID PERIOD)
		STAB $33 				;
		INCB 					;
		INCB 					;
		STX $3FE4 				; B Cnt'r, Bgn nxt Dwell
		INCB 					;
LD2A4: 	MUL 					;
		LDD $3FFC 				; CPU CR
		ORAB #$10 				; SET b4
		BRSET $30 $04 LD2B7 	; BR IF b2, EST ENABLED
								; ... else
		CLR $012A 				; SPK ADV BLEND MULT
		BSET $36 $20 			; SET b5, (BLEND SPK)
		BSET $30 $04 			; SET b2
LD2B7: 	BRSET $36 $02 LD2C0 	; BR IF b1, (IN STALL SVR SPK)
								; ... else
		TST $01 				;
		BPL LD2DA 				; TO CK MODE 4 ROUTINE
								; ... else
LD2C0: 	BCLR $30 $04 			; CLR b2
		ANDB #$EF 				;
		BRCLR $36 $01 LD2D7 	; BR IF NOT b0, (SKP ERR42 FOR STALL SVR)
								; ... else
		PSHB 					;
		LDAB $B4 				; SPK Fd Bk counter, (Err #42)
		CMPB LC227 				;
		PULB 					;
		BLS LD2DA 				;
		BSET $01 $80 			;
		BRA LD2DA 				;
LD2D7: 	BSET $4E $20 			; SET b5, NEW ERR 2
								;--------------------------------------------------
								; CK IF IN MODE 4 <----*****
								; (MODE 4 MD WD 4, b3)
								;
								;--------------------------------------------------
LD2DA: 	BRCLR $35 $08 LD301 	; BR IF NOT b3, MODE 4
								; ... else
		PSHA 					;
		LDAA $0154 				; MODE 4 MD WD 4
		BITA #$08 				; b3
		BEQ LD300 				; BR IF NOT b3, EXIT
								; ... else
		ANDB #$EF 				; CLR b4
		BCLR $30 $04 			; CLR b2
		PULA 					;
		PSHA 					;
		STD $3FFC 				; CPU CR
		SEI 					; TURN OFF INTERUPTS
		JSR LF1E5 				; SPI ROUTINE
		CLI 					; CLEAR & RESTORE INTERUPTS
		CLR $B4 				; SPK Fd Bk counter, (Err #42)
		BSET $36 $01 			; SET b0,
								; (SKIP ERR STALL SAVER SPK BYPASS)
		LDX $3FC8 				; SPK Period
		STX $B2 				; OLD PA2 CNT'R
LD300: 	PULA 					;
								;--------------------------------------------------
LD301: 	STD $3FFC 				; CPU CR
		BRA LD365 				;
		;***************************************************
		;* END OF SPK OUT
		;***************************************************
		;***************************************************
		;* LOW RPM SPARK
		;*
		;***************************************************
LD306: 	LDAA #00 				;
		STAA $400C 				; CPU COP
		LDX #$FFFF 				;
		STX $0468 				;
		INX 					;
		STX $56 				; RPM/25
		STX $58 				; RPM/12.5
		LDAA $01 				;
		BITA #$08 				; b3
		BNE LD31E 				; BR IF b, BAD SHUTDOWN
								; ... else
		STX $1A 				; Eng Run Time (sec)
LD31E: 	LDAA LC33B 				; MINOR LOOP MD WD 1
		CLRB 					;
		STD $E5 				; Trans flt'rd Ld Val
		STX $D9 				; Accel Enrich, Acum Fuel
		STAB $E2 				; Diff Ld Val Accel Enr Factor
		STAB $0104 				;
		LDAA $F3 				;
		BITA #$04 				; IAC Md Wd #2
		BNE LD35D 				;
								; ... else
		BSR LD335 				;
		BRA LD35D 				;
LD335: 	LDAB $3D 				; AIR MODE WD
		ASLB 					;
		BPL LD33F 				;
								; ... else
		LDAB LC665 				; A/C LRN, .. STEPS
		STAB $2D 				; Idle Spd A/C Antic
LD33F: 	LDAB LC62F 				; ... STP'S IDLE RPM START UP
		JSR LD3DE 				;
		LDAA #255 				; LIMIT
		STAA $0109 				;
		STAA $0102 				;
								;
		LDAA #$80 				; SET BIT 7, AC STATUS OFF
		STAA $F2 				; IAC Cnt'l Word
		CLRA 					;
		LDAB $37 				; MCU INPUT STATUS WD
		BITB #$01 				; b0, IN PK/NEUT
		BEQ LD35A 				; BR IF NOT b0, IN PK/NEUT
								; ... else
		LDAA #$90 				;
LD35A: 	STAA $F3 				; IAC Md Wd #2
		RTS 					;
								;--------------------------------------------------
LD35D: 	LDAA #$EF 				; TIME OUT AF CNT'R
		JSR LF49A 				;
		BCLR $30 $04 			; CLEAR EST ENABLE BIT
LD365: 	SEI 					; TURN OFF INTERUPTS
		JSR LF1E5 				; FMD serial byte read
		CLI 					; CLEAR & RESTORE INTERUPTS
		JSR LF92A 				; TO FUEL OUT
		JSR LE436 				; TO ASYNC OUT
		;*******************************************************
		;*******************************************************
		;* PID MINOR LOOP
		;*
		;* 12.5 MSEC CODE
		;*
		;*
		;*******************************************************
		LDAB $F3 				; IAC Md Wd #2
		BITB #$04 				; b2
		BNE LD393 				; BR IF b2
								; ... else
		LDAA $35 				; MNR LOOP MD WD2
		BITA #$20 				; b5, DIAG SW IN ALDL MODE
		BEQ LD387 				; BR IF NOT b5
								; ... else
		LDAA $3C 				;
		RORA 					;
		BCS LD387 				;
								; ... else
		LDAA $57 				; RPM/25
		CMPA #80 				; 2000 RPM
		BCC LD393 				;
								; ... else
LD387: 	LDAA $02 				; Mode Wd2
		BITA #$10 				; b4, SKP IAC MOROR RESET
		BNE LD3F0 				; BR IF B4
								; ... else
		LDAA $33 				;
		BITA #$10 				; b4, 1 = IGN OFF
		BEQ LD3F0 				; BR IF NOT b4
								; ... else
LD393: 	LDAA $2C 				; IAC Present Posit
		BITB #$02 				;
		BNE LD3AC 				;
								; ... else
		BITB #$04 				; b2
		BNE LD3A7 				; BR IF NOT b2
								; ... else
		ORAB #$04 				; b2
		LDAA #$FF 				;
		STAA $2C 				; IAC Present Posit
LD3A3: 	LDAA #$FF 				;
		BRA LD3B4 				;
LD3A7: 	TSTA 					;
		BNE LD3A3 				;
								; ... else
		ORAB #$02 				; b1
LD3AC: 	SUBA LC62F 				;
		NEGA 					;
		BPL LD3B4 				;
								; ... else
		LDAA #127 				;
LD3B4: 	STAA $0101 				;
		BNE LD3D0 				; BR IF NZ
								; ... else
		LDAA $35 				; MNR LP MD WD 2
		BSET $02 $10 			; SET b4, SKIP IAC MOTOR RESET
		BITA #$20 				; b5,
		BEQ LD3C5 				; BR IF NOT b5
								; ... else
		BSET $3C $01 			;
LD3C5: 	LDAA $33 				; MINOR LOOP MD WD 1
		BITA #$10 				; b4, 1 = IGN OFF
		BNE LD3D0 				; BR IF b4
								; ... else
		JSR LD335 				;
		BRA LD3F0 				;
LD3D0: 	STAB $F3 				; IAC Md Wd #2
LD3D2: 	CLR $0105 				;
		LDAB $F2 				; IAC Cnt'l Word
		ANDB #$EF 				; CLR b4
		STAB $F2 				;
								;
		JMP LD6CA 				;
LD3DE: 	LDAA $2C 				; IAC Present Posit
		SBA 					;
		BCC LD3E4 				;
								; ... else
		NEGA 					;
LD3E4: 	BPL LD3E8 				;
								; ... else
		LDAA #127 				;
LD3E8: 	BCS LD3EC 				;
								; ... else
		ORAA #$80 				; b7
LD3EC: 	STAA $0101 				;
		RTS 					;
								;--------------------------------------------------
LD3F0: 	LDAA $34 				; MINOR LP MD WD
		BPL LD3D2 				;
		LDAA $84 				; TPS T/F Ld Axies Var
		STAA $85 				; Old Val Fm TPS Tbl
		LDAA $82 				; TPS Ld Axis Var
		TAB 					;
		SUBB $84 				; TPS T/F Ld Axies Var
		BCC LD400 				;
		NEGB 					;
LD400: 	CMPB LC64C 				; CMD speed Param
		BLS LD407 				;
		STAA $84 				; TPS T/F Ld Axies Var
LD407: 	LDAB $3F 				;
		ANDB #$24 				; b2 & b5
		BEQ LD413 				;
		LDAA LC64B 				; CMD speed Param
		ASLA 					;
		STAA $84 				; TPS T/F Ld Axies Var
LD413: 	JSR LE87A 				; EXIT TF CODE
		;******************************************************
		;*******************************************************
		;* PID ENGINE SPEED REGULATOR
		;*
		;* 50 Msec PID LOGIC
		;*
		;*
		;*
		;*******************************************************
		LDAA $00 				;
		ANDA #$07 				; ALL EXCEPT b3
		BEQ LD41F 				; BR IF b3
								; ... else
		JMP LD6CE 				;
		;****************************************
		;* ESTABLISH IDLE SPEED CMD SPEEDS
		;****************************************
LD41F: 	PSHX 					;
		PSHX 					;
		PSHX 					;
		PSHX 					;
		TSY 					;
								;--------------------------------------
								; LK UP IAC TARGET RPM vs coolant Temp.
								;
								;
								;--------------------------------------
		LDAA $5B 				; Deg c COOL
		LDX #$C637 				; INDEX TGT RPM Tbl
								;
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		LDX #$C62E 				; INDEX IAC Tables & Params
								;
								;
								; CL P/N RPM CMD FLOOR
								;
		LDAB $F3 				; IAC Md Wd #2
		ANDB #$DF 				; CLR b5
		STAB $F3 				; IAC Md Wd #2
		BPL LD43A 				;
								; .... else
								;
								; ADD IN AN OFFSET FOR P/N
								;
		ADDA 6,X 				; IAC offset added for pk/neut, (50 RPM)
LD43A: 	LDAB $F2 				;
		BMI LD440 				;
								; ... else
		ADDA 8,X 				; IAC Offset Cmd Spd Added, (0 RPM)
LD440: 	LDAB $3A 				; HU DISPLAY MD WD
		ANDB #$20 				; b5, SET 1K RPM (1000 RPM)
		BEQ LD448 				; BR IF NOT b5
								; ... else
		ADDA $57,X 				; Add 300 RPM speed offset if hot spark
LD448: 	LDAB $35 				; MNR LOOP MD WD 2
		BITB #$20 				; b5, ALDL SW IN DIAG MODE
		BEQ LD450 				; BR IF NOT b5
								; ... else
		LDAA #80 				; 2000/25
LD450: 	STAA $0102 				; MIN RPM IN P/N AS REQ RPM
								;--------------------------------------------------
								; CK IF IN MODE 4
								; MODE 4 CNT'L WD 8, b0 & b1
								; MODE 4 CNT'L WD .. HAS NEW IAC CMD SPEED
								;--------------------------------------------------
		BRCLR $35 $08 LD468 	; BR IF NOT b3, MODE 4
								; ... else
		LDAA $0158 				; ALDL FUNCT MOD ENABLE WD
		BITA #$01 				; b0
		BEQ LD468 				; BR IF NOT b0, MOD'ING, (EXIT)
								; ... else
		BITA #$02 				;
		BEQ LD468 				; BR IF NOT b1,
								; ... else, MOD'ING, (EXIT)
		LDAA $0159 				; GET ALDL IAC CMD SPEED
		STAA $0102 				; IAC CNT'L SPEED
								;--------------------------------------------------
LD468: 	CMPA $58 				; RPM/12.5
		BCC LD472 				;
								; ... else
		LDAA $F2 				; IAC Cnt'l Word
		ORAA #$02 				; SET b1, SET MOTOR RESET STARTED BIT
		STAA $F2 				;
								;----------------------------------------
								; CALC 50 Msec RPM RATE
								;
								;
								;----------------------------------------
LD472: 	LDD $FF 				; 50 Mssec OLD FILT RPM RATE
		SUBD $58 				; RPM/12.5
		RORA 					; SCALE RPM RATE
		RORB 					;
		ASRA 					;
		RORB 					;
		ASRA 					;
		RORB 					;
		ASRA 					;
		RORB 					;
		BSR LD4CD 				;
		STAB $FF 				; SAVE 50 Mssec FILT RPM RATE
								;----------------------------------------
		CLRA 					;
		CLRB 					;
		STD 0,Y 				;
		LDAB $58 				; RPM/12.5
		STD 4,Y 				;
		LDAB $0102 				; IAC CNT'L SPEED
		SUBD 4,Y 				;
		BSR LD4CD 				;
		STAB 4,Y 				;
		TBA 					;
		BPL LD49B 				;
								; ... else
		NEGA 					;
LD49B: 	STAA 2,Y 				;
		CMPA $1C,X 				;
		BLS LD4A5 				;
								; ... else
		CLRA 					;
		BRA LD4AB				;
LD4A5: 	LDAA $0107 				; DEAD BAND TMR
		INCA 					; BUMP UP TMR
		BEQ LD4AE 				; BR IF Z
								; ... else
LD4AB: 	STAA $0107 				; DEAD BAND TMR
LD4AE: 	LDAA $010B 				; SAVE NEW DISABLE CLSD LP PID TMR
		BEQ LD4B7 				; BR IF Z
								; ... else
		DECA 					; DECR TMR
		STAA $010B 				; SAVE NEW DISABLE CLSD LP PID TMR
LD4B7: 	LDAA $84 				; TPS T/F Ld Axies Var
		CMPA $1D,X 				;
		BCC LD4C3 				;
								; ... else
		LDAA $67 				; VSS TABLE INPUT, MPH/(16/5)
		CMPA $20,X 				;
		BLS LD4DD 				;
								; ... else
LD4C3: 	LDAA $F2 				; IAC Cnt'l Word
		ANDA #$FE 				; CLR b0
		STAA $F2 				; IAC Cnt'l Word
		TBA 					;
		JMP LD550 				;
LD4CD: 	ASLD 					; MILT * 2
		BCC LD4D6 				; IF NO OVERFLOW
								; ... else
		INCA 					;
		BEQ LD4DB 				; BR IF Z
								; ... else
		CLRB 					;
		BRA LD4DB 				;
LD4D6: 	TSTA 					;
		BEQ LD4DB 				; BR IF Z
								; ... else
		LDAB #255 				; MAX POS LMY
LD4DB: 	RORB 					;
		RTS 					;
								;------------------------------------------
								;--------------------------------------------------
								;
								;
								;
								;--------------------------------------------------
LD4DD: 	TST $67 				; VSS TABLE INPUT, MPH/(16/5)
		BNE LD53C 				; BR IF MOVING, SOME Vss
								; .... else
		LDAA $010B 				; DISABLE CLSD LP PID TMR
		BNE LD54F 				;
								; .... else
		PSHB 					; SAVE RPM ERR TO STX
		LDAA $F3 				; IAC Md Wd #2
		BPL LD4F5 				;
								; .... else
								;
								; NOW IN NEUTRAL
								;
		BITA #$10 				; TST OLD PK/NEUT STATUS
		BNE LD503 				; BR IF IT WAS NEUT
								; .... else
		CLRB 					;
		ORAA #$10 				; SET b4, DLY TMR, XISTION DELAY
		BRA LD4FD 				;
								;
								; NOW IN DRIVE
								;
LD4F5: 	BITA #$10 				; b4, WE WE IN DRIVE, (OLD VAL)
		BEQ LD503 				; BR IF NOT b4
								; ... else
								;
								; OLD STATE WAS NEUT
								;
		LDAB $47,X 				; LC675, DELAY TMR, 320 Msec
		ANDA #$EF 				; CLR b4, OVR SPEED ENABLE
LD4FD: 	STAB $010B 				; DISABLE CLSD LP PID TMR
		CLR $0107 				;
LD503: 	STAA $F3 				; IAC Md Wd #2
		BMI LD53B 				; GOTO TO OPN LOOP TESTS
								; ... else
								;
								; OLD STATE WAS IN DRIVE
								; SINCE IN DRIVE, LEARN MIN POSIT FOR IDLE
								;
								; 400 MSEC MIN RPM ERR DEAD BAND TIME BEFORE
								; LEARN ENABLE/DISABLE
								;
		LDAA $0107 				; RPM ERR TMR
		CMPA $3B,X 				; 400 Msec MIN RPM ERR TIME
		BLS LD53B 				; GOTO TO OPN LOOP TESTS
								; ... else
								;
								; LEARN NEW MIN POSIT FOR IDLE
								; IF SLEWED ABS, DON'T LEARN
								;
		LDAA $F3 				; IAC Md Wd #2
		ASRA 					;
		BCS LD53B 				;
								; ... else
								;
								; HOT RESTART ACTIVE ?
								;
		LDAA $3A 				; HU DISPLAY MD WD
		BITA #$20 				; b5
		BNE LD53B 				;
								; ... else
								;
								; LEARN MIN MOTOR AT 100 MSEC LEARN RATE
								;
		LDAA $00 				; NINOR LP CNTR
		CMPA #$30 				;
		BNE LD53B 				; BR IF NOT LP # $30
								; ... else
								;
								; COMPARE PRESSENT vs MIN MOTOR POSIT'S
								;
		LDAA $0109 				;
		LDAB $010F 				;
		BEQ LD53B 				;
								; ... else
		BMI LD534 				;
								; ... else
		INCA 					;
		CMPA $010C 				; INIT IAC WARM PARK POSIT
		BLS LD538 				;
								; ... else
		LDAA $010C 				; INIT IAC WARM PARK POSIT
		BRA LD538 				;
LD534: 	DECA 					;
		BNE LD538 				;
								; .... else
		INCA 					;
LD538: 	STAA $0109 				;
LD53B: 	PULB 					; GET RPM ERR FM STX
LD53C: 	LDAA $F2 				; IAC Cnt'l Word
		BITA #$04 				; B2, MOTOR RESET WORKING
		BEQ LD553 				;
								; ... else
		BITA #$18 				; b3 & b4
		BNE LD54F 				;
								; ... else
		BRSET $37 $08 LD54F 	; BR IF b,3, MCU INPUT STATUS WD PW STEER SW
								; ... else
								;
								; BYPASS PID IF A/C ANTICIPATE STEPS
								; ARE BEING TAKEN
								;
		LDAA $0106 				; PID A/C TIMER
		BEQ LD59A 				;
								; ... else
								;
								; DISABLE A/C LEARNING IF TPS OPEN or
								; Vss GT 0
								;
								; UNDER SPEED ENABLE CHECK FOR A PID OVERIDE
								;
LD54F: 	TBA 					; SAVE ABC VSL OF RPM ERR
LD550: 	TSTA 					; UNDER SPD ERR ?
		BPL LD588 				; BR IF SO,
								; ... else
LD553: 	CLR $0107 				; CLR DEAD BAND COUNTER
		LDAB $84 				; TPS T/F Ld Axies Var
		CMPB LC64D 				; CMD speed Param
		BCS LD585 				;
								; ... else
		LDAB $67 				; MPH/ (16/5)
		CMPB $20,X 				; 1.8 MPH, LC64E, Enab C/L PID if MPH <=
		BLS LD585 				; BR IF
								; ... else
		LDAB $010F 				;
		BLE LD585 				;
								; ... else
		LDAA $00 				; NINOR LP CNTR
		ANDA #$18 				; MASK 0001 1000
		CMPA #$18 				;
		BNE LD585 				; BR IF b3 & b4
								; ... else
								;
								; WAIT FOR COMP MODE TO BE DONE
								;
		LDAA $F2 				; IAC Cnt'l WorD
		BITA #$08 				; b3, COMP MODE
		BNE LD585 				; BR IF b3
								; ... else
		ANDA #$EF 				; CLR b4, TF ACTIVE
		STAA $F2 				; IAC Cnt'l Word
		LDAB #129 				; 129 STEPS
		STAB $0101 				; REQUEST MOTOR STEPS
		LDAA $F3 				; IAC Md Wd #2
		ORAA #$20 				; SET b5,
		STAA $F3 				; IAC Md Wd #2
LD585: 	JMP LD6C6 				; EXIT
								;--------------------------------------------------
								;
								; CK IF MANUAL XMISH
								;
LD588: 	LDAB $54,X 				; FLAG, '1' if manual vech sel
		BEQ LD590 				; BE IF NOT MANUAL
								; ... else
		LDAB $67 				; MPH/(16/5)
		BNE LD553 				; BR IF MOVING, (BYPASS UNDER SPD CK)
								; ... else
								;
								; CK FOR PID ENABLE
								;
LD590: 	CMPA $21,X 				; 50 RPM, Min under spd err to enab PID
		BLS LD553 				; IF ERR LT 50, BP PID REQULATOR
								; ... else
		LDAA $F2 				; IAC Cnt'l Word
		ANDA #$E7 				; CLR b3 & b4, TF & COMP MODE
		STAA $F2 				; IAC Cnt'l Word
		;**********************
		;* PID ENABLED
		;*
		;**********************
LD59A: 	LDAA $F3 				; IAC Md Wd #2
		BMI LD59F 				; CK P/N
								; ... else
		INX 					; POINT TO 'IN DRIVE' SCALARS
								;
LD59F: 	LDAA $1A,X 				; LC648, Max RPM Err for deadband in P/N
		STAA $05,Y				;
								;
		LDAA $2D,X 				; 187.5 RPM/SEC, LC65B,
								; NEUT RATE DEAD BAND
		LDAB $2B,X 				; LC659, PK/NEUT intigrator gain
		STD $06,Y				;
								;
		LDAB $04,Y				; RESSTORE RPM ERR
								;
		;***************************************************
		LDX #$C62E 				; RE-INDEX IAC Tables & Params
								; TABLES FOR PID F.B.
		LDAA $02,Y				; ABS VAL OF RP ERR
		CMPA $05,Y				;
		BLS LD5F9 				;
								; ... else
LD5B9: 	TSTB 					;
		BMI LD5E1 				;
								; ... else
		LDAA $22,X 				;
		TST $04,Y				;
		BMI LD5EA 				;
								; ... else
		PSHA 					;
		LDAA $02,Y				;
		CMPA LC658 				; 50 RPM Min UNDER SPD ERROR
								; TO ENABLE HI PROP & DERIV
								; GAIN ADDITIONS, (RPM/12.5)
		PULA 					;
		BLS LD5EA 				;
								; ... else
		CMPB $24,X 				;
		BLS LD5EA 				;
		ADDA $25,X 				;
		BCC LD5EA 				;
								; ... else
		PSHB 					;
		MUL 					;
		ADCA #$00 				;
		PULB 					;
		ABA 					;
		BPL LD5ED 				;
		LDAA #$7F 				;
		BRA LD5ED 				;
LD5E1: 	LDAA $23,X 				;
		NEGB 					;
		MUL 					;
		ADCA #0 				; ROUND OFF
		NEGA 					;
		BRA LD5ED 				;
LD5EA: 	MUL 					;
		ADCA #0 				; ROUND OFF
LD5ED: 	TAB 					;
		CLRA 					;
		TSTB 					;
		BPL LD5F3 				;
								; ... else
		COMA 					;
LD5F3: 	ADDD 0,Y				;
		STD 0,Y					;		
LD5F9: 	CPX #$C632 				; STP'S ADDED FOR FAN 2
		BEQ LD611 				;
								; ... else
		LDX #$C632 				; STP'S ADDED FOR FAN 2
		LDAB $FF 				; RPM
		TBA 					;
		BPL LD607 				;
								; ... else
		NEGA 					;
LD607: 	STAA $03,Y				;
		CMPA $4A,X 				;
		BCC LD5B9 				;
								; ... else
		LDD 0,Y					;		
								;
LD611: 	JSR LD4CD 				;
		STAB 0,Y				;
		BPL LD61A 				;
								; ... else
		NEGB 					;
LD61A: 	TBA 					;
		LDAB $F3 				; IAC Md Wd #2
		BPL LD624 				;
								; ... else
		LDAB $002B,X 			;
		MUL 					;
		ADCA #0 				; ROUND
LD624: 	CMPA $3A,X 				;
		BCS LD633 				;
								; ... else
		CLR $0103 				;
		LDAB 0,Y				;
		STAB 4,Y				;
		BRA LD675 				;
LD633: 	LDAB 3,Y				;
		CMPB 6,Y				;
		BLS LD63E 				;
								; ... else
		JMP LD6C0 				;
								;---------------------------
LD63E: 	LDAB 7,Y				;
		LDAA 2,Y				;
		CMPA 5,Y				;
		BHI LD656 				;
								; ... else
		CMPA $48,X 				;
		BLS LD6C0 				;
								; ... else
		LDAB $47,X 				;
		TST $F3 				;
		BPL LD656 				;
								; ... else
		LDAB $46,X 				;
LD656: 	MUL 					;
		ASLD 					;
		LDAB 4,Y				;
		BPL LD65E 				;
								; ... else
		NEGA 					;
LD65E: 	ADDA $0103 				;
		BVC LD668 				;
								; ... else
		LDAA #$007F 			;
		BCC LD668 				;
								; ... else
		NEGA 					;
LD668: 	STAA $0004,Y			;
		STAA $0103 				;
		BPL LD671 				;
								; ... else
		NEGA 					;
LD671: 	CMPA $003A,X 			;
		BCS LD6C3 				;
								; ... else
LD675: 	LDAB $39,X 				;
		MUL 					;
		ADCA #0 				;
		STD 6,Y					;
		LDAA $3A,X 				;
		MUL 					;
		ADCA #0 				;
		BPL LD686 				;
								; ... else
		LDAA #$7F 				;
LD686: 	STAA 5,Y				;
								;---------------------------------------------
								; LK UP IAC Mult vs Cool Temp
								;
								;
								;---------------------------------------------
		LDX #$C686 				; Tbl Addr,
		LDAA $5B 				; COOLANT
		JSR LF2BF 				; 2d Lk Up W/ Interp
		PSHY 					;
		PULX 					;
		LDAB #6 				;
		ABX 					;
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		ASLB 					;
		ADCA #0 				;
		TAB 					;
		LDAA 4,Y				;
		ROLA 					;
		LDAA 5,Y				;
		BCC LD6AA 				;
								; ... else
		NEGA 					;
		ORAB #$80 				;
LD6AA: 	STAB $0101 				; REQUEST MOTOR STEPS
		ASLB 					;
		BEQ LD6B6 				;
								; ... else
		LDAB $F3 				; IAC Md Wd #2
		ORAB #$20 				; SET BIT 5
		STAB $F3 				; IAC Md Wd #2
LD6B6: 	LDAB $0103 				;
		BEQ LD6C6 				;
								; ... else
		STAA $0103 				;
		BRA LD6C6 				;
LD6C0: 	CLR $0103 				;
LD6C3: 	CLR $0101 				; REQUEST MOTOR STEPS
LD6C6: 	PULX 					;
		PULX 					;
		PULX 					;
		PULX 					;
								;-----------------------
								;-----------------------
								;
								;
								;-----------------------
LD6CA: 	LDD $58 				; RPM/12.5
		STD $FF 				; rpm
LD6CE: 	JMP LCCAC 				; TO MJR LP EXEC
		;*******************************************************
		;***************************************************
		;* Air Fuel 12.5 Msec LOOP,
		;*
		;* BUA, $32 ECM
		;*
		;*
		;*
		;***************************************************
LD6D1: 	LDX $8B 				; Ign Off tmr
		BRCLR $33 $10 LD721 	; BR IF NOT b4, (1 = IGN ON)
		CPX #$08 				;
		BCS LD724 				;
								; ... else
		BCLR $01 $FF 			; NON VOL RAM Mode word
		BCLR $02 $EF 			; CLR ALL EST MD BITS
								; EXCEPT FAILED THIS TEST BIT, (b4)
								; b4, SKIP IAC MOTOR RESET
								;
		BCLR $03 $BF 			; Mode Wd3, CLR ALL EXCEPT b6
		CPX LC012 				; SEC, IF IGN OFF THIS TIME TURN OFF ECM
								;
		BCS LD6ED 				;
								;------------------------------------
								;
								; WAIT HERE FOR SOFTWARE INTERUPT
								; SWI VECTOR TO $????
								;------------------------------------
LD6EA: 	SWI 					;
		BRA LD6EA 				;
								;------------------------------------
LD6ED: 	INX 					;
		STX $8B 				; Ign Off tmr
		CLRA 					; CLR ENG RUNNING TIMER
		CLRB 					;
		STD $1A 				; Eng Run Time (sec)
		LDAA $34 				;
		BPL LD70F 				;
								; ... else
		JSR LF447 				; Go init ...
		LDAA $34 				;
		BRCLR $33 $20 LD70F 	; BR IF NOT b5, (1 = IGN OFF)
								; ... else
		LDAB $65 				;
		BNE LD70F 				;
								; ... else
		BRSET $3E $10 LD70F 	;
								; ... else
		LDAB $3E 				;
		ORAB #$40 				; SET b6
		STAB $3E 				;
LD70F: 	ANDA #$7F 				; CLR b7, ENG RUNNING FLAG
		STAA $34 				;
		CLR $018D 				;
		CLR $93 				; Cntr for Eng Strt Up
		BCLR $3F $01 			;
		BCLR $39 $80 			; CLE b7, (IN SINGLE FIRE)
		BRA LD727 				;
								;---------------------
								;---------------------
LD721: 	LDX #$FFFF 				;
LD724: 	INX 					;
		STX $8B 				; Ign Off tmr
LD727: 	LDAB $3F 				;
		ANDB #$DF 				;1101 1111
		LDAA $34 				;
		BPL LD744 				;
								; ... else
								;
								; CK IF ERR 21 ENABLED
								;
		LDAA LC1D0 				; Mask For Mal Funct flg 1
		BITA #$02 				; b1, ERR Code 22, TPS low
		BEQ LD74C 				; BE IF NOT b1
								; ... else
		LDAA $81 				; TPS, (A/D)
		CMPA LC1EB 				; If TPS A/D >= 12 then disable ERR #22
		BCC LD74C 				;
								; ... else
		BSET $4C $02 			; set B1, NEW ERR 1
		ORAB #$20 				; b5
		BRA LD74C 				;
LD744: 	LDAA LC39B 				; 14% Min TPS Throttle Posit (init val)
		STAA $86 				; Fltered low TPS, (A/D)
		CLR $87 				;
LD74C: 	STAB $3F 				;
		LDD $82 				; TPS Ld Axis Var
		SBA 					;
		BCS LD769 				;
								; ... else
		CMPA LC66E 				; A/C Clutch Value
		BCS LD769 				;
								; ... else
		LDAA LC66F 				; A/C Clutch, 0d
		CMPA $0106 				;
		BCS LD769 				;
								; ... else
		STAA $0106 				;
		LDAA $34 				;
		ORAA #$20 				; SET b5
		STAA $34 				;
LD769: 	LDAA $82 				; TPS Ld Axis Var
		STAA $83 				; Old ___
		LDX $95 				; Current Minor LP Ref Period
		BRSET $34 $80 LD77B 	; BR IF b7, ENG RUNNING FLAG
								;
		CPX LC357 				; Crank RPM lmt
		BCS LD77B 				;
								; ... else
								;
								; Go do 16 * 16 Mult
								;
		LDX LC357 				; Mult'cnd to X  (Crk RPM lmt
LD77B: 	LDD $EA 				; Gms/Sec Disp Value, (Mult'plr)
		JSR LF2F0 				; 16 * 16 Mult routine
		PSHB 					; Save Middle 2 Bytes of Product
		PSHA 					; Save Middle 2 Bytes of Product
		LDAA $63 				; FILTERED LD VALUE
		LDAB $61 				;
		STD $61 				;
		TSX 					;
		LDAA LC69A 				; Ld VAR FOR SCALING LV8 Scale factors
								; 80d = 80/64 = 16
		JSR LF472 				; FACTOR IN 8x16 MULT
		LDD 0,X 				;
		ROLB 					;
		ROLA 					;
		BCC LD798 				;
								; ... else
		LDD #$FFFF 				;
LD798: 	ASLB 					;
		ADCA #0 				; ROUND
LD79B: 	PULX 					;
		BCC LD7A0 				;
								; ... else
		LDAA #255 				; USE MAX VAL
LD7A0: 	STAA $63 				; FILTERED LD VALUE
		LDAA $0129 				; FF CNT'R
		CMPA LC35A 				; FF CNTR , Crank Param, 0d
		BCS LD7B1 				; BR F=IF FF CNT LT 0
								; ... else
		BRCLR $34 $80 LD7B1 	; BR IF NOT b7, ENG RUNNING FLAG
		JMP LD86B 				;
								;-------------------------------------
								; LK UP Crank Fuel PW Mult vs TPS %
								;
								; 9 LINE TBL, 0 - 100% TPS
								;
								; TYPE $32 ECM
								;
								; Table = Mult * 64
								;--------------------------------------
LD7B1: 	LDAA $82 				; TPS Ld Axis Var for Lk Up
		LDX #$C390 				; Tbl Addr
		JSR LF2BF 				; Call 2d Lk Up W/ Interp
		PSHA 					; Save Lk Up Result
		LDAA $5B 				; COOLANT
		CMPA #208 				; 115c
		BLS LD7C2 				;
								;--------------------------------------------------
								; CRANK FUEL vs COOLANT Temp.
								; LC36 = 6554 Scale Factor for Max Crk P.W.
								;
								; TYPE $32 ECM
								;
								; Table = Msec * (65.536 * 256) / LC36C
								;--------------------------------------------------
		LDAA #$D0 				; Lk Up Var
LD7C2: 	LDX #$C36E 				; Tbl Addr
		JSR LF2C6 				; Call 2d Lk Up, (No Offset)
		LDX #$C36C 				;
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		STD $C2 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; CRANK FUEL PW MULT vs DRP's
								;
								;
								; Tbl = Mult * 256
								;--------------------------------------------------
		LDAA $0118 				; Get Var for Lk Up
		ASLA 					;
		LDX #$C37F 				; Tbl Addr
		JSR LF2C6 				; Call 2d Lk Up, (No Offset)
		LDX #$00C2 				;
		JSR LF266 				; MUL8X16
		STD $C2 				;
		LDX #$00C2 				;
		PULA 					;
		JSR LF472 				; FACTOR IN 8x16 MULT
		ROLB 					;
		ROLA 					;
		BCC LD7F0 				;
								; ... else
		LDD #$FFFF 				;
LD7F0: 	LDX #0 					;
		STD $C2 				;
		BEQ LD863 				;
								; ... else
		JSR LCDE6 				;
		ASLD 					;
		BEQ LD7FF 				; BR IF Z
								; ... else
		BCC LD802 				; BR IF NO OVERFLOW
LD7FF: 	LDD #$FFFF 				; USE MAX
LD802: 	PSHB 					;
		PSHA 					;
		PULX 					;
		LDD #2457 				;
		JSR LF215 				; TO F DIV SUBTOUTINE
		LDAB LC359 				; CRANK RPM Coef, (0.5)
		LDX $0127 				; OLD CRANK RPM
		JSR LF250 				; FILTER ROUTINE
								;----------------------------------
								; LK UP CRANK FUEL MULT vs RPM
								;
								; TYPE $32 ECM
								;
								; TBL = Mult * 256
								;----------------------------------
		STD $0127 				; RRM
		LDX #$C35B 				; Tbl Addr,
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		CMPA #255 				;
		BNE LD82B 				;
								; ... else
		INC $0129 				; FF CNT'R
		BNE LD82E 				;
								; ... else
		DEC $0129 				; FF CNT'R
		BRA LD82E 				;
LD82B: 	CLR $0129 				; FF CNT'R
LD82E: 	TSTA 					;
		BEQ LD865 				;
								; ... else
		PSHA 					;
		JSR LDA2D 				; GO DO BPW'S, (LK UP or CALC)
		JSR LDD2D 				; GO DO BPW'S, (LK UP or CALC)
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDAA $0A 				; Sty Alive Mem Cell A
		JSR LF472 				; FACTOR IN 8x16 MULT
		PULX 					;
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDD $C2 				;
		SUBD 0,X 				;
		PULX 					;
		PSHB 					;
		PSHA 					;
		PULY 					;
		PULA 					;
		BCS LD863 				;
								; ... else
		PSHY 					;
		TSX 					;
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		PULX 					;
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDD $C2 				;
		SUBD 0,X				;
		STD $C2 				;
		PULX 					;
		BRA LD865 				;
LD863: 	STX $C2 				;
LD865: 	JSR LF92A 				;
		JMP LDDC2 				;
								;--------------------------------------------------
LD86B: 	BRCLR $39 $80 LD872 	; BR IF NOT b7, (IN SINGLE FIRE)
								; ... else
		BSET $39 $10 			; SET b4, (TOGGLE S/F F/F)
LD872: 	LDAA $E5 				; Trans flt'rd Ld Val
		SUBA $63 				; filtered ld value
		BCS LD89E 				;
								; ... else
		BRCLR $3E $80 LD886 	; BR IF NOT b7
								; ... else
		TST $E8 				; Decel Active Time
		BEQ LD886 				;
								; ... else
		DEC $E8 				; Decel Active Time
		BRA LD896 				;
LD886: 	CMPA LC5F5 				; Diff Ld Val Tresh For Decel enlean
		BCS LD89E 				;
								; ... else
		LDAA $E9 				; Trans Fltr'ed TPS for
								; -> Decel Enlean
		SUBA $82 				; TPS Ld Axis Var
		BCS LD89E 				;
								; ... else
		CMPA LC5F4 				; Diff TPS Tresh For Decel enlean
		BCS LD89E 				;
								; ... else
LD896: 	BSET $3E $A0 			;
		BCLR $38 $10 			;
		BRA LD8A6 				;
LD89E: 	LDAA LC5F6 				; Dcel Enlean time period
		STAA $E8 				; Decel Active Time
		BCLR $3E $80 			; CLR b7
LD8A6: 	LDAA $5B 				; COOLANT
		CMPA LC601 				; Min Cool for Cut Off
		BCS LD928 				;
								; ... else
		BRSET $37 $01 LD928 	; BR IF b0, MCU INPUT STATUS WD (IN PK/NEUT)
								; ... else
		LDAA $44 				; AF MODE WD #2
		BITA #$10 				; b4, Vss FAILURE
		BNE LD8BE 				; BR IF b4
								; ... else
		LDAA $65 				;
		CMPA LC602 				; Vss Lmt for Cut Off
		BLS LD928 				;
								; ... else
LD8BE: 	LDAB $3F 				;
		BITB #$24 				;
		BNE LD8D1 				;
								; ... else
		LDAA $82 				; TPS Ld Axis Var
		CMPA LC600 				; Lo TPS Lmt for Cut Off
		BLS LD8D1 				;
								; ... else
		CLRA 					;
		CLRB 					;
		STD $88 				; DECEL Fuel C/O Stall svr timer
		BRA LD928 				;
LD8D1: 	LDAA $5A 				;
		SUBA $58 				; RPM/12.5
		BCS LD8DC 				;
		CMPA LC5FE 				; Max RPM Time
		BCC LD928 				;
								; ... else
								;
								; CK IF
								; b6 HIGH LIMIT FUEL CUTOFF ENABLED
								; b7 not used
								;
LD8DC: 	LDAA $43 				; AF MODE WD
		BITB #$C0 				; b6 & b7
		BNE LD8F0 				; BR IF b6 & b7
								; ... else
		LDAB LC5FC 				; Max Ld val For Cut Off
		BITA #$02 				; b1, DECELL FUEL CUT OFF ENABLED
		BEQ LD8EC 				;br if not b1
								; ... else
		LDAB LC5FD 				; Stay In Cut off Ld val
LD8EC: 	CMPB $63 				; FILTERED LD VALUE
		BCS LD928 				; BR IF LD VAL GT THRESH
								; ... else
								;
								; CK RPM WINDOW TO STAY IN DECELL FUEL C/O
								;
LD8F0: 	LDAB LC5FA 				; 1200 RPM, Cut off Decel Lmt
		BITA #$02 				; b1, DECELL FUEL CUT OFF ENABLED
		BEQ LD8FA 				; BR IF NOT b1,
								; ... else
		LDAB LC5FB 				; Stay In Cut off RPM Lmt
LD8FA: 	CMPB $57 				; RPM/25
		BCC LD928 				; BR IF RPM LT 900 RPM
								; ... else
								;
								; CK TIME Req FOR DECEL Fuel C/O
								;
		LDAB $D3 				; TIME Req FOR DECEL Fuel C/O
		BNE LD925 				; BR IF NZ
								; ... else
		BITA #$02 				; b1, DECELL FUEL CUT OFF ENABLED
		BNE LD90A 				; BR IF b1
								; ... else
		LDX $88 				; DECEL FUEL C/O STALL SVR TIMER
		BNE LD928 				;
								; ... else
LD90A: 	ORAA #$02 				; SET DECELL FUEL CUT OFF ENABLED FLG
		STAA $43 				; AF MODE WD
								;
								; CK SINGLE FIRE MODE SEL
								;
		LDAB LC014 				; AFR MD WD 1
		BITB #$02 				; b1, SINGLE FIRE MODE SEL
		BEQ LD918 				; BR IF NOT SINGLE FIRE MODE SEL
								; ... else
		BSET $39 $A0 			; SET b7 & b5, (IN SF & CLR SF)
LD918: 	LDAB $3D 				;
		ANDB #$EF 				; CLR b4
		STAB $3D 				;
		LDX LC608 				;6.38 Sec, Min Time Between Consec DEFCO'S
		STX $88 				; Decel Fuel C/O Stall svr timer
		BRA LD97C 				;
LD925: 	DECB 					;
		BRA LD92D 				;
								;
LD928: 	LDAA $43 				; AF MODE WD
								;
		LDAB LC5FF 				; Cut off Min Time
LD92D: 	STAB $D3 				; Time Req for Dcel Fuel C/O
								;
		ANDA #$FD 				; CLR b1, DECELL FUEL CUT OFF ENABLED
		STAA $43 				; AF MODE WD
								;
		BRCLR $39 $20 LD93A 	; BR IF NOT b5, (CLR SF FF)
								; ... else
		BCLR $39 $60 			; CLR b5 & b6, (CLR SF & 0 PW)
								;
LD93A: 	LDAA $3D 				;
		LDX $88 				; Decel Fuel C/O Stall svr timer
								;
		BNE LD946 				; BR IF
								; ... else
		ANDA #$EF 				;
		STAA $3D 				;
		BRA LD96D 				;
LD946: 	DEX 					;
		STX $88 				; Decel Fuel C/O Stall svr timer
		LDAB LC5FB 				; Cut Off RPM
		CMPB $57 				; RPM/25
		BCC LD95B 				;
								; ... else
		LDAB $5A 				;
		SUBB $58 				; RPM/12.5
		BCS LD96D 				;
								; ... else
		CMPB LC5FE 				; Max RPM Decel val
		BCS LD96D 				;
								; ... else
LD95B: 	BITA #$10 				; 0001 0000
		BNE LD96D 				;
								; ... else
		ORAA #$10 				;
		STAA $3D 				;
		LDAA LC607 				; Num of Stall Svr Pulses
		STAA $8A 				; Accel Enrich After Decel C/O
		LDAA LC604 				; Stall Sve Val
		STAA $C5 				; Stall Save IAC Stps, (Sec*80)
LD96D: 	JSR LFA1E 				; TPS ACCEL ENR ??
		LDAA $3F 				;
		BITA #$C0 				;
		BNE LD97C 				;
								; ... else
		LDAA $63 				; FILTERED LD VALUE
		SUBA $E5 				; TRANS FLT'RD Ld Val
		BCC LD97F 				;
								; ... else
LD97C: 	CLRA 					;
		BRA LD99E 				;
LD97F: 	SUBA LC33C 				; Min Delta LV8 for Acel ENR
		BCC LD987 				;
								; ... else
		CLRA 					;
		BRA LD998 				;
								;--------------------------------------------------
								; LK UP LV8 AE FACTOR vs DELTA LV8
								;
								; DELTA LV8 ABOVE MINIMUM (LC33C)
								;
								; TYPE $32 ECM
								;
								; TBL = Mult * 128
								;--------------------------------------------------
LD987: 	STAA $E1 				; Diff Ld Val for Accel Enrich
		BSET $38 $08 			; SET b3
		LDX #$C33D 				; LV8 AE FACTOR Tbl
		JSR LF2BF 				; 2d Lk Up W/ Interp
		LDAB $E7 				; Accel Enr Cool Factor
		MUL 					;
		ASLD 					;
		BCS LD99C 				; BR IF NO OVERFLOW
								; ... else
LD998: 	ADDA $E2 				; Diff Ld Val Accel Enr Factor
		BCC LD99E 				; BR IF NO OVERFLOW
								; ... else
LD99C: 	LDAA #255 				; USE MAX VAL
LD99E: 	STAA $E2 				; Diff Ld Val Accel Enr Factor
		BNE LD9A9 				; BR IF NZ
								; ... else
		LDX $D9 				; Accel Enrich, Acum Fuel
		BNE LD9A9 				; BR IF NZ
								; ... else
		BCLR $38 $0E 			; CLR b0, b1 & b2
								;
								; GET o2 VDC & FILTER
								;
LD9A9: 	LDAA $6F 				; FILT o2 VDC
		STAA $C8 				; Filtered o2, (12.5Ms)
		LDAA #$20 				; A/D CH 2, (o2 Sense Voltage)
		JSR LF1BE 				; To A/D subroutine
		LDX $6F 				; FILT o2 VDC
		LDAB LC39E 				; o2 Sens Coef, (time)
		JSR LF250 				; FILTER ROUTINE
		STD $6F 				; FILT o2 VDC
								;--------------------------------------------------
								; CK IF IN MODE 4 <----*****
								;
								; DO AFR MOD VIA ALDL MODE 4
								;
								; MODE 4 MODE WD .. b2 HAS AFR STATUS
								; MODE 4 MODE WD .. HAS NEW AFR VALUE
								;--------------------------------------------------
		BRCLR $35 $08 LD9D7 	; BR IF NOT b3, MODE 4
								; ... else
		LDAA $0158 				; MODE 4 MODE WD ..
		BITA #$04 				; b2
		BEQ LD9D7 				; EXIT
								; ... else
		LDAB $015A 				; GET ALDL AFR VAL FROM MODE WD ...
								;
								; CALC NEW AFR
								;
		CLRA 					;
		PSHB 					;
		PSHA 					;
		PULX 					;
		LDD #$FFFF 				;
		IDIV 					;
		STX $CE 				; Total AFR Value
		JMP LDA5A 				; EXIT AND BYPASS FOLLOWING AFR
								; MOD ROUTINES
								;--------------------------------------------------
		;***************************************************
		;* PWR ENRICHMENT TPS TRESHOLD vs RPM
		;* TYPE $32 ECM
		;*
		;*
		;* 400 - 4800 RPM, 5 LINES
		;*
		;* TBL = TPS% * 2.56
		;***************************************************
LD9D7: 	LDX #$C60E 				; Pwr Enrichment TPS Treshold vs RPM
		LDAA $56 				; RPM/25
		LSRA 					; Div By 4
		LSRA 					;
		JSR LF2C6 				; Call 2d Lk Up, (No Offset)
		LDAB $43 				; AF MODE WD
		BITB #$20 				; b5, PWR ENR IS ACTIVE
		BEQ LD9ED 				; BR IF NOT b5
								; ... else
		SUBA LC60D 				; TPS Hyst. Disable P.E. if TPS < 10
		BCC LD9ED 				;
								; ... else
		CLRA 					;
LD9ED: 	PSHB 					;
		ANDB #$DF 				; CLR b5, PWR ENR IS ACTIVE
		STAB $43 				; AF MODE WD
		CMPA $82 				; TPS Ld Axis Var
		PULB 					;
		BHI LDA28 				;
								; ... else
		LDAA LC60B 				; If Ld Val > 50, Enab Pwr Enrich
		BITB #$20 				; b5
		BEQ LDA01 				; BR IF NOT b5
		SUBA LC60C 				; 10d, Hyst for Pwr En.
LDA01: 	CMPA $63 				; Filtered ld value
		BHI LDA28 				;
		LDAB $43 				; AF MODE WD
		ORAB #$20 				; SET b5, PWR ENR IS ACTIVE
		STAB $43 				; AF MODE WD
		BCLR $39 $80 			; CLR b7, (IN SINGLE FIRE)
								;--------------------------------------------------
								; LK UP Pwr Enrichment Air/Fuel Pct Change vs RPM
								; 0 -> 6400 RPM, 128 = NO CHG
								;
								; TYPE $32 ECM
								;
								; TBL = (Pct Change * 1.28) + 128
								;--------------------------------------------------
		LDX #$C61D 				; Tbl Addr
		LDAA $57 				; RPM/25
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		TAB 					; RESULT TO B reg
		LDX #128 				; O% CHG VALUE
		ABX 					; ADD LK'ED UP VALUE TO NEUTRAL VAL
		LDAB $D0 				; CURRENT LK'ED UP AFR % Chg/Cool Temp
		ABX 					; ADD LK'ED UP VALUE
								;
								; MULT PCT CHNG BY CURRENT STOCH VAL
								; (DIV RESULT /256
								;
		LDD LC3CB 				; Get Stoch Ratio
		JSR LF2F0 				; Go To 16 * 16 Mult routine
		STD $CE 				; NEW AFR Value
		BRA LDA5A 				;
		;***************************************************
		;* END OF PWR ENRICH
		;*
		;***************************************************
LDA28: 	JSR LDA2D 				;
		BRA LDA5A 				;
								;--------------------------------------------------
								;
								;
								;
								;--------------------------------------------------
LDA2D: 	LDAB $0E 				; START UP A/F Ratio
		LDAA $44 				; AF MODE WD #2
		ASLA 					;
		LDAA #$01 				;
		BCS LDA51 				;
								; ... else
								;
								; ADD IN Open Lp AFR Bias to Set up Lean Lmt
								; 221, 20:1 AFR
								;
		CLRA 					;
		ADDB LC41E 				; Open Lp AFR Bias to Set up Lean Lmt
		ADCA #0 				; ROUND
		ADDB $CD 				; AFR Time out Cntr
		ADCA #0 				; ROUND
		STD $CE 				; Total AFR Value
								;-------------------------------------------
								; LK UP Opn Lp A/F Pct Chg vs Base Ld Value
								;
								;--------------------------------------------
		LDAA $63 				; Filtered ld value
		LDX #$C3D3 				; Tbl Addr,
		JSR LF2C6 				; Lk Up, (No Offset)
		TAB 					;
		CLRA 					;
		STAB $046A 				;
		ADDD $CE 				; Total AFR Value
LDA51: 	LDX LC3CB 				; Get Stoch ratio
		JSR LF2F0 				; Go to 16 * 16 Mult routine
		STD $CE 				; Total AFR Value (middle 2 Of Prod)
		RTS 					;
								;------------------------------------------
								;------------------------------------------
								; JMP HERE IF ALDL IS CONTROLING AFR
								; W/MODE 4, (BYPASS OTHER AFR MOD ROUTINES
								;
								;------------------------------------------
LDA5A: 	TST $B0 				; MAF Hi TMR, (100Ms)
		BNE LDA66 				; BR IF NZ
								; ... else
		LDAA $63 				; FILTERED LD VALUE
		CMPA LC22D 				; If LV < 170 then Disable #43
		BCC LDA6C 				; BR IF LD VAL GT 170
								; ... else
LDA66: 	LDAA $02 				;
		ANDA #$FE 				; CLR b0, ERR 43B TEST IN WORK
		STAA $02 				;
								;---------------------------------------------
								; CK IF HEADS UP ON LINE
								; (MOD AFR)
								;---------------------------------------------
LDA6C: 	LDAA $33 				; MINOR LOOP MD WD 1
		BPL LDA73 				;
								; ... else
		JSR $5800 				; H.U. ROM Addr
								;---------------------------------------------
LDA73: 	LDY #$00EA 				; Gms/Sec Disp Value
								;
								; CL AFR MD WD FOR
								;
		LDAA LC017 				; Get Fuel/Air Mode Wd 4
		RORA 					;
		BCC LDA81 				;
								; ... else
								;---------------------------------------------
								; BLM UPDATE ROUTINE
								;
								;---------------------------------------------
		LDY #$0063 				; FILTERED LD VALUE
LDA81: 	LDAA $43 				; AF MODE WD
		ANDA #$FB 				; CLR b2, BL CELL CHANGE,
								; .. (delay BLM update)
		STAA $43 				; AF MODE WD
		LDAB $BF 				; BLM CELL NUM, (cells 0-7)
		ANDB #3 				; 0000 0011,
		LDX #$C5D4 				; BLM BOUNDS
		ABX 					; ADD CELL NUM TO ADDRESS POINTER
								;
								; CK IF IDLE CELL
								;
		TSTB 					; CK FOR CELL 0
		BEQ LDAA1 				; BR IF CELL 0, (IDLE)
								; ... else
								;
								;
								; CK RPM BOUNDS
								;
		LDAA 0,X 				; GET CURRENT RPM BOUND
		SUBA LC5DB 				; BLM Hysteresis, (100 RPM RPM)
		BCS LDA9D 				; BR IF RPM L.T. BOUND VALUE
								; ... else
		CMPA $57 				; RPM/25
		BHI LDAD9 				; BR IF RPM L.T. BOUND VALUE
								; ... else
LDA9D: 	CMPB #3 				; CELL 3 ?
		BEQ LDAAC 				;
								; ... else
LDAA1: 	LDAA 1,X 				;
		ADDA LC5DB 				; 100 RPM, BLM Hysterisis
		BCS LDAAC 				;
								; ... else
		CMPA $57 				; RPM/25
		BCS LDAD9 				;
								; ... else
LDAAC: 	LDAB $BF 				; BLM CEL NUM, (cells 0-7)
		ANDB #$0C 				; MASK, 0000 1100
		LSRB 					; SHFT TO b0 & b1
		LSRB 					;
		LDX #$C5D7 				; Pt to BLM Lmts In CALIB
		ABX 					; ADD CELL NUM TO ADDRESS POINTER
		TSTB 					; CK FIR ZERO
		BEQ LDAC9 				; BR IF Z
								; ... else
		LDAA 0,X 				; GET CELL CONTENTS
		SUBA LC5DC 				; 2, BLM Hysteresis
		BCS LDAC5 				; BR IF CELL VAL LT 2
								; ... else
		CMPA 0,Y				; AIR FLW to CURRENT CELL BOUND
		BHI LDAD9 				;
								; ... else
LDAC5: 	CMPB #3 				; CK CELL NUM
		BEQ LDAD5 				;
								; ... else
LDAC9: 	LDAA 1,X 				; GET BLM AIR FLOW BOUND
		ADDA LC5DC 				; BLM Hysteresis, (2 gms/sec)
		BCS LDAD5 				;
								; ... else
		CMPA 0,Y				;
		BCS LDAD9 				;
								; ... else
LDAD5: 	LDAB $BF 				; BLM CELL ADDRESS (cells 0-7)
		BRA LDB0C 				;
LDAD9: 	CLRB 					; CLR BLM CELL NUMBER CNT'R
								;
								; SET AF MODE WD
								; b2 BL CELL CHANGE, DELAY BLM UPDATE
								; b3 DELAY BLM UPDATE, CELL CHANGE
								;
		LDAA $43 				; AF MODE WD
		ORAA #$0C 				; SET b2 & b3
		STAA $43 				; AF MODE WD
		LDAA $57 				; RPM/25
		CMPA LC5D5 				; Ck Lo BLM RPM LMT, 700 RPM
		BCS LDAF4 				;
								; ... else
		INCB 					; INCR CELL POINTER
		CMPA LC5D6 				; Ck Mid BLM RPM LMT, 1200 RPM
		BCS LDAF4 				;
								; ... else
		INCB 					; INCR CELL POINTER
		CMPA LC5D7 				; Ck Hi BLM RPM LMT, 2000 RPM
		BCS LDAF4 				;
								; ... else
		INCB 					; INCR CELL POINTER
LDAF4: 	LDAA 0,Y				; GET FLOW VAL
		CMPA LC5D8 				; BLM Lo MAF Flow Lmt, 12 gms/S
		BCS LDB0C 				; BR IF AIR FLOW LT 12 gms/S
								; ... else
		ADDB #4 				; BUMP CELL NUMBER
		CMPA LC5D9 				; BLM Mid MAF Flow Lmt, 22 gms/S
		BCS LDB0C 				; BR IF AIR FLOW LT 22 gms/S
								; ... else
		ADDB #4 				; BUMP CELL NUMBER
		CMPA LC5DA 				; BLM Hi MAF FlowBOUND, 34 gms/S
		BCS LDB0C 				; BR IF AIR FLOW LT 34 gms/S
								; ... else
		ADDB #4 				; BUMP CELL NUMBER
								;
								; UP DATE CELLS
								;
LDB0C: 	LDX #$001C 				; POINT TO BLM CELL MATRIX IN MEMORY
		ABX 					; ADD CELL NUM TO POINTER
		LDAA 0,X 				; GET CURRENT CELL BLM VLUE
		CMPA LC5E2 				; 160, Max BLM LMT
		BHI LDB1C 				; BR IF BLM LT MAX BLM LMT
								; ... else
		CMPA LC5E3 				; 108, MIN BLM LMT
		BCC LDB25 				; BR IF BLM GT MIN LMT
LDB1C: 	LDAA $3D 				;
		ORAA #$40 				; SET b6
		STAA $3D 				;
		JSR LF434 				;
LDB25: 	STAB $BF 				; BLM (cells 0-7)
		STAA $C0 				; BLM
		BMI LDB2D 				;
		LDAA #128 				;
LDB2D: 	LDAB $43 				; AF MODE WD
		BITB #$20 				; b5, PWR ENR IS ACTIVE
		BEQ LDB35 				; BR IF NOT b5
								; ... else
		STAA $C0 				; BLM
LDB35: 	LDD $EA 				; Gms/Sec Disp Value
		ASLD 					; AIR FLOW * 2
		CMPA #128 				;
		BLS LDB3E 				;
								;-------------------------------------------
								; LK UP Fast o2 R/L Treshold vs Air Flow
								;
								;-------------------------------------------
		LDAA #128 				;Lk Up Var
LDB3E: 	STAA $D2 				; Ld Val Limited for Lk Up
		LDX #$C579 				; Tbl Addr,
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		SUBA $D1 				; Lean Offset, (if Cool)
		BRSET $3E $02 LDB4F 	;
		SUBA LC562 				;
LDB4F: 	PSHA 					;
		LDD LC559 				;
		CMPA $6F 				; FILT o2 VDC
		BCS LDB5B 				;
								; ... else
		CMPB $6F 				;
		BLS LDB64 				;
LDB5B: 	CLR $BE 				; Cl/Lp o2 NR timer, (200Ms inc)
		LDAB $01 				; NON VOL RAM Mode word
		ORAB #$0001 			;
		STAB $01 				; NON VOL RAM Mode word
LDB64: 	PULA 					;
		PSHA 					;
		LDAB LC561 				;
		ABA 					;
		CMPA $6F 				;
		PULA 					;
		BCS LDB80 				;
								; ... else
		SBA 					;
		CMPA $6F 				;
		BHI LDB7A 				; GO CLR RICH FLG
								; ... else
		LDAA $6F 				;
		CMPA $C8 				; Filtered o2, (12.5Ms)
		BHI LDB80 				;
								; ... else
LDB7A: 	LDAB $44 				; AF MODE WD #2
		ANDB #$BF 				; CLR b6, 1 =RICH, 0 = LEAN
		BRA LDB84 				;
LDB80: 	LDAB $44 				; AF MODE WD #2
		ORAB #$40 				; SET b6 1 = RICH
LDB84: 	CMPB $44 				; AF MODE WD #2
		BEQ LDB94 				; BR IF Z, 0 = LEAN
								; ... else
		INC $F1 				; INCR ALDL Rich/Lean Chg Cnt'r
		LDAA $3C 				;
		ORAA #$10 				; SET b4
		STAA $3C 				;
		CLRA 					;
		BRA LDB99 				;
LDB94: 	LDAA $C7 				; Cls Lp Duration Cntr, (Sec*80)
		INCA 					;
		BEQ LDB9B 				; BR IF Z
								; ... else
LDB99: 	STAA $C7 				; Cls Lp Duration Cntr, (Sec*80)
LDB9B: 	STAB $44 				; AF MODE WD #2
		LDAB $44 				; AF MODE WD #2
		BPL LDBFA 				;
								; ... else
								;
								; CK AF MODE WD
								; b1 DECELL FUEL CUT OFF ENABLED
								; b5 PWR ENR IS ACTIVE
								;
		LDAA $43 				; AF MODE WD
		BITA #$22 				; b1 & b5
		BNE LDBF4 				; BR IF b5 & b1
								; ... else
								;
								; CK AFR WD 3
								;
		LDAB LC016 				; AFR MD WD 3
		BITB #$20 				; b5, Int Reset ON BLM CELL CHG
		BEQ LDBB2 				; BR IF NOT b5
								; ... else
		BITA #$04 				; b2
		BNE LDBFA 				; BR IF b2
								; ... else
LDBB2: 	LDAA $33 				; MINOR LOOP MD 1
		BITA #2 				; b1, 1 = FORCE LOW PULSE, RESULT OPN LP
		BNE LDBFA 				; BR IF NOT b1
								; .... else
		LDAA $3E 				;
		BMI LDBE8 				;
								; ... else
		BITB #2 				; b1
		BEQ LDBC6 				; BR IF NOT b1
								; ... else
		BRCLR $38 $88 LDBC6 	; BR IF b7 & b3
								; ... else
		BRA LDBFA 				;
								;
								; CK IF CCP Chg FORCE INT TO 128 ENABLED
								;
LDBC6: 	LDAA LC017 				; AFR MD WD 4
		BITA #$08 				; b3, ON CCP Chg FORCE INT TO 128
		BEQ LDBD4 				; BR IF NOT b3
								; ... else
		LDAA $3B 				;
		ADDA $3E 				;
		RORA 					;
		BCS LDBFA 				;
LDBD4: 	LDAB $35 				; MNR LOOP MD WD 2
		BITB #$20 				; b5, DIAG SW IN ALDL MODE
		BNE LDC05 				; BR IF b5
		LDAB $63 				; Filtered ld value
		CMPB LC5F1 				; Dcel lean Param
		BHI LDC05 				;
								; ... else
		LDAB $57 				; RPM/25
		CMPB LC5F2 				; Dcel lean hi lmt param
		BLS LDC05 				;
								; ... else
LDBE8: 	LDAA $44 				; AF MODE WD #2
		BITA #$40 				; b6, 1 =RICH, 0 = LEAN
		BNE LDC05 				; BR IF b1
								; ... else
		LDAA $C9 				; Cls/Lp Int, Add Fuel if > 128
		CMPA #128 				;
		BCS LDC05 				;
								; ... else
LDBF4: 	LDAA $41 				; DIAGNOSTIC MODE WD 3
		ORAA #$04 				; SET b2, INT FORCED 128. ERR 44/45
		STAA $41 				; DIAGNOSTIC MODE WD 3
LDBFA: 	LDAA #128 				;
		STAA $C9 				; RESET Cls Lp Int to 128
		CLRA 					; CLR CLSD LP TMR
		STAA $C4 				; Cls Lp Int Tmr (Sec * 40)
		PSHA 					;
		JMP LDD21 				;
								;--------------------------------------------------
								; LK UP Upper 0 error for Slow 02 Vs Air flow
								;
								; Table value = Volts * 226
								;---------------------------------------------------
LDC05: 	LDAA $D2 				; Ld Val Limited for Clsd Lp L.U.'s
		LDX #$C567 				; Tbl Addr
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		SUBA $D1 				; Lean Offset, (if Cool)
		BRSET $3E $02 LDC16 	; BR IF b1
								; ... else
		SUBA LC562 				; Diff Value to Make up Rich/Lean Window
								; When Air div
LDC16: 	CMPA $71 				;
		BCS LDC38 				;
								; ... else
								;--------------------------------------------------
								; LK UP Lower o2 error for Slow 02 Vs Air flow
								;
								; Table value = Volts * 226
								;--------------------------------------------------
		LDAA $D2 				; Ld Val Limited for Clsd Lp LK Up
		LDX #$C570 				; Tbl Addr,
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		SUBA $D1 				; Lean Offset, (if Cool)
		BRSET $3E $02 LDC2B 	; BR IF b1
								; ... else
		SUBA LC562 				; Diff Value to Make up Rich/Lean Window
								; When Air div
LDC2B: 	SUBA $71 				;
		BHI LDC32 				;
								; ... else
		CLRA 					;
		BRA LDC69 				;
LDC32: 	LDAB $44 				; AF MODE WD #2
		ANDB #$DF 				; CLR b5, (RICH FLG)
		BRA LDC44 				;
LDC38: 	TAB 					;
		LDAA $71 				;
		SBA 					;
		LDAB LC564 				; o2 Filter Constant when Coolant Tmp Low
		MUL 					;
		LDAB $44 				; AF MODE WD #2
		ORAB #$20 				; SET b5, (RICH FLG)
LDC44: 	STAB $44 				; AF MODE WD #2
		PSHA 					;
		STAA $046B 				;
								;--------------------------------------------------
								; LK UP o2 Error Reduction Vs Air flow
								;
								; Table value = Mult (0-1) * 256
								;--------------------------------------------------
		LDAA $D2 				; Ld Val Limited for Lk Up
		LDX #$C594 				; Tbl Addr,
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		LDAB $82 				; TPS Ld Axis Var
		CMPB LC55E 				; Clsed Lp Idle TPS 1.17% Gain Factor
		BHI LDC61 				; BR IF TPS L.T. 1.17%
								; ... else
		LDAB $67 				; MPH/(16/5)
		BNE LDC61 				;
								; ... else
		LDAB LC560 				; EECC Clsd Lp Gain factor
		MUL 					;
LDC61: 	PULB 					;
		MUL 					;
		CMPA #88 				;
		BLS LDC69 				;
								; ... else
		LDAA #88 				; USE MAX VALUE
								;-------------------------------------------
								; LK UP PROPORTIONAL STEP WIDTH Vs Error
								;
								;-------------------------------------------
LDC69: 	PSHA 					; SAVE LIMITED o2 ERROR TO STX
		STAA $046B 				; LIMITED o2 ERROR
		ASLA 					; x 2 FOR LK UP
		LDX #$C5AF 				; Tbl Addr
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		PSHA 					; Save PORP STEP WIDTH
								;----------------------------------------------
								; LK UP Porp Step width offset vs RPM
								;
								;
								;----------------------------------------------
		LDAA $56 				; RPM/25
		CMPA #160 				; 4000 RPM
		BLS LDC7D 				;
								; ... else
		LDAA #160 				; 4000 RPM MAX FOR LK UP
LDC7D: 	LSRA 					;
		LDX #$C59D 				; Tbl Addr,
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		PULB 					;
		ABA 					; ADD IN OFFSET
		BCC LDC8A 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VALUE
								;
								; b5 EECC SLO o2 R/L FLAG
								; b6 1 =RICH, 0 = LEAN
								;
LDC8A: 	TAB 					;
		LDAA $44 				; AF MODE WD #2
		ANDA #$60 				; CLR ALL EXCEPT b5 & b6
		BEQ LDC95 				; BR IF NOT b5 & b6 (RICH & ?)
								; ... else
		CMPA #$60 				; b5 & b6
		BNE LDC9C 				; BR IF b5 & b6 (RICH & ?)
								; ... else
LDC95: 	PULA 					;
		PSHA 					;
		CMPA LC563 				; Min Error To Implement Int
		BHI LDCA6 				;
								; ... else
LDC9C: 	CLR $C4 				; CLR Cls Lp Int Tmr (Sec * 40)
		LDAA $C7 				; Cls Lp Duration Cntr, (Sec * 80)
		CBA 					;
		BHI LDD01 				;
		BRA LDD06 				;
								;------------------------------------------
								; LK UP Integrator Delay Vs Error
								;
								;------------------------------------------
LDCA6: 	PULA 					; Get ERR VAL
		PSHA 					;
		ASLA 					; VAL * 2
		LDX #$C5C9 				; Tlb Addr
		JSR LF2C6 				; Call 2d Lk Up, (No Offset)
		LDAB $5B 				; COOLANT
		NEGB 					;
		SUBB LC566 				; Cool Int Offset Int Gain
		BCS LDCBC 				;
								; ... else
		ABA 					;
		BCC LDCBC 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				;
LDCBC: 	PSHA 					;
								;---------------------------------------
								; LK UP INTEGRATOR DELAY Vs Air flow
								;
								;---------------------------------------
		LDAA $D2 				; Ld Val Limited for Cls Lp L.U.'s
		LDX #$C582 				; Tbl Addr,
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		PULB 					;
		MUL 					; DELAY x ____
		LDAB $82 				; TPS Ld Axis Var
		CMPB LC55E 				; Clsed Lp Idle TPS 1.17% Gain Factor
		BHI LDCD9 				;
								; ... else
		LDAB $67 				; MPH/(16/5)
		BNE LDCD9 				;
								; ... else
		ADDA LC55F 				; 0.2 sec Added Corr To Int Dly At Idle
		BCC LDCD9 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VAL
LDCD9: 	CMPA $C4 				; Cls Lp Int Tmr (Sec * 40)
		BHI LDCE0 				;
								; ... else
		CLRA 					;
		BRA LDCE3 				;
								;
								; CK CLSD LP TMR
								;
LDCE0: 	LDAA $C4 				; Clsd Lp Int Tmr (Sec * 40)
		INCA 					; INCR TMR
LDCE3: 	STAA $C4 				; Cls Lp Int Tmr (Sec * 40)
		BNE LDD06 				; BR IF TMR NZ
								; ... else
								;
								; CK MAX/MIN CLSD LP INTAGRATOR VAL'S
								;
		LDAA $C9 				; Clsd Lp Intagrator
		LDAB $44 				; AF MODE WD #2
		BITB #$20 				; b5, EECC SLO o2 R/L FLAG
		BNE LDCF7 				; BR IF b5, RICH
								; ... else
								;
		CMPA LC55D 				; Clsd Lp, Max INT Value, 180
		BEQ LDCFD 				; BR IF INT VAL E.Q. 180, (EXIT)
								; ... else
		INCA 					; INCR INTAGRATOR VAL, (LEAN o2)
		BRA LDCFD 				; EXIT
LDCF7: 	CMPA LC55C 				; Clsd Lp, Min INT Value, 40
		BEQ LDCFD 				; BR IF INT VAL E.Q. 40, (EXIT)
								; ... else
		DECA 					; DECR INTAGRATOR VAL, (RICH o2)
LDCFD: 	STAA $C9 				; SAVE NEW CLSD LOOP INTAGRATOR
		BRA LDD06 				;
								;--------------------------------------------------
LDD01: 	PULA 					;
		CLRA 					;
		PSHA 					;
		BRA LDD12 				;
								;---------------------------------------
								; LK UP Proportional Step Size Vs Error
								;
								;----------------------------------------
LDD06: 	PULA 					; GET ERRV VALUE
		ASLA 					; VAL * 2
		LDX #$C5A3 				; POINT TO Proportional Step Size Vs Err
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		PSHA 					; Save Result on stack
		STAA $046C 				; SAVE Proportional Step Size
LDD12: 	LDAB $44 				; AF MODE WD #2
		ASLB 					;
		BPL LDD21 				;
								; ... else
		LDAA $C9 				; GET Clsd Lp INTAGRATOR
		PULB 					;
		PSHB 					;
		SBA 					;
		BCC LDD27 				;
								; ... else
		CLRA 					;
		BRA LDD27 				; EXIT W/SAVE
LDD21: 	ADDA $C9 				; Clsd Lp INTAGRATOR
		BCC LDD27 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VAL
LDD27: 	STAA $C6 				; Base PW CLSD Lp Corr
		BSR LDD2D 				; CALC BPW VAL'S OR LK THEM UP
		BRA LDD7A 				; APPLY BLM CORR TO BPW CLS LP CORR VALUE
								;--------------------------------------------------
								; CALC BPW VAL'S OR LK THEM UP ??
								;
								;--------------------------------------------------
								;
								; CK AFR MODE
								; FOR Calc Base Inj PW, not Tbl
								;
LDD2D: 	LDAA LC016 				; Get Fuel/Air Mode Wd 3 0110 0010,
								; b7 =
								; b6 = Calc Base Inj PW, not Tbl
								; b5 = Int Reset when B.Lrn cell Chnge
								; b4 =
								;
								; b3 =
								; b2 =
								; b1 = Reset Corr/init of Auto Enrich
								; b0 =
		ASLA 					; SHIFT LEFT
		BPL LDD60 				; LK UP Base fuel injection PW's vs LOAD
								; ... else
								;--------------------------------------------------
								; CALC BPW VAL'S
								;
								;--------------------------------------------------
		LDX $95 				; Current Minor LP Ref Period
								; -> fm MPU
								;
		BRSET $34 $80 LDD41 	; BR IF b7, ENG RUNNING FLAG
								; ... else
								;
		CPX LC357 				; 0000 DRP VAL If Crank RPM <
								; then use this
		BCS LDD41 				;
								; .... else
		LDX LC357 				; 0000, Crank RPM < then use this
LDD41: 	LDD $EA 				; Gms/Sec Disp Value, (Mult'er)
		JSR LF2F0 				; Go to 16 * 16 Mult routine
								; (Gms/Sec * DRP)
		LSRD 					; VAL/2
		LDX $CE 				; Total AFR Value
		JSR LF2F0 				; Go to 16 * 16 Mult routine
								;-------------------------
								; INJ FLOW RATE, (23#/HR)
								; (STOCH 14.7 = 445d)
								;-------------------------
		LDX LC3CF 				; Double Fire Inj Flow rate, (441)
		BRCLR $39 $80 LDD56 	; BR IF NOT b7 (NOT SNGL FIRE)
								; ... else
								;
		LDX LC3D1 				; SINGLE FIRE Inj Flow rate, (882)
LDD56: 	JSR LF2F0 				; Go to 16 * 16 Mult routine
								; (AFR * INJ FLOW RATE)
								;
		ROLB 					;
		PSHX 					; SAVE TO STX
		PULA 					;
		PULB 					;
		ROLB 					;
		ROLA 					;
		RTS 					;
								;--------------------------------------------------
								;--------------------------------------------------
								; LK UP Base fuel injection PW's vs LOAD
								;
								; BUA TYPE $32 ECM MY(86)
								;--------------------------------------------------
LDD60: 	LDX $CE 				; Total AFR Value
		LDD LC3CD 				; Table LU Multiplier, (735d)
		JSR LF2F0 				; 16 * 16 Mult routine
		PSHB 					; SAVE TO STX
		PSHA 					;
		LDAA $57 				; RPM/25
		LDAB $63 				; Filtered LD VALUE
		LDX #$C42D 				; POINT TO TBL
		JSR LF27C 				; 3d Lk Up
		TSX 					;
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		PULX 					;
		RTS 					;
								;--------------------------------------------------
		;***************************************************
		;* APPLY BLM CORR TO BPW CLS LP CORR VALUE
		;*
		;*
		;***************************************************
LDD7A STD $C2 	;
		LDX #$00C2 				; Base PW Cls Lp Corr value
		LDAA $C0 				; Blk Lrn Mult, Scaled 1, rng 0-2
		JSR LF472 				; FACTOR IN 8x16 MULT
		CLRA 					;
								;
								; CK IF WE ARE TO LEAN or RICHEN FUEL
								;
		LDAB $C6 				; Base PW Cls Lp Corr value
		SUBB #128 				; CK IF R or LRAN CORR
		BMI LDD8F 				; BR IF BLM LT 128, (LEAN)
								; ... else
								;
								; RICHEN
								;
		ADDD $C2 				; SUM IN Base PW Cls Lp Corr Value
		BRA LDD9C 				; EXIT W/NEW Base PW Cls Lp Corr Value
								;
								;
								; LEAN OUT
								;
LDD8F: 	NEGB 					; INVERT Base PW Cls Lp Corr Value
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDD $C2 				; SUB Base PW Cls Lp Corr value
		SUBD 0,X 				;
		PULX 					;
		BCC LDD9C 				; BR IF NO UNDERFLOW
								; ... else
		CLRA 					;
		CLRB 					;
LDD9C: 	STD $C2 				; SAVE NEW Base PW Cls Lp Corr Value
		PULB 					;
		LSRB 					;
		BCC LDDA7 				;
								; ... else
		LDAA $3D 				;
		RORA 					;
		ADCB #0 				; ROUND
LDDA7: 	CLRA 					;
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDAA $44 				; AF MODE WD #2
		ASLA 					;
		ASLA 					;
		LDD $C2 				;
		BCS LDDB7 				;
								; ... else
		SUBD 0,X 				;
		BRA LDDB9 				;
LDDB7: 	ADDD 0,X 				;
LDDB9: 	STD $C2 				;
		PULX 					;
		JSR LF901 				;
		JSR LE436 				;
		;***************************************************
		;***************************************************
		;* AIR FUEL MINOR LOOP
		;* 50 Msec
		;*
		;*
		;***************************************************
LDDC2: 	LDAA $00 				;
		ANDA #$06 				; b1 & b2
		BEQ LDDCB 				; BR IF NOT b1 & b2
								; ... else
		JMP LDE6A 				;
LDDCB: 	LDD $43 				; AF MODE WD
		BITB #$02 				; b1, DECELL FUEL CUT OFF ENABLED
		BEQ LDE30 				; BR IF NOT b1
								; ... else_
		BITA #$08 				; b3
		BNE LDE30 				;
								; ... else_
								;
								; CK CLSD LP INT FOR STOCH
								;
		LDAA $C9 				; Clsd Lp Int
		CMPA #128 				;
		BEQ LDE30 				; BR IF INT = 128
								; ... else_
								;
								; CK BLM UP DATE TIMER
								;
		LDAB $CA 				; Blk Lrn Up Date TMR, (50Ms Inc)
		INCB 					; INCR TMR
		BNE LDDE1 				;
								; ... else
		DECB 					; DECR TMR
LDDE1: 	STAB $CA 				; SAVE NEW Blk Lrn Up Date TMR VAL
		CMPB LC5E0 				; BLM Update Rate, 450 Msec
		BCS LDE33 				; BR IF TMR L.T. 450 Msec
								; ... else
		SUBA #128 				; CK CLSD LP INT VAL
		BCS LDDF8 				; BR IF INT L.T 128,
								; ... else
		CMPA LC5E4 				; CK If Clsd Lp INT > 5, Enab BLM Update
		BLS LDE33 				; BR IF INT VAL GT 5
								; ... else
		LDAA $44 				; AF MODE WD #2
		ASLA 					;
		BMI LDE33 				;
								; ... else
		BRA LDE03 				;
LDDF8: 	NEGA 					; INVERT CLSD LP INT VALUE
		CMPA LC5E5 				; CK If Clsd Lp INT < 5, Enab BLM Update
		BLS LDE33 				; BR IF INT VAL GT 5
								; ... else
		LDAA $44 				; AF MODE WD #2
		ASLA 					;
		BPL LDE33 				;
								; ... else
								;
								;
								;
LDE03: 	LDX #$001C 				; POINT TO BLM CELL MATRIX IN RAM
		LDAB $BF 				; BLM CELL NUMBER
		ABX 					; ADJ CELL POINTER ADDRESS
		LDAA 0,X 				; GET SELECTED BLM CELL VALUE
		LDAB $44 				; AF MODE WD #2
		ASLB 					;
		BPL LDE1F 				;
								; ... else
		SUBA LC5E1 				; SUB OFF 2, BLM Update Amt
		BCS LDE1A 				; BR IF UNDERFLOW
								; ... else
		CMPA LC5E3 				; 108, Min Allowable BLM
		BCC LDE2C 				; BR IF BLM G.T. MIN BLM VALUE
								; ... else
LDE1A: 	LDAA LC5E3 				; 108, Min Allowable BLM
		BRA LDE2C 				;
LDE1F: 	ADDA LC5E1 				; ADD 2, BLM Mult Update Amt
		BCS LDE29 				; BR IF BLM OVERFLOW
								; ... else
		CMPA LC5E2 				; 160, Max Allowable BLM
		BLS LDE2C 				; BR IF BLM G.T. MAX VALUE
								; ... else
LDE29: 	LDAA LC5E2 				; USE 160, Max Allowable BLM
LDE2C: 	STAA 0,X 				; SAVE NEW BLM VAL TO CELL ADDR
		STAA $C0 				; SAVE NEW BLM
LDE30: 	CLRB 					; CLR BLM UP DATE TIMER
		STAB $CA 				; Blk Lrn U/D Cntr, (50Ms Inc)
		;************************************
		;*
		;* UPDATE BLM CELLS
		;*
		;************************************
LDE33: 	LDAB $BF 				; Blk Lrn Mumt (cells 0-7)
		CMPB LC5EA 				; SAM Cell A NUMBER, IDLE CELL NUM
		BEQ LDE3D 				; BR IF IDLE CELL
								; ... else
		CLRB 					;
		STAB $C1 				; Base PW
LDE3D: 	LDAA $43 				; AF MODE WD;
		ANDA #$F7 				; CLR b3, DELAY BLM UPDATE, CELL CHANGE
		STAA $43 				; AF MODE WD
		LDAA $58 				; RPM/12.5
		STAA $5A 				;
								;
								; CK TRANS FLTR Lp TMR FOR LD VAL & TPS
								;
		DEC $E4 				; TRANS FLTR Lp TMR
		BPL LDE6A 				; BR IF TIMED OUT (EXIT)
								; ... else
		LDD LC339 				; Loops between Filtering
								; of Ld Vals, (1 sec)
		STAA $E4 				; Trans Fltr Lp TMR
								;
								; FILTER TRANSIENT LD VAL
								;
		LDX $E5 				; TRANSIENT FLT'RD Ld Val
		LDAA $63 				; Filtered ld value
		JSR LF250 				; FILTER ROUTINE
		STD $E5 				; NEW TRANSIENT FLT'RD Ld Val
								;
								; FILTER TRANSIENT TPS VAL
								;
		LDAA $E9 				; Trans Fltr'ed TPS for Decel Enlean
		CLRB 					;
		PSHB 					;
		PSHA 					;
		PULX 					; GET OLD TPS Ld Axis Var
		LDAB LC5F3 				; TPS Flt Coef (Enlean)
		LDAA $82 				; TPS Ld Axis Var
		JSR LF250 				; FILTER ROUTINE
		STAA $E9 				; Trans Fltr'ed TPS for Decel Enlean
LDE6A: 	JMP LCCAC 				; TO MJR LP EXEC
		;***************************************************
		;***************************************************
		;* MAJOR LOOP SUBROUTINE SEG E
		;*
		;* TCC (A/D voltage convert)
		;*
		;***************************************************
								;
								; Test Batt Voltage
								;
LDE6D: 	LDAA #$10 				; A/D Ch 1, (Battery Voltage)
		JSR LF1BE 				; To A/D subroutine
LDE72: 	STAA $7E 				; Batt Volts Value, (A/D)
		CMPA #90 				; 9.0 VDC
		BCC LDE7E 				;
		CMPA #40 				; 4 VDC
		BCS LDEA0 				;
								; ... else
		BRA LDEA3 				;
LDE7E: 	BCLR $3E $40 			; CLR b6
								;
		BRCLR $33 $10 LDE8D 	; BR IF NOT b4, (1 = IGN OFF)
								; ... else
		LDX $3FC8 				; SPK Period
		STX $B2 				;
		CLR $B4 				; SPK Fd Bk counter, (Err #42)
LDE8D: 	LDD LC69B 				;
		STD $B7 				;
		CLR $011F 				;
		CLR $0120 				;
		BCLR $33 $10 			; CLR b4, (1 = IGN OFF)
		JSR LE0D7 				; SHIFTER SWITCH READ SUBROUTINE
		BRA LDEA3 				;
LDEA0: 	BSET $33 $10 			; SET b4, (1 = IGN OFF)
LDEA3: 	RTS 					;
								;-------------------------------------
LDEA4: 	LDD $3FC2 				; Timer #1
		SUBD $6D 				;
		BEQ LDED3 				;
		INC $011E 				;
								;
		LDAB $34 				;
		BITB #$10 				; b4
		BNE LDEC1 				; BR IF b4
								; ... else
		ORAB #$10 				; SET b4
		STAB $34 				;
								;
		LDX $3FC2 				; Timer #1
		INX 					;
		STX $6D 				;
		INX 					;
		STX $6B 				;
								;
LDEC1: 	CLR $68 				;
		LDX $6B 				;
		STX $69 				;
		LDX $6D 				;
		STX $6B 				;
								;
		LDX $3FC2 				; Timer #1
		STX $6D 				;
		BRA LDEE4 				;
								;
LDED3: 	LDAB $68 				;
		INCB 					;
		CMPB #$9F 				;
		BLS LDEE2 				;
								; ... else
		LDAB $34 				; CLR b4
		ANDB #$EF 				;
		STAB $34 				;
		LDAB #$A0 				;
LDEE2: 	STAB $68 				;
LDEE4: 	RTS 					;
		;***************************************************
		;***************************************************
		;* MAJOR LOOP SUBROUTINE SEG B
		;*
		;* EGR
		;*
		;***************************************************
LDEE5: 	LDAB $37 				; MCU INPUT STATUS WD
		RORB 					;
		BCC LDEF0 				;
								; ... else
		LDAB $35 				; MNR LOOP MD WD 2
		BITB #$20 				; b5, DIAG SW IN ALDL MODE
		BEQ LDF45 				; BR IF NOT b5,
								; ... else
LDEF0: 	LDAA $60 				;
		CMPA LC23A 				; If MAT < 30d then disable EGR
		BCS LDF45 				;
								; ... else
		LDD LC238 				; Enable EGR if TPS > 1.95%,
		TST $0112 				; ERG PW
		BNE LDF00 				; BR IF NZ
								; ... else
		TBA 					;
LDF00: 	CMPA $82 				; TPS Ld Axis Var                                  
		BHI LDF45 				;                                                  
								; ... else                                         
		LDAA $43 				; AF MODE WD                                       
		BITA #$20 				; b5, PWR ENR IS ACTIVE                            
		BNE LDF45 				; BR IF NOT b5                                     
								; ... else                                         
								;--------------------------------------------------
								; Lk up EGR Duty CYC vs LV8 vs RPM                 
								;                                                  
								; Tbl Val(DC) = CALIB * (256/100)                  
								;                                                  
								; BUA TYPE $32 ECM MY(86)                          
								;--------------------------------------------------
		LDX #$C23B 				; EGR Duty CYC vs LV8 & RPM                        
		LDAB $63 				; filt ld value                                    
		CMPB #160 				; 160 Gms/Sec                                      
		BCS LDF15 				; BR IF LD VAL LT 160 Gms/Sec                      
								; ... else                                         
		LDAB #160 				; USE 160 Gms/Sec MAX FOR LK UP                    
LDF15: 	LDAA $57 				; RPM/25                                           
								;--------------------------------------------------
								; LK UP Base fuel injection PW's vs LOAD VAL vs RPM
								;                                                  
								; BUA TYPE $32 ECM MY(86)                          
								;                                                  
								; TBL = Msec * (65536/5)                           
								;--------------------------------------------------
		CMPA #80 				;                                                  
		BLS LDF1D 				; TO 3d Lk UP                                      
								; ... else                                         
		LDAA #$50 				;                                                  
LDF1D: 	JSR LF27C 				; 3d Lk Up Routine                                 
		STAA $0112 				; EGR PW                                           
								;-------------------------------------------       
								;-------------------------------------------       
								; EGR DUTY CYCLE MULTIPLIER (0-2) vs COOLANT       
								; Gain vs Coolant Temp.                            
								;                                                  
								; TABLE VAL = Mult * 128                           
								;--------------------------------------------      
		LDAA $5B 				; COOLANT                                          
		CMPA #160 				; 80c MAX FOR LK UP                                
		BLS LDF2B 				; BR IF COOL LT 80c                                
								; ... else                                         
		LDAA #160 				; USE 80c AS MAX VAL FOR LK UP                     
LDF2B: 	LDAB #64 				; MIN COOL VAL, 8c                                 
		LDX #$C262 				; EGR DUTY CYCLE MULTIPLIER Tbl                    
		JSR LF2B9 				; 2d Lk Up                                         
		LDAB $0112 				; ERG PW                                           
		MUL 					; x LK UP RESULT                                   
		ASLD 					; x2                                               
		BCC LDF3C 				; BR IF NO OVERFLOW                                
								; ... else                                         
		LDAA #255 				; USE MAX VAL                                      
								;---------------------------------------------     
								; CK IF IN HEADS UP                                
								;                                                  
								;                                                  
								;---------------------------------------------     
LDF3C: 	LDAB $33 				; MINOR LOOP MD WD 1                               
		BPL LDF46 				;                                                  
								; ... else                                         
		JSR $581B 				; TO HEADS UO <---------                           
		BRA LDF46 				; EXIT                                             
LDF45: 	CLRA 					; DISABLE EGR
								;---------------------------------------------
LDF46: 	STAA $0112 				; ERG PW
		RTS 					;
								;--------------------------------------------------
								;--------------------------------------------------
LDF4A: 	BRCLR $35 $02 LDF52 	; BR IF NOT b1, (ERR14/15 THIS START UP)
								; ... else
		LDAA #$00FF 			;
		BRA LDF89 				;
LDF52: 	LDAA $65 				; Vss
		BRSET $37 $40 LDF64 	; BR IF b6, MCU INPUT STATUS WD
								; COOL FAN REQUEST
								; ... else
		CMPA LC2F0 				; 35 MPH
		BCC LDF64 				; BR IF Vss G.T 35 MPH
								; ... else
		LDAA LC2F3 				; 99.6%, Fan Duty Cyc (FAN FULL ON)
		BEQ LDF90 				;
								; ... else
		BRA LDF89 				;
LDF64: 	LDX #$C2F6 				; POINT TO FAN 2
		BRCLR $37 $80 LDF70 	; BR IF NOT b7, MCU INPUT STATUS WD
								; A/C COMP NOT ON
								; ... else
		CMPA LC2F1 				; 11 MPH
		BHI LDF73 				; BR IF GT 11 MPH
								; ... else
LDF70: 	LDX #$C2F4 				; FAN 1 HYST PARAM'S
LDF73: 	BRCLR $F4 $FF LDF78 	; BE IF NOT $FF
								; ... else
		INX 					; OFF VAL
								;
								; Call 2D Lk Up
								;
LDF78: 	LDAA $5B 				; CURRENT COOLANT
		CMPA 0,X 				; ON or OFF THRESH
		BCS LDF90 				; BE IF COOL L.T. THRSH
								; ... else
		LDAB #160 				; 80c MIN VAL FOR LK UP
		LDX #$C2F8 				; Fan Duty Cycle Tabl Addr
		JSR LF2B9 				; 2d Lk Up
		TSTA 					; CK FOR Z
		BEQ LDF90 				;
								; ... else
LDF89: 	LDAB LC2F2 				; 15 Sec, Min Fan On Time
		STAB $F5 				; SAVE MIN FAN TIME
		BRA LDF9A 				;
LDF90: 	BRCLR $F5 $FF LDF99 	; Br IF MIN FAN TIME EXPIRED
								; ... else
		DEC $F5 				; DEC MIN FAN TIME
		BRA LDF9C 				; EXIT via RTS
LDF99: 	CLRA 					; CLR MIN FAN TIME
LDF9A: 	STAA $F4 				; NEW MIN FAN TIME
LDF9C: 	RTS 					;
								;------------------------------
LDF9D: 	LDX #$C62E 				; INDEX IAC Tables & Params
		LDAA #$80 				;
		PSHA 					;
		LDAB $34 				;
		LDAA $F3 				; IAC Md Wd #2
		BITA #4 				;
		BNE LDFFF 				;
		BITA #8 				;
		BEQ LDFFF 				;
		LDAA $010B 				; DISABLE CLSD LP PID TMR
		BEQ LDFBB 				;
		LDAA $F2 				; IAC Cnt'l Word
		ANDA #$FE				;
		JMP LE07B 				;
LDFBB: 	LDAA $3F,X 				;
		BITB #$0020 			;
		BEQ LDFC3 				;
		SUBA #$10 				;
LDFC3: 	CMPA $84 				;TPS T/F Ld Axies Var
		BCS LDFFF 				;
		LDAA #$F9 				;
		BITB #$20				;
		BNE LDFCE 				;
		CLRA 					;
LDFCE: 	ADDA $44,X 				;
		CMPA $5B 				; COOLANT
		BCS LDFFF 				;
								; ... else
		LDAA $37 				;
		ANDA #$80 				; b7, MCU INPUT STATUS WD
								; A/C COMP NOT ON
		BNE LDFDC 				; BR IF NOT b7
								; ... else
		INS 					;
		PSHA 					;
LDFDC: 	ADDA $F2 				; IAC Cnt'l Word
		BPL LDFF0 				;
								; ... else
		LDAA $42,X 				;
		TST $37 				; STATUS WD
		BPL LDFE9 				;
								; ... else
		LDAA $43,X 				;
LDFE9: 	CMPA $0106 				;
		BCS LE005 				;
		BRA LE002 				;
LDFF0: 	LDAA $0106 				;
		BEQ LE005 				;
								; ... else
		DECA 					;
		BNE LE002 				;
								; ... else
		ANDB #$DF 				; 1101 11111
		TST $37 				; MCU INPUT STATUS WD
		BPL LE002 				;
								; ... else
LDFFF: 	CLRA 					;
		ORAB #$20 				; SET b5
LE002: 	STAA $0106 				;
LE005: 	STAB $34 				;
		LDAA $F2 				; IAC Cnt'l Word
		PULB 					;
		PSHB 					;
		TSTB 					;
		BPL LE016 				;
								; ... else
		ANDA #$00FE 			;
		BMI LE076 				;
								; ... else
		ORAA #$0020 			;
		BRA LE02E 				;
LE016: 	ANDA #$DF 				;
		ORAA #$40 				;
		BITA #$81 				;
		BEQ LE07B 				;
								; ... else
		LDAB $0107 				;
		CMPB $3B,X 				;
		BCS LE076 				;
								; ... else
		EORA #$01 				;
		BPL LE033 				;
								; ... else
		LDAB $2C 				; IAC Present Posit
		STAB $0108 				;
LE02E: 	CLR $0107 				;
		BRA LE076 				;
LE033: 	PSHA 					;
		LDAA $2C 				; IAC Present Posit
		SUBA $0108 				;
		BCC LE03C 				;
		CLRA 					;
LE03C: 	LDAB $F3 				;
		BPL LE044 				;
								; ... else
		LDAB $3C,X 				;
		MUL 					;
		ASLD 					;
LE044: 	LDAB $38,X 				;
		CBA 					;
		BLS LE04A 				;
								; ... else
		TBA 					;
LE04A: 	LDAB $39,X 				;
		CBA 					;
		BHI LE050 				;
								; ... else
		TBA 					;
LE050: 	TAB 					;
		SUBA $2D 				; Idle Spd A/C Antic
		BCS LE05E 				;
								; ... else
		CMPA $3A,X 				;
		BLS LE05E 				;
								; ... else
		LDAB $3A,X 				;
		TBA 					;
		ADDB $2D 				; Idle Spd A/C Antic
LE05E: 	STAB $2D 				; Idle Spd A/C Antic
		LDAB $F3 				;
		BPL LE06F 				;
								; ... else
		LDAB $36,X 				;
		TSTA 					;
		BPL LE06E 				;
		NEGA 					;
		MUL 					;
		NEGA 					;
		BRA LE06F 				;
LE06E: 	MUL 					;
LE06F: 	ADDA $0105 				;
		STAA $0105 				;
		PULA 					;
LE076: 	ANDA #$7F 				; CLEAR b7
		TSX 					;
		ORAA 0,X 				;
LE07B: 	STAA $F2 				; IAC Cnt'l Word
		PULA 					;
		RTS 					;
		;***************************************************
		;***************************************************
		;* MAJOR LOOP SUBROUTINE SEG 2, Vss Calc
		;*
		;*
		;* CALCULATE ROAD SPEED IN MPH
		;***************************************************
LE07F: 	LDAA $33 				; MINOR LP MOD WD 1
		BITA #$10 				; b4, (1 = IGN OFF)
		BNE LE0D6 				;
								; ... else
		LDAA #160 				;
		CMPA $68 				;
		BHI LE091 				;
		CLRA 					;
		CLRB 					;
		STD $65 				;
		BRA LE0C9 				;
LE091: 	LDD $6B 				;
		SUBD $69 				;
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDD $6D 				;
		SUBD $6B 				;
		ADDD $0000,X 			;
		PULX 					;
		RORA 					;
		RORB 					;
		PSHB 					;
		PSHA 					;
		PULX 					;
		LDD #$01CC 				;
		JSR LF215 				; TO F DIV SUBTOUTINE
		LDX $65 				;
		LDAB LC284 				; Vss Filter coef
		JSR LF250 				; FILTER ROUTINE
								;
		STD $65 				;
		ADDD #$80 				;
		BCC LE0BA 				;
		DECA 					;
LE0BA: 	LDAB #$CD 				;
		MUL 					;
		ADDD #$20 				;
		CMPA #$2F 				;
		BLS LE0C7 				;
		LDD #$3000 				;
LE0C7: 	ASLD 					;
		ASLD 					;
LE0C9: 	LDAB $AB 				; VSS Err cnt'r
		CMPB LC1F7 				;
		BLS LE0D4 				;
								; ... else
		LDAA LC64E 				; CMD speed Param
		INCA 					;
LE0D4: 	STAA $67 				; MPH/(16/5)
LE0D6: 	RTS 					;
		;*******************************************************
		;*******************************************************
		;* PRNDL SWITCH READ
		;*
		;* REQUEST VALID FOR 100 Msec
		;*
		;*******************************************************
LE0D7: 	LDAA $37 				; MCU INPUT STATUS WD
		LDAB $45 				; SHFT MW
		RORA 					;
		RORA 					;
		RORA 					;
		RORA 					;
		BMI LE0E7 				;
								; ... else
		ANDB #$F7 				; 1111 0111
		STAB $45 				;
		BRA LE102 				;
LE0E7: 	BITB #$08 				; b3
		BEQ LE0ED 				; BR IF NOT b3
								; ... else
		ANDA #$7F 				; CLR b7
LE0ED: 	ORAB #$08 				; b3
		STAB $45 				;
		BITA #$02 				; b1
		BNE LE101 				; BR IF b1
								; ... else
		LDAB $04 				; Xmission Mode word
		ASRB 					;
		BCC LE102 				;
								; ... else
		LDAB $65 				; Vss (MPH/1)
		CMPB LC2AD 				; Unlock at 15 MPH 1st Man & Low grs Man
		BHI LE102 				; BR IF Vss LT 15 MPH
								; ... else
LE101: 	LSRA 					;
LE102: 	LSRA 					;
		ANDA #$60 				; 0110 0000, b5 & b6
		BITA #$40 				; b5
		BEQ LE10A 				; BR IF NOT b5
								; ... else
		INCA 					;
LE10A: 	ORAA $04 				; Xmission Mode word
		STAA $04 				; Xmission Mode word
		NOP 					;
		;***************************************************
		;***************************************************
		;* RPM/Vss RATIO CODE
		;*
		;*
		;*
		;*
		;***************************************************
		LDX $65 				;
		LDAA $57 				;
		LDAB #25 				;
		MUL 					;
		JSR LF215 				; TO F DIV SUBTOUTINE
		STAA $8F 				; N/V ratio
		BRCLR $36 $08 LE122 	; BR IF NOT b3, (TCC LOCKED FOR PASSBY)
								; ... else
		JMP LE2D1 				;
LE122: 	LDAB $04 				; Xmission Mode word
		ANDB #$E5 				; 1110 0101
		BITB #$80 				; b7, OD ACTIVE
		BEQ LE151 				; BR IF NOT b7
								; ... else
		CMPA LC287 				; 35, 1st Gear val
		BCS LE143 				;
								; ... else
		LDAA $012C 				;
		BNE LE14B 				;
								; ... else
		ORAB #$10 				; SET b4
		LDAA $04 				; Xmission Mode word
		BITA #$12 				; b1 & b4, 1st gr dir & 1st gr OD
		BNE LE16A 				; BR IF b4 & b1
								; ... else
		LDAA LC296 				; 0 SEC'S, Hi to Lo Gear lock
		STAA $90 				; 4 -> 3 Dnw Shft Lk Interupt tmr
		BRA LE16A 				;
LE143: 	LDAA LC28D 				; 0 SEC'S, 1st gear dly
		STAA $012C 				;
		BRA LE16A 				;
LE14B: 	DECA 					;
		STAA $012C 				;
		BRA LE16A 				;
LE151: 	LDAA LC28D 				; 0 SEC'S, 1st Gear dly
		STAA $012C 				;
		LDAA $8F 				;
		CMPA LC286 				; 41 1st gear direct RATIO
		BCS LE160 				;
								; ... else
		ORAB #$02 				; b1,
LE160: 	CMPA LC288 				; 30d, Min 4th grar
		BCS LE16A 				;
								; ... else
		CMPA LC289 				; 35d, Max 4th gear
		BLS LE16F 				;
								; ... else
LE16A: 	LDAA LC28A 				; 4th gear tmr
		BRA LE178 				;
LE16F: 	LDAA $8E 				; 4th Gr N/V tmr
		BNE LE177 				;
								; ... else
		ORAB #$08 				; b3, 4TH GEAR
		BRA LE17A 				;
LE177: 	DECA 					;
LE178: 	STAA $8E 				; 4th Gr N/V tmr
LE17A: 	STAB $04 				; Xmission Mode word
		NOP 					;
		LDAA $5B 				; COOLANT
		CMPA LC285 				; 50c, Cool thres for TCC
		BCC LE193 				;
								; ... else
								;
								; CK AFR MD WD 1 FOR AUTO/MAN XMISH
								;
		LDAA LC014 				; Get Fuel/Air Mode Wd 1
		BITA #01 				; b0, MAN XMISH
		BNE LE18D 				; BR IF MAN XMISH
								; ... else
		ANDB #$DF 				; CLR b5
LE18D: 	TSTB 					;
		BPL LE1D4 				;
								; ... else
		JMP LE262 				;
								;------------------------------
LE193: 	LDAA $65 				;
		PSHA 					;
								;
								; CK AFR MD 1 FOR XMISH PARAMS
								;
		LDAA LC014 				; Get Fuel/Air Mode Wd 1
		ASRA 					;
		PULA 					;
		BCS LE1A9 				;
								; ... else
		BITB #$20 				; b5
		BEQ LE1A9 				;
								; ... else
		ANDB #$DF 				; CLR b5
		SUBA LC297 				; Bias to Lock Enab Thresh, (26 MPH)
		BCC LE1A9 				;
								; ... else
		CLRA			
								;------------------------------------------------------
								; MANUAL - 1ST GEAR DNSHFT
								; AUTO TRANS - NON 4TH GEAR UNLOCK
								;
								; % OF FULL LOAD * 2.56
								; 12 ENTRIES
								;------------------------------------------------------
LE1A9: 	LDX #$C2A0 				; 14 MPH, Unlock 1st(man), Lo Gr (Auto)
		BITB #$12 				; b4 & b1
		BNE LE1B3 				; BR IF b4 & b1
								; ... else
		LDX #$C2BA 				; Unlock 14 MPH Hi gr Auto & Non 1st Man
LE1B3: 	BITB #$C0 				; b7 & b6
		BNE LE1C1 				; BR IF b7 & b6
								; ... else
								;------------------------------------------------------
								; Manual - 1st gear Up shift
								; Auto Trans - Non 4th Gear lock
								;
								; % Of Full Load * 2.56
								;------------------------------------------------------
								;
								; Unlock at 15 MPH 1st Man & Low grs Man
								;
		LDX #$C2AD 				; 15 MPH Unlock, 1st Man & Low grs Man
		BITB #$08 				; b3
		BEQ LE1C1 				; BR IF NOT b3
								; ... else
								;-----------------------------------------------------
								; Manual - 4TH gear Up shift
								; Auto Trans - 4th Gear lock
								;
								; Tble Val = Pct full ld * (256/100)
								;-----------------------------------------------------
								;
								; 15 MPH Min lk hi gear Auto & Non 1st Man
								;
		LDX #$C2C7 				; 15 MPH Min lk hi gear Auto &
								; Non 1st Man
LE1C1: 	TST $90 				; 4-3 Dnw Shft Lk Interupt tmr
		BEQ LE1C9 				;
								; ... else
		JMP LE28F 				;
LE1C9: 	BITB #$01 				; b0
		BEQ LE1D4 				; BR IF NOT b0
								; ... else
		CMPA 0,X 				;
		INX 					; INCR INDEX
		BCC LE1D7 				; BR IF
								; ... else
		ORAB #$06 				;
LE1D4: 	JMP LE27E 				;
								;---------------------------------------------------
LE1D7: 	BITB #$02 				; b1,
		BEQ LE1F0 				; BR IF NOT b1
								; ... else
		ORAB #4 				; b2,
		BITB #$C0 				; b7 & b6
		BNE LE1F0 				; BR IF b7 & b6
								; ... else
		BITB #$04 				;
		BEQ LE1FB 				; BR IF NOT b2
								; ... else
		LDAA LC28B 				; 100% TPS, Lk Ahead 1st GR
		CMPA $82 				; TPS Ld Axis Var
		BCC LE1FB 				;
								; ... else
		ANDB #$FB 				; 1111 1011
		BRA LE241 				;
LE1F0: 	BITB #$04 				; b2
		BEQ LE1FB 				; BR IF NOT b2
								; ... else
		ORAB #$80 				; b7, OD ACTIVE
		ANDB #$FB 				; CLR b2
		LDX #$C2BB 				;
LE1FB: 	STAB $04 				; Xmission Mode word
		LDAB LC28F 				; Rd Spd Coast, 32 MPH
		LDAA $45 				;
		BITA #$10 				; b4
		BNE LE20A 				; BR IF b4
								; ... else
		ADDB #5 				; 5 MPH HYST
		BCS LE20E 				; BR IF OVERFLOW
								; ... else
LE20A: 	CMPB $65 				; CK Vss
		BLS LE215 				; BR IF Vss LT 32+5 MPH (COAST)
								; ... else
LE20E: 	ANDA #$EF 				; CLR b4
		LDAB LC290 				;
		BRA LE21A 				;
LE215: 	ORAA #$10 				; b4
		LDAB LC291 				; Hi MPH Coast TPS
LE21A: 	STAA $45 				;
		LDAA $04 				; Xmission Mode word
		PSHA 					;
		BPL LE227 				;
								; ... else
		SUBB LC28E 				; Hyst for TCC
		BCC LE227 				;
								; .... else
		CLRB 					;
LE227: 	LDAA $82 				; TPS Ld Axis Var
		CBA 					;
		PULB 					;
		BCS LE25E 				;
								; ... else
		BITB #$C0 				; b7 & b6
		BEQ LE245 				; BR IF NOT b7 & b6
								; ... else
		JSR LE2BE 				;
		BCS LE25E 				;
								; ... else
		BITB #$20 				; b5
		BEQ LE25A 				; BR IF NOT b5
								; ... else
		LDAA $57 				;
		CMPA LC292 				; kick Dn Prevent (Shifter Sw)
		BHI LE285 				;
								; ... else
LE241: 	ANDB #$FE 				; CLR b0
		BRA LE272 				;
								;
								;
								;
LE245: 	LDAA LC014 				; Get Fuel/Air Mode Wd 1
		ASRA 					; SHIFT RIGHT
		BCC LE256 				;
								; ... else
		TBA 					;
		BITB #$08 				; b3
		BNE LE256 				;
								; ... else
		ANDA #$06 				; CLR ALL EXCEPT b1 & b2
								; ... else
		CMPA #$06 				;
		BNE LE272 				;
								; ... else
LE256: 	BSR LE2BE 				;
		BLS LE272 				;
								; ... else
LE25A: 	ORAB #$81 				; SET b7 & b0
		BRA LE285 				;
LE25E: 	BITB #$80 				; b7
		BEQ LE272 				; BR IF NOT b7
								; ... else
LE262: 	LDAA $57 				;
		BITB #$12 				; b4 & b1
		BEQ LE26D 				; BE IF
								; ... else
		CMPA LC294 				; Kick Dn/Unlock TPS Prev'nt RPM,
								; (3200 RPM)
		BRA LE270 				;
LE26D: 	CMPA LC293 				; Kick Dn/Unlock TPS RPM, (3200 RPM)
LE270: 	BHI LE279 				; BR IF RPM LT THRESH'S
								; ... else
LE272: 	LDAA $65 				; CURRENT Vss
		CMPA LC295 				; Min VSS, (60 MPH) for Force Upshift
		BLS LE27E 				; BR IF Vss GT 60 MPH
								; ... else
LE279: 	CLR $8D 				; CLR TCC LK Dly tmr
		BRA LE25A 				; EXIT
LE27E: 	ANDB #$7F 				; CLR b7
		LDAA LC28C 				; 0 sec's, Up Shft lk delay
		STAA $8D 				; TCC LK Dly tmr, (.1sec)
LE285: 	BITB #$20 				; b5
		BEQ LE28F 				;
								; ... else
		BITB #$80 				; b7
		BNE LE28F 				; BR IF NOT b7
								; ... else
		ANDB #$FE 				; CLR b0
LE28F: 	ANDB #$9F 				; CLR b5 & b6
		LDAA $37 				; MCU INPUT STATUS WD
		ANDA #$DF 				; CLR b5, (TCC)
		PSHA 					;
		LDAA $8D 				; TCC LK Dly tmr, (.1sec)
		BEQ LE29B 				;
								; ... else
		DECA 					;
LE29B: 	STAA $8D 				; TCC LK Dly tmr, (.1sec)
		LDAA $90 				; 4-3 Dnw Shft Lk -> Interupt tmr
		BEQ LE2A7 				;
								; ... else
		DECA 					;
		STAA $90 				; 4-3 Dnw Shft Lk -> Interupt tmr
		PULA 					;
		BRA LE2B1 				;
LE2A7: 	LDAA $8D 				; TCC LK Dly tmr, (.1sec)
		PULA 					;
		BNE LE2B1 				;
								; ... else
		TSTB 					;
		BPL LE2B1 				;
								; ... else
		ORAA #$20 				; SET b5, TCC LOCKED
LE2B1: 	STAA $37 				; MCU INPUT STATUS WD
		STAB $04 				; Xmission Mode word
		ROR $35 				; MNR LOOP MD WD 2
		RORB 					;
		ROL $35 				;
		BRA LE2D1 				;
LE2BE: 	PSHB 					;
		LDAA $65 				;
		CMPA #$64 				; 0110 0100
		BLS LE2C7 				;
								; ... else
								;
								; Call 2D Lk Up
								;
		LDAA #$64 				; Get Var for Lk Up
LE2C7: 	ASLA 					;
		LDAB #$18 				; Off set for Lk Up
		JSR LF2B9 				; Call 2D Lk Up
		PULB 					;
		CMPA $82 				; TPS Ld Axis Var
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* CARS SHIFT LIGHT CODE
		;*
		;* This code to provide a shift schedule for best
		;* fuel economy.
		;*
		;*******************************************************
								;
								; QUALS FOR CHECK ENG LIGHT
								;
LE2D1: 	LDAA $5B 				; COOLANT
		CMPA LC2D4 				; 50c, Cool Temp for shift lite
		BLS LE347 				;
								; ... else
		LDAA $65 				;
		CMPA LC2D5 				; 40 MPH, Lmt for Ck Shift lamp
		BLS LE347 				;
								; ... else
		LDAA $82 				; 0%, TPS Ld Axis Var
		CMPA LC2D6 				; TPS min Val for lite
		BCS LE347 				;
								; ... else
		LDAA $63 				; Filtered ld value
		CMPA LC2D7 				; 0, Ld Val for Shift lite on
		BLS LE347 				;
								; ... else
		LDAA $8F 				;
		CMPA LC288 				; 30d, Min 4th gr param
		BLS LE347 				;
								; ... else
								;
								; CK SHIFT LIGHT hi/lo RPM QUALS
								;
		LDAA $57 				;
		LDX #$C2D8 				; INDEX 2 line HIGH RPM HYST TBL
								;
		BRCLR $35 $80 LE300 	; BR IF NOT b7, (OVERDRIVE ON)
								; ... else
		LDX #$C2E1 				; INDEX 2 LINE LOW RPM HYST TBL
LE300: 	CMPA 0,X 				; Lite off, 625 RPM
		BLS LE347 				;
								; ... else
		CMPA 1,X 				; lite Off, 5100 RPM
		BLS LE315 				;
								; ... else
		LDX #0 					;
		STX $91 				; Err Lmp On Dly
		BSET $35 $80 			; SET b7, (OVERDRIVE ON)
		BSET $36 $10 			; SET b4, (SHIFT LIGHT DELAY STARTED)
		BRA LE34D 				;
								;
								;Call 2D Lk Up
								;
LE315: 	LDAB #$10 				; Offset Value for Lk Up
		LSRA 					; n/2
		INX 					; INDEX L___/L___ Table Addr
		INX 					;
		JSR LF2B9 				; Call 2D Lk Up
		CMPA $82 				; TPS Ld Axis Var w/table value
		BCS LE347 				;
								; .... else
		BRSET $36 $10 LE33B 	; BR IF b4, (SHFT LITE DLY STARTED)
								; .... else
		BSET $36 $10 			; SET b4, (SHFT LITE DLY STARTED)
								;---------------------------------------------
								; LK UP Eng Lite on Modifier Dly vs % TPS
								;
								; Tbl Val = Sec * 10/delay mult
								;---------------------------------------------
		LDAA $82 				; TPS Ld Axis Var For Lk Up
		BPL LE32E 				;
								; ... else
		LDAA #$80 				;
LE32E: 	LSRA 					;
		LDX #$C2EB 				; Tbl Addr
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		LDAB LC2EA 				;
		MUL 					;
		STD $91 				; Err Lmp On Dly
LE33B: 	BSET $35 $80 			; SET b7, (OVERDRIVE ON)
		LDX $91 				; Err Lmp On Dly
		BEQ LE34D 				; BR IF Z
								; .... else
		DEX 					; DECR ERR Lmp On Dly
		STX $91 				; ERR Lmp On Dly
		BRA LE34A 				;
LE347: 	BCLR $36 $10 			; CLR b4 (SHFT LITE DLY STARTED)
LE34A: 	BCLR $35 $80 			; CLR b7, (OVERDRIVE ON)
LE34D: 	RTS 					;
		;******************************************************
		;*******************************************************
		;* MAJOR LOOP SUNBROUTINE SEG 9
		;*
		;* Inj Air Managment
		;*
		;*******************************************************
LE34E: 	LDX #$C2FF 				; POINT TO AIR INJ PARAM'S
		LDAB $44 				; AF MODE WD #2
		ANDB #$80 				; b7, MASK FOR b7, 1 = CLS LOOP
		ORAB $3D 				;
		STAB $3D 				;
		LDAA $5B 				; COOLANT
		CMPA $0C,X 				; $820B, If Cool < 14c, divert
		BCS LE365 				;
								; ... else
		LDAA $3B 				;
		BITA #$04 				; b2
		BEQ LE368 				; BR IF NOT b2
								; ... else
LE365: 	JMP LE42B 				;
LE368: 	LDAA $44 				; AF MODE WD #2
		BPL LE375 				;
								; ... else
		CLR $FC 				;
		LDAA $0D,X 				; LC3C, If in Open Lp => 25.4 Sec after
								; closed loop, Divert
		STAA $FD 				;
		BRA LE39A 				;
LE375: 	TSTB 					;
		BPL LE37F 				;
								; ... else
		LDAA $FD 				;
		BEQ LE365 				;
								; ... else
		DECA 					;
		STAA $FD 				;
LE37F: 	BRCLR $34 $80 LE39A 	; BR IF NOT b7, ENG RUNNING FLAG
								; ... else
		LDAB $FC 				;
		LDAA $FB 				;
		SUBA $63 				; FILTERED LD VALUE
		BLS LE391 				;
								; ... else
		CMPA $0B,X 				; LC30A, Enable air to conv if in open lp
								; & 100 Msec Drop in D VAL L.T. 64 gms/S
		BLS LE391 				;
		LDAB 7,X 				; LC306, Enable air to conv If > 1 SEC
								; since last neg delta LV8 > is <
LE391: 	TSTB 					;			
		BEQ LE39A 				;
								; ... else
		DECB 					;
		STAB $FC 				;
		JMP LE41E 				;
LE39A: 	LDAB $56 				; RPM/25
		CMPB 6,X 				; Divert if cont > 4000 RPM & time >
		BLS LE3AB 				;
								; ... else
		LDAB $F6 				;
		CMPB 1,X 				;
		BHI LE3E0 				;
								; ... else
		INCB 					;
		BNE LE3AC 				;
								; ... else
		BRA LE3AE 				;
LE3AB: 	CLRB 					;
LE3AC: 	STAB $F6 				;
LE3AE: 	LDAA $43 				; AF MODE WD
		BITA #$20 				; b5, PWR ENR IS ACTIVE
		BEQ LE3BF 				; BR IF NOT b5
								; ... else
		LDAB $FE 				;
		CMPB 0,X 				;
		BEQ LE3E0 				;
								; ... else
		INCB 					;
		STAB $FE 				;
		BRA LE418 				;
LE3BF: 	CLR $FE 				;
		LDX $F7 				;
		LDAB $73 				;
		CMPB LC301 				; If Filtered o2 > 752 MVDC for ___
								; Then divert
		BLS LE3D3 				;
								; ... else
		CPX LC303 				;
		BCC LE3DC 				;
								; ... else
		INX 					;
		BRA LE3D8 				;
LE3D3: 	LDX $F7 				;
		BEQ LE3E2 				;
		DEX 					;
LE3D8: 	STX $F7 				;
		BRA LE3E6 				;
LE3DC: 	ORAA #$01 				; SET b0, AIR INJ DIVERT
		STAA $43 				; AF MODE WD
LE3E0: 	BRA LE42B 				;
LE3E2: 	ANDA #$FE 				; CLR b0, AIR INJ DIVERT
		STAA $43 				; AF MODE WD
LE3E6: 	RORA 					;
		BCS LE42B 				;
								; ... else
		CMPB LC302 				; If Filtered o2 > 247 MVDC & IN CLS LP
								; Then divert
		BCC LE3F2 				;
								; ... else
		LDAA $44 				; AF MODE WD #2
		BMI LE3F7 				;
								; ... else
LE3F2: 	LDX #$0000 				;
		BRA LE3FF 				;
LE3F7: 	LDX $F9 				; RICH OR LEAN o2 DIV TIMER
		CPX LC303 				; 200, 20 sec Rich or Lean o2 Div timer
		BHI LE42B 				; BR IF TIME LT 20 Sec
								; ... else
		INX 					; INCR TMR
LE3FF: 	STX $F9 				; RICH OR LEAN o2 DIV TIMER
		LDAA $63 				; Filtered ld value
		CMPA LC307 				; If LV8 < 25 then Divert
		BCS LE42B 				;
								; ... else
		CMPA LC308 				; If LV8 > 100 & MPH > Divert Air
		BLS LE414 				;
								; ... else
		LDAA $65 				; CURRENT Vss
		CMPA LC309 				; If > 60 MPH & LV8 > then divert AIR
		BHI LE42B 				;
								; ... else
LE414: 	LDAB $3D 				;
		BMI LE41E 				;
								; ... else
LE418: 	LDAB $3E 				;
		ORAB #$06 				; b1 & b2
		BRA LE42F 				;
LE41E: 	LDAB $3E 				;
								;
								; CK AFR MD WD FOR NUM OF AIR VALVES
								;
		LDAA LC015 				; $04, AFR Mode Wd 2
		BEQ LE42B 				; BR If zero, 1 AIR VALVE
								; ... else
		ANDB #$FB 				; CLR b2,
		ORAB #$02 				; SET b1, 1 = 2 AIR VALVES IN USE
		BRA LE42F 				;
LE42B: 	LDAB $3E 				;
		ANDB #$F9 				; CLR b1 & b2
LE42F: 	STAB $3E 				;
		LDAA $63 				; FILT LD VALUE
		STAA $FB 				;
		RTS 					;
								;-------------------------------------
LE436: 	BRCLR $34 $80 LE44E 	; BR IF NOT b7
								; ... else
		BRCLR $38 $80 LE44E 	; BR IF NOT b7
								; ... else
		LDAB $DF 				; Num of Async Pulses since Accel Enr
		CMPB $D7 				; Num of Async Pulses to do
		BCS LE451 				; BR IF TO DO G.T. DONE VAL (AS FUEL)
								; ... else
		BRSET $38 $40 LE44E 	; BR IF b6,
		CLR $DF 				; Num of Async Pulses since Accel Enr
		BCLR $38 $80 			; CLR b7
LE44E: 	JMP LE4AD 				;
		;***************************************************
		;***************************************************
		;* Accel Enrichment Factor % BPW INJ (0-4)
		;* (ASYNC FUEL CODE FOR DISFF TPS & STALL SVR)
		;*
		;*
		;* INDEXED LK UP, 8 LINES
		;*
		;* TBL = FACTOR * 64
		;***************************************************
LE451: 	INC $DF 				; Num of Async Pulses since Accel Enr
		CMPB #7 				; MAX OF 8 PULSES
		BLS LE45A 				; BR IF COUNT GT 7
								; ... else
		LDAB #7 				; USE MAX FOR LK UP
LE45A: 	LDX #$C315 				; TBL ADDRESS
		ABX 					; ADJ TBL POINTER
		LDAA 0,X 				; GET VAL FM TBL
		PSHA 					; SAVE TO STX
		LDD $C2 				;
		BRCLR $39 $80 LE468 	; BR IF NOT b7, (IN SINGLE FIRE)
								; ... else
		LSRD 					;
LE468: 	PSHB 					;
		PSHA 					;
		TSX 					;
		LDAA 2,X 				;
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		PULX 					;
		INS 					;
		ASLD 					;
		BCS LE478 				;
		ASLD 					;
		BCC LE47B 				; BR IF NO OVER FLOW
								; ... else
LE478: 	LDD #$FFFF 				; USE MAX VAL
LE47B: 	STD $DB 				; New Accel Enrich PW
		LDAA $D8 				; Cool Fact for Async Pulses
		LDX #$DB 				;
		JSR LF472 				; FACTOR IN 8x16 MULT
		LDX LC312 				; 0364d, 5.5 msec Max Async Pulse Width
		CPX $DB 				; New Accel Enrich PW
		BHI LE48E 				; BR IF GT 5.5 Msec THRESH
								; ... else
		STX $DB 				; New Accel Enrich PW
LE48E: 	BRCLR $38 $40 LE4A1 	; BR IF NOT b6
								; ... else
		JSR LF9E5 				;
		PSHX 					;
		TSX 					;
		SUBD 0,X 				;
		PULX 					;
		BLS LE4AD 				;
								; ... else
		CPD $DB 				; New Accel Enrich PW
		BCS LE4A3 				;
								; ... else
LE4A1: 	LDD $DB 				; New Accel Enrich PW
LE4A3: 	CPD LC310 				; 1.69 msec Min Async Pulse Width
		BCS LE4AD 				;
								; ... else
		STD $DB 				; New Accel Enrich PW
		BRA LE4B1 				;
LE4AD: 	CLRB 					;
		CLRA 					;
		STD $DB 				; New Accel Enrich PW
LE4B1: 	BRCLR $3D $10 LE4CA 	; BR IF NOT b4
								; ... else
		BRCLR $8A $FF LE4CA 	; BR IF NOT $FF
								; ... else
		DEC $8A 				; Accel Enrich After Decel C/O
		LDD LC605 				; Cut off Stall Save PW
		ADDD $DB 				; New Accel Enrich PW
		STD $DB 				; New Accel Enrich PW
		ADDD $D9 				; Accel Enrich, Acum Fuel
		BCC LE4CA 				;
								; ... else
		LDD #$FFFF 				;
LE4CA: 	STD $D9 				; Accel Enrich, Acum Fuel
		LDX $DB 				; New Accel Enrich PW
		BEQ LE4E7 				;
		LDAB $CB 				; Inj Offset Corr, Ms*65.636
		ABX 					;
		ABX 					;
		STX $3FF2 				; Async PW Cnt'r
		JSR LF3B5 				; 11 Usec Delay
		LDAA #$04 				;
		JSR LF4C3 				;
		LDAA #$FB 				;
		JSR LF3B5 				; 11 Usec Delay
		JSR LF4CE 				;
								;---------------------------------------------
								; CK IF HEADS UP CONNECTED
								; Accel Enrich
								;
								;--------------------------------------------
LE4E7: 	BRCLR $33 $80 LE4EE 	; BR IF NOT b7, (HEADS UP ON LINE)
								; ... else
		JSR $5818 				; <---- TO HEADS UP
								;--------------------------------------------------
LE4EE: 	LDD $011A 				; ACCEL ENR PW
		ADDD $DB 				; New Accel Enrich PW
		STD $011A 				; ACCEL ENR PW
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* DIAGNOSTIC QUALIFICATION CODE
		;*
		;*
		;*
		;*******************************************************
								;
								; ERR 13 o2 SENSOR
								;
								;
LE4F7: 	LDAB $3F 				;
		BITB #$24 				;
		BNE LE546 				;
								; ... else
		LDAA $41 				; DIAGNOSTIC MODE WD 3
		ASLA 					;
		BMI LE510 				;
								; ... else
		LDD $1A 				; Eng Run Time (sec)
		LSRD 					; DIV BY 2
		CMPB LC1DC 				; ERR 13,If eng run time < 30 Sec,
								; disable ERR
		BCS LE546 				;
								; ... else
		LDAA $41 				; DIAGNOSTIC MODE WD 3
		ORAA #$40 				; SET b6
		STAA $41 				; DIAGNOSTIC MODE WD 3
								;
								; CK ERR 13 o2 LIMITS
								;
LE510: 	LDAB $6F 				; o2 A/D VALUE
		CMPB LC1DE 				; ERR 13, 548 Mvdc
		BHI LE546 				; BR IF o2 VDC LT 548 Mvdc
								; ... else
		CMPB LC1DD 				; ERR 13, 350 Mvdc
		BLS LE546 				; BR IF o2 VDC GT 350 Mvdc
								; ... else
		LDAB $AA 				; o2 Sensor Hvy Ld Cnt'r -> (2 Sec)
		CMPB LC1E0 				;
		BHI LE54B 				; ERR 13, 15 Sec TPS Min time limit
								; ... else
		LDAA $3F 				;
		BITA #$10 				; b4
		BEQ LE551 				; BR IF NOT b4
								; ... else
		LDAB $5B 				; COOLANT
		CMPB LC1DB 				; ERR 13, COOL LOW LMT 70c
		BLS LE551 				; BR IF COOL GT 70c
								; ... else
		LDAA $82 				; TPS Ld Axis Var
		CMPA LC1DF 				; ERR 13, 5% TPS Min
		BHI LE541 				; BR IF TPS LT 5%
								; ... else
		TST $AA 				; o2 Sensor Hvy Ld Cnt'r, (2 Sec)
		BEQ LE551 				; BR IF
		DEC $AA 				; o2 Sensor Hvy Ld Cnt'r, (2 Sec)
		BRA LE551 				;
LE541: 	INC $AA 				; o2 Sensor Hvy Ld Cnt'r, (2 Sec)
		BRA LE551 				;
LE546: 	CLR $AA 				; o2 Sensor Hvy Ld Cnt'r, (2 Sec)
		BRA LE551 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; ERR 21 HIGH TPS SENSOR ERROR
								;
								;--------------------------------------------------
LE54B: 	BSET $4C $40 			; SET b6, NEW ERR 1
		BSET $41 $10 			; SET b4, ERR 13 DETECTED
LE551: 	LDAB $3F 				;
		BITB #$C0 				; b6 & b7
		BNE LE57D 				; BR IF b6 & b7
								; ... else
		LDAA $81 				; TPS, (A/D)
		CMPA LC1E7 				; If TPS <= disable ERR 21
		BLS LE571 				; BR IF TPS GT 50%
								; ... else
		LDAA $A7 				; TPS Err cnt'r,(100Ms)
		CMPA LC1E8 				; 20 Sec's Time req for ERR 21
		BHI LE578 				; BR IF TIME LT 20 SEC
								; ... else
		LDAA $B6 				;
		CMPA LC1E9 				; If Air Flow >= 12 gm?sec,
								; disable ERR 21
		BCC LE573 				;
								; ... else
		INC $A7 				; TPS Err cnt'r,(100Ms)
		BRA LE57D 				;
LE571: 	ANDB #$FB 				; 1111 1011
LE573: 	CLR $A7 				; TPS Err cnt'r,(100Ms)
		BRA LE57D 				;
LE578: 	BSET $4C $04 			; NEW ERR 1
		ORAB #$04 				; SET b2
LE57D: 	STAB $3F 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; ERR 24 VSS SENSOR ERROR
								;
								;--------------------------------------------------
		BITB #$E4 				; 1110 0100
		BNE LE5B0 				;
								; ... else
		LDAA $65 				;
		CMPA LC1F2 				; If > 3 MPH then disable ERR 24
		BHI LE5B0 				;
								; ... else
		LDAA $AB 				; VSS Err TIMER
		CMPA LC1F7 				; 2 Sec's for ERR 24
		BHI LE5BE 				; BR IF TIME LT 2 Sec
								; ... else
		LDAA $63 				; Filtered LD VALUE
		CMPA LC1F6 				; If LV8 >= 26 LV8, Disable ERR 24
		BCC LE5B0 				; BR IF LD VAL GT 26
								; ... else
		LDAA $82 				; TPS Ld Axis Var
		CMPA LC1F5 				; If => 2% TPS then Disable ERR 24
		BCC LE5B0 				; BR IF TPS GT 2%
								; ... else
		LDAA $37 				; MCU INPUT STATUS WD
		RORA 					;
		BCS LE5B0 				;
								; ... else
		LDAA $56 				; RPM/25
		CMPA LC1F3 				; If <= 1000 RPM, Disable ERR 24
		BLS LE5B0 				; BR IF RPM GT 1000 RPM
								; ... else
		CMPA LC1F4 				; If RPM > 6000 then disable ERR 24
		BLS LE5B5 				; BR IF RPM GT
								; ... else
LE5B0: 	CLR $AB 				; VSS Err cnt'r
		BRA LE5C7 				;
LE5B5: 	BITB #$08 				; b3
		BEQ LE5C7 				; BR IF NOT b3
								; ... else
		INC $AB 				; VSS Err cnt'r
		BRA LE5C7 				;
LE5BE: 	BSET $4D $80 			; SET b7
		LDAA $44 				; AF MODE WD #2
		ORAA #$10 				; b4, Vss FAILURE
		STAA $44 				; AF MODE WD #2
								;--------------------------------------------------
								;--------------------------------------------------
								; ERR 32 EGR DIAGNOSTIC ERROR
								;
								;--------------------------------------------------
LE5C7: 	LDAB $41 				; DIAGNOSTIC MODE WD 3
		BRSET $2E $20 LE5D3 	; BR IF b5
		ANDB #$20 				; CLR
		BNE LE5E6 				; ALL EXCEPT b5, EGR DIAG SW
								; BAD AT START UP, ERR 32
								; ... else
		BRA LE612 				;
LE5D3: 	ANDB #$DF 				; CLR b5
		STAB $41 				; DIAGNOSTIC MODE WD 3
		LDX #$C1FB 				;
		LDAA $3F 				;
		BITA #$00E4 			;
		BNE LE612 				;
								; ... else
		LDD $AE 				; EGR Diag Cnt'tr
		SUBD $0005,X 			;
		BLS LE5EB 				;
								; ... else
LE5E6: 	BSET $4D $08 			;
		BRA LE61F 				;
LE5EB: 	LDAA $63 				; Filtered ld value
		CMPA 0,X 				;
		BCC LE608 				;
								; ... else
		LDAA $5B 				; COOLANT
		CMPA 1,X 				;
		BCS LE612 				;
								; ... else
		LDAA $0112 				;
		CMPA 2,X 				;
		BLS LE608 				;
								; ... else
		LDAA $82 				; TPS Ld Axis Var
		CMPA 3,X 				;
		BHI LE608 				;
								; ... else
		CMPA 4,X 				;
		BHI LE617 				;
LE608: 	LDX $AE 				; EGR Diag Cnt'tr
		BEQ LE61D 				;
								; ... else
		DEX 					;
		BEQ LE61D 				;
								; ... else
		DEX 					;
		BRA LE61D 				;
LE612: 	LDX #0 					;
		BRA LE61D 				;
LE617: 	LDX $AE 				; EGR Diag Cnt'tr
		LDAB LC202 				;
		ABX 					;
LE61D: 	STX $AE 				; EGR Diag Cnt'tr
								;--------------------------------------------------
								;--------------------------------------------------
								; ERR 33 MASS FLOW HIGH ERROR
								;
								;--------------------------------------------------
LE61F: 	LDAB $3F 				;
		BMI LE658 				;
								; ... else
		BITB #$24 				; ERR 33
		BNE LE653 				; BR IF ERR 33
								; ... else
		LDAA $7F 				; Pump Volts, (A/D)
		CMPA LC208 				; 10.0 VDC MIN PUMP V FOR ERR 33 ENABLE
		BLS LE653 				; BR IF PUMP V GT 10 VDC
								; ... else
		BRSET $40 $20 LE65F 	; BR IF b5, ERR 54A
								; ... else
		LDAA $B6 				; AIR FLOW
		CMPA LC204 				; 45 gms/sec AIR MIN FOR ERR 33 ENABLE
		BLS LE653 				; BR IF AIR FLOW GT 45 gms/sec
								; ... else
		LDAA $B0 				; MAF Hi Cnt'r, (100Ms)
		CMPA LC206 				; 800 Msec MIN TIME QUAL FOR ERR 33
		BHI LE658 				; BR IF TIME LT 800 Msec
								; ... else
		LDAA $82 				; TPS Ld Axis Var
		SUBA LC203 				; 15% TPS MIN FOR ENABLE ERR 34
		BCC LE653 				; BR IF TPS GT 15%
								; ... else
		LDAA $56 				; RPM/25
		CMPA LC207 				; 3200 RPM
		BHI LE653 				; BR IF RPM LT 3200 RPM
								; ... else
		INC $B0 				; MAF Hi Cnt'r, (100Ms)
		BRA LE65D 				;
LE653: 	CLR $B0 				; CLR MAF Hi Cnt'r, (100Ms)
		BRA LE65D 				;
LE658: 	BSET $4D $04 			; SET ERR 33
		ORAB #$80 				; b7
LE65D: 	STAB $3F 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; ERR 34B MAF SENSOR
								;
								;--------------------------------------------------
LE65F: 	BRSET $3F $40 LE696 	; BR IF b6,
								; ... else
		LDX $EF 				; Diff Val to calc Air Flow
		CPX LC20A 				; IF Analog CTS * 7 or PP2DLT < 55
		BHI LE69E 				; BR IF .... ,(EXIT & CLR TMR)
								; ... else
		LDAA $B5 				; ERR 34 TMR
		CMPA LC20C 				; ERR #34, 200 MSEC Min time req.
		BCC LE696 				; BR IF TMR GT 200 MSEC
								; ... else
		BRCLR $3F $A4 LE677 	;
		BRA LE6A1 				;
								;
								; ERR 34 CRITERIA
								;
LE677: 	LDAB $56 				; RPM/25
		CMPB LC20D 				; If RPM > 600, Ck ERR #34
		BCS LE6A1 				; BR IF RPM LT 600 RPM, (EXIT)
								; ... else
		LDAB $82 				; TPS Ld Axis Var
		CMPB LC20E 				; If TPS > 6.25%, Ck ERR #34
		BCS LE6A1 				; BR IF TPS LT 6.25%, (EXIT)
								; ... else
		LDAB $63 				; Filtered ld value
		CMPB LC20F 				; If LV8 >= 0, Ck ERR #34
		BCS LE6A1 				; BR IF LV8 LT 0 gms/sec, (EXIT)
								; ... else
		CMPB LC210 				; If LV8 <= 130, Ck ERR #34
		BHI LE6A1 				; BR IF LV8 LT 130 gms/sec, (EXIT)
								; ... else
		INCA 					; INCR ERR 34 TIMER
		STAA $B5 				; ERR 34 TMR
		BRA LE6A1 				; EXIT
								;---------------------------------------------
LE696: 	BSET $4D $02 			; SET
		BSET $3F $40 			; SET ERR 34
		BRA LE6A1 				; 69E
								;
								; CLER ERROR TMR
								;
LE69E: 	CLR $B5 				; ERR 34 TMR
		;***************************************************
		;***************************************************
		;* ERR 41 CYL SELECT
		;*
		;***************************************************
LE6A1: 	LDAA $40 				; DIAGNOSTIC MODE WD 2
		LSRA 					;
		BCC LE6A9 				; BR IF
								; ... else
		BSET $4E $40 			; b6, ERR 41
LE6A9: 	ASLA 					;
		STAA $40 				; CLR ERR LAST 100 Msec FLAG
		;***************************************************
		;***************************************************
		;* ERR 42B SPK MONITORING
		;*
		;* 2 SPK MPN NOT TOGGLING WHEN SPK ENABLED
		;***************************************************
		LDAB $3F 				;
		BRSET $36 $01 LE6DF 	; BR IF b0, (SKP ERR42 FOR STALL SVR)
								; ... else
		LDAA $00 				;
		BITA #$10 				; b4
		BNE LE6DF 				; BR IF b4
								; ... else
		LDAA $34 				;
		ASLA 					;
		BPL LE6DF 				;
								; ... else
		LDAA $56 				; RPM/25
		CMPA LC226 				; RPM, tbl1,ntrpm
		BLS LE6D8 				; BR IF RPM GT THRESH
								; ... else
		LDAA $B4 				; SPK Fd Bk counter, (Err #42)
		BNE LE6D8 				; BR IF NZ
								; ... else
		BITB #$01 				;
		BNE LE6D0 				; NR IF b0
								; ... else
		ORAB #$01 				; SET b0
		BRA LE6DA 				;
LE6D0: 	LDAA $01 				;
		ORAA #$80 				; SET b7
		STAA $01 				;
		BRA LE6DC 				;
LE6D8: 	ANDB #$FE 				;
LE6DA: 	STAB $3F 				;
LE6DC: 	CLR $B4 				; SPK Fd Bk counter, (Err #42)
		;***************************************************
		;***************************************************
		;* ERR 43 KNOCK FAIL
		;*
		;*
		;***************************************************
LE6DF: 	LDAA $3B 				;
		BITA #$20 				; b5, ERR 43A
		BNE LE6E9 				; BR IF ERR 43A
								; ... else
		LDAA $02 				; MD WD 2
		BPL LE6EC 				;
								; ... else
LE6E9: 	BSET $4E $10 			; SET b4, ERR43
LE6EC: 	LDAB $41 				; DIAGNOSTIC MODE WD 3
		ANDB #$FD 				; CLR b1, ERR 44/45 FLG
		;***************************************************
		;***************************************************
		;* ERR 44 LEAN o2 SENSOR
		;*
		;*
		;***************************************************
		LDAA $3F 				;
		BITA #$00C0 			;
		BNE LE758 				;
								; ... else
		LDAA LC22E 				;
		CMPA $73 				;
		BLS LE717 				;
								; ... else
		LDAA $A8 				; o2 Lan Cnt'r (2 Sec)
		CMPA LC22F 				;
		BHI LE71C 				;
								; ... else
		LDAA $44 				; AF MODE WD #2
		BPL LE717 				;
								; ... else
		BITB #$0004 			;
		BNE LE717 				;
								; ... else
		LDAA $3F 				;
		BITA #$08 				;
		BEQ LE721 				;
								; ... else
		INC $A8 				; o2 Lan Cnt'r (2 Sec)
		BRA LE721 				;
LE717: 	CLR $A8 				;o2 Lan Cnt'r (2 Sec)
		BRA LE721 				;
LE71C: 	BSET $4E $08 			; b3, NEW ERR 2
		ORAB #$02 				;
		;***************************************************
		;***************************************************
		;* ERR 45 RICH o2 SENSOR
		;*
		;*
		;***************************************************
LE721: 	LDAA $73 				; o2 VDC
		CMPA LC230 				;
		BLS LE74E 				;
								; ... else
		LDAA $A9 				; o2 Sensor Rich tmr,
								; -> (2 Sec)
		CMPA LC231 				;
		BHI LE753 				;
								; ... else
		LDAA $44 				; AF MODE WD #2
		BPL LE74E 				;
		BITB #4 				; 0000 0100
		BNE LE74E 				;
								; ... else
		LDAA $82 				; TPS Ld Axis Var
		CMPA LC232 				;
		BHI LE743 				;
								; ... else
		CMPA LC233 				;
		BCC LE74E 				;
LE743: 	LDAA $3F 				;
		BITA #8 				; 0000 1000
		BEQ LE758 				;
		INC $A9 				; o2 Sensor Rich tmr,
								; -> (2 Sec)
		BRA LE758 				;
								;
LE74E: 	CLR $A9 				; o2 Sensor Rich tmr,
								; -> (2 Sec)
		BRA LE758 				;
LE753: 	BSET $4E $04 			; b2, NEW ERR 2
		ORAB #$02 				; SET b1 ERR 44/55 HAS BEEN CAUGHT
LE758: 	ANDB #$FB 				; CLR b2 INT FORCED 128. ERR 44/45
		STAB $41 				; DIAGNOSTIC MODE WD 3
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* MAJOR LOOP SUBROUTINE, SEG C
		;*
		;* Can Purge
		;*
		;*******************************************************
LE75D: 	LDAA $3B 				; OLD CCP STATUS BYTE
		LSRD 					;
		LDAA $3E 				;
		RORA 					;
		ASLD 					;
		STAA $3E 				;
								;
								; CCP SOLENOID SHALL NOT BE ON UNLESS
								; RUNTIME GT CAL PARAM
								;
								;
		BRSET $01 $10 LE774 	;
		LDAA $1B 				;
		CMPA LC269 				;
		BLS LE787 				;
								; .... else
		BSET $01 $10 	; b4
								;
								; CK IF IN ALDL
								;
LE774: 	LDAA $35 				;
		BITA #$20 				; b5, DIAG SW IN ALDL MODE
		BNE LE787 				;
								; .... else
		LDAA $5B 				; COOLANT
		CMPA LC26A 				;
		BCS LE787 				;
								; .... else
		LDD $43 				; AF MODE WD
		BITA #$02 				; b1, DECELL FUEL CUT OFF ENABLED
		BEQ LE789 				; BE IF NOT b1
								; ... else
LE787: 	BRA LE808 				;
								;
								; CK AFR MD WD 1 FOR CCP CLSD LOOP OPTION ....
								;
LE789: 	LDAA LC014 				; Get Fuel/Air Mode Wd 1
		BITA #$20 				; Bit Cl'sed Lp for CCP ?
		BEQ LE797 				; BR IF NOT b5
								; .... else
		TSTB 					;
		BMI LE797 				; BR IF
								;.... else
								;
								; b1 ERR 44/55 HAS BEEN CAUGHT
								; b4 ERR 13 DETECTED
								;
		BRCLR $41 $12 LE808 	; BR IF NOT b1 & b4
								; ... else
LE797: 	LDX #$C26B 				; INDEX Enable CCP
		LDAB $3B 				;
		RORB 					;
		BCC LE7A2 				;
								; ... else
								;----------------------
								; CCP on to off params
								;
								;----------------------
		LDX #$C26E 				; INDEX CCP on to off params
LE7A2: 	LDAA $67 				; Vss, (MPH/(16/5))
		CMPA 0,X 				; If Vss < 3.12 MPH Disable CCP
		BLS LE808 				; BR
								; ... else
		LDAA $82 				; TPS Ld Axis Var
		CMPA 1,X 				; IF TPS < 1.95% then Disable purge
		BLS LE808 				; BR IF
								; ... else
		LDD $EA 				; Gms/Sec Disp Value
		CMPA 2,X 				; If Flow > 0 gms/sec don't disable CCP
		BLS LE808 				; BR
								; ... else
		CMPA #$20 				;
		BCS LE7BB 				; BR IF
								; ... else
		LDD #$FFFF 				;
LE7BB: 	ASLD 					;
		ASLD 					;
		ASLD 					;
								;--------------------------------------------------
								; CCP Duty Cycle vs MAF, Gms/Sec
								;
								; Table Value = %DC * 2.56
								;--------------------------------------------------
		LDX #$C272 				; Tbl Addr
								; - CCP vs MAF, (%DC)
		JSR LF2BF 				; Call 2d Lk Up W/ Interp
		PSHA 					;
								;-----------------------------------------------------
								; CCP Duty Cycle Gain VS Load Value
								;
								; Tle Value = Factor * 128
								;-----------------------------------------------------
		LDX #$C27C 				; CCP Duty Cycle Gain Table
		LDAB #16 				; Offset Val for Lk Up
		LDAA $63 				; Filtered ld value
		LSRA 					; SAVE, DIV by 2
		JSR LF2B9 				; Call 2D Lk Up
		PULB 					;
		MUL 					;
		ROLB 					;
		ROLA 					;
		BCC LE7D8 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VALUE
LE7D8: 	LDAB $0113 				; CCP PW
		BEQ LE7E5 				;
								; ... else
		LDAB $0114 				;
		BEQ LE7FF 				;
								; ... else
		CBA 					;
		BCC LE7E8 				;
								; ... else
LE7E5: 	STAA $0114 				;
LE7E8: 	PSHA 					;
		LDAA $0113 				; CCP PW
		CLRB 					;
		PSHB 					;
		PSHA 					;
		PULX 					;
		LDAB LC271 				;
		PULA 					;
		JSR LF250 				;
		CMPA $0114 				;
		BCS LE7FF 				;
								; ... else
		CLR $0114 				;
								;---------------------------------------------
								; CK IF HEADS UP CONNECTED
								; CCP
								;---------------------------------------------
LE7FF: 	LDX $33 				; MINOR LOOP MD WD 1
		BPL LE809 				;
								; ... else
		JSR $581E 				; H.U. ROM Addr
		BRA LE809			
LE808: 	CLRA 					; CLEAR CCP PW
								;---------------------------------------------
								;--------------------------------
								; ERR # 54 Params
								; >> Fuel Pump voltage <<
								;--------------------------------
LE809: 	LDAB $3B 				;
		ANDB #$FE 				; 1111 1110
		STAA $0113 				; CCP PW
		BEQ LE814 				; BR IF ZERO PW
								; ... else
		ORAB #$01 				; SET b0
LE814: 	STAB $3B 				;
LE816: 	LDAA #$60 				; A/D Ch 6, (Pump Voltage)
		JSR LF1BE 				; To A/D subroutine
		STAA $7F 				; PUMP VOLTS, (A/D)
		LDAA $40 				; DIAGNOSTIC MODE WD 2
		BITA #$10 				; b4, SET ERR 54 (SET NOT RUNNING)
		BNE LE85A 				; BR IF b4
								; ... else
		BITA #$08 				; b3
		BEQ LE85F 				; BR IF NOT b3, (BYPASS ERR 54)
								; ... else
		LDAB $7E 				; 2.0 VDC Batt Volts Value
		CMPB LC237 				; 2.O VDC MIN IGN VOLTAGE FOR ERR 54
		BCS LE85F 				; BR IF IGN VDC LT 2 VDC, (bypass ERR #54)
								; ... else
		BITA #$40 				; b6
		BEQ LE83F 				; BR IF NOT b6,
								; ... else
		LDX $34 				;
		BPL LE85F 				; (bypass ERR #54)
								; ... else
		LDAB $7F 				; PUMP VOLTS, (a/d)
		CMPB LC236 				; 2.O VDC MIN PUMP VOLTS FOR ERR 54
		BCC LE854 				; IF PUMP VOLTS < 2V, (bypass ERR #54)
								; ... else
		BRA LE85A 				;
								;--------------------------------
								; ERR # 54 Params
								; >> Fuel Pump voltage <<
								;--------------------------------
LE83F: 	LDAB $80 				; ERR 54 TIMER
		INCB 					; INCR ERR 54 TME
		BEQ LE846 				; BR IF TMR = 0
								; ... else
		STAB $80 				; ERR 54 TIMER
LE846: 	CMPB LC234 				; 1.5 SEC'S MIN FOR ERR 54
		BCC LE858 				; BR IF TMR GT 1.5 SEC
								; ... else
		LDAB $7F 				; PUMP VOLTS, (A/D)
		CMPB LC235 				; 2 VDC, MIN Pump Volts
		BCS LE85F 				; BR IF PUMP VDC LT 2 VDC, (SET ERR #54)
								; ... else
		ORAA #$40 				; b6
LE854: 	ANDA #$DF 				; 1101 1111
		BRA LE85F 				;
LE858: 	ORAA #$10 				; b4
LE85A: 	BSET $4F $20 			; SET b5
		ORAA #$20 				; b5
								;
								; BYPASS ERR 54 TESTS
								;
LE85F: 	TAB 					;
		ANDB #04 				; 0000 0100
		ANDA #$F3 				; 1111 0011
		ASLB 					;
		ABA 					;
		STAA $40 				;
		TAB 					;
		LDAA $7F 				; Pump Volts, (A/D)
		BITB #$20 				; b5
		BEQ LE871 				;
								; ....else
								;-----------------------------
								; Get Batt V & CALL L.U.
								;
								;-----------------------------
		LDAA $7E 				; Batt Volts Value, (A/D), For Lk Up
LE871: 	LDX #$C3AB 				; Point to Inj Offset vs Batt Tbl
		JSR LF2C6 				;Call 2d Lk Up, (No Offset)
		STAA $CB 				;Inj Offset Corr, Ms*65.636 fm tbl
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* >>> IAC Tables & Params <<<
		;*
		;*
		;*******************************************************
LE87A: 	LDX #$C62E 				; INDEX IAC Tables & Params
		LDAA $F3 				;
		BITA #$08 				; b3
		BEQ LE8E2 				; BR IF NOT b3
								; ... else
		LDAB $F2 				; IAC Cnt'l Word
		LDAA $34 				;
		BITA #$08 				; b3
		BNE LE88F 				; BR IF b3
								; ... else
		LDAA $F4 				;
		BNE LE8B7 				; BR IF NZ
								; ... else
LE88F: 	LDAA $01 				;
		ASLA 					;
		BMI LE8D7 				;
								; ... else
		LDAA $3B 				;
		ASLA 					;
		BPL LE8D7 				;
								; ... else
		LDAA LC633 				;
		ASLA 					;
		SUBA $010E 				;
		BHI LE8B3 				;
								; ... else
		LDAA $01 				;
		ORAA #$40 				; SET b6
		STAA $01 				;
		LDAA $3B 				;
		ANDA #$00BF 			;
		STAA $3B 				;
		LDAA LC677 				;
		BRA LE8D4 				;
LE8B3: 	LDAA #$0082 			;
		BRA LE8C2 				;
LE8B7: 	LDAA LC632 				;
		ASLA 					;
		SUBA $010E 				;
		BLS LE8CB 				;
								; ... else
		LDAA #$02 				;
LE8C2: 	STAA $0101 				;
		ORAB #$08 				; b3
		ANDB #$EF 				; 1110 1111
		BRA LE8E0 				;
LE8CB: 	LDAA $34 				;
		ORAA #$08 				; SET b3
		STAA $34 				;
		LDAA LC676 				;
LE8D4: 	STAA $010B 				; DISABLE PID CLD LP TIMER
LE8D7: 	ANDB #$F7 				; 1111 0111
		STAB $F2 				; IAC Cnt'l Word
		CLR $010E 				;
		BRA LE8E3 				;
LE8E0: 	STAB $F2 				; IAC Cnt'l Word
LE8E2: 	RTS 					;
								;--------------------------------------------------
								;--------------------------------------------------
								;
								;
								;
								;--------------------------------------------------
LE8E3: 	LDAA $F3 				;
		BITA #$20 				;
		BNE LE8E2 				;
								; ... else
		LDAA $84 				; TPS T/F Ld Axies Var
		LDAB $3D 				;
		BITB #$10 				; b4
		BEQ LE8FD 				; BR IF NOT b4,
								; ... else
		LDAB $C5 				; Stall Save IAC Stps, (Sec*80)
		BEQ LE8FD 				;
								; ... else
		DECB 					;
		STAB $C5 				; Stall Save IAC Stps, (Sec*80)
		ADDA LC603 				; 3.125% TPS Cut off STALL SAVER
								; TPS DEFAULT
		BCS LE911 				;
								; ... else
LE8FD: 	LDAB $43 				;
		BITB #$02 				; b1
		BEQ LE908 				; BR IF NOT b1
								; ... else
		ADDA LC60A 				; 4% TPS, DFCO T/F TPS Default
		BRA LE90F 				;
LE908: 	LDAB $3E 				;
		BPL LE913 				;
								; ... else
		ADDA LC5F7 				; 0d, Decel Enlean T/F Default
LE90F: 	BCC LE913 				;
								; ... else
LE911: 	LDAA #$FF 				;
LE913: 	LDAB $F2 				; IAC Cnt'l Word
		CMPA $1D,X 				;
		BHI LE936 				;
								; ... else
		LDAA $85 				; Old Val Fm TPS Tbl
		CMPA $1D,X 				;
		BLS LE924 				;
								; ... else
		LDAA $46,X 				;
		STAA $010B 				; DISABLE PID CLD LP TIMER
LE924: 	CLRA 					;
		BITB #$50 				; b4 & b6
		BNE LE944 				; BR IF NOT b4 & b6
								; ... else
								;
								; CK PWR STEERING
								;
		PSHB 					;
		LDAB $37 				; MCU INPUT STATUS WD
		BITB #$08 				; b3, PWR STEER SW BY HI PRESSURE
		PULB 					;
		BNE LE944 				; BR IF b3, PWR STEER SW
								;
								; TPS NOISE ELEM, (DON'T MOD A/C COMPENSATION
								;
								; ... else
		TST $0105 				; CK IF TF IS DONE
		BEQ LE995 				; BR IF NOT DONE
								; ... else
LE936: 	LDAB $34,X 				; TF GAIN TO B Reg
		MUL 					; STEP CMD = TPS * TF GAIN
		ASLD 					; x2
		BCC LE93E 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VAL
								;
								; CK IF STEP CMG GT TF MAX
LE93E: 	LDAB $35,X 				;
		CBA 					;
		BLS LE944 				; BR IF SEEP CMD IS OK
								; ... else
		TBA 					;
								;
								; CK IF PWR STEER PRESS HI
								;
LE944: 	LDAB $37 				; MCU INPUT STATUS WD
		BITB #$08 				; b3, PWR STEER SW
		BEQ LE961 				; BR IF NOT b3,
								; ... else
		ADDA $52,X 				; ADD PWR STEER ANTICIPATE
		BCC LE950 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VAL
LE950: 	LDAB $F2 				; IAC Cnt'l Word
		BMI LE95B 				; BR IF
								; ... else
		ADDA LC681 				; 0 IAC STPS, Pwr Steer Anticipate STEPS
		BCC LE95B 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VALUE
LE95B: 	ORAB #$40 				; SET b6, LD IS BEING CONTROLED
		ANDB #$FE 				; 1111 1110
		BRA LE98A 				;
LE961: 	LDAB $F2 				; IAC Cnt'l Word, A/C REQUEST
		BMI LE984 				;
								; ... else
		LDAB $2D 				; Idle Spd A/C Antic
		CMPB $38,X 				;
		BHI LE96F 				;
								; ... else
		CMPB $39,X 				;
		BCC LE97A 				;
								; ... else
LE96F: 	LDAB LC665 				;
		STAB $2D 				; Idle Spd A/C Antic
		LDAB $3D 				;
		ORAB #$40 				; SET b6
		STAB $3D 				;
LE97A: 	LDAB $F2 				; IAC Cnt'l Word
		ADDA $2D 				;
		BCC LE98A 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VAL
		BRA LE98A 				;
LE984: 	BITB #$40 				; b6, LOAD IS BEING CNT'ED
		BEQ LE98A 				; BR IF NOT b6
								; ... else
		ORAB #$20 				; SET b5, LOAD IS BEING REMOVED
LE98A: 	STAB $F2 				; IAC Cnt'l Word 1
		LDAB $F3 				; IAC Cnt'l Word 2, PK/NEUT
		BPL LE997 				; BR IF IN DRIVE
								; ... else
		LDAB $36,X 				; PK/NEUT MULT
		MUL 					;
		BRA LE9A4 				;
LE995: 	BRA LE9E3 				;
LE997: 	LDAB $67 				; MPH/(16/5)
		CMPB $20,X 				;
		BLS LE9A4 				;
								; ... else
		ADDA LC683 				; 5 Steps added to T.F. in drive & MOVING
		BCC LE9A4 				; BR IF NO OVERFLOW
								; ... else
		LDAA #255 				; USE MAX VALUE
LE9A4: 	LDAB $F2 				; AC Cnt'l Word
		ORAB #$10 				; b4
		SUBA $0105 				;
		BCS LE9B3 				;
								; ... else
		BPL LE9C5 				;
								; ... else
		LDAA #127 				;
		BRA LE9C5 				;
LE9B3: 	BMI LE9B7 				;
								; ... else
		LDAA #$0081 			;
LE9B7: 	NEGA 					;
		PSHB 					;
		CMPA #$03 				;
		BCC LE9C2 				;
								; ... else
		LDAB $46,X 				;
		STAB $010B 				; DISABLE PID CLD LP TIMER
LE9C2: 	ORAA #$80 				; b6
		PULB 					;
LE9C5: 	STAA $0101 				;
		ASLA 					;
		BEQ LE9D7 				;
								; ... else
		BCS LEA25 				; EXIT
								; ... else
		LDAA $2C 				; IAC Present Posit
		CMPA $4D,X 				;
		BCS LEA25 				;
								; ... else
		ANDB #$EF 				; 1110 1111
		BRA LE9E3 				;
LE9D7: 	ANDB #$CF 				; 1100 1111
								; ... else
		BPL LE9E3 				;
								; ... else
		LDAA $37 				;
		BITA #$08 				;
		BNE LE9E3 				;
								; ... else
		ANDB #$BE 				;
								;
								;--------------------------------------------------
								; CK IF IN MODE 4 <---- *****
								; SET IAC MODE
								;
								; ECM TYPE $E6
								;
								; MODE 4 CNT'L WD ... HAS FLAGS
								;--------------------------------------------------
LE9E3: 	BRCLR $35 $08 LE9F9 	; BR IF NOT b3, MODE 4
								; ... else
		LDAA $0154 				;
		BITA #$02 				; b1
		BEQ LE9F9 				; BR IF NOT b1
								; ... else
		LDAA $0155 				; GET ALDL
		ANDB #$FB 				; CLR b2
		BITA #$02 				; b1
		BNE LEA25 				; BR IF b1 GO SAVE IAC CNT'L WD
								; AND EXIT via RTS
								; ... else
		ORAB #$04 				; SET b2
								;--------------------------------------------------
								;--------------------------------------------------
LE9F9: 	BITB #$04 				; b2
		BNE LEA25 				; B IF NOT b2
								; ... else
		LDAA $67 				; MPH/(16/5)
		BNE LEA21 				; BR IF NZ, MOVING
								; ... else
		LDAA $84 				; TPS T/F Ld Axies Var
		CMPA $1D,X 				;
		BCC LEA21 				;
								; ... else
		LDAA $58 				; RPM/12.5
		CMPA $0102 				;
		BLS LEA14 				;
								; ... else
		LDAA #$0082 			;
		ORAB #$0002 			;
		BRA LEA22 				;
LEA14: 	LDAA #$02 				;
		BITB #$02 				;
		BEQ LEA22 				;
								; ... else
		ORAB #$04 				;
		LDAA $45,X 				;
		STAA $010B 				; DISABLE PID CLD LP TIMER
LEA21: 	CLRA 					;
LEA22: 	STAA $0101 				;
								;--------------------------------------------------
LEA25: 	STAB $F2 				; IAC Cnt'l Word
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* MAJOR LOOP SUBROUTINE SEG 3, Misc 100 Msec
		;*
		;*
		;*******************************************************
LEA28: 	LDAB $35 				;
		ANDB #$CF 				; Clr b4 & b5 (DAIG MD & ALDL MODE)
								;
								; READ A/D AND CK DIAGNOSTIC PIN B VOLTAGE....
								;
		LDAA #$70 				; A/D Ch 7, (Start/Run Pwr)
		JSR LF1BE 				; To A/D subroutine
		CMPA #40 				; 800 Mv
		BCS LEA45 				; BR IF A/D LT 800 Mv, (GROUND'ED TST)
								; ... else
		CMPA #100 				; 2.00 VDV
		BCS LEA47 				; BR IF A/D LT 2.00 VDC
								; ... else
		CMPA #152 				; 3.04 VDV
		BCC LEA47 				; BR IF A/D GT 3.05 VDC, (ROAD TST MODE)
								; ... else
		BRSET $46 $08 LEA47 	; BR If 8192 Mode in locked Mode
								; ... else
		ORAB #$20 				; SET b5
		BRA LEA47 				;
LEA45: 	ORAB #$10 				; SET b4, (DIAG SW IN DIAG MODE)
								; (GROUND'ED DIAG PIN)
LEA47: 	STAB $35 				;
								;--------------------------------------------------
								;--------------------------------------------------
		LDAA $01B2 				;
		BEQ LEA54 				;
								; ... else
		LDAA $33 				; MINOR LOOP MD WD 1
		ORAA #$04 				; SET b2, STACK OVER WRITE
		STAA $33 				;
LEA54: 	CLRA 					;
		CLRB 					;
		STD $3FCE 				; ??? DELAY
		LDAA $34 				;
		BMI LEA80 				;
								; ... else
		LDAA $43 				;
		ANDA #$D5 				; CLEAR b1 b3 & b5
		STAA $43 				;
								;--------------------------------------------------
		LDAA $5B 				; COOLANT
		STAA $5F 				; STARTUP COOLANT
		BRSET $01 $40 LEA76 	; BR IF b6
								; ... else
		CMPA LC02C 				; Hot Restart Limit, (129F)
		BCS LEA76 				;
								; ... else
		LDAA $01 				; .
		ORAA #$40 				; SET b6
		STAA $01 				;
								;--------------------------------------------------
LEA76: 	CLRB 					;
		LDAA LC3A0 				; o2 filter init val fm ROM
		STD $6F 				;
		STAA $73 				;
		BRA LEA8F 				;
								;--------------------------------------------------
LEA80: 	BCLR $02 $10 			; CLR b4, SKIP IAC MOTOR RESET
								;--------------------------------------------------
		LDX $73 				;
		LDAA $6F 				;
		LDAB LC39F 				; 100 Msec o2 Coef
		JSR LF250 				;
		STD $73 				;
LEA8F: 	LDAA #$8F 				; 1000 11111
		STAA $4003 				;
		RTS
		;*******************************************************
		;*******************************************************
		;* MAJOR LOOP SUBROUTINE, SEG 7
		;*
		;* LK UP Coolant var's
		;*
		;* BUA TYPE 32 ECM
		;*******************************************************
								;
								; LK UP Rich/Lean Offset Vs Coolant Temp
								;
								; TBL = BIN VAL
								;
LEA95: 	LDAA $5B 				; COOLANT
		CMPA #208 				; 115c
		BLS LEA9D 				; BR IF LT 115c
								; ... else
		LDAA #208 				; USE 115c MAX FOR LK UP
LEA9D: 	STAA $5E 				; LMT'ED COOLANT
		LDX #$C5BB 				; Rich/Lean Offset Tbl
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		STAA $D1 				; Lean Offset, (if Cool) fm Tbl
								;---------------------------------------
								;-----------------------------------------
								; LK UP Number of ASYNC Pulses vs Coolant
								;
								;
								;-----------------------------------------
		LDAA $5B 				; COOLANT
		LDX #$C31D 				; Number of ASYNC Pulses Tbl
		JSR LF2BF 				; 2d Lk Up W/ Interp
		STAA $D7 				; Num of Async Pulses to do
								;--------------------------------------------------
								;--------------------------------------------------
								; Async Factor vs COOLANT
								;
								; TBL = Factor * 128
								;--------------------------------------------------
		LDAA $5B 				; COOLANT
		LDX #$C327 				; Async Factor Tbl
		JSR LF2BF 				; 2d Lk Up W/ Interp
		STAA $D8 				; Cool Fact for Async Pulses
								;--------------------------------------------------
								;
								; ANALOG MAF BURN OFF ROUTINE
								;
		LDAA $5B 				; COOLANT
		LDX #$C613 				; Pwr Enrichment Air/Fuel Tbl
		JSR LF2BF 				; 2d Lk Up W/ Interp
		STAA $D0 				; LU AFR % Chg vs Cool Temp
								;------------------------------------------
								;
								; ANALOG MAF BURN OFF ROUTINE
								;
		BRCLR $3B $02 LEAD5 	; BR IF NOT b1, BR IF START NOT ENAB'LED
								; ... else
		BRSET $33 $32 LEAD5 	; BR IF b1 & b4 & b5, (1 = IGN OFF, ENAB
								; MAF B/O)
								; ... else,
		INC $0472 				; START TIME
		BNE LEAD5 				; BR IF NZ,
								; ... else
		DEC $0472 				;
LEAD5: 	BRCLR $33 $10 LEB2C 	; BR IF NOT b4, (1 = IGN NOT ON)
								; ... else
		BRCLR $3E $40 LEB2C 	; BR IF NOT b6, IF NO ENABLED
								; ... else
		BRCLR $B8 $FF LEAE6 	; BR IF NOT $FF, BR IG DELAY = 0
								; ... else
		DEC $B8 				; DECR CNT'R
		BRA LEB2C 				; EXIT B/O
LEAE6: 	BRCLR $B7 $FF LEB2C 	; BR IF NOT $FF, BR IF B/O TOME = 0
								; ... else
		DEC $B7 				; DECR CNT'R
		LDX #$DFFF 				; 57,343
		STX $3FDA 				; Mass Air B.U. PW TMR
								;
								; ANALOG MAF BURN OFF ERR 36
								;
		BRSET $3F $40 LEB39 	; BR IF b6
								; ... else
		LDAA $011F 				; MAF Burn off delay tmr
		CMPA LC221 				; 300 msec Dly prior to Burn off
		BCC LEB05 				; BR IF TIME LT 300 msec
								; ... else
		INCA 					; INCR MAF Burn off delay tmr
		STAA $011F 				; SAVE NEW MAF Burn off delay tmr
		BRA LEB39 				;
								;---------------------------
								; CK ERR 36 Params
								; MAF Burn off Diag
								;---------------------------
LEB05: 	LDAA #$A0 				; A/D Ch A, (Mass Air Sensor Ch)
		JSR LF1BE 				; To A/D subroutine
		CMPA LC223 				; Fail B/O test if HLM A/D > 1.90 VDC
		BHI LEB19 				; BR IF MAF A/D VAL LT 1.90 VDC
								; ... else
		CMPA LC224 				; Fail B/O test if A/D LT 400 mvdc
		BCS LEB19 				; BR IF MAF VAL L.T. 20
								; ... else
		BCLR $03 $40 			; CLR b6, ERR 36, BO FAILED
		BRA LEB39 				; EXIT via RTS
LEB19: 	LDAA $0120 				; ERR 36 COUNTER
		CMPA LC222 				; 6 fails req for ERR 36
		BCC LEB27 				; BR IF ERR EVENTS L.T. 6
								; ... else
		INCA 					; INCR ERR 36 COUNT
		STAA $0120 				; SAVE NEW ERR 36 COUNTER
		BRA LEB39 				;
LEB27: 	BSET $03 $40 			; SET b6, ERR 36, MAF BURN OFF FAIL
		BRA LEB39 				; EXIT via RTS
LEB2C: 	LDX #$D000 				; 53,248
		STX $3FDA 				; Mass Air B.U. PW TMR
		BRCLR $03 $40 LEB39 	; BR IF b6, ERR 36, MAF BURN OFF FAIL
								; ... else
		BSET $4E $80 			; SET b7, NEW ERR 2
LEB39: 	RTS 					;
		;*******************************************************
		;*******************************************************
		;* MAJOR LOOP SUBROUTINE
		;*
		;* SEG A, Lk UP Man Air temp Var's
		;*
		;*******************************************************
LEB3A: 	LDAB $00 				; MJR LOOP CNTR
		BITB #$10 				; b4
		BNE LEB5B 				; BR IF b4
								; ... else
								;----------------------------------------
								; LK UP KNOCK % Recovery rate vs RPM
								;
								; RECOVERY/SEC * 256/500
								;----------------------------------------
		LDAA $56 				; RPM/25
		LSRA 					; DIV BY 4
		LSRA 					;
		LDX #$C1C3 				; KNOCK PCT. RECOVERY RATE Tbl
		JSR LF2C6 				; 2d Lk Up, (No Offset)
								;
								; CALC KNOCK RETARD AS PROD OF
								; RATE AND KNOCK RETARD WITH MIN
								; VAL HELD TO ONE
								;
		LDAB $A5 				; Retard for knock
		MUL 					; RETARD * LK UP RESULT
		ADCA #0 				; ROUND
		NEGA 					;
		BNE LEB54 				; BR IF NZ
								; .. else
		LDAA #255 				; USE MAX VAL
LEB54: 	ADDA $A5 				; Retard for knock
		BCS LEB59 				;
								; ... else
		CLRA 					; CLR RETARD VALUE
LEB59: 	STAA $A5 				; Retard for knock
								;
								; CK IF ERR #43 ENABLED
								; (KNOCK FAIL)
								;
LEB5B: 	LDAA LC1D2 				; Mask For ERR flag 3
		BITA #$10 				; b4 ERR #42 KNOCK FAIL
		BEQ LEBB3 				; BE IF NOT b4
								; ... else
		LDAB $00 				; MJR LOOP COUNTER
		CMPB #26 				; CK IF TIME FOR 1 SEC LOGIC
		BNE LEB8F 				;
								; ... else
		LDAA $02 				; MDWD1
		BITA #$40 				; b6, ERR 43 TEST OVER THIS STARTUP
		BNE LEB8F 				; BR IF b6
								; ... else
		BITA #$01 				; b0, ERR 43 TEST IN WORK
		BNE LEB82 				; BR IF b0
								; ... else
		TST $B0 				; MAP Hi Cnt'r, (100Ms)
		BNE LEB8F 				;
								; ... else
		LDAB $63 				; Filtered ld value
		CMPB LC22D 				; LV 170
		BCS LEB8F 				; BR IF LD VAL LT 170, (DISABLE ERR 43)
								; ... else
		ORAA #$01 				; b0
		BRA LEB8D 				;
LEB82: 	ANDA #$FE 				; 1111 1110
		LDAB $5B 				; COOLANT
		CMPB LC22C 				; 90c MIN FOR ERR 43 ENABLE
		BLS LEB8D 				; BR IF COOL LT 90c, (DISABLE ERR 43)
								; ... else
		ORAA #$C0 				; SET b6 & b7, ERR 34 OVER & FAILED
LEB8D: 	STAA $02 				;
LEB8F: 	LDAA $A4 				;
		INCA 					; INCR
		CMPA #39 				;
		BEQ LEB9A 				; BR IF
								; ... else
		STAA $A4 				; SAVE NEW
		BRA LEBB3 				;
LEB9A: 	CLR $A4 				;
		LDD $3FCA 				; Counter #3, 16.5Khz
		PSHA 					;
		LDAB $3B 				;
		ANDB #$DF 				; 1101 1111
		SUBA $A3 				;
		CMPA LC229 				; If KNOCK low time > 3.67 Sec,Enab
								; ERR #43
		BCS LEBAE 				; BR IF
								; ... else
		ORAB #$20 				; b5
LEBAE: 	STAB $3B 				;
		PULA 					;
		STAA $A3 				;
								;--------------------------------------------------
								;-------------------------------------------
								; READ MAN AIR TEMP AND CK ERROR 23/25
								;
								;-------------------------------------------
LEBB3: 	LDAA #$80 				; A/D Ch 8, MAT
		JSR LF1BE 				; To A/D subroutine
		COMA 					; INVERT TEMPERATURE
		STAA $012B 				; A/D MAT VALUE
		STAA $60 				; MAT
								;
								; ERR 23 MAT LOW
								;
		LDX $1A 				; ENG RUN TIME (Sec)
		CPX LC1EE 				; 120 SEC'S MIN RUN TIME FOR ERR 23/25
		BLS LEC22 				; BR IF RUN TIME GT 120 SEC'S (EXIT)
								; ... else
		LDAB $65 				; MPH/1
		LDAA $60 				; MAT
		CMPA LC1EC 				; 4d MAX MAT TO ENABLE ERR 23
		BCC LEBDF 				; BR IF MAT GT 4d (CLR ERR 23 THIS PASS)
								; ... else
		LDAA $AC 				; Lo MAT Err #23 TIMER
		CMPA LC1ED 				; 12 Sec req for ERR 23
		BHI LEBE7 				; BR IF TIMER LT 12 Sec
								; ... else
		CMPB LC1F0 				; 1 MPH MIN TO ENABLE ERR 23/25
		BHI LEBE2 				; BR IF Vss LT 1 MPH, (CLR ERR 23)
								; ... else
		INC $AC 				; INCR Lo MAT Err 23 TMR
		BRA LEBEA 				; TO HI MAT ROUTINE
LEBDF: 	BCLR $43 $10 			; CLR ERR 23 THIS PASS FLAG
LEBE2: 	CLR $AC 				; CLR Lo MAT Err 23
		BRA LEBEA 				; TO HI MAT ROUTINE
LEBE7: 	BSET $43 $10 			; SET ERR 23 THIS PASS FLAG
								;--------------------------------------------------
								;
								; ERR 25, HIGH MAT
								;
LEBEA: 	LDAA $60 				; MAT
		CMPA LC1F8 				; If MAT > 243 then Enable ERR 25
		BCS LEC02 				; BR IF MAT LT 243d
								; ... else
		LDAA $AD 				; Hi MAT TIMER, Err 25
		CMPA LC1F9 				; 12 Sec's req for ERR 25
		BHI LEC0A 				;
								; ... else
		CMPB LC1F0 				; 1 MPH MIN TO ENABLE ERR 23/25
		BLS LEC05 				; BR IF Vss GT 1 MPH
								; ... else
		INC $AD 				; BUMP UP Hi MAT TIMER, Err 25
		BRA LEC0D 				; EXIT ERR 25 ROUTINE
LEC02: 	BCLR $40 $02 			; CLR b1,
LEC05: 	CLR $AD 				; CLEAR Hi MAT TIMER, Err 25
		BRA LEC0D 				; EXIT ERR 25 ROUTINE
LEC0A: 	BSET $40 $02 			; SET ERR 25 THIS PASS
LEC0D: 	BRSET $43 $10 LEC1A 	; BR IF B4, ERR 23 THIS PASS
								; ... else
		BRCLR $40 $02 LEC22 	; BR IF NOT b1, NO ERR 24, EXIT via RTS
								; ... else
		BSET $4D $40 			; SET b6, ERR 25 FLAG
		BRA LEC1D 				; BR TO SET DEFULT MAT VALUE
LEC1A: 	BSET $4C $01 			; SET b0, ERR 23 FLAG
LEC1D: 	LDAA LC1F1 				; 29d, If ERR Use as Default MAT
		STAA $60 				; MAT
LEC22: 	RTS 					;
		;*******************************************************
		;*******************************************************
		;* MAJOR LOOP SUBROUTINE SEG F
		;*
		;* Fuel/Air Major Lp
		;*
		;*******************************************************
LEC23: 	LDAA $00 				; MJR LOOP CNTR
		BITA #$10 				;
		BNE LEC72 				; BYPASS 200 MSEC STUFF ...
								;
								; 200 MESC ROUTINE
								;
		LDX #$001C 				; POINT TO BLM CELLS
		LDAA $C1 				; CELL ZERO TMER
		INCA 					; INCR TMR
		BEQ LEC33 				; BR IF Z
								; ... else
		STAA $C1 				; NEW CELL ZERO TMER VALUE
								;
								; CK IF BL ENABLED ??
								; ($44 b1)
								;
LEC33: 	LDAB $44 				;
		BITB #$02 				; b1
		BEQ LEC71 				; BR IF BL NOT ENABLED
								; ... else
								;
								; CK COOLANT FOR SAM ENABLE
								; (88 - 100 C)
								;
		LDAB $5B 				; COOLANT
		CMPB LC5ED 				; If Temp < 88c Then Skip SAM Update
		BCS LEC71 				; BR IF COOL LT 88c
								; ... else
		CMPB LC5EE 				; If Temp > 100c Then Skip SAM Update
		BCC LEC71 				; BR IF COOL LT 100c
								; ... else
								;
								; CK FOR IDLE/NON IDLE CELLS
								;
		LDAB $BF 				; BLM (cells 0-7)
		ABX 					; ADD CELL NUMBER TO ADDRESS POINTER
		LDAA 0,X 				; GET CELL VALUE
		CMPB LC5EA 				; 0, Sam Cell A Number, Idle
		BEQ LEC60 				; BR IF CELL 0
								; ... else
		CMPB LC5EB 				; 9, SAM Cell B Number, Non Idle
		BNE LEC71 				; BR IF NOT CELL 9
								; ... else
		LDX $0C 				; SAM Cell B
		LDAB LC5F0 				; 24d, SAM B Filter Coef.
		JSR LF250 				; DO LAG FILTER
		STD $0C 				; SAM Cell B
		BRA LEC71 				; EXIT VIA RTS
LEC60: 	LDAB $C1 				; CELL ZERO TMR
		CMPB LC5EC 				; Stop Cell O update when time up
								; 15 SEC'S
		BCC LEC71 				; BR IF TIME LT 15 SEC'S
								; ... else
		LDX $0A 				; Err #5 Flg Word
		LDAB LC5EF 				; SAM A Filter Coef.
		JSR LF250 				; DO LAG FILTER
		STD $0A 				;Err #5 Flg Word
LEC71: 	RTS 					;
								;------------------------------------------------
								;-------------------------------------------------
								; VOLTAGE COMP, COOL or MAT
								; LK UP DECELL & ACCEL Enr Cool Factor vs COOL
								;
								;--------------------------------------------------
LEC72: 	LDAA $5B 				; COOLANT
		LDX #$C343 				; COOL MULT Tbl,
		JSR LF2BF 				; 2d Lk Up W/ Interp
		STAA $E7 				; Accel Enr Cool Factor
		LDAB $34 				;
		BMI LECC2 				;
								; ... else
		LDAB $01 				; NON VOL RAM Mode word
		BITB #$08 				; b3
		BNE LECC2 				;
		;******************************************
		;* AFR MAJOR LOOP ROUTINE
		;*
		;*******************************************
								;--------------------------------------------------
								; LK UP START UP ENRICH vs COOL
								;
								;
								; TBL = 2.56 * PCT CHG
								;--------------------------------------------------
		LDAA $5E 				; FILT COOL TEMP
		LDX #$C3E4 				; START UP ENRICH
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		CLRB 					;
		STD $0E 				; A/F Ratio time out
								;--------------------------------------------------
								;--------------------------------------------------
								; START UP SPARK ADVANCE vs START UP COOLANT TEMP
								;
								; TBL = SA deg * (256/90)
								;--------------------------------------------------
		LDX #$C17A 				; START UP SPARK TBL
		LDAA $5F 				; START UP COOL
		CMPA #208 				; CK MAX LK UP VAL, 115c
		BCS LEC9C 				;
		LDAA #208 				; USE 115c MAX FOR LK UP
LEC9C: 	PSHA 					;
		JSR LF2C6 				; Call 2d Lk Up, (No Offset)
		CLRB 					;
		STD $13 				; Spark Adv Time out
								;--------------------------------------------------
								;-----------------------------------------
								; LK UP START UP SPK ADV DECAY vs COOLANT
								; Table value = INJECTS
								;
								; TBL = 2.844 * DEG SPK
								;-----------------------------------------
		PULA 					; Lk Up Var
		LDX #$C18A 				; Tbl Addr,
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		LDX #$C188 				; Mult for Start Up Spark adv
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		STD $15 				; Spark Decay Delay
								;--------------------------------------------------
								;-----------------------------------------
								; LK UP START UP ENR DELAY DECAY vs COOLANT
								; Tbl Val = Num of Injects
								;-----------------------------------------
		LDX #$C3F4 				; START UP ENR DELAY DECAY TBL
		LDAA $5E 				; COOL 116
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		LDX #$C3F2 				; Scale factor for Table, (2)
		JSR LF266 				; MUL 8 X 16 SUROUTINE
		STD $10 				;
								;--------------------------------------------------
								;-----------------------------------------------
								; LK UP, Open Loop A/F % Change vs Cool Temp
								; TBL = 2.56 * %CHG TO AFR
								;-----------------------------------------------
LECC2: 	LDX #$C41F 				; Opn Lp A/F TBL
		LDAA $5E 				; COOL 116
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		STAA $CD 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; LK UP Accel Enrich Decay Factor vs Coolant Temp.
								;
								; Table Value + Diff Ld Val Pct'age Per Inject
								;
								; Tbl Val = %Chg * (256/100)
								;--------------------------------------------------
		LDX #$C34D 				; Accel Enrich Decay Factor TBL
		LDAA $5B 				; COOLANT
		JSR LF2BF 				; 2d Lk Up W/ Interp
		STAA $E3 				;
								;--------------------------------------------------
								;-----------------------------------------------
								; LK UP WARM PARK IAC POSIT vs COOLANT
								;
								;-----------------------------------------------
		LDX #$C690 				; Tbl Addr,
		LDAA $5B 				; COOLANT
		JSR LF2BF 				; Call 2d Lk Up W/ Interp
		STAA $010C 				; SAVE PARK POSIT
		CMPA $0109 				;
		BCC LECE9 				; BR IF TBL VAL LT CURRENT POSIT
								; ... else
		STAA $0109 				;
LECE9: 	RTS 					;
		;*******************************************************
		;*******************************************************
		;* MJR LOOP SUBROUTINE SEG 5
		;*
		;* Clsd Lp QUAL'S, JP TO A/C & Cool FAN
		;*
		;*******************************************************
								;
								; CK CLOSED LOOP QUAL'S
								;
LECEA: 	LDAA $00 				; MJR LP CNTR
		BITA #$10 				; b4
		BNE LECF6 				; BR IF b4
								; ... else
		JSR LDF4A 				; GO DO FAN & A/C
		JMP LDF9D 				; A/C CLUTCH
								;
								; CK IF ENG RUNNING
								;
LECF6: 	LDAA $34 				;
		BMI LECFE 				; BR IF RUNNING
								; ... else
		BRCLR $01 $08 LED77 	; BR IF NOT b3, NOT RUNNING,
								; PROPER SHUT DN
								; ... else
								;
								; CK IF ERR 44/45
								;
LECFE: 	LDAA $41 				;
		BITA #$02 				; b1, ERR 44/45
		BEQ LED09 				; BR IF NOT b1
								; ... else
		CLR $BE 				; CLR Clsd Lp o2 Not Rdy timer,
								; (200Ms inc) & FORCE OPEN LOOP
		BRA LED77 				;
								;
								; CK IF ALDL or DIAG MODE
								;
LED09: 	LDAA $35 				;
		ANDA #$30 				; MASK FOR b4 & b5, (ALDL/DIAG MODE)
		BNE LED48 				; BR IF b4 & b5
								; ... else
								;----------------------------------------------------
								; CK IF IN ALDL MODE 4 <----*****
								; MODE WD ... and MD WD ...
								;
								;----------------------------------------------------
		BRCLR $35 $08 LED24 	; BR IF NOT b3, (NOT IN 8192 MODE 4)
								; .... else
		LDX #$0154 				; INDEX ALDL MD WD ..
		BRCLR 0,X $01 LED20 	; BR IF NOT b0, IF COMMANDING FUEL <***
								; ... else
		BRCLR 1,X $01 LED77 	; BR IF NOT b0, IF COMMANDING OPEN LOOP <*
								; ... else
		BRA LED48 				; BR TO CLOSED LOOP
LED20: 	BRSET 4,X $04 LED77 	; BR IF b2, IF CONTROLLING AFR <*****
								; .... else
								;----------------------------------------------------
								;
								; CK IF CLSD LOOP TIMER IS TIMED OUT ??
								;
LED24: 	LDAB $01 				;
		BITB #$02 				;
		BNE LED48 				;
								; ... else
		LDD $1A 				; Eng Run Time (sec)
		LSRD 					; SACE BY 2 FOR TMR CK
		LDX #$C556 				; 75 SEC, Cold Closed Lp Timer
		LDAA $5F 				; COOL
		CMPA LC553 				; Use Cold C Loop timer if Cool <= 15c
		BLS LED3E 				; BR IF COOL GT 16c
								; ... else
		INX 					; LC557, Warm Closed Lp Timer
		CMPA LC552 				; Use Hot C Loop timer If Cool T >= 70c
		BLS LED3E 				; BR IF COOL GT 70c
								; ... else
		INX 					; LC558, Hot Closed Lp Timer
LED3E: 	CMPB 0,X 				;
		BCS LED77 				;
								; ... else
		LDAA $01 				;
		ORAA #$02 				; SET b1,
		STAA $01 				;
LED48: 	LDAA $5B 				; COOLANT
		CMPA LC551 				; 41c, Min Temp for Closed Loop
		BLS LED77 				; BR IF COOL GT 41c
								; ... else
		LDAA $33 				; MINOR LOOP MD WD 1
		BPL LED59 				;
								; ... else
		LDAA $4B 				; HU SPK FLAGS
		ANDA #$30 				; 0011 0000
		BNE LED77 				;
								; ... else
LED59: 	BSET $33 $20 			; SET b5, (ENABLE MAF B/O)
		LDAB $01 				;
								;
								; CK Cl/Lp o2 Not Ready timer
								;
		LDAA $BE 				; Cl/Lp o2 NR timer, (200Ms inc)
		CMPA LC55B 				; 10 Sec for o2 Max/Min
		BCC LED6A 				; BR IF TIME GT 10 SEC'S
								; ... else
		INCA 					; INCR TMR
		STAA $BE 				; Cl/Lp o2 NR timer, (200Ms inc)
		BRA LED6E 				;
LED6A: 	ANDB #$FE 				; CLR b0
		STAB $01 				;
LED6E: 	RORB 					;
		BCC LED77 				;
								; ... else
		LDAB $44 				;
		ORAB #$80 				; SET b7
		BRA LED7B 				;
LED77: 	LDAB $44 				;
		ANDB #$7F 				; CLR b7
LED7B: 	STAB $44 				;
		BPL LED9B 				;
								; ... else
		;************************************
		;* BL STORE QUAL'S
		;*
		;************************************
		LDAA $5B 				; GET CURRENT COOLANT
		CMPA LC5DD 				; If Coolant 50c <= Disable BLM update
		BLS LED9B 				; BR IF COOL GT 50c
								; ... else
		CMPA LC5DE 				; If Coolant >= 140c Disable BLM update
		BHI LED9B 				; BR IF COOL LT 140c
								; ... else
		LDX $CE 				; Total AFR Value
		CPX LC3CB 				; STOCH RATIO
		BNE LED9B 				; BR IF AFR IS NOT AT STOCH
								; ... else
								;
								; QUAL'S MET, ENABLE BL STORE
								;
		ORAB #$02 				; SET b1
		LDAA $63 				;
		CMPA LC5DF 				; 0, Min ld val for BLM
		BCC LED9D 				;
								; ... else
								;
								; QUAL'S NOT MET, DISABLE BL STORE
								;
LED9B: 	ANDB #$FD 				; CLR b1
LED9D: 	STAB $44 				;
		JSR LDF9D 				; TO A/C ROUTINE
		RTS
		;*******************************************************
		;*******************************************************
		;* MJR LOOP SEG 1, Output Bit Sig', (TCC, CCP etc)
		;*
		;*
		;*
		;*******************************************************
LEDA3: 	LDAA $7E 				; Batt Volts Value, (A/D)
		CMPA #171 				; 17.1 VDC
		BCS LEDBA 				; BR IF BATT V LT 17.1 VDC
								; ... else
		BSET $3E $10 			; Set Flg to Disable Burn off
		BRCLR $35 $40 LEDB5 	; BR IF NOT b6, (HI V BATTERY)
								; ... else
		BSET $4F $40 			; if yes set flg (b6)
		BRA LEDC5 				;
LEDB5: 	BSET $35 $40 			; SET b6, (HI V BATTERY)
		BRA LEDBD 				;
LEDBA: 	BCLR $35 $40 			; CLR b6, (HI V BATTERY)                                       
LEDBD: 	LDX $34 				; Ck Eng Run & Diag Modes                                      
		BMI LEDFD 				; Go if Eng Running                                            
								; ... else                                                     
		LDX $41 				; Get #rd Diag word                                            
		BMI LEDCA 				; Go if In diag                                                
								; ... else
LEDC5: 	LDX #$D000 				;                                                              
		BRA LEDCD 				;                                                              
								;-----------------------------------                           
								; In Diag & Eng not running                                    
								;------------------------------------                          
LEDCA: 	LDX #$DFFF 				;                                                              
LEDCD: 	STX $3FD2 				; EGR PW Out Cnt'r
		JSR LF3B5 				; 11 Usec Delay                                                
		STX $3FD6 				; TCC PW Out Cnt'r                                             
		JSR LF3B5 				; 11 Usec Delay                                                
		STX $3FD8 				; CCP PW Out Cnt'r                                             
		JSR LF3B5 				; 11 Usec Delay                                                
		STX $3FCC 				; PW Mod cnt'r, (P8) ARC                                       
		JSR LF3B5 				; 11 Usec Delay                                                
		STX $3FD4 				;PW Mod Cnt'r, Acell Enrich                                    
		CPX #$D000 				;                                                              
		BNE LEDF4 				;                                                              
								; ... else                                                     
		LDAA $4004 				;Par I/O CSR                                                   
		ANDA #$FD			    ;                                                              
		BRA LEDF9 				;                                                              
LEDF4: 	LDAA $4004 				;Par I/O CSR                                                   
		ORAA #2 				;0000 0010                                                     
LEDF9: 	STAA $4004 				;Par I/O CSR                                                   
		RTS                     ;                                                              
								;--------------------                                          
								; Air Inj Cnt'l                                                
								;--------------------                                          
LEDFD: 	LDX #$DFFF 				;                                                              
								;--------------------------------------------------            
								; CK IF IN ALDL MODE 4 <----**** LEE2A                         
								; Air Inj Cnt'l                                                
								;                                                              
								; CNT'L WD 5, b5, ENABLE AIR MOD                               
								; CNT'L WD 6, AIR INJ PWM VAL                                  
								;--------------------------------------------------            
		BRCLR $35 $08 LEE1B 	; BR IF NOT b3, MODE 4                                         
		LDAB $0156 				; CNT'L WD, b5                                                 
		BITB #$20 				; b5, ENABLE AIR MOD                                           
		BEQ LEE1B 				; BR IF NOT 1, (ENABLED)                                       
								; ... else                                                     
		LDAB $0157 				; CNT'L WD 6, AIR INJ PWM VAL                                  
		LDAA #$34 				;                                                              
		ASLD 					;                                                              
		ASLD 					;                                                              
		ORAB #03 				; b0 & b1                                                      
		PSHB 					;                                                              
		PSHA 					;                                                              
		PULX 					;                                                               
		LDAA $3E 				; Get Air Mode Wd.                                              
		BRA LEE24 				;                                                               
								;--------------------------------------------------             
LEE1B: 	LDAA $3E 				; GET AIR MODE Wd.                                              
		BITA #2 				; AIR INJ REQUESTED                                             
		BNE LEE24 				; iF YES Go TURN ON                                             
								; ... else                                                      
		LDX #$D000 				; IF NO...                                                      
LEE24: 	STX $3FCC 				; (MPU CSR), TURN OFF                                           
		LDX #$DFFF 				; Load 100% D.C.                                                
								;--------------------------------------------------             
								; CK IF IN ALDL MODE 4 <----**** EE2A:                          
								;                                                               
								; CNT'L WD 5, b6,
								; CNT'L WD 6, PWM VAL
								;--------------------------------------------------
		BRCLR $35 $08 LEE43 	; BR IF NOT b3, MODE 4                                          
								; ... else                                                      
		LDAB $0156 				; CNT'L WD 5, b6                                                
		BITB #$40 				; b6                                                            
		BEQ LEE43 				; BR IF NOT b6                                                  
								; ... esle                                                      
		LDAB $0157 				; CNT'L WD 6, PWM D.C.
		LDAA #$34 				;
		ASLD 					;
		ASLD 					;
		ORAB #$03 				;
		PSHB 					;
		PSHA 					;
		PULX 					;
		BRA LEE4F 				;
								;--------------------------------------------------
LEE43: 	LDAB LC015 				; Get Fuel/Air Mode Wd 2
		BEQ LEE4C 				; BR IF Z
								; ... else
		BITA #$04 				; b2 = NUM OF AIR VALVES
		BNE LEE4F 				; Go If Yes
								; ... else
LEE4C: 	LDX #$D000 				; CLR ENAB Bit
LEE4F: 	STX $3FD4 				; ENRICH PW CNT'R
								; - > Cont with A/C En/Dis
								;----------------------------
								; TCC and or A/C Clutch
								;----------------------------
		LDX #$DFFF 				;
								;
								; CK IF Ck IF A/C if not do TCC
								;
		LDAB LC017 				; AFR MD WD 4
								; b7 = Use TCC out to Cnt'l A/C Clutch
		BPL LEE6D 				;
								; ... else
								;--------------------------------------------------
								; CK IF IN ALDL MODE 4 <----****
								; MODE 4 MD WD .., b1
								; A/C ??
								;--------------------------------------------------
		BRCLR $35 $08 LEE65 	; BR IF NOT b3, MODE 4
								; ... else
		LDAA $0156 				; MODE 4 MD WD ..
		BITA #$02 				; b1
		BNE LEE84 				; BR IF b1
								; ... else
								;--------------------------------------------------
								;--------------------------------------------------
								; If A/C Then Set Up A/C Bit Invert
								;
								;--------------------------------------------------
LEE65: 	LDAB $34 				; Get Mode Wd #1
		ANDB #$20 				; 0010 0000
		EORB #$20 				; Invert
		BRA LEE96 				; To output Tst
LEE6D: 	LDAB LC014 				; Get Fuel/Air Mode Wd 1
		ASLB 					; Is TCC sig used as Shft Lamp
		BPL LEE79 				; Go If not
		BRCLR $35 $80 LEE98 	; BR IF NOT b7, OVERDRIVE ON
								; Turn off If Lamp out
		BRA LEE9B 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; CK IF IN ALDL MODE 4 <----****
								; MODE 4 MD WD .., b2
								;
								; TCC ??
								;-------------------------------------------------
LEE79: 	BRCLR $35 $08 LEE92 	; BR IG NOT b3, MODE 4
								; ... else
		LDAA $0156 				; MODE 4 MD WD ..
		BITA #$04 				; b2
		BEQ LEE92 				; BR IF NOT b2
								; ... else
								;--------------------------------------------------
								;--------------------------------------------------
								; BR HERE FROM ALDL MODE 4 ROUTINE
								;
								;
								;--------------------------------------------------
LEE84: 	LDAA #$34 				;
		LDAB $0157 				; ALDL MD WD ...
		ASLD 					;
		ASLD 					;
		ORAB #3 				;
		PSHB 					;
		PSHA 					;
		PULX 					;
		BRA LEE9B 				;
								;--------------------------------------------------
								;--------------------------------------------------
								; BR HERE FROM ALDL MODE 4 ROUTINE
								; TCC CNT'L
								;
								;--------------------------------------------------
LEE92: 	LDAB $37 				; Tst If TCC S/B locked
		BITB #$20 				; b5
LEE96: 	BNE LEE9B 				; BR IF b5, TCC LOCKEDs
								; ... else
LEE98: 	LDX #$D000 				; If Not, Clr Lock Bit
LEE9B: 	STX $3FD6 				; TCC PW Cnt'r
								;--------------------------------------------------
								;--------------------------------------------------
								; CCP Cnt'l
								;
								;
								;--------------------------------------------------
								;--------------------------------------------------
								; CK IF IN ALDL MODE 4 <----****
								; MODE 4 MD WD .., b3
								;
								; CCP DC
								;-------------------------------------------------
		BRCLR $35 $08 LEEAE 	; BR IF NOT b3, MODE 4
								; ... else
		LDAA $0156 				; MODE 4 MD WD .., b3
		BITA #8 				; b3
		BEQ LEEAE 				;
								; ... else
		LDAB $0157 				; Get Purge Dty Cyc
		BRA LEEB1 				;
								;-------------------------------------------------
LEEAE: 	LDAB $0113 				; CCP PW
LEEB1: 	LDAA #$34 				; $D0 when shfted 2 times
		COMB 					;
		ASLD 					; Mult Dty Cycl By 4 (32Hz)
		ASLD 					;
		ORAB #3 				; 0000 0011
		STD $3FD8 				; CCP PW Cnt'r
								;----------------------
								; Cool Fan Cnt'l
								;----------------------
								;--------------------------------------------------
								; CK IF IN ALDL MODE 4 <----****
								; MODE 4 MD WD .., b3
								;
								; COOL FAN
								;-------------------------------------------------
		BRCLR $35 $08 LEED3 	; BR IF NOT b3, MODE 4
								; ... else
		LDAA $0152 				; MODE 4 MD WD .., b3
		BITA #2 				; 0000 0010 , Bit 1
		BEQ LEED3 				; If Set
								; ... else
		LDAA $4004 				; Par I/O CSR
		LDAB $0153 				;
		ANDB #$02 				; 0000 0010, Bit 1
		ANDA #$FD 				; 1111 1101, Mask off bit 2
		ABA 					;
		BRA LEEE3 				;
								;-------------------------------------------------
LEED3: 	LDAA $4004 				; Par I/O CSR
		ANDA #$FD 				; 1111 1101
		TST $F4 				; Ck Fan Dty Cyc
		BEQ LEEE3 				;
								; ... else
		BRCLR $34 8 LEEE3 		; Go if Fan Disable not set
								; ... else
		ORAA #$02 				; SET b1
LEEE3: 	STAA $4004 				; Par I/O CSR
								;------------------------
								; EGR & Air Switch
								;------------------------
								;--------------------------------------------------
								; CK IF IN ALDL MODE 4
								; EGR PW
								; ALDL MODE 4, MD WD ... b0
								;--------------------------------------------------
		BRCLR $35 $08 LEEF6 	; BR IF NOT b3, MODE 4
								; ... else
		LDAA $0156 				; MD WD ... b0
		BITA #$01 				; b0
		BEQ LEEF6 				; BR IF NOT b0
								; ... else
		LDAB $0157 				; GET MODE 4 MD WD ..
		BRA LEEFA 				;
								;--------------------------------------------------
LEEF6: 	LDAB $0112 				; Out Fm EGR PW Mod
		COMB 					;
								;--------------------------------------------------
LEEFA: 	LDAA #$34 				;
		ASLD 					; Mult x 4 for 32 Hz op's
		ASLD 					;
		ORAB #$03 				; SET b0 & b1
		STD $3FD2 				; Wr to EGR PW Cnt'r
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* MAJOR LOOP SUBROUTINE SEG D,
		;*
		;* Diagnostics
		;*
		;*******************************************************
								;
								; DIAGNOSTICS
								;
								; Eexcuted on 100 Ms Clk
								; Do Loging, and blinking of Err lamp
								;
LEF04: 	LDAB $00 				; Get Minor Lp counter
		LDAA $3F 				; Get Diag mode wd #1
		ANDB #$F0 				; 1111 0000, Mask ?
		BNE LEF15 				;
								; ... else
		ORAA #8 				; 0000 1000, Bit 3
		LDAB $1B 				;
		RORB 					;
		BCC LEF15 				;
								; ... else
		ORAA #$10 				; 0001 0000, Bit 4
LEF15: 	STAA $3F 				; Save Diag wd #1
		LDAB $41 				;
		LDAA $35 				; MNR LOOP MD WD 2
		BITA #$10 				; b4, DIAG SW IN DIAG MODE
		BNE LEF2C 				; BR IF b4,
								; ... else
		ANDB #$7F 				;
		STAB $41 				;
								;
		LDAA $3C 				;
		ANDA #$E3 				;
		STAA $3C 				;
								;
		JMP LEF92 				;
LEF2C: 	ORAB #$80 				;
		STAB $41 				;
		CLR $B1 				; Err Lamp Tmr, (100Ms)
		LDAA $34 				;
		BMI LEF40 				;
								; ... else
		LDAA $3C 				;
		ANDA #$E3 				;
		STAA $3C 				;
		JMP LEFFE 				;
LEF40: 	LDAB $44 				;
		BMI LEF51 				;
								; ... else
LEF44: 	LDAA $3C 				;
		ANDA #$F7 				;
		EORA #$40 				;
		STAA $3C 				;
		ASLA 					;
		BMI LEF87 				;
								; ... else
		BRA LEF78 				;
LEF51: 	LDAA $3C 				;
		BITA #$0008 			;
		BNE LEF68 				;
								; ... else
		LDAB $3F 				;
		BITB #8 				;
		BEQ LEF44 				;
								; ... else
		ORAA #8 				;
		STAA $3C 				;
LEF61: 	LDAB $44 				;
		ASLB 					;
		BMI LEF7F 				;
								; ... else
		BRA LEF84 				;
LEF68: 	LDAA $3F 				;
		BITA #8 				;
		BEQ LEF87 				;
								; ... else
		LDAA $3C 				;
		BITA #$10 				; b4
		BEQ LEF61 				; BR IF NOT b4
								; ... else
		ANDA #$EF 				; 1110 1111
		STAA $3C 				;
LEF78: 	LDD $3FFC 				; MCU CSR
		BITB #$08 				; b3
		BEQ LEF84 				; BR IF NOT b3
								; ... else
LEF7F: 	JSR LF498 				;
		BRA LEF87 				;
LEF84: 	JSR LF4A6 				;
LEF87: 	CLRB 					;
		STAB $42 				; DIAG CNT'L WD
		LDAA $3C 				;
		ORAA #$04 				;
		STAA $3C 				;
		BRA LEFCC 				;
								;----------------------------
								; End of Field Sev Mode
								;----------------------------
LEF92: 	CLRB 					; Reset Flash Cnt'l Wd
		STAB $42 				; DIAG CNT'L WD
		LDAA $4E 				; NEW ERR 2
		ANDA $53 				;
		ANDA #$01 				;
		BEQ LEFA9 				;
								; ... else
		ORAA $07 				; Err #3 Flg word
		STAA $07 				; Err #3 Flg word
		JSR LF3A7 				; NV MEM Wd's Ck Sum
		STD $18 				; Save Ck Sum OF Err Words
		JMP LF0FC 				;
LEFA9: 	LDAA $34 				; Ck If ENG Running
		BMI LEFCC 				; BR IF YES, GOT DO DIAG
								; ... else
		STAB $A7 				; TPS Err cnt'r,(100Ms)
		STAB $AB 				; VSS Err cnt'r
		STAB $A8 				; o2 Lan Cnt'r (2 Sec)
		STAB $A9 				; o2 Sensor Rich tmr, (2 Sec)
		STAB $AA 				; o2 Sensor Hvy Ld Cnt'r, (2 Sec)
		STAB $B5 				; ERR 34 TMR
		STAB $AC 				; Lo MAT Err #23
		STAB $AD 				; Hi MAT cnt'r, Err#25
		STAB $AE 				; EGR Diag Cnt'tr
		STAB $AF 				;
		LDAA $33 				; MINOR LOOP MD WD 1
		BITA #$10 				; B4, (1 = IGN ON)
		BEQ LEFC9 				; GO if Yes
								; ... else
		STAB $B0 				; Clr Err #3 Timer
LEFC9: 	JMP LEFFE 				; Goto to Err log filter
		;*******************************************************
		;*******************************************************
		;* MAIN BACKGROUND DIAGNOSTICS
		;*
		;*
		;*******************************************************
LEFCC: 	LDAA $3F 				; Get Diag Mode Wd #1
		BITA #$02 				; b1
		BNE LEFFB 				; Go do Err Checks
								; ... else
								;
								; Clr bulb Tst After Dly time
								; & eng Running
								;
		LDAB $1B 				;
		CMPB LC1DA 				; 0, Ck Eng Lmp Bulb tst time
		BCS LEFFB 				;
								; ... else
		ORAA #$02 				; b1
		STAA $3F 				;
		INC $17 				; Inc No Err Counter
		LDAA $17 				; No Err Counter
		CMPA LC1D5 				;
		BLS LEFFB 				; Begin Err Chk's
								; ... else
								;--------------------
								; CLR ALL ERRORS
								;-------------------
		CLRA 					; If Yes, Clr All Errs & ck sum
		STAA $05 				; Err #1 Flag word
		STAA $06 				; Err #2 Flg word
		STAA $07 				; Err #3 Flg word
		STAA $08 				; Err #4 Flaf word
		STAA $09 				; Err #5 Flg Word
		STAA $17 				; No Err Counter
		STAA $42 				; DIAG CNT'L WD
		JSR LF3A7 				; Call EEPROM ERR Wd's Ck Sum
		STD $18 				; Ck Sum OF Err Words
LEFFB: 	JSR LE4F7 				;
								;
								; Err Loggong Filter
								; Turn Sys Err If Time constraints meet
								;
								;
LEFFE: 	LDAA $3C 				; ALCL MOD WD
		BITA #$04 				; b2, FIEL SERVICE MODE
		BEQ LF007 				; BR IF NOT b2
								; ... else
		JMP LF0FF 				; EXIT THIS ROUTINE
LF007: 	LDAA $41 				; DIAG MODE WD
		BPL LF00E 				; BR IF IN
								; ... else
		JMP LF10E 				;
LF00E: 	LDAA $33 				; MINOR LOOP MD WD 1
		BITA #$10 				; b4, (1 = IGN ON)
		BEQ LF017 				; BR IF NOT b4
								; ... else
		JMP LF0FF 				;
LF017: 	LDAA $51 				;
		ORAA $52 				;
		ORAA $53 				;
		ORAA $54 				;
		ORAA $55 				;
		BNE LF058 				; BR IF NZ
								; ... else
		LDD $4C 				; NEW ERR 1 	;
		ANDA LC1D0 				; 1111 0111, Mask For Mal Funct flg 1
		ANDB LC1D1 				; 1100 1110, Mask For Mal Funct flg 2
		STD $51 				;
		LDD LC1D2 				; Mask For ERR flag 3
		ORAA #$01 				; ERR Code 51 Prom Error
		ANDA $4E 				; NEW ERR 2
		ANDB $4F 				;
		STD $53 				;
		LDAA $50 				; NEW ERR 3
		ANDA LC1D4 				; Mask For ERR flag 5
		STAA $55 				;
		LDAA $B1 				; Err Lamp Tmr, (100Ms)
		BEQ LF049 				;
								; ... else
		DEC $B1 				; Err Lamp Tmr, (100Ms)
		JMP LF0C8 				;
LF049: 	LDAA $41 				;
		ANDA #$00FE 			;
		STAA $41 				;
		LDAA $3B 				;
		ANDA #$00FB 			;
		STAA $3B 				;
		JMP LF0C8 				;
LF058: 	CLRA 					;
		LDY #$C1D4 				;
		LDX #$0050 				;
LF060: 	LDAB 0,Y				;
		ANDB 0,X 				;
		ANDB 5,X 				;
		STAB 5,X 				;
		BEQ LF06C 				;
								; ... else
		INCA 					;
LF06C: 	DEY 					;
		DEX 					;
		CPX #$004B 				;
		BNE LF060 				;
								; ... else
		INC $B1 				; Err Lamp Tmr, (100Ms)
		LDAB $41 				;
		TSTA 					;
		BNE LF090 				;
								; ... else
		LDAA $B1 				; Err Lamp Tmr, (100Ms)
		RORB 					;
		BCS LF086 				;
								; ... else
		LDAB LC1D6 				;
		BRA LF089 				;
LF086: 	LDAB LC1D8 				;
LF089: 	CBA 					;
		BCS LF0C8 				;
								; ... else
		STAB $B1 				;Err Lamp Tmr, (100Ms)
		BRA LF0C8 				;
LF090: 	LDAA $B1 				;Err Lamp Tmr, (100Ms)
		RORB 					;
		BCS LF09A 				;
								; ... else
		LDAB LC1D7 				;
		BRA LF09D 				;
LF09A: 	LDAB LC1D9 				;
LF09D: 	CBA 					;
		BCS LF0C8 				;
								; ... else
		LDAA LC1D8 				;
		STAA $B1 				; Err Lamp Tmr, (100Ms)
		LDAA $41 				;
		ORAA #$0001 			;
		STAA $41 				;
		CLRB 					;
		LDX #$0005 				;
LF0AF: 	LDAA 4,X 				;
		ORAA $50,X 				;
		STAA 4,X 				;
		STAB $50,X 				;
		DEX 					;
		BNE LF0AF 				;
								; ... else
		CLR $17 				; No Err Counter
								;
		JSR LF3A7 				; Call EEPROM ERR Wd's Ck Sum
		STD $18 				; Ck Sum OF Err Words
		LDAA $3B 				;
		ORAA #04 					;
		STAA $3B 				;
LF0C8: 	LDAA $34 				;
		BMI LF0D4 				;
								; ... else
		BITA #2 				;
		BNE LF0FC 				;
								; ... else
		ORAA #2 				;
		BRA LF0D6 				;
LF0D4: 	ANDA #$FD 				;
LF0D6: 	STAA $34 				;
								;--------------------------------------------------
								; CK IF IN ALDL MODE 4
								;
								;
								; ALDL MODE 4, MD WD ... b0
								;
								;--------------------------------------------------
		BRCLR $35 $08 LF0EC 	;
								; ... else
		LDAA $0152 				; MD WD .. b0
		BITA #$01 				;
		BEQ LF0EC 				;
								; ... else
		LDAA $0153 				; MD WD .. b0
		BITA #$01 				;
		BEQ LF0F7 				;
								; ... else
		BRA LF0FC 				;
								;--------------------------------------------------
LF0EC: 	LDAA $41 				;
		RORA 					;
		BCS LF0FC 				;
								; ... else
		LDAA $3F 				;
		BITA #2 				;
		BEQ LF0FC 				;
								; ... else
LF0F7: 	JSR LF4A6 				;
		BRA LF0FF 				;
LF0FC: 	JSR LF498 				;
								;
								; EXIT HERE ...
								;
LF0FF: 	LDAA $3F 				; ERR MD WD
		ANDA #$E7 				; CLE 1 & 2 SEC FLAGS
		STAA $3F 				;
								;
								;CLEAR ALL NEW ERR'S
								;
		CLRA 					;
		CLRB 					;
		STD $4C 				; NEW ERR 1
		STD $4E 				; NEW ERR 2
		STAA $50 				; NEW ERR 3
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* FLASH ERROR CODES FOR EACH ERR IN MEM
		;*
		;*******************************************************
LF10E: 	LDAA $42 				;
		BITA #$40 				; b6,
		BEQ LF166 				;
								; ... else
		DEC $BD 				;
		LDAB $BD 				;
		BEQ LF11D 				;
								; ... else
		BRA LF0FF 				;
LF11D: 	BITA #$10 				; b4, LIGHT ON
		BEQ LF12B 				; BR IF NOT
								; ... else
		ANDA #$EF 				; CLR b4, TURN OFF LIGHT
		LDAB #$04 				; b2,
LF125: 	STAA $42 				;
LF127: 	STAB $BD 				;
		BRA LF0F7 				;
								;
								; READY FOR 2ND BYTE ?
								;
LF12B: 	BITA #$20 				; b5,
		BNE LF139 				;
								; ... else
		LDAB $B9 				;CNTR for blk 1st ERR Code DIgit
		BNE LF142 				;
		LDAB #8 				;
		ORAA #$20 				;
		BRA LF125 				;
								;
								; 2ND BYTE DONE ?
								;
LF139: 	LDAB $BA 				;CNTR for blk 2nd ERR Code digit
		BEQ LF14F 				;
								; ... else
		DECB 					;
		STAB $BA 				; CNTR for blk 2nd ERR Code digit
		BRA LF145 				;
LF142: 	DECB 					;
		STAB $B9 				; CNTR for blk 1st ERR Code digit
LF145: 	ORAA #$0010 			; TURN LIGHT ON
		STAA $42 				;
		LDAA #$04 				;
		STAA $BD 				;
		BRA LF0FC 				;
								;
								;
								;
LF14F: 	LDAB $BB 				; Cnt for Current Err in Blink out
		ANDA #$03 				;
		BEQ LF15C 				;
								; ... else
		LDAA $42 				;
		DECA 					;
		ANDA #$00DF 			;
		BRA LF1AB 				;
LF15C: 	LDAA $BC 				; Cntr for Rotaint Err In Blnk Disp
LF15E: 	INCB 					;
		CMPB #$25 				;
		BCS LF168 				;
								; ... else
		CLRA 					;
		BRA LF125 				;
LF166: 	LDAB #2 				;
LF168: 	CMPB #2 				;
		BNE LF174 				;
								; ... else
		LDAA $05 				; Err #1 Flag word
		ANDA LC1D0 				;
		ROLA 					;
		BRA LF1A5 				;
LF174: 	CMPB #10 				;
		BNE LF17F 				;
								; ... else
		LDAA $06 				; Err #2 Flg word
		ANDA LC1D1 				; Mask for ERR flag 2
		BRA LF1A2 				;
LF17F: 	CMPB #$12 				;
		BNE LF18C 				;
								; ... else
		LDAA LC1D2 				; Mask For ERR flag 3
		ORAA #1 				; SET b0
		ANDA $07 				; Err #3 Flg word
		BRA LF1A2 				;
LF18C: 	CMPB #$1A 				;
		BNE LF197 				;
								; ... else
		LDAA LC1D3 				; Mask for ERR flg 4
		ANDA $08 				; Err #4 Flag word
		BRA LF1A2 				;
LF197: 	CMPB #$22 				;
		BNE LF1A2 				;
								; ... else
		LDAA $09 				; Err #5 Flg Word
		ANDA #$E0 				;
		ANDA LC1D4 				; Mask for ERR flg 5
LF1A2: 	ROLA 					;
		BCC LF15E 				;
								; ... else
LF1A5: 	STAA $BC 				; Cntr for Rotatate Err In Blnk Disp
		STAB $BB 				; Cnt for Current Err in Blink out
		LDAA #$42 				;
LF1AB: 	STAA $42 				;
		CLRA 					;
LF1AE: 	INCA 					;
		SUBB #6 				;
		BHI LF1AE 				;
								; ... else
		ADDB #$0006 			;
		STAA $B9 				; CNTR for blk 1st ERR Code digit
		STAB $BA 				; CNTR for blk 2nd ERR Code digit
		LDAB #$1C 				;
		JMP LF127 				;
		;*******************************************************
		;*******************************************************
		;* SUBROUTINES
		;*
		;* PUBLIC Enties for:
		;* LF1BE A/D Read
		;* LF1C3 READ A/D PORT & MONITOR BUSS FOR FAIL
		;* LF1E0 ACCESS FMD LINE STATUS
		;* LF1F9 XMIT DATA ON S3 SERIAL BUSS
		;* LF215 FRACTIONAL DIVIDE
		;* LF21A TOGGLE FMD COP
		;* LF231 GET IDLE AIR CONDITION
		;* LF242 TURN ON/OFF IAC COILS
		;* LF250 LAG FILTER242
		;* LF266 8 x 16 MULT (RE-ENTRANT)
		;* LF27C 3d TABLE LK UP
		;* LF2B9 2d TABLE LK UP
		;* LF2F0 16 x 16 MULTIPLY
		;* LF32E CALCULATE A STD GM CHECKSUM
		;* LF3A7 CALCULATE A ERR WD CHECKSUM
		;* LF NORMALAZED TPS
		;* LF3B5 11 usec DELAY
		;* LF465 LIMIT A REG VAL'S
		;* LF472 Factor in a 8 x 16 Multiply
		;* LF498 Set Up MCU CSR
		;* LF49A Turn off a CSR bit
		;* LF4A6 TURN OFF ERR LAMP
		;* LF4A8 Turn On a CSR bit
		;*******************************************************
		;*******************************************************
		;* A/D sub Routine
		;*
		;* Enter With A/D Ch 0 - B in A Reg.
		;* Return w/Resultds in A Reg.
		;*
		;*
		;*******************************************************
LF1BE: 	SEI 						; Set Int Mask
		BSR LF1C3 					; To A/D routine
		CLI 						; Clr int mask, Int's Enabled
		RTS 						;
									;-----------------------------------
									; A/D Subroutine
									;-----------------------------------
LF1C3: 	PSHB 						; Save Reg,s
		PSHX 						;
		LDX #$4002 					; Point to Serial Data Latch
		BCLR 0,X 08 				; Clr Bit 8
		JSR LF1F9 					; Xmit to Serial Data
		STAA $64 					; Save to RAM Buffer
		LDAA #7 					; DELAY VALUE
LF1D2: 	DECA 						;
		BNE LF1D2 					; Loop till 0
									; ... else
		LDAA #$B0 					; 176d
		JSR LF1F9 					; Xmit to Serial Data
		BSET 0,X 08 				; Set b3, DISABLE SAD
		PULX 						; Restore Reg's
		PULB 						;
		RTS 						;
		;*******************************************************
		;*******************************************************
		;* P4 MODE'ING DEVICE, SPI READ
		;*
		;* 1. LF1E0, SEL FMD ON SPI & READ BYTE 1, WHICH HAS FMD
		;* DESCRETES
		;*
		;* 2. LF1E5, SEL FMD ON SPI & READ BYTE 2
		;*
		;*
		;* RETURNS WITH:
		;* BYTE IN A Reg
		;* CY CLR IF NO FAILURE
		;*******************************************************
									;
									; READ BYTE 1
									;
LF1E0: 	BCLR $30 $80 				; CLR b7, SEL FMD b1 READ
		BRA LF1E8 					;
									;
									; READ BYTE 2
									;
LF1E5: 	BSET $30 $80 				; SET b7, SEL FMD b2 READ
LF1E8: 	PSHX 						;
		LDX #$4002 					; ENABLE CS FOR FMD
		BSET 0,X 04 				; Set b2
		LDAA $30 					; SET UP FMD SPI INPUT
		JSR LF1F9 					; READ SEL FMD BYTES
		BCLR 0,X 04 				; CLR b2, DISABLE FMD
		PULX 						;
		RTS 						;
		;***************************************************
		;***************************************************
		;* Xmit to SPI Data
		;*
		;* DATA FROM A Reg LOAD SPI SR AND TRANSMITS
		;* TO SELECED DEVICE VIA SPI
		;*
		;* CALL:
		;* A = DATA TO BE XMITED VIA SPI
		;*
		;* RETURNING:
		;* A = RX'ED SPI DATA (O IF SPI FAIL)
		;*
		;***************************************************
LF1F9: 	PSHX 						; Save X reg
		STAA $4000 					; To SPI S/Reg.
		LDX #$4001 					;
		BCLR 0,X $80 				; Clr b7, TO START SPI XMIT
									;
									; WAIT FOR SPI TO FINISH, 256 CYC MIN
									;
		LDAA #22 					; TIME DELAY VAL
		CLC 						; CLR CY, SPI I/O ERR FLG
LF206: 	BRSET 0,X $80 LF210 		; BR IF b7, SPI DONE
									; ... else
		DECA 						; DEC DELAY VALUE
		BNE LF206 					; BR IF DELAY NOT DONE
									; ... else
		SEC 						;
		BRA LF213 					;
									;
									; GET RETURNED DATA FROM SPI
									;
LF210: 	LDAA $4000 					; Fm SPI S/Reg
LF213: 	PULX 						;
		RTS 						;
		;***************************************************
		;***************************************************
		;* FRACTIONAL DIVIDE
		;*
		;*
		;***************************************************
LF215: 	FDIV 						; DIVIDE
		PSHX 						; SAVE QOUTIENT
		PULA 						;
		PULB 						;
		RTS 						;
		;***************************************************
		;**************************************************
		;* TOGGLE FMD COP
		;* LD FMD SPI IMAGE
		;* TOGGLE COP2 in BYTE
		;* OUTPUT via SPI
		;*
		;**************************************************
LF21A: 	PSHX 						;
		LDAA $30 					; ACCESS SPI BYTE
		EORA #$02 					; COP2
		STAA $30 					;
		SEI 						;
		LDX #$4002 					; Pt to SPI Data Latch
		BSET 0,X 04 				; SEL FMD CHIP
		JSR LF1F9 					; Xmit via SPI
		BCLR 0,X 04 				; UN-SEL FMD CHIP
		CLI 						;
		PULX 						;
		RTS
		;***************************************************
		;***************************************************
		;* GET IDLE AIR CONDITIONS
		;*
		;* RETURNS WITH:
		;* STATUS OF IAC A,B & C
		;***************************************************
LF231: 	PSHX 					;
		LDAA $4002 				; Get SPI Data Latch
		ANDA #$03 				; MASK OFF IAC BITS
		LDX #$4004 				; IO CNT'L REG PORT ADDRESS
		BRCLR 0,X 4 LF240 		; IAC C ON
								; ... else
		ORAA #$04 				; TURN ON BIT
LF240: 	PULX 					;
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* OUTPUT THE IAC SOLENOIDS
		;* TOGGLES OF IAC SOLENOIDS
		;*
		;*
		;*******************************************************
LF242: 	PSHB 					;
		ANDA #$03 				; MASK b0 & b1
		LDAB $4002 				; Get SPI Data Latch
		ANDB #$FC 				; MASK OFF LOW ORDER
		ABA 					; NEW STATE
		STAA $4002 				; To SPI Data Latch
		PULB 					;
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* FIRST ORDER LAG FILTER
		;*
		;* CALLING:
		;* X = OLD FILT VALUE
		;* A = NEW VALUE TO BE FILTERED
		;* B = FILT CONSTANT
		;*
		;* RETURNING:
		;* D = NEW FILT VALUE
		;*
		;*******************************************************
LF250: 	PSHX 					;
		PSHB 					;
		MUL 					;
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDAA $2,X 				;
		INX 					;
		INX 					;
		INX 					;
		NEGA 					;
		JSR LF266 				; MUL8X16
		TSX 					;
		ADDD 0,X 				;
		PULX 					;
		INS 					;
		PULX 					;
		RTS
		;*******************************************************
		;*******************************************************
		;* MUL8X16
		;* 8 X 16 Multiply with 16 bit result.
		;*
		;* output is rounded for 16 bit result.
		;*
		;* Calling Arg:
		;* A ACC = 8 BIT Multiplier
		;* X REG = Address of 16 Bit Multiplicand
		;*
		;* Returns with:
		;* A ACC = MSB of result
		;* B ACC = LSB 0f result
		;* Regesters affected:
		;* A, B & Conditions Regester
		;* Execution time:
		;* 68 CPU Cycles
		;* Stack Used:
		;* 4 bytes
		;* Subs Called:
		;* None
		;*******************************************************
		ORG $F266
LF266: 	
		PSHX 					; Save mutiplcand on stx
		PSHA 					; save Mult'er on stx
		LDAB 1,X			
		MUL 					; Get 1st partial prod
		ADCA #0			
		PSHA 					; Save partial prod.
		LDAA 0,X			
		TSX			
		LDAB 1,X			
		MUL 					; Get 2nd Partial prod.
		ADDB 0,X 				; Add 1st part prod to 2nd.
		ADCA #0			
		PULX 					; Restore STX
		PULX 					; Get Mutl'cnd to X Reg.
		RTS 					; & Return
								;-------------------------------------------------
								; Throw out Extranious Interupts
								;-------------------------------------------------
		RTI
		;*******************************************************
		;*******************************************************
		; LKUP3D.src FROM BUA.BIN, 86 VETTE
		; Main Look Up Subroutine, (3 Dem)
		;
		; Enter with:
		; R = number of rows in table
		; Q = minimum value of columns
		; RNUM = Number of Q values, (NUM OF BK POINTS)
		;
		; Max Value Diff between talbe entries = 255
		; Call With:
		; A = Row Arg
		; B = Col Arg
		; X = Tbl Addr
		;
		; Returns with:
		; ACCA = Result, (Y = F(Q,R)
		;
		; Regesters affected:
		; A ACC & Conditions Regester
		;
		; Execution time:
		; 155 CPU Cycles + (2 * p4lkupq+p4lkupr)
		;
		; Stack Used:
		; 11 bytes
		;
		; Subs Called:
		; p4lkupq
		; p4lkupr
		;
		;
		; $F27C - $F2EF, ($79, 121d BYTES)
		;
		;*******************************************************
		ORG $F27C
LF27C: 	SUBA 0,X 				; Calc Row Arg offset, (Lmt to 0)
		PSHB 					; Save Col Arg to stack
		PSHX 					; Save Tbl offset to stack
		BCC LF283 				; BR IF NO UNDERFLOW
								; ... else
		CLRA 					;
LF283: 	SUBB 1,X 				; Calc Col Arg offset, (Lmt to 0)
		BCC LF288 				; BR IF NO UNDERFLOW
								; ... else
		CLRB 					;
LF288: 	PSHB 					; Save Col Arg to stack
		LDAB #16 				; Make In Arg Into Interp Inc's
		MUL 					; Calc Col Arg offset, (Lmt to 0)
		PSHB 					; Save Interp Inc's tp stack
		LDAB 2,X 				; Set Addr of tbl
		PSHB 					;
		MUL 					;
		ABX 					;
		INX 					;
		INX 					;
		INX 					;
		PSHX 					; Save Tbl Addr on stack
		TSX 					;
								;
								;Call 2d Lk Up, (No Offset)
								;
		LDAA 4,X 				;Get Col Arg for Lk Up
		LDX 0,X 				;Tbl Addr
		JSR LF2C6 				;Call 2d Lk Up, (No Offset)
		PSHA 					;Save result Of Lk Up to stack
		TSX 					;
		LDAA 5,X 				; Get Col Arg
		LDAB 4,X 				;
		STAB 5,X 				; Put R(low) in highest STX
		LDAB 3,X 				; RNUM
		LDX 1,X 				; Addr of Table
		ABX 					; R1 + RNUM
		JSR LF2C6 				; Call 2d Lk Up, (No Offset)
		TAB 					;
		PULA 					;
		PULX 					;
		PULX 					;
		JSR LF2D6 				; Call Interpolate routine
		INS 					; Restore Stack
		PULX 					; Restore X
		PULB 					; Restore B
		RTS
		;*******************************************************
		;--------------------------------------------------
		;*******************************************************
		; Look Up Routine 2 dimentional
		;
		; Use for tables with 1 byte Unsigned values and
		; fixed spacing and a 1 byte unsigned Variable
		;
		; Four entries are provides, see each for enty & exit
		; conditions.
		; Enter with:
		; FOUR ENTRY POINTS:
		;
		; 1. LKUP2D: A ACC = Independent Var Value
		; B ACC = Offset to be subtracted fm A ACC
		; prior to table look up
		;
		; 2. LKUPQ: A ACC = Independent Var Value, offset=0)
		; X REG = Address of table
		;
		; 3. LKUPR: A ACC = 1st value for interpelation
		; B ACC = 2nd value for interpelation
		;
		; 4. LKUP2C: A ACC = Independent Var Value, offset=0)
		; X REG = Address of table
		;
		; Returns with:
		; ACC A = Result, (Single byte Un-signed table value)
		;
		; Regesters affected:
		; A ACC & Conditions Regester
		;
		; Execution time:
		; LKUP2D: 128 CPU cycles
		; LKUPQ: 118
		; LKUPR: 62
		; LKUP2C: 126
		;
		; Stack Used:
		; LKUP2D: 10 bytes
		; LKUPQ: 10 bytes
		; LKUPR: 4 bytes
		; LKUP2C 10 bytes
		;
		; Subs Called:
		; None
		;*******************************************************
		;*******************************************************
		;p4lkup2d Entry Point
		;
		; Use When an Offset of the Independent Var is in B
		;
		; Enter with:
		; X = Addr of table
		; A = Independent Var
		; B = Offset to be sub'ed fm A
		;
		; Return with:
		; A = Results
		;
		; 128 Cycles
		;*******************************************************
LF2B9: 	SBA 					;Sub Off set
		BCC LF2C6 				;if not L.T. Zero,
								; - Call 2d Lk Up, (No Offset)
								; ... else
		CLRA 					;Set To Zero if L.T.
		BRA LF2C6 				;& go to Lk Up
								; - Call 2d Lk Up, (No Offset)
								;--------------------------------------------------
								;
								; Use to interpolate fn table with fixed spacing which
								; is specified in 1st byte of table, No B offset is
								; allowed.
								;
								; Enter with:
								; X = Addr of table
								; A = Independent Variable
								;
								; Return with:
								; A = Results
								;
								; 126 Cycles
								;---------------------------------------------------
LF2BF: 	PSHB 					; Save B
		PSHX 					; Save Table Addr
		LDAB 0,X 				; Get Spacing value
		INX 					;
		BRA LF2CA 				; Goto to Lk Up
								;----------------------------------------------------------
								; 2d LK UP, Use When no Offset of the Independent Var.
								;
								; Enter with:
								; X = Addr of table
								; A = Independent Var, (offset = 0)
								;
								; Return with:
								; A = Results
								;
								; 118 Cycles
								;---------------------------------------------------
LF2C6: 	PSHB 					; Save Col Input Arg
		PSHX 					; Save Tabl Addr
		LDAB #16 				; Set Spacing
LF2CA: 	MUL 					; Calc Increment
		PSHB 					; Save Increment on Stack
		TAB 					;
		ABX 					;
		LDD 0,X 				;
		BSR LF2D6 				; Call Interpolate routine
		PULB 					; Restore Stack
		PULX 					;
		PULB 					;
		RTS 					; Return to Caller
		;*******************************************************
		;*******************************************************
		; Interpolation Subroutine
		;
		; Use to interplate two 8 bit Values
		;
		; Enter with:
		; A = 1st Vaulue for interpolation
		; B = 2nd Value for interpolation
		;
		; Return with:
		; Stack = Results
		;
		; 62 Cycles
		;*******************************************************
LF2D6: 	PSHX 					;Save X
		PSHB 					;Save B
		PSHA 					;Vave A
		SBA 					;Less than 0 ?
		TSX 					;
		LDAB 6,X 				;Get Increment Multiplier
		BCS LF2E8 				;If Cy Set go
								; ... else
								;-----------------------------
								; Interp & rnd Dwn if required
								;-----------------------------
		MUL 					;
		NEGA 					;
		ADDA 0,X			
		ASLB 					;
		SBCA #0 				;
		BRA LF2EC 				;
								;--------------------
								; Interp & round UP
								;--------------------
LF2E8: 	NEGA			
		MUL 					;
		ADCA 0,X 				;
LF2EC: 	INS 					;Restore Stack
		PULB 					;
		PULX 					;
		RTS 					;
		;*******************************************************
		;*******************************************************
		; MUL16X16
		; 16 * 16 Mult Fixed Point Routine
		;
		; Call with:
		; X = Multiplicand
		; A = MSB of Multiplier
		; B = LSB of Multiplier
		;
		; Returns with:
		; A ACC = Middle 2 bytes of 32 bit product
		; B ACC = Middle 2 bytes of 32 bit product
		; X REG = upper 2 bytes of 32 bit result.
		;
		; Regesters affected:
		; A, B, X & Conditions Regester
		; Execution time = 182 CVycles max
		;
		; Middle (A & B) will be set to $FFFF if MSB oof the
		; result is N.Z.
		;*******************************************************
		ORG $F2F0
LF2F0: 	
		PSHA 					; Reserve for MSB of Result
		PSHX 					; Reserve for Part Result
		PSHX 					; Save Mutlpcand to Stack
		PSHB 					; Save LSB of Mult to Stack
		PSHA 					; Save MSB of Mult to Stack
		TSX 					;
		LDAA 3,X 				;
		MUL 					; LSB of Mult'cnd * LSM of Mult
		ADCA #0 				;
		STAA 5,X 				;
		LDD 1,X 				;
		MUL 					; LSB of Mult * MSB of Mult'cnd
		ADDB 5,X 				;
		ADCA #0 				;
		STD 4,X 				;
		LDAA 0,X 				; MSB mult'plr X LSB -
								; Mult'cand
		LDAB 3,X 				;
		CLR 6,X 				; Clr MSB Of Result
		MUL 					; MSB Mult * LSB of Mult'cnd
		ADDD 4,X 				;
		ROL 6,X 				;
		STD 4,X 				;
		LDAA 0,X			
		LDAB 2,X 				;
		MUL 					; MSB Mult * MSB Mult'cnd
		ADDB 4,X 				; MSB Result In A&B
		ADCA 6,X 				;
		STAB 4,X 				;
		STD 2,X 				; 2 High Bytes to X
		TSTA 					; Hi Byte N.Z. ?
		BEQ LF328 				; If No go
								; ... else
		LDD #$FFFF 				; Else Set To $FFFF
		STD 4,X 				;
LF328: 	PULX 					; Higher Bytes to X
		PULX 					;
		PULA 					; Middle bytes Results to A & B
		PULB 					;
		INS 					; Restore Stack
		RTS
		;******************************************************
		;******************************************************
		;CKSUM:
		; CALCULATE A STD GM CHECKSUM
		;
		; Output is A 16 bit result.
		; Calling Arg:
		; B = Number of bytes to cksum
		; X = Starting address
		; Returns with:
		; Y = Checksum
		;
		; Execution time:
		; 855,726 CPU Cycles, (FFFF bytes)
		;******************************************************
		ORG $F32E
;		EQU COP $400B 		; Watch dog timer
LF32E: 	
		PSHX 				; Save start addr of SUM
		PSHB		
		PSHA 				; Save num of bytes to sum
		LSRD 				; count in 2's
		PSHA 				; save hi byte (/2) on stx
		LDY #0 				; clear Y Reg.
		TBA 				; Ck for ACCB = 0 after shift
		BEQ LF348		
							;...else
LF33A: 	LDAB 0,X 			;Get byte
		ABY 				;Add to CHECKSUM
		LDAB 1,X 			;Next byte
		ABY 				;Add to CHECKSUM
		LDAB #2		
		ABX 				;Point to next pair of bytes
		DECA 				;Dec counter
		BNE LF33A		
							; ... else
LF348: 		
		PSHX 				; Save current Address
		LDX #$FF00 			; COP Time Value
		STX $400B 			; Toggle COP (every 512 bytes)
		TSX		
		DEC 2,X 			; Dec hi order byte of count
		PULX 				; Continue till all bytes done
		BPL LF33A		
							;...else
		INS 				; Dump hi order counter
		PULA 				; Restore regesters
		PULB		
		PULX		
		RTS 				;& Return
		;*******************************************************
		;*******************************************************
		;
		;
		;
		;*******************************************************
LF35A: 	LDAB $3F 					;
									;
									; CK ID ERR 21 ENABLED
									;
		LDAA LC1D0 					; Mask For Mal Funct flg 1
		BITA #$04 					; b2, ERR Code 21 TPS High
		BEQ LF36F 					; BR IF NOT b2
									; ...else
		LDAA $81 					; TPS, (A/D)
		CMPA LC1E6 					; IF TPS A/D > set ERR #21
		BLS LF36F 					;
									; ...else
		ORAB #$04 					; SET b2
		BSET $4C $04 				; SET b2
LF36F: 	STAB $3F 					;
		LDAA $81 					; TPS, (A/D)
		BITB #$24 					; b5 & b2
		BEQ LF37A 					; BR IF NOT b5 & b2
									; ...else
		LDAA LC1EA 					;
LF37A: 	PSHA 						;
		LDAB $86 					;F ltered low TPS, (A/D)
		CBA 						;
		BHI LF38A 					;
									; ...else
		LDX $86 					; Fltered low TPS, (A/D)
		LDAB LC39C 					; Lo TPS filter coef
		JSR LF250 					;
		STD $86 					; Fltered low TPS, (A/D)
LF38A: 	LDD $86 					; Fltered low TPS, (A/D)
		ADDD #$0080 				;
		TAB 						;
		PULA 						;
		SBA 						;
		BCC LF395 					;
									; ...else
		CLRA 						;
LF395: 	LDAB LC39A 					; Hi/Lo TPS%
		MUL 						;
		ADDD #32 					;
		ASLD 						;
									; ...else
		BCS LF3A2 					;
									; ...else
		ASLD 						;
		BCC LF3A4 					;
									; ...else
LF3A2: 	LDAA #255 					; MAX VALUE
LF3A4: 	STAA $82 					; TPS Ld Axis Var
		RTS 						;
		;*******************************************************
		;*******************************************************
		;* Do Check sum of NON VOL MEMORY Err Flg Word's
		;*
		;* Return with:
		;* A = MSB of Ck Sum
		;* B = LSB of Ck Sum
		;*******************************************************
		ORG $F3A7
LF3A7: 	LDX #5 					; Set For 5 EEPROM Flg,s
		LDD #1 					; Inc for non zero sum
LF3AD: 	ADDB 4,X 				; Sum Flg Words
		ADCA #0 				;
		DEX 					; Dec till done
		BNE LF3AD 				; Loop Till done
								;...else
								;
								; ECU DELAY HERE
								; 11 uswc
		RTS						;
LF3B5: 	RTS 					;Return to caller
		;*******************************************************
		;*******************************************************
		;* MAJOR LOOP SUBROUTINE SEG 6,
		;*
		;* Log RAM to MCU, Cool A/D
		;*
		;*
		;*******************************************************
								;
								; Read coolant temp & correct
								; reading if failure occurs
								;
LF3B6: 	JSR LF8F9 				; TO SEG 4, Log RAM to H.U.
		LDAA #$40 				; A/D Ch 4, (Coolant Temp)
		JSR LF1BE 				; Go to to A/D
		PSHA 					; Save A/D Coolant Value
								;-------------------------------------------
								; LK UP COOL VALUE FM 2D TBL
								; 3840 Ohm COOL Table
								;
								;
								;-------------------------------------------
		LDX #$FEA7 				; INDEX 3840 ohm COLLANT Table
		BRSET $3C $80 LF3CF 	; BR IF b7, COP 2 not toggled
								; ... else
		BRCLR $30 $01 LF3CF 	; BR IF NOT b0, If 4K Res value in use
								; ... else
								;-------------------------------------------
								; LK UP COOL VALUE FM 2D TBL
								; 348 Ohm Table
								;
								;
								;-------------------------------------------
		LDX #$FEB8 				; INDEX 348 OHM Cool sensor Table
		BRA LF3D5 				; Lk Up Val
								;
LF3CF: 	ADDA #10 				; Wrap ?
		BCC LF3D5 				; BR IF NO OVER FLOW
								; ... else
		LDAA #255 				; limit A/D VAL, (lk Up Var)
LF3D5: 	JSR LF2C6 				; Call 2d Lk Up, (No Offset)
		STAA $5D 				; Save For ALDL
		CMPA #120 				; 50 Deg C
		BHI LF3E7 				; If > 50 C Assume 348 Ohms
								; ... else
		CMPA #106 				; 39.5 C
		BHI LF3EA 				; If 39.5 to 50 C assume No Change
								; ... else
		BCLR $30 $01 			; Use 4 K Ohms
		BRA LF3EA 				;
LF3E7: 	BSET $30 $01 			; Use 348 Ohms
LF3EA: 	BCLR $3C $80 			; Reset COP2 Cool Flag
								;
								; Error #14 Hi Temp Failure
								;
		PULA
		BRSET $3B $08 LF42A
		PSHA
		LDAA $5D 				;
		LDAB #$20 				;Do Error #14
		CMPA LC1E1 				;If Cool < Calib ck Timmer 1st.
		PULA
		BCC LF404
								;
								; Error #15 Low Temp Failure
								;
		LDAB #$10 				; Do Err #15
		CMPA LC1E5
		BLS LF41A 				; If Cool > Calib, Filter
								; ... else
LF404: 	LDX $1A 				;
		CPX LC1E3 				; Ck Eng Run time
		BLS LF412
								; ... else
		BSET $35 $02 			; SET b1, ERR 14/15 THIS START UP
		ORAB $4C
		STAB $4C
LF412: 	LDAA LC1E2 				; Use Default Cool Fm Equate
		CLRB
		STD $5B 				; COOLANT
		BRA LF42A
LF41A: 	LDAA $5D
		BRCLR $3B $10 LF42B
								; ... else
		LDAB LC39D 				;Cool Temp Coef fm ROM
		LDX $5B 				; COOLANT
		JSR LF250
		STD $5B 				; COOLANT
LF42A: 	RTS
LF42B: 	CLRB
		STD $5B 				; COOLANT
		STAA $5F
		BSET $3B $10
		RTS
								;
								; Intilize BLM Table
								;
LF434: 	CLRB 					;
		LDAA #$80 				; Get Default Val
		STD $0A 				; Err #5 Flg Word
		STD $0C 				; Sty Alive Mem Cell B
LF43B: 	LDX #$10 				; Init BLM Cells
LF43E: 	DEX 					;
		STAA $1C,X			
		CPX #0			
		BNE LF43E 				;
								; ... else
		RTS 					;
								;
								; CK LOW LMT FOR SAM CELL'S 0 & B
								;
LF447: 	LDAA $0C 				; Sty Alive Mem Cell B
		LDX #$C5E8 				; 118, Low Lmt for other SAM Cell's
		BSR LF465 				;
		STAA $0C 				; Sty Alive Mem Cell B
		BSR LF43B 				;
		LDAA $0A 				;
		LDX #$C5E6 				; 118, Low Lmt for SAM Cell 0
		BSR LF465 				;
		STAA $0A 				;
		LDX #$001C 				; POINT TO BLM CELLS
		LDAB LC5EA 				; 0, SAM Cell A Number, Idle
		ABX 					; ADD TO BLM POINTER
		STAA 0,X 				;
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* LIMIT
		;*
		;* Limit Bounds on 8 Bit Interger
		;* (Upper & Lower)
		;*
		;* Call with:
		;* A = Input Arg
		;* X = Bound
		;* Return:
		;* A
		;*******************************************************
LF465: 	
		CMPA 0,X 				; Is A reg > Low limit ?
		BHI LF46B 				; If yes
								; ... else
		LDAA 0,X 				; If no Ld Low Limit
LF46B: 	CMPA 1,X 				; Hi Limit ?
		BLS LF471 				; If Yes, then return
		LDAA 1,X 				; If no Ld High Limit
LF471: 	RTS 					;
		;*******************************************************
		;*******************************************************
		;* FACTIN
		;* Factor in a 8 x 16 Multiply
		;*
		;*******************************************************
LF472: 	
		PSHA 					;Save Input Arg.
		LDAB 1,X 				;
		MUL 					;Mult LSB of 16 Bit Num
		PSHB 					;
		PSHA 					;
		TSY 					;
		LDAA 2,Y				; Set For 2nd Byte
		LDAB 0,X 				;
		MUL 					; Mult MSB of 16 bit Num
		ADDB 0,Y				; Add LSB result to MSB result
		ADCA #0 				;
		ROL 1,Y					; Shift MSB for X 2 Mult
		ROLB 					; Prior result LSB
		ROLA 					;
		BCC LF48F 				;
								; ... else
		LDD #$FFFF 				; Max Limmit of Exit Val
LF48F: 			
		STD 0,X 				;
		ROL 1,Y					;Clr Cy
		PULY 					;
		INS 					;
		RTS
		;*******************************************************
		;*******************************************************
		;* Set Up MCU CSR
		;* Enter as required to operate LSB of CSR
		;*
		;* Entry:
		;* LF4A8 To turn ON a bit
		;* LF49A To turn OFF a bit
		;*******************************************************
								;
								; To turn ON a BIT
								;
LF498: 	LDAA #$F7 				; 1111 1000, Sw on Err Lamp
								;
								; To turn OFF a BIT
								;
LF49A: 	PSHX 					;
		LDX $3FFC 				; MCU CSR, (Turn off a CSR bit)
		PSHX 					;
		TSY 					;
		ANDA 1,Y			
		BRA LF4B2 				;
								;
								; TURN OFF ERR LAMP
								;
LF4A6: 	LDAA #$08 				; b3, Turn off err lamp
								;
								; Turn On a CSR bit
								;
LF4A8: 	PSHX 					;
		LDX $3FFC 				; MCU CSR (Turn On a CSR bit)
		PSHX 					;
		TSY 					;
		ORAA 1,Y				;
LF4B2: 	ANDA #$FE 				; MCU test normal
		ORAA #2 				;
		STAA 1,Y				;
		PULA 					;
		ORAA #$FB 				; All Bits, (0-7) Enable
		PSHA 					;
		PULX 					;
		STX $3FFC 				; MCU CSR
		PULX 					; Restore X
		RTS			
								;
								; Toggle ECU byte 1
								;
LF4C3: 			
		LDX $3FFC 				; MCU CSR
		PSHX 					;
		TSY 					;
		ORAA 0,Y			
		BRA LF4D7 				;
LF4CE: 			
		LDX $3FFC 				; MCU CSR
		PSHX 					;
		TSY 					;
		ANDA 0,Y				; Reset bit
LF4D7: 			
		STAA 0,Y				;
		PULX 					;
		STX $3FFC 				; MCU CSR
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* IAC
		;*
		;* PID IDLE CONTROL
		;*
		;*******************************************************
LF4DF: 	LDX #$C62E 				; INDEX IAC Tables & Params
		LDY #$4004 				; Use Y as Port Addr for IAC
		JSR LF231 				; Get IAC motor State
		ASL $F3			
		LDAB $37 				; P/N status in Bit 0
		RORB 					; Xfer P/N status to Bit 7
		ROR $F3
		BRSET $4E $01 LF502 	; Br if M51
		LDAB $33 				; MINOR LOOP MD WD 1
		BITB #$10 				; b4, (1 = IGN ON)
		BNE LF511 				; If not B/P open loop & T/F
								; ... else
		LDAB $F3 				;
		BITB #4 				; Is motor reset done ?
		BNE LF511 				; If not B/P open loop & T/F
								; ... else
LF502: 			
		LDAB $34 				; Get Eng Status fn minor LP MW
		BMI LF514 				; If running, GT open loop & T/F
								; ... else
		LDAB $35 				; MNR LOOP MD WD 2
		BITB #$10 				; b4, DIAG SW IN DIAG MODE
		BEQ LF511 				; BR IF NOT b4
								; ... else
		LDAB #$81 				; If in Diag, Extend motor
		STAB $0101 				; Save Val as cmd'ed steps
LF511: 	JMP LF5B0 				; & Step the motor
								;
								; IDLE SPEED OPEN LOOP
								;
LF514: 	
		LDAB $7E 				; Get Present battery volts
		CMPB #$AB 				; Is it >17.5 V ?
		BHI LF51E 				; If so shut off Motor
		CMPB #$5A 				; Is battery low ?
		BHI LF521 				; If not, go check engine
								;
								; DO START UP ROUTINE
								;
LF51E: 	JMP LF640 				;
LF521: 	LDAB $F3 				; Is startup delay over ?
		BITB #$0008 			; If so then B/P Delay
		BNE LF550 				;
		LDAB LC630 				;
		ADDB $010C 				;
		BCS LF538 				; Ck for overflow
		BRSET $37 $80 LF53A 	; If A/C requested
		ADDB LC631 				; Startup offset
		BCC LF53A 				; BR if no overflow on add
LF538: 	LDAB #$00FF 			; Limit to 255
LF53A: 	JSR LD3DE 				; Jmp to Cmd down Steps
		LDAA $0104 				; Get the delay counter
		INCA 					; Update the delay
		CMPA 7,X 				; Comp dlt to max desired
		BLS LF5AD 				; If dly to short,B/P open loop
		LDAB $F3 				;
		ORAB #8 				; Set dly over
		STAB $F3 				; save mode word
		CLR $0104 				; R/S dly counter after startup
		BRA LF573 				;
								;--------------------------------------------------
LF550: 	BITB #$0020 			; PID commanding
		BNE LF5B0 				;
		LDAB $F2 				; IAC Cnt'l Word
		BITB #$10 				; In the T/F mode ?
		BNE LF562 				; If so issue a step
		BITB #08 				;
		BNE LF5B0 				;
		BITB #$4 				; Open loop step needed ?
		BNE LF573 				; If not, B/P step issue
LF562: 	LDAA $0101 				; Steps being requested ?
		BPL LF573 				; If so,goto Trajectory Decay
								;
								; TRAJECTORY DECAY ROUTINE
								;
		BITB #$20 				; A/C Steps being removed ?
		BNE LF573 				; If so, B/P decay
		DEC $0104 				; Dec decay step timer
		BEQ LF573 				; & if ready step motor
		JMP LF67B 				; Else return with no step
								;---------------------------------------------------
LF573: 	LDAA $30,X 				; Get neutral delay
		LDAB $F3 				; R/S decay timer
		BMI LF58B 				; BR if not in drive
		LDAA $65 				; Mul MPH * 8, limit to 32
		CMPA #$20 				;
		BCS LF581 				;
		LDAA #$FF 				;
LF581: 	ASLA 					;
		ASLA 					;
		ASLA 					;
		LDAB $32,X 				; Mul by drive delay Mult.
		MUL 					;
		ADDA $31,X 				; Add to the drive delay
		BCS LF5A8 				;
LF58B: 	LDAB $F2 				; IAC Cnt'l Word
		BITB #4 				; Ck if sys in open loop ?
		BNE LF5AD 				; If no,BR arround O/L Calc's
		ASLA 					;
		BCS LF5A8 				;
		PSHA 					;
								;---------------
								; Form RPM Error
								;---------------
		LDAB $58 				; RPM/12.5
		SUBB $0102 				; & sub desired speed
		CLRA 					;
		ASLD 					;
		ASLD 					; Divide by Max Error of 800 RPM
		TSTA 					;
		BNE LF5AC 				; B/P > of decay Count for -
								;----------------------
								; Increase Decay Counter
								;----------------------
		COMB 					; B = 1 - pct RPM error
		LDAA $33,X 				; Max delta count's
		MUL 					;
		PULB 					; Get base count
		ABA 					; Add delta to base
		BCC LF5AD 				; Limit to max Num of minor loops
								;-- per step
LF5A8: 	LDAA #$00FF 			;
		BRA LF5AD 				; Store Delta
LF5AC: 	PULA 					; Resore base decay
LF5AD: 	STAA $0104 				; Save base decay
		;*******************************************************
		;* MOVE IAC MOTOR
		;*
		;* Accumulator A will contain MCU1io, and must not
		;* be changed.
		;*
		;* Accumulator B is used as GP scratch Pad.
		;*******************************************************
								;
								; NORMAL MOVE IAC MOTOR
								;
LF5B0: 	BCLR $F3 $01 		;
								;
								; CK IF HEADS UP CONNECTED
								; U/D Motor posit fm HUD
								;
		LDAA $33 				; MINOR LOOP MD WD 1
		BPL LF5BA 				; If no, B/P 12U update
								; ... else
		JSR $5803 				; TO HEADS UP <---------
								;
								; CK IF IN ALDL MODE 4 <----*****
								; MODE WD ... and MD WD ...
								; ECM TYPE $32
								; IAC
								;
LF5BA: 	BRCLR $35 $08 LF5DF 	; BR IF NOT IN ALDL MODE 4
								; ... else
		LDAA $0158 				;
		BITA #$01 				; b0,
		BEQ LF5DF 				; BR IF NOT b0
								; ... else
		BITA #$02 				; b1,
		BNE LF5DF 				; BR IF NOT b1
								; ... else
		LDAA $2C 				; IAC Present Posit
		SUBA $0159 				;
		BCC LF5D1 				;
								; ... else
		NEGA 					;
LF5D1: 	BPL LF5D5 				;
								; ... else
		LDAA #$7F 				; 0111 1111
LF5D5: 	BCS LF5D9 				;
								; ... else
		ORAA #$80 				; SET b7
LF5D9: 	STAA $0101 				;
		BSET $F3 $01 			; SET b0
LF5DF: 	JSR LF231 				; Get Motor status
		LDAB $0101 				; Get num of CMD steps
		ASLB 					; Any to do ?, -->
								; - put sel bits in Carry
		BEQ LF619 				; If no, put PWM the motor coils
								; (12U mod FAN ?)
		BITA #$0004 			; Is step motor on ?
		BEQ LF63A 				; If no, toggle on & -->
								; - wait 6.25 MS
		DEC $0101 				; If yes, cnt this CMD step
		LDAB $2C 				; IAC Present Posit
		BCC LF5FB 				; If C=0, then Retract Pintel
								;
								; EXTEND PINTEL CODE
								;
		BEQ LF5F6 				; If Zero, stay in loop
		DECB 					; Adj pressent motor posit.
LF5F6: 	DEC $010A 				; Dec running counter
		BRA LF604 				; Update A/C & T/F flags
								;
								; RETRACT PINTEL CODE
								;
LF5FB: 	CMPB $4D,X 				; Limit motor posit.
		BCC LF619 				; Inc running counter
		INCB 					; If < max Update posit.
		INC $010A 				;
		CLC
								;
								; UPDATE A/C LEARING & THROTTLE FOLLOWER REG'S
								;
								;
LF604: 	STAB $2C 				; IAC Present Posit
		LDAB $F2 				; IAC Cnt'l Word
		BITB #$0010 			; Is Throt follower active
		BEQ LF619 				; If no T/F, Ck Kwarm or Fan
		LDAB $0105 				; Get the T/F Learn ARG.
		INCB 					; Assume step taken is retract
		BCC LF616 				; If step is retract, B/P DEC
		DECB 					; Get bk orig Val of T/F Delta
		BEQ LF616 				; if orig = 0, then B/P DEC.
		DECB 					; DEC, actual value of delta
LF616: 	STAB $0105 				;
LF619: 	LDAB $F2 				; IAC Cnt'l Word
		BITB #$08 				; b3
		BEQ LF622 				; BR IF NOT b3,
		INC $010E 				;
LF622: 	LDAB $2C 				; IAC Present Posit
		CMPB $4D,X 				;
		BLS LF62D 				;
		LDAB $0101 				;
		BPL LF63A 				;
								;
								;MOVE MOTOR 1 STEP
								;
LF62D: 	LDAB $010A 				; RUNING COUNTER
		LDX #$FEC9 				; STEP TABLE
		ANDB #$03 				; MASK FOR 2 LOWEST BITS
		ANDA #$FC 				; MASK OUT A & B BITS
		ABX 					; ADD TO BASE ADDRES
		ORAA 0,X 				; INSET NEW BITS
								;
								; PW MODULATE MOTOR (100pct. D.C.)
								;
								;
								; IF YES TURN STEPPER ON
								;
LF63A: 	BSET 0,Y 04 			;
		BRA LF644 				;
								;
								; IF NO TURN STEPPER OFF
								;
LF640: 	BCLR 0,Y 04 			;
LF644: 	JSR LF242 				; UP DATE THE PORTS
								;------------------------
								; BASE BIAS DETERMINATION
								;-----------------------
		LDAB $0109 				;
		ADDB $0105 				;
		BCC LF651 				;
		LDAB #$00FF 			;
LF651: 	PSHB 					;
		LDAB $010E 				;
		LDAA $3B 				;
		ASLA 					;
		PULA 					;
		BPL LF65C 				;
		NEGB 					;
LF65C: 	ABA 					;
		TAB 					;
		LDAA $2C 				;IAC Present Posit
		SBA 					;
		STAA $010F 				;
		LDAB $2C 				;IAC Present Posit
		SUBB $010F 				;
		LDAB #$0080 			;
		BCC LF673 				;
								; ... else
		CBA 					;
		BHI LF678 				;
								; ... else
		INCB 					;
		BRA LF677 				;
LF673: 	CBA 					;
		BCS LF678 				;
								; ... else
		DECB 					;
LF677: 	TBA 					;
LF678: 	STAA $010F 				;
								;--------------------------------------------------
								; MAIN INJECTION ROUTINE
								;
								; 1. Cks cyl Select, Set Err If mismach
								; 2.
								; 3.
								; 4.
								; 5.
								; 6.
								;
								;--------------------------------------------------
LF67B: 	BRSET $A0 $40 LF682 	;
		JMP LF76A 				; Exit if no inject
LF682: 	LDAB $2F 				;
		ANDB #$18 				; 0001 1000
		CMPB LC225 				; 0d, Fuel mode FLAG, 0 = 8 Cyl PFI
		BEQ LF68E 				; BR IF Z, (0 = 8 Cyl PFI)
								; ... else
		BSET $40 $01 			; SET b0
LF68E: 	BRSET $34 $80 LF695 	; BR IF b7 (ENG RUNNING)
								; ... else
		JMP LF71C 				; If Eng Not running
LF695: 	BRCLR $0E $FF LF6D5 	; A/F Ratio time out
								; ... else
		LDD $10 				; Blk Lrn Mult Values
		BEQ LF6A4 				;
								; ... else
		SUBD #$01 				;
		STD $10 				; BLM Values
		BRA LF6D5 				;
LF6A4: 	LDAA $CC 				; AFR Time out Cntr
		BEQ LF6AD 				; Skip if <> 0
								; ... else
		DECA 					;
		STAA $CC 				; AFR Time out Cntr
		BRA LF6D5 				;
								;--------------------------------------------------
								; LK UP Start up Enrichment Decay Rep Rate Vs Start
								; up Coolant Temp
								;
								; Tbl Val = Num of Injects
								;---------------------------------------------------
LF6AD: 	LDAA $5F 				;
		CMPA #208 				; CK FOR UPPER TEMP LK UP
		BLS LF6B5 				; BR IF COOL LT 116c
								; ... else
		LDAA #208 				; LMT TO 116c FOR LK UP
LF6B5: 	PSHA 					;
		LDX #$C402 				; Start up Enrichment Decay tbl
		JSR LF2C6 				; 2d Lk Up
		STAA $CC 				; AFR Time out Cntr, Fm Tbl
								;--------------------------------------------------
								; Start up Enrich Decay amt Vs Start up Cool Temp
								;
								; Tbl Val = Pct Chg * 655.36
								;--------------------------------------------------
		PULA 					; Get Lk Up Var
		LDX #$C410 				; Start up Enrich Decay amt Tbl
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		PSHA 					; Save result to stk
		LDD $0E 				; A/F Ratio time out TIMER
		TSX 					;
		SUBB 0,X 				;
		SBCA #0 				;
		BCC LF6D2 				; SAVE TIMER
								; ... else
		LDD #0 					; ZERO OUT AFR TIMER
LF6D2: 	STD $0E 				; A/F Ratio time out
		PULA 					;
LF6D5: 	BRSET $36 $20 LF71C 	; BR IF b5, (BLEND SPK)
								; ... else
		LDD $15 				; Spark Decay Delay
		BEQ LF6E4 				;
								; ... else
		SUBD #$01 				;
		STD $15 				; Spark Decay Delay
		BRA LF71C 				;
LF6E4: 	BRCLR $3B $80 LF6F1 	;
		LDAA $12 				; Sprk Adv Fm Blend Finished
		BEQ LF6F1 				;
								; ... else
		DECA 					;
		STAA $12 				; Sprk Adv Fm Blend Finished
		BRA LF71C 				;
								;--------------------------------------------------
								;-------------------------------------------
								; LK UP START UP SPARK ADVANCE DECAY TIME
								;
								; TBL = INJECTS
								;--------------------------------------------
LF6F1: 	BSET $3B $80 			; SET b7
		LDAA $5F 				; GET CURRENT COOLANT
		CMPA #208 				; 115c COOL
		BCS LF6FC 				; BR IF COOL LT 115c
								; ... else
		LDAA #208 				; USE MAA COOL, 115c
LF6FC: 	PSHA 					; Save LMT'ED COOL TO STX
		LDX #$C198 				; START UP SPARK ADVANCE DECAY Tbl
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		STAA $12 				; START UP Sprk Adv DECAY TIME
								;--------------------------------------------------
								;--------------------------------------------------
								; LK UP Startup Spk Adv Decay vs Startup Coolant
								;
								; TBL = Deg * 256 * 256 / 90
								;--------------------------------------------------
		PULA 					; GET LMT'ED COOLANT FM STX
		LDX #$C1AB 				; Startup Spk Adv Decay Tbl
		JSR LF2C6 				; 2d Lk Up, (No Offset)
		PSHA 					; Save result TO STX
		TSX 					; STX TO X
		LDD $13 				; Spark Adv, (STARTUP)
		SUBB 0,X 				;
		SBCA #0 				;
		BCC LF719 				;
								; ... else
		LDD #0 					; ZERO SPK
LF719: 	STD $13 				; SPARK Adv, (STARTUP)
		PULA 					;
LF71C: 	BRCLR $39 $10 LF726 	; BR IF NOT b4, (TOGGLE SF F/F)
								; ... else
		LDAA $39 				; SINGLE FIRE MD WD
		EORA #$40 				; TOGGLE b6, (O PW)
		STAA $39 				;
LF726: 	LDAA $3D 				;
		EORA #$01 				; TOGGLE b0
		STAA $3D 				;
		BSET $38 $14 			; SET b4 & b2
		LDX $C2 				;
		STX $D5 				; Base PW, Last Inj
		LDD $011A 				; ACCEL ENR PW
		ADDD $011C 				;
		STD $011A 				; ACCEL ENR PW
		BRCLR $34 $80 LF748 	; BR IF NOT b7
								; ... else
		INC $018D 				;
		BNE LF748 				;
		DEC $018D 				;
LF748: 	BRSET $3E $20 LF750 	; BR IF b5
								; ... else
		BRCLR $38 $40 LF758 	; BR IF NOT b6
								; ... else
LF750: 	LDAB $E0 				; Num of Inj,s simce Start Of Acel Enr
		INCB 					; INC NUM INJ'S
		CMPB LC314 				; Injects in fuel limiting
		BCS LF768 				; BR IF NUM INJ'S LT LMT THRESH (2)
								; ... else
LF758: 	BRSET $3E $80 LF76A 	; BR IF b7
								; ... else
		BCLR $38 $40 			; CLR b6
		BCLR $3E $20 			; CLR b5
		CLRB 					; ZERO OUT NUM ASYN PULSES
		STAB $DF 				; NUM OF ASYNC PULSES SINCE ACEL ENR
		CLRA 					;
		STD $D9 				; ACCEL ENRICH, ACUM FUEL
LF768: 	STAB $E0 				; NUM OF INJ,S SINCE START of ACCEL ENR
		;*******************************************************
		;* Calc Air flow Fm MAF Air meter
		;*
		;* ESTABLISH 1 of 2 TYPES OF MAF, ANALOG (0 5 VDC)
		;* OR DIFITAL ) 32 Hz to ___ Hz)
		;*******************************************************
								;
								; CK AFR MD WD1 FOR ANALOG MAF
								;
LF76A: 	LDAA LC014 				; Fuel/Air Mode Wd 1
		BITA #$04 				; b2, ANALOG MAF
		BNE LF7AC 				; BR IF b2, (ANALOG MAF )
								; ... else
								;
								; DIGITAL (FM) MAF
								; (OLD BOSCH TYPE)
								;
		LDD $ED 				; 8 cntr Lps MAS
		SUBD $3FF8 				; CNT'r
		BNE LF790 				; BR IF NZ
								; ... else
		LDAB $33 				; MINOR LOOP MD WD 1
		BITB #$10 				; b4, (1 = IGN OFF) Skip Err 34 Ck
		BNE LF790 				;
								; ... else
		LDAA $EC 				; Num Lps w/o MAF input
		CMPA LC209 				; 6.4 SEC'S, MIN TIME W/O SIGNAL
		BHI LF788 				; Set ERR Flg
								; ... else
		INCA 					; BUMP UP TIMER
		BRA LF7B9 				;
LF788: 	LDAB $3F 				;
		ORAB #$40 				; Set b6, Err Flg #34, LO MAF VALUE
		STAB $3F			
		BRA LF7C1 				;
								;
								; UPDATE COUNTER VALUES AFTER MAF PULSE OCOURRANCE
								;
								;
LF790: 	LDD $ED 				; 8 cntr Lps MAS
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDD $3FF8 				; CNT'R
		STD $ED 				; 8 cntr Lps MAF
		SUBD 0,X 				; Calc Diff in period
		PULX 					;
		STD $EF 				; Diff Val to calc Air Flow
		LDD LC6E8 				; 2048d, Min Out Val for MAF, (32 Hz)
								; CALIB = 65536/hZ
		SUBD $EF 				; Diff Val to calc Air Flow
		BCC LF7A8 				; BR IF NO UNDEFLOW
								; ... else
		CLRA 					; ZERO CNT'R VALUE
		CLRB 					;
LF7A8: 	STD $EF 				; USE DIFF CNT'R VAL TO CALC air FLOW
		BRA LF7B8 				; CLR NO PULSE CNTR & CALC AIR FLOW
		;***************************************************
		;***************************************************
		;* ANALOG MAF
		;*
		;* MULT ANALOG VAL x 7 TO LOOK
		;* LIKE FM MAF AND USE FM TABLES
		;***************************************************
LF7AC: 	LDAA #$A0 				; A/D Ch A, Mass Air Sensor
		JSR LF1BE 				; To A/D subroutine
		STAA $ED 				; MAF A/F VALUE
		LDAB #7 				;
		MUL 					; 7 * MAF A/D VALUE
		STD $EF 				; Val to calc Air Flow
		;***************************************************
LF7B8: 	CLRA 					;
LF7B9: 	STAA $EC 				; Num Loops w/o MAF input
								;
								; Calulate Air Flow
								;
		LDAB $3F 				;
		ANDB #$C0 				; b7 & b6
		BEQ LF7EE 				; BR IF NOT b7 & b6
								; ... else
								;
								; Make Default Airflow
								;
								;--------------------------------------------------
								; LK UP Default air flow offset per pct TPS vs RPM
								; P/O ERR 33/34
								;
								; Value = Gms/Sec * 100 pct, (Offset)
								;
								; TBL = Arg * 100
								;-------------------------------------------------
LF7C1: 	LDAA $56 				; RPM/25
		LDX #$C217 				; Default air flow offset TBL
		JSR LF2BF 				; 2d Lk Up Routine
		LDAB LC213 				; If TPS > 65.2%, use for MAF Default
		CMPB $82 				; TPS Ld Axis Var
		BCS LF7D2 				; BR IF TPS LT THRESH
								; ... else
		LDAB $82 				;
LF7D2: 	MUL 					;
		STD $EA 				; Gms/Sec Disp Value
								;--------------------------------------------------
		LDAA $2C 				; IAC Present Posit
		LDAB LC214 				; Gms/sec Scale factor
		MUL 					;
		ADDD $EA 				; Gms/Sec Disp Value
		BCS LF7E4 				;
								; ... else
		ADDD LC215 				; 4 Gms/Sec default air flow offset
		BCC LF7E7 				;
LF7E4: 	LDD #$FFFF 				; Max Val if Overflow
LF7E7: 	JMP LF86E 				;
								;--------------------------------------------------
								;
								; Table For ADD overflows
								;
								;
LF7EA: 	FCB $00			
		FCB $40			
		FCB $80			
		FCB $C0			
								;-------------------------
								; Scale Raw MAF VAL'S
								;
								;-------------------------
LF7EE: 	TSTA 					;
		BEQ LF7F4 				;
								; ... else
		JMP LF87F 				;
LF7F4: 	LDD $EF 				; Diff Val to calc Air Flow
		CMPA #6 				;
		BLS LF7FD 				;
								; ... else
								;----------------------------------------
								; Mass Air Flow TABLE 1
								;
								; TBL = gms/Sec * 11.1
								;
								;----------------------------------------
		LDD #$06FF 				;
LF7FD: 	LDX #$C69D 				; TABLE SCALAR Mass Air Flow TABLE 1
								; Table SCALER Multiplier
		CMPA #$02 				;
		BCC LF807 				;
								; ... else
		LSRD 					; Div by 2
		LDAA #1 				;
LF807: 	PSHB 					;
		LDAB #11 				;
		DECA 					;
		MUL 					;
		ABX 					;
		PULA 					; Val for Lk Up
		LDAB 0,X 				;
		PSHB 					;
		INX 					; ADJ Tbl Addr
		JSR LF2BF 				; Call 2d Lk Up W/ Interp
		PULB 					;
								;
								; Mass Air FLow Limit Ck's
								;
		MUL 					; To Make Gms/Sec Disp value
		PSHB 					;
		PSHA 					;
		STAA $B6 				;
		LDX LC014 				; AFR MD WD 1
		PULX 					;
		BPL LF862 				;
		PSHX 					;
								;
								; Up Date Air Filter tbl
								;
		LDX #$0075 				; OLD AIR FLOW
		LDAB $7D 				; Current Air flow
		INCB 					;
		STAB $7D 				; Current Air flow
		ASLB 					; Mult by 2
		ANDB #$07 				; Lmt to 7 bytes
		ABX 					; Point X to Value to Replace
		PULA 					; Get NewSPK Unlimited Value
		PULB 					;
		STD 0,X 				; Save In Avg Buffer
								;-------------------------------
								; CALC MASS AIR FLOW TERM
								;
								;-------------------------------
		LDX #$00EA 				; Gms/Sec Disp Value
		LDAA LC6E7 				; Mult for sliding filter
		NEGA 					;
		JSR LF266 				;
		STD $EA 				; Gms/Sec Disp Value
								;
								; SUM UNLIMITED READS FOR NEW VALUE
								;
		LDX #$F7EA 				;
		LDD $75 				; OLD AIR FLOW
		ADDD $77 				;
		BCC LF848 				;
								; ... else
		INX 					;
LF848: 	ADDD $79 				;
		BCC LF84D 				;
								; ... else
		INX 					;
LF84D: 	ADDD $7B 				;
		BCC LF852 				;
								; ... else
		INX 					;
LF852: 	LSRD 					; Avg By dividing by 4
		LSRD 					;
		ORAA 0,X 				; Add in Overflow
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDAA LC6E7 				; Mult for sliding filter
		JSR LF266 				; To 8 x 16 Mult routine
		PULX 					;
		ADDD $EA 				; Gms/Sec Disp Value
LF862: 	STD $EA 				; Gms/Sec Disp Value
								;
								; Max Flow Vs RPM
								;
		LDX $EA 				; Gms/Sec Disp Value
		CPX LC6EA 				; Min Allowable Flow, (3 gms/Sec)
		BCC LF870 				; GO CK AGANST MAX FLOW vs RPM
								; ... else
		LDD LC6EA 				; USE Min Allowable Flow, (3 gms/Sec)
LF86E: 	STD $EA 				; gms/Sec Disp Value
								;--------------------------------------------------
								; LK UP Max Air Flow vs RPM
								;
								; 17 LINE, 0 -> 6400 RPM
								;
								; TBL = Grams/Sec
								;--------------------------------------------------
LF870: 	LDX #$C6EC 				; Max Air Flow Tbl
		LDAA $57 				; RPM/25
		JSR LF2C6 				; 2d Lk Up, (No Offset)
								;
		CMPA $EA 				; Gms/Sec Disp Value
		BHI LF87F 				; BR IF MAX fm tbl LT ACTUAL Gms/Sec
								; ... else
		CLRB 					;
		STD $EA 				; Gms/Sec AIR FLOW
LF87F: 	RTS
		;***************************************************
		;***************************************************
		;* (SCI) Serial Data Status
		;*
		;* 160 BAUD ???
		;*
		;***************************************************
LF880: 	LDAA $0189 				; ALCL NUM BITS TO TX
		BNE LF8D9 				; BR IF NON Z
								; ... else
		LDAA #$08 				;
		STAA $0189 				; ALCL NUM BITS TO TX
								;
		LDAA $47 				;
		BPL LF8B6 				; TO 160 BAUD TBL MANAGER
								; ... else
		LDAA $48 				;
		BNE LF895 				; NR IF NZ
								; ... else
		JMP LF8F8 				; EXIT via RTS
								;-------------------------------------------------
LF895: 	LDX #$FC62 				;
		LDAB #16 				;
		MUL 					;
		ABX 					;
		LDAB $018A 				; LOCATION OF NEXT ENTRY IN EPROM TBL
		CMPB #16 				;
		BCC LF8CF 				;
								; ... else
		LSRB 					;
		BCC LF8AB 				;
								; ... else
		LDAA $017A 				;
		BRA LF8ED 				;
								;-------------------------------------------------
LF8AB: 	ASLB 					;
		ABX 					;
		LDX 0,X 				;
		LDD 0,X 				;
		STAB $017A 				;
		BRA LF8ED 				;
								;-------------------------------------------------
								;--------------------------------------------------
								; 160 BAUD DIAGNOSTICS
								;
								; MINOR LP MD WD 2
								; b4 = DIAG SW IN DIAG MODE
								; b5 = DIAG SW IN ALDL MODE
								;--------------------------------------------------
LF8B6: 	LDAA $35 				; MINOR LP MD WD 2
		LDAB $018A 				; LOCATION OF NEXT ENTRY IN EPROM TBL
		BEQ LF8ED 				; BR IF Z
								; ... else
		ANDA #$30 				; MASK FOR b4 & b5
		BNE LF8CB 				; BR IF b4 & b5
								; ... else
								;-------------------------------------------------
								; DIS ADDRESSES TABLE, Xmit
								; (7 ADDRESS'S)
								;
								;-------------------------------------------------
		CMPB LC6FD 				; 4d, DISP STRG LENGTH
		BHI LF8CF 				;
								; ... else
		LDX #$C6FE 				; DIS ADDRESSES TABLE
		BRA LF8E6 				;
								;-------------------------------------------------
								;-------------------------------------------------
								; 160 BAUD DIAGNOSTICS
								;
								;
								;-------------------------------------------------
LF8CB: 	CMPB #25 				;
		BNE LF8E3 				; TO 160 BAUD DIAGNOSTICS
								; ... else
LF8CF: 	CLRA 					;
		STAA $018A 				; LOCATION OF NEXT ENTRY IN EPROM TBL
		DECA 					;
		STAA $018B 				; ALCL XMIT BYTE
		BRA LF8F8 				; EXIT via RTS
LF8D9: 	DEC $0189 				; ALCL NUM BITS TO TX
		ASL $018B 				; ALCL XMIT BYTE
		BCC LF8F3 				; Output Nxt byte
								; ... else
		BRA LF8F8 				; EXIT via RTS
								;-------------------------------------------------
		;*************************************************
		;* ALCL Xmit Table
		;* (160 BAUD DIAGNOSTICS)
		;* Table Of Addr's vs DATA
		;* (23 ADDRESS'S)
		;*
		;* TYPE $32 ECM P/N 1227165
		;*************************************************
LF8E3: 	LDX #$C70D 				; INDEX ADDRESS TABLE
LF8E6: 	DECB 					;
		ASLB 					;
		ABX 					; ADJ INDEX
		LDX 0,X 				; GET DATA RAM ADDRESS
		LDAA 0,X 				; GET DATA
LF8ED: 	STAA $018B 				; SAVE DATA TO ALCL XMIT BYTE
		INC $018A 				; LOCATION OF NEXT ENTRY IN EPROM TBL
LF8F3: 	LDAA #04 				; Ld Ser Data w/0, 350 usec
		JSR LF4A8 				; To turn ON a bit
LF8F8: 	RTS
		;*************************************************
		;***************************************************
		;* MJR LOOP SUBROUTINE SEG 4,
		;*
		;* LOG RAM ASSIGNMENT
		;*
		;***************************************************
								;
								; CK IF HEADS UP CONNECTED
								;
LF8F9: 	LDAA $33 				; MINOR LOOP MD WD 1
		BPL LF900 				; Exit if not H.U.
								; ... else
		JSR $5806 				; TO HEADS UP <------
								;-------------------------------------------------
LF900: 	RTS 					;
								;-------------------------------------------------
								; Fuel Out Subroutine
								;
								;-------------------------------------------------
LF901: 	LDAA $E2 				; DIFF Ld Val ACCEL ENR FACTOR
		BRCLR $38 $02 LF915 	; If ACCEL ENR FLG CLR
		BRCLR $38 $04 LF915 	; IF NO KiNJ SINCE LAST PULSE
		SUBA $E3 				; DIFF LlD VvAL FOR ACCCEL ENRICH
		BCC LF913 				; if > = 0
		CLRA 					; LIMIT TO ZERO
		BCLR $38 $08 			; CLR ACEL ENR FLAG
LF913: 	STAA $E2 				; Diff Ld Val Acel Enr Factor
LF915: 	BCLR $38 $04 			; Reset Inj Flg
		BSET $38 $02 			; Set 1st time Acel enr flg
		LDX #$C2 				; Get data
		JSR LF266 				; Scale w/8 X 16 Mult
		ADDD $C2 				; Add to data
		BCC LF928 				;
		LDD #$FFFF 				; Set Max limit
LF928: 	STD $C2 				;
								;---------------------------
								; Fuel Output Cnt'l
								;--------------------------
LF92A: 	BRCLR $03 $20 LF957 	; Mode Wd3, VATS OK
								; ... else
		BRSET $33 $10 LF957 	; BR IF b4, (1= IGN OFF)
								; ... else
		BRSET $43 $02 LF957 	; 0 if in Decel C/O
								; ... else
		BRCLR $34 $80 LF95E 	; B/P hi RPM & MPH C/O tst if
								; -> not running
								; ... else
		LDX #$C3A5 				; Hi fuel C/O limit
		LDAA $65 				;
		BRCLR $43 $40 LF946 	; If fuel not C/O
		LDX #$C3A8 				; Hi fuel C/O limit
LF946: 	BCLR $43 $40 			;
		CMPA 0,X 				; Is MPH > than lmt
		BHI LF954 				; If Yes
		LDD $95 				; Current Minor LP Ref Period fm MPU
		CPD 1,X 				; Is RPM > than limit
		BCC LF95E 				; If not
LF954: 	BSET $43 $40 			; Set Fuel cutoff flg
LF957: 	LDX #0 					; Set Acel Enr to 0
		STX $D9 				; Acel Enrich, Acum Fuel
		STX $C2 				;
LF95E: 	BRCLR $3E $A0 LF964 	;
		BRA LF96D 				;
LF964: 	BRCLR $38 $C0 LF999 	;
		LDX #$F9E5 				;
		BRA LF973 				;
LF96D: 	LDD LC5F8 				; Lmt Fuel Factor fm ROM
		LDX #$F9FC 				;
LF973: 	JSR 0,X 				;
		PSHX 					;
		PSHB 					;
		PSHA 					;
		PULX 					;
		PULA 					;
		PULB 					;
		PSHX 					;
		TSX 					;
		SUBD 0,X 				; Fuel Supplied - lmt < 0 ?
		PULX 					;
		BCS LF999 				;If so
		PSHB 					;
		PSHA 					;
		TSX 					;
		LDD $C2 				;
		SUBD 0,X 				;
		PULX 					;
		BCS LF992 				;
		CPD LC3A1 				;Min Base PW fm ROM
		BCC LF994 				;if so do 0 fuel
LF992: 	CLRA 					;
		CLRB 					;
LF994: 	PSHB 					;
		PSHA 					;
		PULX 					;
		BRA LF99B 				;
								;--------------------------------------------------
LF999: 	LDX $C2 				;
LF99B: 	BRCLR $39 $80 LF9A6 	; BR IF NOT b7, (IN SNGL FIRE MODE)
								; ... else
		BRCLR $39 $40 LF9A6 	; BR IF NOT b6, (0 PW)
		LDX #0 					;
LF9A6: 	STX $011C 				; Save Pending fuel for
								; -> acum fuel
		BEQ LF9D2 				; If 0 JMP AROUND OFFSET
								; ... else
		CPX #0256 				; 3900 usec
		BCC LF9C0 				; BR IF PW'S GT 3900 usec
								; ... else
		PSHX 					; TEMP SAVE PW TO STX
								;--------------------------------------------------
								;--------------------------------------------------
								; Inj Offset for Small PW's vs Base PW
								;
								; FOR PW'S LT 3.9 Msec
								;
								; TBL = Msec * 65.536 Msec
								;--------------------------------------------------
		PULA 					;
		PULA 					; GET LSB OF PW FM STX
		LDAB #32 				; Offset for Lk Up, (Min Lk up val)
		LDX #$C3BC 				; Table Addr
		JSR LF2B9 				; Call 2D Lk Up, (Low PW)
		TAB 					; COPY RESULTS TO B Reg
		LDX $011C 				; GET BPW
		ABX 					; ADD IN PW OFFSET
LF9C0: 	LDAB $CB 				; INJ OFFSET CORR, Msec * 65.636
		ABX 					; ADD IN PW OFFSET, (twice)
		ABX 					;
								;------------------------
								; Min Base PW check
								;------------------------
		BCLR $33 $02 			; b1, FORCE LOP PW, RESULT OPN LP
		CPX LC3A1 				; base PW
		BHI LF9D2 				;
		BSET $33 $02 			; Set LOW PW RESULT OPN LP
		LDX LC3A3 				; Default Base PW fm ROM
								;
								; Simu Fuel
								;
LF9D2: 	CPX #$7FFF 				;
		BCS LF9DA 				;
		LDX #$7FFF 				;
LF9DA: 	STX $3FD0 				; To MCU
		BRSET $39 $C0 LF9E4 	; BR IF b7 & b6, (IN SF & 0 PW)
								; ... else
		STX $0466 				; To Base PW
LF9E4: 	RTS 					;
								;--------------------------------------------------
								;--------------------------------------------------
								; Accel enrich Fuel Subroutine
								; INDEXED LK UP
								;
								;
								; Bits 0 - 3 Are after Next Inject
								; Bits 4 - 7 Trigger To Next inject
								;--------------------------------------------------
LF9E5: 	LDX #$C331 				; Fuel Lmt vs COOL
		LDAB $5B 				; COOLANT
		LSRB 					; Scale Coolant
		LSRB 					; COOL/32
LF9EC: 	LSRB 					;
		LSRB 					;
		LSRB 					;
								;
								; AFTER NEXT INJECT
								;
		ABX 					; ADJ TBL POINTER (0 - 9)
		LDAB 0,X 				; Get fuel lmt factor
		ASLD 					; Get 4 bits TO MSB
		ASLD 					;
		ASLD 					;
		ASLD 					;
		ANDB #$F0 				; 1111 0000, Mask, after Next Inject
								;
								; TRIGGER To NEXT INJECT
								;
		LDAA 0,X 				; Get fuel lmt factor FM TBL
		ANDA #$F0 				; 1111 0000, Mask, Trig To Next inject
		BRCLR $38 $10 LFA01 	; BR IF NOT b4
								; ... else
		TBA 					;
LFA01: 	LDX #$00C2 				;
		JSR LF266 				; Fuel Lmt x ?? (8 x 16 mult)
		ASLD 					;
		BCC LFA0D 				; BR IF NO OVERFLOW
								; ... else
		LDD #$FFFF 				; USE MAX LMT
LFA0D: 	PSHB 					; SAVE TO STX
		PSHA 					;
								;
		LDD $D5 				; Base PW, Last Inj
		ADDD $D9 				; Acel Enrich, Acum Fuel
		BCC LFA18 				; BR IF NO OVERFLOW
								; ... else
		LDD #$FFFF 				; USE MAX LMT
LFA18: 	PSHB 					; SAVE TO STX
		PSHA 					;
		PULX 					;
		PULA 					;
		PULB 					;
		RTS 					;
								;-------------------------------------------------
								;--------------------------------------------------
								; TPS ACCEL ENR ROUTINE
								;
								;--------------------------------------------------
LFA1E: 	LDAA $3F 				;
		ANDA #$24 				; b5 & b 2
		BNE LFA51 				; BR IF NZ
		LDAA $82 				; TPS Ld Axis Var
		SUBA $DD 				; Transient TPS
		BCC LFA32 				; BR IF TPS GT TRANSIENT TPS
								; ... else
								;
								; NEG TPS DIFF
								;
		COMA 					;
		CMPA LC30F 				; 0 Async Accel Enrich if Neg
								; Diff TPS > 3.9%
		BHI LFA51 				;
								; ... else
		BRA LFA57 				;
								;
								; POS TPS DIFF
								;
LFA32: 	CMPA LC30E 				; 3.9% Min Diff TPS for Pwr Enrichment
		BCS LFA51 				; BR IF TPS L.T. 3.9% Min Diff
		BRSET $38 $80 LFA4C 	; BR IF b7
								; ... else
		BCLR $38 $10 			; CLR b4
		LDX #$018E 				;
LFA41: 	CLR 0,X 				;
		INX 					;
		INX 					;
		CPX #$01AE 				;
		BNE LFA41 				;
								; ... else
		STAA $D4 				; DIFF TPS for ACEL ENRICH
LFA4C: 	BSET $38 $E0 			; SET b&, 6 & 5
		BRA LFA57 				;
								;---------------------------------------------
LFA51: 	CLR $DF 				; CLR Num of Async Pulses Since Accel Enr
		BCLR $38 $80 			; CLR b7, (IN TPS ACELL ENRICH)
LFA57: 	RTS 					;
		;***************************************************
		;***************************************************
		;* SERIAL DATA INIT SUBROUTINE (SCI)
		;*
		;*
		;***************************************************
LFA58: 	CLRA 					; CLEAR SCI RAM STUFF
		CLRB 					;
		STAA $012D 				; CLEAR STATUS FLAGS
		STD $0131 				; CLEAR DEVICE TBL POINTER (2 BYTES)
		STAA $012F 				; CLRAR GET BYTE COUNTER
		STAA $0130 				; CLEAR CK SUM
		LDX #$C73D 				; Get 1st ROM Tbl device
LFA69: 	PSHX 					; TEMP SAVE TBL DEV ADDRESS
		LDX 8,X 				; INDEX INPUT CNT'L BLOCK, (ICB)
		STAA 0,X 				; CLEAR DCB ....
		PULX 					; RESTORE ROM Tbl device
		PSHX 					; TEMP SAVE ROM Tbl device
		LDX 6,X 				; INDEX OUTPUT CNT'L BLOCK, (OCB)
		STAA 0,X 				; CLEAR DCB FLAGS
		PULX 					; RETREIVE TBL DEV ADDRESS
		LDX 0,X 				; INDEX NXT MSG ENTRY ADDRESS
		BNE LFA69 				; IF NOT LAST LINE
								; ... else
		RTS 					;
								;------------------------------------------------------
LFA7A: 	LDAA $4008 				; CLR TX/RX STATUS REG
		LDAA #$27 				; 0010 0111
								; Tx off, Rx on, RX Int on Wake up enable
		STAA $4007 				; SET SCI Cntl, (CSR)
		LDAA $4009 				; SERIAL DATA REG
								;
								; SET MPU CSR ....
								;
		LDD $3FFC 				; MPU CSR
		ORAB #04 				; SET b2
		JSR LF3B5 				; 11 usec DELAY
		STD $3FFC 				; MPU CSR
		RTS
		;***************************************************
		;***************************************************
		;* SERIAL DATA RECIEVE SUBROUTINE
		;*
		;* CALLED BY SCI INTERUPT
		;*
		;* CK'S FOR ERR'S, FRAME, OVER RUN ETC, IF ERR THEN
		;* FLAG ERR, SET SCI WAKE UP AND EXIT.
		;*
		;* IF NO SCI ERR, UP DATE RX CK SUM, CK RX BYTE CNT'R,
		;*
		;* IF 1st BYTE:
		;* CK vs ROM TBLS FOR A MATCH TO QUAL.
		;*
		;* SAVE INDEX ADDRESS IF A MATCH & INCR RX BNYTE CNT'R
		;* ELSE .... PUT SCI BACK TO SLEEP IF NO MATCH.
		;*
		;* IF 2nd BYTE:
		;* SUB OFF $55 TO GET BYTE COUNT.
		;*
		;* IF LAST BYTE:
		;* CK THE RX CK SUM, SW SCI TO TX AND RDY TO TX DATA
		;* IF BAD CK SUM (NZ), FLAG ERR & PUT SCI BACK TO SLEEP
		;***************************************************
LFA91: 	LDAB $4008 				; TX/RX STATUS REG
		LDAA $4009 				; SERIAL DATA REG
		LDX $0131 				; INDEX CURRENT I/O BUFFER
								;
								; CK FRAMING & OVERRUN ERRORS
								;
		BITB #$0E 				; Ck for OR or FRAME ERROR
		BNE LFB0E 				; BR IF ERROR
								; ... else
								;
								; UPDATE CKSUM
								;
		TAB 					; DATA TO B reg
		ADDB $0130 				; ADD DATA TO CK SUM
		STAB $0130 				; SAVE NEW CK SUM
								;
								; CK SCI BYTE CNT'R TO SEE WHICH BYTE IS EXPECTED
								;
		LDAB $012F 				; GET BYTE COUNTER
		BNE LFADD 				; BR IF NZ
								; ... else
								;
								; IF BYTE CNT = 0, THE INCOMMING BYTE S/B A DEVICE CODE
								;
		LDX #$C73D 				; INDEX EPROM TBL
LFAAD: 	CMPA 2,X 				; DEVICE CODE
		BEQ LFABE 				; IF CODE (Z)
								; ... else
		LDX 0,X 				; NXT ROM ADDR
		BNE LFAAD 				; LAST ADDR ?
								;
								; DEVICE CODE NOT FOUND
								;
		LDAA #$27 				; 0010 0111,
		STAA $4007 				; SLEEP CMD, TX/Rx CSR
		CLRB 					;
		JMP LFBA2 				; EXIT
								;
								; DEVICE CODE FOUND
								;
LFABE: 	STAA $016D 				; SAVE DEVICE CODE
		STX $0131 				; SAVE TBL POINTER
		LDAA $012D 				; GET STATUS FLGS
		ORAA #$40 				; SET b6, (Input in work)
		STAA $012D 				; RESTORE FLGS
		LDX 8,X 				; POINT TO INPUT CNT'L BLOCK (ICB)
		LDAA 0,X 				; GET ICB STATUS FLGS
		ANDA #$BF 				; INPUT NOT COMPLTETE
		ORAA #$80 				; b7, INPUT IN WORK
		STAA 0,X 				; RESTORE ICB FLAGS
		LDAA #$25 				; 0010 0101,
		STAA $4007 				; SET SCI CNTL, (CSR)
		BRA LFB08 				; GO INCR BYTE CNT & EXIT via RTS
LFADD: 	CMPB #01 				; IS SERIAL DATA 2nd DATA BYTE ?
		BNE LFAE8 				; BR IF NOT
								; ... else
								;
								; IF BYTE CNT = 1, INCOMING BYTE S/B
								; MSG LENGTH + $55
								;
		SUBA #$55 				; SUB DATA BIAS, (85d)
		STAA $0170 				; SAVE MSG LENGTH
		BRA LFB08 				; GO INCR BYTE CNT & EXIT via RTS
LFAE8: 	SUBB #02 				; SER BYTE CNT => DATA BYTE CNT
		CMPB $0170 				; IS SERIAL BYTE DATA (MSG LEN)
		BCC LFB10 				; BR IF NOT A DATA BYTE
								;
								; IF ( 0 <= $012F - BYTE CNT < NIN) THEN THE SERIAL BYTE
								; IS A SCI DATA BYTE, IS DROPED IN TO DATA BUFFER, BYTE CNT,
								; IN THIS CASE A 2: INI & DEVICE CODE.
								;
								; $012F IS THE SERIAL BYTE CNTR
								;
		TSTB 					;
		BNE LFB03 				;
								; ... else
		PSHB 					;
		TAB 					;
		CMPB #04 				;
		BHI LFAFA 				;
								; ... else
		ASLB 					;
		ABX 					;
LFAFA: 	LDAB #10 				;
		ABX 					;
		LDX 0,X 				;
		STX $0131 				; SCI TBL POINTER
		PULB 					;
LFB03: 	LDX 8,X 				; INDEX ICB
		ABX 					; ADJ POINTER
		STAA 1,X 				; SAVE SCI DATA BYTE
LFB08: 	INC $012F 				; INCR BYTE COUNTER
		JMP LFBA5 				; EXIT VIA RTS
								;-------------------------------------------------
LFB0E: 	BRA LFB6D 				; TO BRIDGE
								;
								; IF ( $012F - BYTE CNT >= NIN) THEN THE SERIAL BYTE
								; IS ASSUMED TO BE THE LAST BYTE, (CHKSUM).
								; CKSUM S/B ZERO, IF NO A CKSUM ERROR EXISTS
								;
LFB10: 	LDAB $0130 				; CKSUM
		BEQ LFB19 				; BR IF Z (OK CKCUM)
								;
								; BAD CKSUM, SET CK SUM ERROR
								;
		LDAA #03 				; CK SUM ERR FLAGS
		BRA LFB7A 				;
LFB19: 	LDAA $012D 				; SCI STATUS FLAGS
		ORAA #$10 				; DEV HAD ERR FREE XMISSION
		ANDA #$BF 				; INPUT NO LONGER IN WORK
		STAA $012D 				; SCI STATUS FLAGS
		PSHX 					; TEMP INDEX SAVE
		LDX 8,X 				; INDEX ICB
		LDAA 0,X 				; GET ICB STATUS FLAGS
		ANDA #$7F 				; INPUT NO LONGER IN WORK
		ORAA #$40 				; SET b5, INPUT COMPLETE
		STAA 0,X 				; RESTOR ICB STATUS FLAGS
		PULX 					;
								;
								; IF REPLY EXPECETED (NOUT >= 0) THEN XMIT INTERUPT
								; IS ENABLED AND FUTUTRE SCI INTERUPTS WILL CALL
								; SDI INTL, XMIT MODE ROUTINE. IF NOUT < 0 GOTO TO
								; SLEEP
								;
		CLRA 					;
		LDAB 4,X 				; GOING TO REPLY
		BMI LFB7A 				; BR IF NOT
								; .. else
								;
								; GOING TO RESPOND
								;
		LDAA $012D 				; SCI STATUS FLAGS
		ORAA #$80 				; SET b7, OUTPUT IN WORK
		STAA $012D 				; SCI STATUS FLAGS
		LDX 6,X 				; INDEX OUTPUT CNT'L BLOCK (OCB)
		LDAA 0,X 				; GET OCB STATUS FLAGS
		ORAA #$80 				; SET OUTPUT IN WORK
		STAA 0,X 				;
		LDX $0131 				; TBL POINTER FOR OCB
		LDAA 2,X 				; GET MSG ID
		LDAB $4008 				; SCI TX/RC STATUS REG
								;
								; TX MSG ID
								;
		STAA $400A 				; TX Data Reg, (SCI)
		STAA $0130 				; SCI CKSUM
		LDAB #01 				; INIT BYTE CNT
		STAB $012F 				; SAVE BYTE COUNTER
		LDAA $4004 				; Par I/O CSR
		ORAA #08 				; ENABLE TX VIA SXR IC
		STAA $4004 				; Par I/O CSR
		LDAA #$81 				; 1000 0001, CLT TE TO FORCE IDLE
		STAA $4007 				; SET SCI Cntl, (CSR)
		LDAA #$89 				; 1000 1001, RE-ENABLE TX
		STAA $4007 				; SET SCI Cntl, (CSR)
		JSR LFBA6 				; XFER ICB --> ICB
		RTS 					;
								;
								; HANDLE OVER RUN/FRAMING ERROR
								;
LFB6D: 	LDAA #04 				; SET FRAME ERR
		BITB #$02 				; b1, FRAME ERR
		BNE LFB75 				; BR IF FRAME ERROR
								; ... else
		LDAA #05 				; SET OVER RUN ERR
LFB75: 	LDAB $0131 				; TBL POINTER, DEV CODE DEFINED ?
		BEQ LFB87 				; BR IF NOT
								; ... else
LFB7A: 	LDX 8,X 				; INDEX ICB
		LDAB 0,X 				; GET ICB STATUS FLAGS
		ANDB #$70 				; CLR ERR'S & NO INPUT IN WK
		STAB 0,X 				; RESTORE ICB STATUS FLAGS
		TAB 					; ERR FLAGS TO B Reg
		ORAB 0,X 				; SET NEW ERROR
		STAB 0,X 				; SAVE ERR IN STATUS Reg
LFB87: 	LDAB $012D 				; SCI STATUS FLAGS
		ANDB #$B0 				; SET INPUT NO LONGER IN WORK
								; & CLR PRIOR ERORS
		STAB $012D 				; SCI STATUS FLAGS
		ORAA $012D 				; SCI STATUS FLAGS
		STAA $012D 				; SCI STATUS FLAGS
								;
								; CLR CURRENT TBL POINTER
								;
		CLRA			
		CLRB			
		STD $0131 				; CURRENT TBL POINTER
								;
								; SET SLEEP MODE
								;
		LDAA #$27 				; 0010 0111,
		STAA $4007 				; SET SCI Cntl, (CSR)
		STAB $012F 				; CLR BYTE COUNTER
LFBA2: 	STAB $0130 				; CLR CKSUM
LFBA5: 	RTS 					;
								;
								; XFER DATA FROM ICB TO MODE 2,3 & 4 OCB
								; [BUA TYPE $32]
								;
LFBA6: 	PSHA 					;
		PSHB 					;
		LDAA 5,X 				; GET INPUT MSG LENGTH, {$0131+5}
		LDX #$0134 				; INPUT BUFFER (ICB) ADDRESS+1
LFBAD: 	LDAB 0,X 				; GET
		STAB $1D,X 				; XFER BYTE ICB TO ICB ?? ($0151)
								; START AT $0151, MODE 2,3 & 4 OCB
		INX 					; NEXT ADDRESS
		DECA 					; DEC BYTE CNT
		BNE LFBAD 				; TILL DONE
								; ... else
		CLR $0171 				; SERIAL TIMER
		PULB 					;
		PULA 					;
		RTS 					;
		;*******************************************************
		;*******************************************************
		;* SERIAL MODE XMIT SUBROUTINE
		;*
		;* PROCESS SERIAL DATA via SCI PORT OF HC6801
		;* READ BYTE COUNTER, $012F.
		;* FROM THE BYTE COUNT THE BYTE TO BE OUTPUTED IS
		;* DETERMINED TO BE:
		;* 1. A DEVICE CODE
		;* 2. NUMBER OF DATA BYTES
		;* 3. DATA, FROM ROM DATA TBL or RAM DATA TBL
		;* 4. 2 COMP CHECKSUM
		;* NOTE: (If last byte sent was cksum,
		;* the SCI is switched to wakeup mode)
		;*
		;*
		;* SEND DATA TO SCI PORT
		;*******************************************************
								;
								; THE 1st BYTE WAS SENT IN RX ROUTINE WHEN
								; XMIT WAS STARTED
								;
LFBBB: 	LDX $0131 				; INDEX CURRENT DEVICE TBL
		LDAB $012F 				; GET BYTE COUNTER
		CMPB #01 				; 2nd BYTE TO BE SENT
		BNE LFBE2 				;
								; ... else
								;
								; IF $012F = 1 THEN DATA = NOUT + NUM OF DATA BYTES
								;
		LDAA 4,X 				; DATA = NOUT + NUM OF DATA BYTES
		LDAB $0151 				; INPUT CONTROL BLOCK (ICB+1)
		CMPB #02 				;
		BLS LFBDB 				;
								; ... else
		LDAA $0170 				; TX MSG LEN
		SUBA #01 				;
		CMPB #03 				;
		BEQ LFBD9 				;
								; ... else
		SUBA #10 				;
LFBD9: 	ASRA 					;
		INCA 					; INR MSG LENGTH
LFBDB: 	STAA $0170 				; TX MSG LEN
		ADDA #$55 				; ADD ON TO MSG LENGTH, (85d)
		BRA LFC33 				; GO TX SCI DATA
LFBE2: 	CMPB #2 				;
		BNE LFBEB 				;
								;
								; IF ... = 2 THEN DATA IS MODE (SAME AS MODE IN RX'ED MSG)
								; 		; ... else
		LDAA $0151 				; INPUT CONTROL BLOCK (ICB+1)
		BRA LFC33 				; GO TX SCI DATA
LFBEB: 	SUBB #02 				; SER BYTE CNT TO DATA BYTE CNT
		CMPB $0170 				; DATA BYTE TO BE SENT, (TX MSG LEN)
		BCC LFC2D 				; BR IF NOT
								; ... else
								;
								; IF (0 <= $012F- .... < NOUT) THEN SER DATA
								; IS REGULAR DATA BYTE. .... IS NUM OF SERIAL
								; SENT PRIOR TO SENDING REGULAR DATA, IN THIS
								; CASE IT IS 2: THE DEVICE CODE & NUMBER OF DATA BYTES
								;
		DECB 					;
		LDAA 3,X 				; GET EPROM TBL OPT fLG'S
		BITA #$80 				; b7, EPROM TBL INPUT
		BNE LFC04 				; BR IF EPROM TBL
								; ... else
		BITA #$40 				; b6, RAM TBL
		BNE LFC0C 				; BR IF RAM TBL
								; ... else
								;
								; DATA IS FM OUTPUT BUFFER
								;
		LDX 6,X 				; Point to Output DCB
		ABX 					; ADD BYTE CNT TO INDEX
		LDAA 1,X 				; Get Data fm Output buff
		BRA LFC33 				; GO TX DATA ...
								;
								; DATA IS EPROM TABLE DRIVEN
								;
LFC04: 	ASLB 					; GET DATA FROM EPROM TBL
		ABX 					; ADJUST INDEX
		LDX $0A,X 				; POINT to ERPOM Tbl
		LDAA 0,X 				; GET DATA FROM TBL
		BRA LFC33 				; GO TX DATA ...
								;
								; DATA IS FM RAM TBL
								;
LFC0C: 	LDX 6,X 				; RAM Tbl Addr
		LDAA $0151 				; INPUT CONTROL BLOCK (ICB+1)
		CMPA #02 				; Mode 2 ?
		BEQ LFC25 				; BR IF MODE 2
								; ... else
		CMPA #04 				; Mode 4 ?
		BNE LFC1E 				; BR IF MODE 4
								; ... else
		PSHB 					;
		LDAB #10 				;
		ABX 					;
		PULB 					;
								;
								; MODE 4 <-----*****
								;
LFC1E: 	INX 					; NEXT ADDRESS FROM RAM
		ASLB 					;
		ABX 					; ADJ POINTER
		LDX 1,X 				; GET NEW ADDRESS
		BRA LFC29 				; GET DATA & XMIT
								;
								; MODE 2 <-----*****
								;
LFC25: 	INX 					; MODE 2 CALC NEXT ADDRESS
		LDX 1,X 				;
		ABX 					;
								;
								; GET 8 DATA FROM INDEXED VAL & GO TX DATA
								;
LFC29: 	LDAA 0,X 				; GET DATA
		BRA LFC33 				; GO XMIT DATA
LFC2D: 	BNE LFC41 				; BR IF NZ
								; ... else
								;
								; IF ( ) THEN THE DATA TO BE SENT
								; IS THE CK SUM (2'S COMPLEMENT)
								;
		LDAA $0130 				; CKSUM
		NEGA 					; NEGATE CKSUM FOR 2'S COMP
								;
								; XMIT DATA, UP DATE CKSUM & INCREMENT BYTE CNTR
								;
LFC33: 	STAA $400A 				; SCI TX Data <-----***
		ADDA $0130 				; UPDATE CKSUM
		STAA $0130 				; NEW CKSUM
		INC $012F 				; INCR BYTE COUNTER
		BRA LFC71 				; EXIT via RTS
								;---------------------------------------------
								; ALL BYTES HAVE BEEN SENT, SCI CLEARED &
								; SWITCHED TO WAKE MODE
								;---------------------------------------------
LFC41: 	CLRA 					;
		CLRB 					;
		STAA $0130 				; CLR CKSUM
		STD $0131 				; CLEAR DEVICE TBL POINTER
		STAA $012F 				; CLR BYTE COUNTER
		LDAA $012D 				; STATUS FLAGS
		ANDA #$70 				; b4,5 & 6, OUTPUT NOT IN WORK/CLR ERROR
		STAA $012D 				; RESTORE STATUS FLAGS
		PSHX 					; TEMP SAVE INDEX
		LDX 8,X 				; INDEX INPUT CNT'L BLOCK (ICB)
		LDAA 0,X 				;
		ANDA #$F0 				; ERR CODE, NO ERRORS
		STAA 0,X 				; RESTORE ICB STATUS FLAGS
		PULX 					; RESTORE INDEX
		LDX 6,X 				; INDEX OUTPUT CNT'L BLOCK
		LDAA 0,X 				; GET OCB STATUS FLAGS REG
		ANDA #$7F 				; OUTPUT NO LONGER IN WORK
		STAA 0,X 				; RESTORE OCB STATUS FLAGS
								;
								; CLR REG'S
								;
		LDAA $4008 				; tx/rx status reg
		LDAA $4009 				; SERIAL DATA REG, (CLR FRAME ERR)
								;
								; SWITCH FROM XMIT TO WAKE UP MODE
								;
		LDAA #$41 				; b6 & b0
		STAA $4007 				; SET SCI Cntl, (CSR)
LFC71: 	RTS
		;***************************************************
		;***************************************************
		;*
		;* TEST TABLE
		;***************************************************
		;****************************************************
		;* FACTST.SRC, BUA CHIP, 86 Y CAR
		;*
		;* Factory System test (ECM test)
		;*
		;*
		;* CALLS: P4 XMIT S3
		;* A/D READ BATTTERY
		;* P4 CHECK SUM
		;* P4 A/D READ
		;*
		;*
		;* TABLE: HARDWARE I/F: BOFFECU
		;* PRP
		;*
		;*
		;* $FC72 - $FEA6
		;***************************************************
		ORG $FC72
LFC72: 	FDB $017B 	; A/D, Ch 0/1, PRESSURE              2
;		FDB $017C 	; A/D, Ch 1/2,                       4
		FDB $017D 	; A/D, Ch 2/3, o2                    6
;		FDB $017E 	; A/D, Ch 3/4,                       8
		FDB $017F 	; A/D, Ch 4/5, Coolant, 4K           A
;		FDB $0180 	; A/D, Ch 5/6                        C
		FDB $0181 	; A/D, Ch 6/7, PUMP V                E 
;		FDB $0182 	; A.D, CH 7/8                        80
		FDB $0183 	; A/D, Ch 8/9, MAT Temp              2
;		FDB $0184 	; A/D, Ch 9/10                       4
		FDB $0185 	; A/D, Ch 10/11, MAN A               6
;		FDB $0186 	; A/D, Ch 11/12                      8
		FDB $0187 	; 384 OHM Coolant temp               A 
;		FDB $0188 	; 4K Coolant temp                    C
		FDB $49 	; FMD BYTE 1 & 2                     E
LFC82: 	FDB LC000 	; EPROM ID                           90
		FDB LC002 	; Date Code                          2
		FDB LC004 	; Seq Num                            4
		FDB $0173 	; IEEE EPROM Ck Sum                  6
		FDB $0175 	; EEPROM Ck Sum                      8
		FDB $4002 	; Serial Data Latch                  A
		FDB $47 	; MODE WD                            C
		FDB $00 	; MINOR LOOP COUNTER                 E
LFC92: 	FDB $3FC0 	; Ref Period Timer                   A0
		FDB $3FC2 	; Timer #1 (Input 5)                              2 
		FDB $3FC4 	; Timer #2 (Input 6)                              4
		FDB $3FC6 	; Cntr #1 (Cum Pulses)                            6
		FDB $3FC8 	; Cntr #2 SPK PERIOD, (SPK Feed Bk, in 3)         8
		FDB $3FCA 	; Cntr #3 (16.5 Khz cntr if In 4 is hi)           A
		FDB $3FE0 	; Cntr #4 (Cum pulses on in 5)                    C
		FDB $3FF8 	; Timer #1A (B Cnt'r Last H.U Pulse)              E
		;***************************************************
								;--------------------------------------------------
								; FACTORY TEST ROUTINE
								;
								;
								; Read FDM byte 1 & 2
								; (Fuel Modeling Device)
								;--------------------------------------------------
								;
								; READ FMD BYTE 1 & 2
								;
LFCA2: 	LDAA $4006				;                                     B0
		LDS #$01FF 				; Set up STX
		STAA $018C 				; UP CNTS VAL WHEN 6.25 Msec Ins.
		BRCLR $48 $03 LFCBE 	; set b0 & b1
								; Mode = 4, (OD on, & in Drive)
		ADDA #11 				;
		STAA $4006 				; Match register
		BSET $46 3 				; 0000 0011, Ser data out Wd
								; set b0 & b1. 8192 mode
		LDAA #$FB 				; CLR b2
		JSR LF49A 				; Set Bit's, (CPU)
		BRA LFCC6 				;                                      
LFCBE: 	ADDA #205 				;                                     CC
		STAA $4006 				; Match register
		BCLR $46 3 				; clr b0 & b1,Ser data mode wd
								; 11 = 1 & 2nd 160 baud interupt      
LFCC6: 	JSR LFDA7 				; Tst for Factory mode                D4 
								;
								; SEL FMD CHIP
								;
		LDX #$4002 				; FMD Data Latch
		BSET 0,X 04	 			; b2
		LDAA $31 				; FACT TST, FMD SERIAL INPUT
		ORAA #$80 				; Set b7
		PSHA 					; Save on STX
		JSR LF1F9 				; Call Xmit to Serial Data
		TAB 					; Byte 2 to B reg
		PULA 					;
		ANDA #$7F 				; CLR b7
								;
								; Toggle FMD CS To do Read
								; ($4002 FMD Data Latch)
								;
		BCLR 0,X 04	 			; CLR b2
		BSET 0,X 04	 			; SET b2
		JSR LF1F9 				; Call Xmit to Serial Data
		STD $49 				; FACTORY TST FMD BYTE 1 M& 2
		BCLR 0,X 04 			; De-select FMD Chip
		CLI 					; CLR & RESTIORE INTERUPTS
								;
								; FACT TST MINOR LP Cnt'r
								; Roll over at 96 counts
								;
		LDAA $0172 				; FACT TST MINOR LP Cnt'r
		INCA 					; INCR LP CNT'R
		CMPA #96 				; Roll over at 96 counts
		BCS LFCF3 				; BR IF LP CNT'R GT 96
								; ... else
		CLRA 					;
LFCF3: 	STAA $0172 				; FACT TST MINOR LP Cnt'r
								;
								; BATTERY/IGNITION ROUTINE
								;
								;
		JSR LFDA1 				; Get Battery voltage
		LDAB $32 				; Ign Tmr
		CMPA #40 				; Tst for Batt < 4 VDV
		BHI LFD04 				; BR IF BATT LT 4.0 VDC
		CMPB #160 				; Has Ign Been off for 1 sec ?
		BCS LFD20 				;
		SWI 					; Reset MPU for Pwr Down
LFD04: 	TSTB 					;
		BEQ LFD21 				;
								; ... else
		CMPA #90 				; Tst for Ign on, (Batt V > 9 VDC)
		BCS LFD21 				;
		CLRB 					;
		LDAA $49 				; FACTORY TST FMD BYTE 1
		ANDA #$03 				; b0 & b1
		CMPA $48 				; Ck For Mode Changed ?
		BEQ LFD21 				; Yes It Has Changed
								; ... else
		STAA $48 				;
		STAB $0189 				; ALCL NUM BITS TO TX
		STAB $0172 				; FACT TST MINOR LP Cnt'r
		DECB 					;
		STAB $018A 				; LOCATION OF NEXT ENTRY IN EPROM TBL
LFD20: 	INCB 					; Bump up ign timer
LFD21: 	STAB $32 				; IGN OFF TIMER
		LDD #$FF00 				; Restart COP
		STD $400B 				; COP ARM/CLR
								;
								; Using FTMW 1
								;
		LDD $3FFC 				; MCU CSR
		PSHA 					;
		LDAA $31 				; FACT TST, FMD SERIAL INPUT
		EORA #$02 				; Toggle bit 1
		TST $48 				; Mode 1 ?
		BNE LFD51 				;
		ORAB #4 				; SET b2, Rase ALDL Line
		BRSET $49 $04 LFD3E 	; BR IF b2, Fact tst wd #1.
								; ... else
		ANDB #$FB 				; CLR b2
LFD3E: 	TST $32 				; IGN OFF TIMER, Is Ign off ?
		BNE LFD47 				; BR IF TMR NZ
								; ... else
		BRCLR $49 $03 LFD49 	;BR IF NOT b0 & b1, Fact tst wd #1
								; ... else
								;
								; Start toggeling COP 2
								;
LFD47: 	STAA $31 				; FACT TST, FMD SERIAL INPUT
LFD49: 	PULA 					;
		STD $3FFC 				; MCU CSR
		BSR LFDA6 				; Exit w/RTS
		BRA LFD7B 				; Toggel COP 2
								;
								; Toggle COP 2
								;
LFD51: 	STAA $31 				; FACT TST, FMD SERIAL INPUT
		PULA 					;
		STD $3FFC 				; MCU CSR
		LDAA $48 				; Test mode
		LSRA 					;
		BCS LFDB5 				;
								;
								; MISC TEST MODE (3)
								;
		CLRA 					;
		LDAB #$C5 				; Set 3 msec Async PW
		STD $3FF2 				; Async PW Cnt'r
		BSR LFDA6 				; Exit w/rts
		LDD $3FFC 				; MCU CSR
		ORAA #4 				; SET b2
		BSR LFDA6 				; Exit w/RTS
		STD $3FFC 				; MCU CSR
		LDX #0 					; Zero NV Mem Ck sum
		LDD #$002E 				;
		JSR LF32E 				; Ck Sum Subroutine
		STY $0175 				; Save NV RAM Ck sum
								;
								; All Off Mode
								;
LFD7B: 	LDD $3FFC 				; MCU CSR, Get Flags
		ANDA #$FB 				; CLR b2, Reset Async
		ANDB #$EF 				; CLR b0, Disable SPK
		ORAB #8 				; SET b3, Turn Off Err lamp
		BSR LFDA6 				; Exit w/RTS, (Wait)
		STD $3FFC 				; MCU CSR, Save new Arg
		CLRA 					; Clr D reg.
		CLRB 					;
		BSR LFDA6 				; Exit W/RTS
		STD $3FD0 				; Sync Fuel = 0
		LDAA $4004 				; Par I/O CSR, EFI PW
		ANDA #$F9 				; 1111 1001, Trn off FAN & IAC
		STAA $4004 				; Par I/O CSR
		LDY #$7000 				;
		BSR LFDED 				;
		JMP LFEA3 				; Exit FDM
								;
								; Get Battery voltage
								;
LFDA1: 	LDAA #$10 				; A/D CH 1, (Battery Volts)
LFDA3: 	JSR LF1C3 				; To A/D routine
LFDA6: 	RTS
								;-----------------------------------------
								; Test ALDL Mode for an A/D Value
								; between 40 to 100
								;
								; Branch to Appropriate mode on exit
								;-----------------------------------------
LFDA7: 	LDAA #$70 				;A/D Ch 7, (Start/Run Pwr)
		BSR LFDA3 				; If not Reset & return
		CMPA #40 				; 40 ?
		BCS LFDB3 				; If not Reset & return
		CMPA #$64 				; 100
		BCS LFDB4 				; if >= 40 or <= 100, RTS
								; ... else
LFDB3: 	SWI 					; SOFTWARE INTERUPT
LFDB4: 	RTS			
								;-------------------------------------------------
								;-------------------------------
								; I/O Check modes (2 & 4)
								; Modes 2 & 4 xmit different
								; serial data
								;-------------------------------
								;-------------------------------
								; Read all (12), A/D's
								; & save results to table FDMPRES
								;-------------------------------
LFDB5: 	LDX #$017B 				; Set up A/D storage in RAM
		CLRA 					; Set A/D Ch 0, (Rd Cal Pack)
		CLRB 					;
LFDBA: 	JSR LF1BE 				; Call To A/D READ subroutine
		STAA 0,X 				; Save Results
		INX 					; Bump up A/D storage pointer
		ADDB #$10 				; Next A/D chan
		TBA 					; COPY B to A reg
		CMPB #$C0 				; Stop if CH C, ($C0)
		BCS LFDBA 				; Loop if < ch C, (#12)
								; ... else
								;
								; Sw coolant P.U's every
								; 25 MSEC, (bits 0 & 1)
								;
		LDAB $0172 				; FACT TST MINOR LP Cnt'r
		BITB #03 				; b0 & b1
		BNE LFDE3 				; BR IF b0 & b1
								; ... else
		LDAA $017F 				; Get coolant results
								;
								; Br if 4K PU
								;
		BRCLR $31 01 LFDDA 		; BR IF NOT b0, (FACT TST, FMD SERIAL IN)
		STAA $0187 				; Save 348 Coolant results
		BRA LFDDD 				; EXIT COOL FOR NEXT ROUTINE
LFDDA: 	STAA $0188 				; Save 4K Coolant temp
								;
								; Toggle COP 2
								;
LFDDD: 	LDAB $31
		EORB #$01 				; TOGGLE b0
		STAB $31 				; FACT TST, FMD SERIAL INPUT
LFDE3: 	LDAB $0172 				; FACT TST MINOR LP Cnt'r
		BITB #$0F 				;
		BEQ LFDFF 				; BR IF NOT $0F
								; ... else
		JMP LFE4F 				;
								;--------------------------------------------------
								; >>> 100 MSEC logic <<<
								;
								; Subroutine for writing s DC to all 6
								; PWM's, required DC must be in Y reg
								;--------------------------------------------------
LFDED: 	LDX #$3FD2 				; EGR PW Cnt'r
LFDF0: 	STY 0,X 				;
		INX 					;
		INX 					;
		CPX #$3FDC 				; SPK Dwell period cnt'r
		BNE LFDF0 				;
		STY $3FCC 				;
		RTS 					;
								;-------------------------------
								; Exrcise PWM outputs
								;
								;-------------------------------
LFDFF: 	CMPB #$20 				;0010 0000, (seg xo in B reg)
		BLS LFE05 				;
		SUBB #$30 				;
LFE05: 	LDY #$D39A 				;
		CMPB #$10 				;
		BHI LFE17 				;
		LDY #$D200 				;
		BCS LFE17 				;
		LDY #$D066 				;
								;------------------------------
								; Tgggle MALF & Fan
								;-------------------------------
LFE17: 	BSR LFDED 				;
		BSR LFDA6 				;
		LDD $3FFC 				; MCU CSR
		LDX #$4004 				;
		PSHA 					;
								;
		ORAB #$10 				; b4
		LDAA $0172 				; FACT TST MINOR LP Cnt'r
		BITA #$10 				; Test Bit 4
		BEQ LFE32 				;
								; ... else
		ORAB #$08 				; Make sure SPK is enabled (Bit 3)
		BSET 0,X 06
		BRA LFE37 				;
LFE32: 	ANDB #$F7 				; CLR b3,
								; MALF lite on, Fan off
		BCLR 0,X 02			
LFE37: 	PULA 					;
		STD $3FFC 				; MCU CSR
								;---------------------
								; Exercise IAC Motor
								;---------------------
		LDAA $0179 				; IAC motor
		ASLA 					;
		ADCA #0 				;
		STAA $0179 				; Out 1 step to IAC motor
		ANDA #3 				; 0000 0011, save IAC A&B states
		LDAB $4002 				; Get Serial Data Latch
		ANDB #$FC 				; 1111 1100, Mask for old IAC A & B &
		ABA 					; Add in new IAC a & B
		STAA $4002 				; Get Serial Data Latch
								;------------------------------
								; End of 100 Ms Routine
								;------------------------------
								;-------------------------------
								; DO SPARK & FUEL
								; based on RPM
								;-------------------------------
LFE4F: 	LDX $3FC0 				; Ld Last 2 Ref Periods
		PSHX 					; Save ref (RPM)
		PULA 					;
		PULB 					;
		LSRD 					; Divide Ref Per/4
		LSRD 					;
		LDY #0 					;
		STD $0177 				; Save Ref Period for later
		LDD #655 				; 10 Ms Period
		CPX #655 				; < 100 hZ ?
		BHI LFE77 				; If Yes
								; ... else
		CPX #328 				; > 200 Hz ?
		BCS LFE82 				; If yes
								;-----------------
								; Base P.W. = 5Ms
								; Dwell = 4 Ms
								;-----------------
		LSRD 					;
		STD $3FD0 				; BPW = 5 msec
		LDAB #6 				; Set dwell = 4 msec
		PSHB 					;
		PSHA 					;
		CLRA 					;
		CLRB 					;
		BRA LFE94 				;
								;-----------------
								; Base P.W. = 10 Ms
								; Dwell = 5 Ms
								;-----------------
LFE77: 	STD $3FD0 				; Sync fuel cnt'r
		LSRD 					; Div by 2
		PSHB 					; 5 msec Dwell
		PSHA 					;
		LDD $0177 				; SPK Fall= REFPER/4(45 Deg retard)
		BRA LFE94 				; Ref period
								;-----------------
								; Base P.W. = 1 Ms
								; Dwell = 3 Ms
								;-----------------
LFE82: 	LDY #0066 				; 1 Ms Fuel dly
		LDD #0066 				; Set BPW = 1 msec
		STD $3FD0 				; Sync fuel cnt'r
		LDAB #197 				; Set 3 msec Dwell
		PSHB 					;
		PSHA 					;
		CLRB 					;
		SUBD $0177 				;
								;--------------------------------------------------
LFE94: 	PULX 					;
		STX $3FDC 				; SPK Dwell period cnt'r
		BSR LFEA6 				;
		STD $3FF6 				; Tm Fm Ref to Fire IGN Cnt'r
		BSR LFEA6 				; Short delay
		STY $3FCE 				; EFI DLY
LFEA3: 	JMP LCCC6 				; WAITLOOP
LFEA6: 	RTS 					;
								;--------------------------------------------------
								;--------------------------------------------------
								; Cool Temp L.U. Table;
								;
								; System uses 2 different Series resistors with
								; the Coolant thermistor to make the range greater
								;
								; Table Arg = (Deg C + 40) * (256/192)
								;
								; 4k Ohm Table is LFEA7
								; 348 Ohm Table is LFEB8
								;----------------------------------------------------
								;-------------------------------------------
								; 3840 ohm Table
								;-------------------------------------------
LFEA7: 	FCB 255 				;Hot 0 A/D
		FCB 215 				;121 C, 6
		FCB 155 				;76 22
		FCB 132 				;59 38
		FCB 117 				;48 54
		FCB 106 				;39 70
		FCB 97 					;32 86
		FCB 88 					;26 102
		FCB 81 					;21 118
		FCB 74 					;15 134
		FCB 67 					;10 150
		FCB 60 					;5 166
		FCB 52 					;-1 182
		FCB 44 					;-7 198
		FCB 34 					;-14 214
		FCB 22 					;-24 246
		FCB 00 					;-40 246
								;-------------------------------------------
								;-------------------------------------------
								; 348 Ohm Table
								;-------------------------------------------
LFEB8: 	FCB 255 				;Hot 0 A/D
		FCB 255 				;Hot 16
		FCB 250 				;147 C 32
		FCB 223 				;128 48
		FCB 205 				;114 64
		FCB 191 				;103 80
		FCB 179 				;94 96
		FCB 168 				;86 112
		FCB 157 				;78 128
		FCB 147 				;71 144
		FCB 138 				;63 160
		FCB 128 				;56 176
		FCB 117 				;48 192
		FCB 105 				;38 208
		FCB 90 					;27 224
		FCB 67 					;10 240
		FCB 00 					;Cold 256
								;-------------------------------------------
								;-------------------------------------------
								; Stepper Motor State Table
								;-------------------------------------------
LFEC9: 	FCB 0
		FCB 1
		FCB 3
		FCB 2
		
		;*******************************************************
		;***************************************************
		;* VECTOR
		;*
		;* C800 IS ALGO BEGINING
		;***************************************************
		ORG $FFF0
LFFF0: 	FDB $6000 				; SWI ???
LFFF2: 	FDB $C9F4 				; IRQ1
LFFF4: 	FDB $F27B 				; IRQ, NOT USED
LFFF6: 	FDB $6000 				; ILLEGAL OP CODE
LFFF8: 	FDB $C800 				; ILLEAGLE ADDRESS
LFFFA: 	FDB $C800 				; COP TIMED OUT
LFFFC: 	FDB $C800 				; CLOCK FAILED
LFFFE: 	FDB $C800 				; HOT RESTART
		;*****************************************************