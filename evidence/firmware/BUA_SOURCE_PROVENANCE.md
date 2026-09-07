# BUA corrected-source and build provenance

## Purpose

This record establishes the relationship between the corrected assembly source, the zero-error listing already retained by the repository, the generated S-record/binary outputs, and the historical PROM images located in the project owner's Google Drive.

## Located build set

The following files were found together under `/Google Drive/Cars/OBD_Scan/ASM11`:

| File | Size | SHA-256 |
| --- | ---: | --- |
| `bua-hac.asm` | 447,436 bytes | `8ffdae1c94b892098e6b44ed5b380d10334ad34d37a48f9d48655e3713ec633c` |
| `bua-hac.lst` | 938,156 bytes | `5dd9df745532fae26ef6a19deb4402690e64bb8a254aeb33d10c14a1ad6dce83` |
| `bua-hac.s19` | 38,748 bytes | `cb579761ceca0c9d80b6403a662f7fa461d0a29be6cbe0589eb495425afa17e2` |
| `bua-hac.bin` | 16,384 bytes | `302cd1d2cc3ecdf7cf097085c426b9f609dfeb7a6ac8131f268cde10fbb12fc2` |
| `BUA_from_Car.bin` | 16,384 bytes | `c3948761aab0a44578fcc539262bcca58b690142c42d5c2c7afeb64be6a3b4fd` |
| `BUA 86 vette 5.7TPI auto with 3.07.bin` | 16,384 bytes | `9924abd4071ef1660753bf2db1aa61e6c1d815872dfbf2362725aa56341dfb31` |
| `BUA_1728.bin` | 16,384 bytes | `49de6d54d3f1a73ddf646e686fbc61e110223584f0883c62b96fefc48f874483` |

The repository copy of `evidence/firmware/bua-hac.lst` has the same SHA-256 as the Drive listing above, proving that the retained authoritative listing is this exact build artifact.

## Source-to-listing relationship

`bua-hac.asm` contains 14,724 source lines. The listing contains source line numbers 1 through 14,724 in the same order and reports:

- `Number of errors 0`
- `Number of warnings 0`

Its header identifies the assembler output as `AS11M 01.05` and dates the listing to Wed Jul 19, 2023 20:02.

The listing reproduces the assembly source text line-for-line in its source field, while additionally inserting assembled addresses, emitted bytes, and labels into the listing columns. This establishes `bua-hac.asm` as the source corresponding to the authoritative `bua-hac.lst` build.

## Listing / S-record / binary agreement

The listing emits 16,092 explicit bytes between `$C000` and `$FFFF`; the remaining addresses are gaps created by `ORG`/unemitted regions.

The generated `bua-hac.s19` contains exactly the same 16,092 addressed bytes. A byte-for-byte comparison over every emitted address found **zero differences** between the listing and the S-record.

The generated 16 KiB `bua-hac.bin`, mapped to `$C000-$FFFF`, also matches every byte explicitly emitted by the listing: **zero differences across all 16,092 emitted addresses**.

Therefore the surviving build chain is directly verified as:

`bua-hac.asm` -> `bua-hac.lst` / `bua-hac.s19` / `bua-hac.bin`

## Relationship to historical PROM images

The corrected generated `bua-hac.bin` differs from `BUA 86 vette 5.7TPI auto with 3.07.bin` at only two byte positions:

| Address | corrected build | historical image |
| --- | ---: | ---: |
| `$C007` | `$3A` | `$08` |
| `$C6AE` | `$B6` | `$84` |

It differs from `BUA_from_Car.bin` at only three byte positions:

| Address | corrected build | from-car image |
| --- | ---: | ---: |
| `$C005` | `$EE` | `$77` |
| `$C007` | `$3A` | `$08` |
| `$C6AE` | `$B6` | `$84` |

The corrected assembly source explicitly documents the intended correction at `$C6AE` and the resulting checksum change. Around the checksum definition it retains the older `$9708` checksum as a commented line and emits `$973A`, annotated `C6AE B6 instead of 84`. At listing address `$C6AE`, the source emits decimal 182 (`$B6`) and comments that the BUA 86 Vette image contained `$84`/132 and that the revised value appears correct.

The `$C005` difference in `BUA_from_Car.bin` is within the sequence-number word at `$C004-$C005`, so that byte is variant/identity metadata rather than evidence of a code-body discrepancy.

The later `BUA_1728.bin` differs from the corrected build at 58 positions and should be treated as a related but distinct calibration/image variant, not as the direct target of this corrected-source build.

## Evidence conclusion

This evidence resolves the earlier uncertainty over whether the corrected assembly source and generated build products survived. They did.

The strongest supported interpretation is:

1. `bua-hac.asm` is the corrected source used for the zero-error `bua-hac.lst` build retained by the repository.
2. `bua-hac.s19` and `bua-hac.bin` are direct generated outputs of that same source/listing build and are byte-identical to the listing at every emitted address.
3. The corrected build is extremely close to the historical 1986 Vette image: only the checksum byte and the documented `$C6AE` correction differ from `BUA 86 vette 5.7TPI auto with 3.07.bin`.
4. `BUA_from_Car.bin` adds only one further difference, in the sequence-number field.
5. The authoritative status of the listing remains appropriate for address/byte evidence, while the corrected `.asm` can now also be treated as its verified source counterpart rather than an unverified text reconstruction.

The original raw binary and S-record files remain external historical artifacts unless separately imported into the repository.