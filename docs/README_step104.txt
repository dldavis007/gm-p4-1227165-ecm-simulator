BUA / ECM 1227165 - Step 104 frozen normal-operation baseline

OnlineGDB: upload/use main.c as the single C source file.

Step 104 freezes the first deterministic normal-operation behavioral baseline.
The 31-second warm-restart profile covers catch, idle, throttle-tip-in AE,
acceleration, 60-MPH cruise, closed-loop entry, BLM cell selection and learning,
lift/DFCO, deceleration, fuel recovery, and returned idle.

The canonical Step-104 signature is:

    4BA6B7C6

It is a 32-bit FNV-1a stream over every post-IRQ snapshot of source RAM
$0000-$01FF, raw MPU $3FC0-$3FFF, raw I/O $4000-$400F/$5000, profile position,
and explicitly serialized final scheduler statistics.  Host structure layout,
padding, native byte order, console formatting, and floating-point text are not
part of the signature.

Two complete executions produced byte-for-byte identical output and the same
signature.  Step 104 passes 10/10, the integrated drive passes 26/26, all older
regressions remain clean, and strict C89 compilation has no warnings.

This is a normal-operation behavioral freeze, not a claim of total ROM or
electrical completeness.  Exceptional shutdown and diagnostic paths, damaged
source targets, optional H.U. ROM, MPU-internal event generation, output
polarity, and external transfer functions remain explicit boundaries.  The
existing LF250 utility translation is behaviorally regression-covered but
still flagged for a future instruction-level bit-exact utility audit.

See STEP104_SIGNATURE.txt for the machine-readable signature contract and
STEP104_BASELINE_FREEZE.txt for the full completeness/boundary audit.
