# Unexecuted BUA ROM blocks

On simulator baseline `c821166`, the complete ROM replay suite executed 6,164 of 6,598 listing instructions (93.42%). The remaining 434 instruction starts form 218 blocks. Coverage counts execution, including ROM-only probes. A hit alone does not establish ROM-versus-C agreement.

| Disposition | Blocks | Instructions |
|---|---:|---:|
| Unreachable with stock BUA | 64 | 166 |
| Known unverified gaps | 154 | 268 |

The first group requires an immutable option, a fixed table value, or an arithmetic bound that excludes the path in the intact stock image. The second includes reachable behavior and candidates whose complete path from reset is still unverified. Fault inputs and optional external hardware are identified where relevant. Artificially seeding a RAM flag does not establish stock reachability.

Ranges give the first and last instruction **start**, inclusive. Blocks split at transfer targets, calls and returns, vectors, scheduler entries, and gaps in the listing or execution trace. The ROM replay in `make test` uses `--audit-unrun` to check the documented address ranges and instruction counts. The reachability reasons are manually reviewed against [the listing](../evidence/firmware/bua-hac.lst).

## Unreachable with stock BUA

| ROM PC | Instructions | Reason |
|---|---:|---|
| `$CBFC` | 1 | Stock fan polarity at $C017 is zero. BITB #$20 skips the inversion. |
| `$CC11–$CC16` | 3 | The throttle window requires TPS ≥ $C67D and < $C67E. Both bytes are $FF. |
| `$CC18–$CC1F` | 5 | The throttle window requires TPS ≥ $C67D and < $C67E. Both bytes are $FF. |
| `$CC95–$CCA7` | 10 | Either $C1AA must differ from $55 or $C555 from $AA. Both signatures match in the intact image. |
| `$CCA9` | 1 | Either $C1AA must differ from $55 or $C555 from $AA. Both signatures match in the intact image. |
| `$CD9F–$CDA2` | 2 | Single-fire enable is $C014 bit 1. Stock $B4 clears it. |
| `$CDF2` | 1 | Cylinder scaling requires $C009 nonzero. BUA stores zero. |
| `$CF4C–$CF58` | 5 | The unsigned 16-bit pass-by counter must exceed $C29C = $FFFF. |
| `$CFD1–$CFD2` | 2 | The alternate coolant-spark load axis requires nonzero $C0FE. BUA selects axis zero. |
| `$CFE5–$CFE7` | 2 | $C017 bit 4 is clear, bypassing the fan-input gate. |
| `$CFED–$CFF2` | 3 | $C017 bit 2 is clear, bypassing the fourth-gear highway spark check. |
| `$D021` | 1 | Highway spark rejects load above $C16F (100) at $CFF9. The same A would have to exceed 144 at $D01F. |
| `$D58C–$D58E` | 2 | The idle service indexes $C62E. Its manual-transmission flag at X+$54 ($C682) is zero and $D58A branches around the speed test. |
| `$D684` | 1 | The stock quantizer multiplies an 8-bit remainder by 8. Its rounded high byte is at most 8, below the signed clamp. |
| `$D7ED` | 1 | The first two $F266 multiplies bound the crank width by $C36C = 6554. The largest TPS factor is 115/64 at $C397. Even allowing rounding, the scaled width is below 12000 and fits in 16 bits. |
| `$D821–$D824` | 2 | The crank RPM blend table at $C35B returns zero for every axis value. The full-blend counter requires 255. |
| `$D826–$D829` | 2 | The crank RPM blend table at $C35B returns zero for every axis value. The full-blend counter requires 255. |
| `$D831–$D832` | 2 | The all-zero $C35B crank RPM blend table takes the zero-result exit at $D82F before this calculation. |
| `$D835` | 1 | The all-zero $C35B crank RPM blend table takes the zero-result exit at $D82F before this calculation. |
| `$D838–$D83D` | 5 | The all-zero $C35B crank RPM blend table takes the zero-result exit at $D82F before this calculation. |
| `$D840–$D84E` | 12 | The all-zero $C35B crank RPM blend table takes the zero-result exit at $D82F before this calculation. |
| `$D850–$D853` | 3 | The all-zero $C35B crank RPM blend table takes the zero-result exit at $D82F before this calculation. |
| `$D856–$D861` | 9 | The all-zero $C35B crank RPM blend table takes the zero-result exit at $D82F before this calculation. |
| `$D915` | 1 | Single-fire enable is $C014 bit 1. Stock $B4 clears it. |
| `$D9EC` | 1 | The RPM axis is divided by four before the $C60E lookup. Its five possible endpoints are all 180. Subtracting $C60D (16) cannot borrow. |
| `$DA7D` | 1 | The alternate enrichment axis requires $C017 bit 0 set. BUA stores zero. |
| `$DBCD–$DBD2` | 4 | The optional fuel correction check requires $C017 bit 3 set. BUA stores zero. |
| `$DCD7` | 1 | Even a full 8-bit axis at $F2C6 selects endpoints in $C582–$C592, all ≤ 44. The high byte after multiplying by 255 is ≤ 43. Adding $C55F (16) cannot carry. |
| `$DD53` | 1 | The running single-fire latch at $39 bit 7 is set via the $C014 bit 1 option. Both stock enable sites ($CD9F and $D915) are bypassed. |
| `$DD60–$DD65` | 3 | The table-based pulse-width path requires $C016 bit 6 clear. BUA stores $62 and calculates pulse width. |
| `$DD68–$DD71` | 6 | The table-based pulse-width path requires $C016 bit 6 clear. BUA stores $62 and calculates pulse width. |
| `$DD74–$DD75` | 2 | The table-based pulse-width path requires $C016 bit 6 clear. BUA stores $62 and calculates pulse width. |
| `$DD78–$DD79` | 2 | The table-based pulse-width path requires $C016 bit 6 clear. BUA stores $62 and calculates pulse width. |
| `$DF3A` | 1 | The coolant gain table $C262–$C268 peaks at 128. Multiplying by an 8-bit duty gives at most 32640, so ASLD cannot carry. |
| `$E0B9` | 1 | VSS filter $F250 uses $C284=$FF. Its result is at most $FF01, below the $FF80 needed for the rounding add to carry. |
| `$E1EC–$E1EE` | 2 | The branch requires an 8-bit TPS value above the $FF threshold at $C28B. |
| `$E23A–$E23F` | 3 | BUA $C014=$B4 selects automatic transmission. The cold and warm dispatch paths clear shifter bit 5 before the $E236 test. |
| `$E241–$E243` | 2 | Entry via $E236 requires shifter bit 5, cleared by BUA dispatch. The other entry via $E1EE requires TPS above $FF. |
| `$E24B–$E24E` | 3 | $E245–$E249 requires $C014 bit 0 set. Stock $B4 clears it. |
| `$E250–$E254` | 3 | The enclosing $E249 gate requires $C014 bit 0 set. Stock $B4 clears it. |
| `$E289–$E28B` | 2 | The cold and warm automatic-transmission paths clear shifter bit 5. the $E285 test therefore skips this block. |
| `$E28D` | 1 | Reaching the bit-0 clear requires shifter bit 5, cleared by the BUA automatic-transmission paths before $E285. |
| `$E3A9` | 1 | $E3A2 rejects counter values above 10 before INCB. An accepted value cannot wrap from $FF to zero. |
| `$E478` | 1 | All gains at $C315–$C31C are 32. $F266 returns at most 8192 for a 16-bit input, so neither subsequent left shift carries. |
| `$E7D6` | 1 | The $C27C–$C283 table is all 128. The product with an 8-bit duty is at most 32640, so its top bit cannot set rotate carry. |
| `$E94E` | 1 | X points to $C62E. The power-steering offset at X+$52 ($C680) is zero, so the addition at $E94A cannot carry. |
| `$E959` | 1 | The A/C power-steering offset at $C681 is zero. Adding it cannot carry. |
| `$EE5A` | 1 | The alternate output path requires a negative $C017 option byte. BUA stores zero. |
| `$EE5E–$EE63` | 3 | The alternate output path requires a negative $C017 option byte. BUA stores zero. |
| `$EE65–$EE6B` | 4 | The alternate output path requires a negative $C017 option byte. BUA stores zero. |
| `$EE73` | 1 | The alternate lamp output requires $C014 bit 6 set. Stock $B4 clears it. |
| `$EE77` | 1 | The alternate lamp output requires $C014 bit 6 set. Stock $B4 clears it. |
| `$F6CF` | 1 | $F695 admits enrichment ≥ 256 before this subtraction. The one-byte decay is at most 255. |
| `$F771–$F776` | 3 | Stock $C014 bit 2 selects the analog MAF ADC at $F7AC. This block belongs to the digital period-input path. |
| `$F778–$F77C` | 3 | Stock $C014 bit 2 selects the analog MAF ADC at $F7AC. This block belongs to the digital period-input path. |
| `$F77E–$F783` | 3 | Stock $C014 bit 2 selects the analog MAF ADC at $F7AC. This block belongs to the digital period-input path. |
| `$F785–$F786` | 2 | Stock $C014 bit 2 selects the analog MAF ADC at $F7AC. This block belongs to the digital period-input path. |
| `$F788–$F78E` | 4 | Stock $C014 bit 2 selects the analog MAF ADC at $F7AC. This block belongs to the digital period-input path. |
| `$F790–$F7A4` | 12 | Stock $C014 bit 2 selects the analog MAF ADC at $F7AC. This block belongs to the digital period-input path. |
| `$F7A6–$F7A7` | 2 | Stock $C014 bit 2 selects the analog MAF ADC at $F7AC. This block belongs to the digital period-input path. |
| `$F7A8–$F7AA` | 2 | Stock $C014 bit 2 selects the analog MAF ADC at $F7AC. This block belongs to the digital period-input path. |
| `$F7E4` | 1 | The fallback sum is at most 255 × 167 + 255 × 21 + 1024 = 48964 using $C213–$C216. Neither add can overflow. |
| `$F7F1` | 1 | The stock analog path clears A at $F7B8 before testing A at $F7EE. |
| `$F7FA` | 1 | The stock analog axis is at most 255 × 7 = $06F9. Its high byte cannot exceed 6. |

## Reachable behavior and known unverified gaps

| ROM PC | Instructions | Unverified condition or endpoint |
|---|---:|---|
| `$C8EA` | 1 | Reset detects a matching external HUD signature at $5800 and calls its initialization entry. Entry: `JSR $5812`. |
| `$C938` | 1 | Error 51 startup shutdown. The fault replay stops at the SWI. The subsequent loop requires control to resume after SWI. Entry: `SWI`. |
| `$C939` | 1 | Error 51 startup shutdown. The fault replay stops at the SWI. The subsequent loop requires control to resume after SWI. Entry: `BRA LC938`. |
| `$C94B` | 1 | Startup coolant above 208 selects the coolant-axis clamp. Entry: `LDAA #208`. |
| `$C974–$C978` | 3 | Startup coolant reaches $C02C (113) and sets bit 6 of $01. Entry: `LDAB $01`. |
| `$CA16` | 1 | SCI interrupt dispatch checks transmit-complete enable/status, acknowledges the transmitter, or returns with no service. Entry: `BRCLR 0,X $40 LCA2B`. |
| `$CA1A` | 1 | SCI interrupt dispatch checks transmit-complete enable/status, acknowledges the transmitter, or returns with no service. Entry: `BRCLR 1,X $40 LCA2B`. |
| `$CA1E–$CA28` | 5 | SCI interrupt dispatch checks transmit-complete enable/status, acknowledges the transmitter, or returns with no service. Entry: `LDAA $4004`. |
| `$CA2B` | 1 | SCI interrupt dispatch checks transmit-complete enable/status, acknowledges the transmitter, or returns with no service. Entry: `RTI`. |
| `$CA73` | 1 | An IRQ with factory mode selected dispatches to $FCA2. The factory replay begins inside the dispatcher target. Entry: `JMP LFCA2`. |
| `$CA7E–$CA83` | 3 | $34 bit 2 clear and $36 bit 2 set arm the IRQ latch and save the loop count. Entry: `LDAA $00`. |
| `$CAC0` | 1 | Unexpected IRQ stack pointer sets the stack-failure flag. Entry: `BSET $33 $01`. |
| `$CAEA` | 1 | Either retained engine runtime exceeds 32 seconds or TPS exceeds 197. The ROM clears the reference counter. Entry: `CLRB`. |
| `$CAFF–$CB01` | 2 | The spark blend byte plus its calibrated increment of 2 carries and saturates. Entry: `LDAA #255`. |
| `$CB1B` | 1 | $33 bit 4 selects the alternate masked output. Entry: `ANDB #$FB`. |
| `$CD20` | 1 | The coolant byte plus the high byte of its scaled product overflows the O2-filter axis. Entry: `LDAA #255`. |
| `$CD3F–$CD42` | 2 | Spark feedback qualifies Error 42. Its counter increments, then undoes wrap at 255. Entry: `INC $B4`. |
| `$CD44` | 1 | Spark feedback qualifies Error 42. Its counter increments, then undoes wrap at 255. Entry: `DEC $B4`. |
| `$CDCD` | 1 | The no-reference timeout writes $FFFF to $3FC0 and enters the SWI endpoint. The following loop requires a return. Entry: `SWI`. |
| `$CDCE` | 1 | The no-reference timeout writes $FFFF to $3FC0 and enters the SWI endpoint. The following loop requires a return. Entry: `BRA LCDCD`. |
| `$D0BC` | 1 | HUD present ($33 bit 7) selects the external ROM callback at entry. Entry: `JSR $5809`. |
| `$D15F–$D162` | 3 | With HUD present, test $4B for the external spark override. Entry: `LDAB $4B`. |
| `$D190` | 1 | Signed knock-retard comparison selects the maximum-retard limit at $C020. Entry: `STX $9D`. |
| `$D1F3` | 1 | HUD present ($33 bit 7) selects the external ROM callback at entry. Entry: `JSR $580F`. |
| `$D23F–$D241` | 2 | The signed correction crosses its limit. Restore the prior operand and store the corrected word. Entry: `ADDD 0,X`. |
| `$D28A–$D290` | 3 | Spark feedback exceeds the calibrated $C227 threshold. Exercise the indicated feedback status update. Entry: `BSET $33 $40`. |
| `$D2D2–$D2D5` | 2 | Spark feedback exceeds the calibrated $C227 threshold. Exercise the indicated feedback status update. Entry: `BSET $01 $80`. |
| `$D33A–$D33D` | 2 | $3D bit 6 selects the calibrated A/C learn steps. Entry: `LDAB LC665`. |
| `$D3A7–$D3A8` | 2 | Retained idle motor state selects the repeat-position path, zero-position flag or signed step clamp. Entry: `TSTA`. |
| `$D3AA` | 1 | Retained idle motor state selects the repeat-position path, zero-position flag or signed step clamp. Entry: `ORAB #$02`. |
| `$D3AC–$D3B0` | 3 | Retained idle motor state selects the repeat-position path, zero-position flag or signed step clamp. Entry: `SUBA LC62F`. |
| `$D3B2` | 1 | Retained idle motor state selects the repeat-position path, zero-position flag or signed step clamp. Entry: `LDAA #127`. |
| `$D3B9–$D3C0` | 4 | A zero idle step request falls through to the status/diagnostic checks and conditional idle service call. Entry: `LDAA $35`. |
| `$D3C2` | 1 | A zero idle step request falls through to the status/diagnostic checks and conditional idle service call. Entry: `BSET $3C $01`. |
| `$D3C5–$D3C9` | 3 | A zero idle step request falls through to the status/diagnostic checks and conditional idle service call. Entry: `LDAA $33`. |
| `$D3CB` | 1 | A zero idle step request falls through to the status/diagnostic checks and conditional idle service call. Entry: `JSR LD335`. |
| `$D3CE` | 1 | A zero idle step request falls through to the status/diagnostic checks and conditional idle service call. Entry: `BRA LD3F0`. |
| `$D3E3` | 1 | Idle position or throttle difference takes the negative-magnitude or saturation path. Entry: `NEGA`. |
| `$D3E6` | 1 | Idle position or throttle difference takes the negative-magnitude or saturation path. Entry: `LDAA #127`. |
| `$D3FF` | 1 | Idle position or throttle difference takes the negative-magnitude or saturation path. Entry: `NEGB`. |
| `$D40D–$D411` | 3 | An active fault in mask $24 selects the substitute throttle axis. Entry: `LDAA LC64B`. |
| `$D446` | 1 | Idle setpoint selects hot-spark RPM offset ($3A bit 5) or diagnostic target ($35 bit 5). Entry: `ADDA $57,X`. |
| `$D44E` | 1 | Idle setpoint selects hot-spark RPM offset ($3A bit 5) or diagnostic target ($35 bit 5). Entry: `LDAA #80`. |
| `$D4F0–$D4F3` | 3 | Idle control has $F3 bit 7 set, bit 4 clear and zero $010B. It sets bit 4 and clears B. Entry: `CLRB`. |
| `$D6EA` | 1 | Key-off replay reaches the powerdown SWI on service 902 and stops before execution. The back edge also requires a return. Entry: `SWI`. |
| `$D6EB` | 1 | Key-off replay reaches the powerdown SWI on service 902 and stops before execution. The back edge also requires a return. Entry: `BRA LD6EA`. |
| `$D9E7–$D9EA` | 2 | Power enrichment already latched ($43 bit 5) selects hysteresis. The throttle subtraction also has a zero clamp on borrow. Entry: `SUBA LC60D`. |
| `$D9FE` | 1 | Power enrichment already latched ($43 bit 5) selects hysteresis. The throttle subtraction also has a zero clamp on borrow. Entry: `SUBA LC60C`. |
| `$DA70` | 1 | HUD present ($33 bit 7) selects the external ROM callback at entry. Entry: `JSR $5800`. |
| `$DD39–$DD3C` | 2 | Calculated pulse width with engine-running flag clear selects the crank-period comparison. The unsigned $C357 limit is zero, so entry to the comparison entails its clamp. Entry: `CPX LC357`. |
| `$DD3E` | 1 | Calculated pulse width with engine-running flag clear selects the crank-period comparison. The unsigned $C357 limit is zero, so entry to the comparison entails its clamp. Entry: `LDX LC357`. |
| `$DF13` | 1 | EGR enabled with coolant axis $63 at or above 160 takes the upper clamp. Entry: `LDAB #160`. |
| `$DF1B` | 1 | EGR enabled with RPM axis $57 above 80 takes the upper clamp. Entry: `LDAA #$50`. |
| `$DF40` | 1 | External HUD online ($33 bit 7) invokes $581B. Peripheral execution is unverified. Entry: `JSR $581B`. |
| `$DF43` | 1 | Return from the optional $581B HUD call with $33 bit 7 set. Entry: `BRA LDF46`. |
| `$DF4E–$DF50` | 2 | A latched coolant-sensor error ($35 bit 1) requests full fan duty. Entry: `LDAA #$00FF`. |
| `$DF77` | 1 | A running fan ($F4 nonzero) selects the fan-off coolant threshold. Entry: `INX`. |
| `$DF7E–$DF83` | 3 | Coolant at or above the selected fan threshold enters the duty lookup. Entry: `LDAB #160`. |
| `$DF86–$DF87` | 2 | The fan duty lookup reaches this test. The all-$FF duty table at $C2F8–$C2FE excludes its zero-result exit. Entry: `TSTA`. |
| `$DF94–$DF97` | 2 | Coolant below the selected threshold with minimum-on timer $F5 still positive. Its calibrated reload is 75. Entry: `DEC $F5`. |
| `$E0C4` | 1 | A high filtered VSS value raises the scaled high byte above $2F, selecting the $3000 cap. Entry: `LDD #$3000`. |
| `$E0D0–$E0D3` | 2 | VSS error counter $AB above the $C1F7 threshold of 2 selects substitute speed. Entry: `LDAA LC64E`. |
| `$E4EB` | 1 | With external HUD online ($33 bit 7), the dispatcher calls $5818. Peripheral execution is unverified. Entry: `JSR $5818`. |
| `$E803` | 1 | HUD present ($33 bit 7) selects the external ROM callback at entry. Entry: `JSR $581E`. |
| `$E806` | 1 | Return from the external purge HUD callback. Entry: `BRA LE809`. |
| `$E832–$E834` | 2 | Pump-voltage Error 54 qualification or recovery crosses the $C235/$C236 threshold with the required enable and engine state. Entry: `LDX $34`. |
| `$E836–$E83B` | 3 | Pump-voltage Error 54 qualification or recovery crosses the $C235/$C236 threshold with the required enable and engine state. Entry: `LDAB $7F`. |
| `$E83D` | 1 | Pump-voltage Error 54 qualification or recovery crosses the $C235/$C236 threshold with the required enable and engine state. Entry: `BRA LE85A`. |
| `$E852` | 1 | Pump-voltage Error 54 qualification or recovery crosses the $C235/$C236 threshold with the required enable and engine state. Entry: `ORAA #$40`. |
| `$E854–$E856` | 2 | Pump-voltage Error 54 qualification or recovery crosses the $C235/$C236 threshold with the required enable and engine state. Entry: `ANDA #$DF`. |
| `$E980–$E982` | 2 | IAC feed-forward arithmetic reaches the indicated step limit or signed difference clamp. Entry: `LDAA #255`. |
| `$E9A2` | 1 | IAC feed-forward arithmetic reaches the indicated step limit or signed difference clamp. Entry: `LDAA #255`. |
| `$E9AF–$E9B1` | 2 | IAC feed-forward arithmetic reaches the indicated step limit or signed difference clamp. Entry: `LDAA #127`. |
| `$EBD5–$EBD8` | 2 | Air-temperature fault qualification reaches the vehicle-speed test or increments the low/high fault counter. Entry: `CMPB LC1F0`. |
| `$EBDA–$EBDD` | 2 | Air-temperature fault qualification reaches the vehicle-speed test or increments the low/high fault counter. Entry: `INC $AC`. |
| `$EBF8–$EBFB` | 2 | Air-temperature fault qualification reaches the vehicle-speed test or increments the low/high fault counter. Entry: `CMPB LC1F0`. |
| `$EBFD–$EC00` | 2 | Air-temperature fault qualification reaches the vehicle-speed test or increments the low/high fault counter. Entry: `INC $AD`. |
| `$EC39–$EC3E` | 3 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `LDAB $5B`. |
| `$EC40–$EC43` | 2 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `CMPB LC5EE`. |
| `$EC45–$EC4D` | 5 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `LDAB $BF`. |
| `$EC4F–$EC52` | 2 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `CMPB LC5EB`. |
| `$EC54–$EC59` | 3 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `LDX $0C`. |
| `$EC5C–$EC5E` | 2 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `STD $0C`. |
| `$EC60–$EC65` | 3 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `LDAB $C1`. |
| `$EC67–$EC6C` | 3 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `LDX $0A`. |
| `$EC6F` | 1 | Adaptive fuel update requires $44 bit 1 and the coolant window [$C5ED,$C5EE). Cell 9 selects the $0C filter. Cell 0 can update $0A before the $C5EC timeout. Entry: `STD $0A`. |
| `$EC9A` | 1 | Coolant axis at least 208 selects the lookup clamp. Entry: `LDAA #208`. |
| `$ED04–$ED07` | 2 | An O2 fault in $41 bit 1 clears the closed-loop readiness timer. Entry: `CLR $BE`. |
| `$ED13–$ED16` | 2 | Mode 4 fuel and AFR command bytes gate closed-loop entry. Entry: `LDX #$0154`. |
| `$ED1A` | 1 | Mode 4 fuel and AFR command bytes gate closed-loop entry. Entry: `BRCLR 1,X $01 LED77`. |
| `$ED1E` | 1 | Mode 4 fuel and AFR command bytes gate closed-loop entry. Entry: `BRA LED48`. |
| `$ED20` | 1 | Mode 4 fuel and AFR command bytes gate closed-loop entry. Entry: `BRSET 4,X $04 LED77`. |
| `$ED3D` | 1 | Closed-loop entry selects the hot timer or reaches the selected delay threshold. Entry: `INX`. |
| `$ED42–$ED46` | 3 | Closed-loop entry selects the hot timer or reaches the selected delay threshold. Entry: `LDAA $01`. |
| `$ED53–$ED57` | 3 | HUD present selects the external spark-status interlock before closed-loop enable. Entry: `LDAA $4B`. |
| `$ED6A–$ED6C` | 2 | O2 readiness timer expires or the readiness flag allows closed-loop operation. Entry: `ANDB #$FE`. |
| `$ED71–$ED75` | 3 | O2 readiness timer expires or the readiness flag allows closed-loop operation. Entry: `LDAB $44`. |
| `$ED7F–$ED84` | 3 | Closed-loop operation qualifies learning through the low/high coolant bounds, stoichiometric AFR and load. Entry: `LDAA $5B`. |
| `$ED86–$ED89` | 2 | Closed-loop operation qualifies learning through the low/high coolant bounds, stoichiometric AFR and load. Entry: `CMPA LC5DE`. |
| `$ED8B–$ED90` | 3 | Closed-loop operation qualifies learning through the low/high coolant bounds, stoichiometric AFR and load. Entry: `LDX $CE`. |
| `$ED92–$ED99` | 4 | Closed-loop operation qualifies learning through the low/high coolant bounds, stoichiometric AFR and load. Entry: `ORAB #$02`. |
| `$F043–$F046` | 2 | Diagnostic lamp timer is nonzero on this service and decrements. Entry: `DEC $B1`. |
| `$F07C–$F07F` | 3 | The retained diagnostic history scan selects the short/long lamp timer threshold and conditional reload. Entry: `LDAA $B1`. |
| `$F081–$F084` | 2 | The retained diagnostic history scan selects the short/long lamp timer threshold and conditional reload. Entry: `LDAB LC1D6`. |
| `$F086` | 1 | The retained diagnostic history scan selects the short/long lamp timer threshold and conditional reload. Entry: `LDAB LC1D8`. |
| `$F089–$F08A` | 2 | The retained diagnostic history scan selects the short/long lamp timer threshold and conditional reload. Entry: `CBA`. |
| `$F08C–$F08E` | 2 | The retained diagnostic history scan selects the short/long lamp timer threshold and conditional reload. Entry: `STAB $B1`. |
| `$F09A` | 1 | The retained diagnostic history scan selects the short/long lamp timer threshold and conditional reload. Entry: `LDAB LC1D9`. |
| `$F0DC–$F0E1` | 3 | Mode 4 enable and command bits select the field-service output override. Entry: `LDAA $0152`. |
| `$F0E3–$F0E8` | 3 | Mode 4 enable and command bits select the field-service output override. Entry: `LDAA $0153`. |
| `$F0EA` | 1 | Mode 4 enable and command bits select the field-service output override. Entry: `BRA LF0FC`. |
| `$F20A–$F20B` | 2 | The peripheral ready bit stays low for one poll or the entire 22-count timeout. Entry: `DECA`. |
| `$F20D–$F20E` | 2 | The peripheral ready bit stays low for one poll or the entire 22-count timeout. Entry: `SEC`. |
| `$F27B` | 1 | The unused IRQ vector at $FFF4 points to this RTI. No replay asserts this interrupt. Entry: `RTI`. |
| `$F282` | 1 | 3-D lookup enters its lower row-axis clamp on borrow. Entry: `CLRA`. |
| `$F3CA–$F3CD` | 2 | Coolant conversion crosses the resistor-range threshold or uses the alternate lookup table. Entry: `LDX #$FEB8`. |
| `$F3E7` | 1 | Coolant conversion crosses the resistor-range threshold or uses the alternate lookup table. Entry: `BSET $30 $01`. |
| `$F469` | 1 | The helper clips A against the lower or upper byte at X. Entry: `LDAA 0,X`. |
| `$F46F` | 1 | The helper clips A against the lower or upper byte at X. Entry: `LDAA 1,X`. |
| `$F50C–$F50E` | 2 | Diagnostic mode requests motor extension. Entry: `LDAB #$81`. |
| `$F51E` | 1 | Battery sample outside the open-loop IAC operating window disables the motor. Entry: `JMP LF640`. |
| `$F533–$F536` | 2 | Startup IAC request uses the no-A/C offset or saturates on addition. Entry: `ADDB LC631`. |
| `$F538` | 1 | Startup IAC request uses the no-A/C offset or saturates on addition. Entry: `LDAB #$00FF`. |
| `$F5A8–$F5AA` | 2 | IAC delay interpolation saturates the sum or restores the stacked base value. Entry: `LDAA #$00FF`. |
| `$F5AC` | 1 | IAC delay interpolation saturates the sum or restores the stacked base value. Entry: `PULA`. |
| `$F5B7` | 1 | HUD present ($33 bit 7) selects the external ROM callback at entry. Entry: `JSR $5803`. |
| `$F5C5–$F5C7` | 2 | Mode 4 IAC position control selects a signed step request from current and commanded positions, with magnitude limited to 127. Entry: `BITA #$02`. |
| `$F5C9–$F5CE` | 3 | Mode 4 IAC position control selects a signed step request from current and commanded positions, with magnitude limited to 127. Entry: `LDAA $2C`. |
| `$F5D0` | 1 | Mode 4 IAC position control selects a signed step request from current and commanded positions, with magnitude limited to 127. Entry: `NEGA`. |
| `$F5D1` | 1 | Mode 4 IAC position control selects a signed step request from current and commanded positions, with magnitude limited to 127. Entry: `BPL LF5D5`. |
| `$F5D3` | 1 | Mode 4 IAC position control selects a signed step request from current and commanded positions, with magnitude limited to 127. Entry: `LDAA #$7F`. |
| `$F5D5` | 1 | Mode 4 IAC position control selects a signed step request from current and commanded positions, with magnitude limited to 127. Entry: `BCS LF5D9`. |
| `$F5D7` | 1 | Mode 4 IAC position control selects a signed step request from current and commanded positions, with magnitude limited to 127. Entry: `ORAA #$80`. |
| `$F5D9–$F5DC` | 2 | Mode 4 IAC position control selects a signed step request from current and commanded positions, with magnitude limited to 127. Entry: `STAA $0101`. |
| `$F61F` | 1 | Retained IAC position and learning state select the trim increment, travel limit, saturation or signed correction. Entry: `INC $010E`. |
| `$F628–$F62B` | 2 | Retained IAC position and learning state select the trim increment, travel limit, saturation or signed correction. Entry: `LDAB $0101`. |
| `$F640` | 1 | Battery sample outside the open-loop IAC operating window disables the motor. Entry: `BCLR 0,Y 04`. |
| `$F64F` | 1 | Retained IAC position and learning state select the trim increment, travel limit, saturation or signed correction. Entry: `LDAB #$00FF`. |
| `$F65B` | 1 | Retained IAC position and learning state select the trim increment, travel limit, saturation or signed correction. Entry: `NEGB`. |
| `$F670–$F671` | 2 | Retained IAC position and learning state select the trim increment, travel limit, saturation or signed correction. Entry: `INCB`. |
| `$F8FD` | 1 | HUD present ($33 bit 7) selects the external ROM callback at entry. Entry: `JSR $5806`. |
| `$FA2A–$FA2E` | 3 | With TPS faults clear, a falling throttle puts the raw load axis $82 below its transient filter $DD. COMA yields the drop minus one, tested against $C30F = 10. Entry: `COMA`. |
| `$FA30` | 1 | A drop of 1–11 counts gives a complemented difference of 0–10, so $FA2E falls through and returns without clearing the enrichment latch. Entry: `BRA LFA57`. |
| `$FAB1–$FAB3` | 2 | The first SCI byte differs from device ID $80 at $C73F. The receiver follows the next-device link at $C73D (zero in BUA). Entry: `LDX 0,X`. |
| `$FAB5–$FABB` | 4 | The zero next-device link ends the scan for an unmatched ID. The receiver sleeps and clears the packet checksum. Entry: `LDAA #$27`. |
| `$FB0E` | 1 | The receive IRQ sees at least one hardware error bit in mask $0E at $4008 and branches to the error handler. Entry: `BRA LFB6D`. |
| `$FB6D–$FB71` | 3 | A receive error selects the framing-error result (4) when $4008 bit 1 is set. Entry: `LDAA #04`. |
| `$FB73` | 1 | A receive error has mask $0E nonzero but bit 1 clear. The handler selects the alternate error result (5). Entry: `LDAA #05`. |
| `$FB75–$FB78` | 2 | After either receive error, the high byte at $0131 distinguishes an active device pointer from an error before device selection. Entry: `LDAB $0131`. |
| `$FBFD–$FC02` | 4 | Direct output-buffer bytes require descriptor bits $C0 clear and payload beyond the mode. Valid Mode 0 has length 1. Modes 1–4 select table flags $80/$40. Unsupported modes default to the Mode-0 descriptor while reply-length logic still reads the request mode. Outside valid requests, reachability remains unverified. Entry: `LDX 6,X`. |
| `$FCFF–$FD01` | 2 | Factory IRQ battery ADC at or below 40 takes the timer check. $32 below 160 continues counting. Entry: `CMPB #160`. |
| `$FD03` | 1 | Factory low battery with $32 at least 160 takes the powerdown SWI. The C endpoint is modeled. execution of this ROM SWI is unverified. Entry: `SWI`. |
| `$FD47` | 1 | Factory mode $48 = 0 stores the toggled FMD command when ignition timer $32 is nonzero or either low bit of FMD byte $49 is set. Entry: `STAA $31`. |
| `$FDB3` | 1 | Outside the factory diagnostic window (ADC below 40 or at least 100), $FDA7 branches to the diagnostic SWI. The C endpoint is modeled. execution of this ROM SWI is unverified. Entry: `SWI`. |
