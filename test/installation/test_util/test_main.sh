#!/bin/bash
failure=0
expected="/mnt/tsunami/gerb-reproc/Archive/G1/SEV2/G1_SEV2_L20_BARG_SOL_M15_R50/2013/0209/G1_SEV2_L20_BARG_SOL_M15_R50_20130209_124500_V006.hdf.gz"
echo "# expected: $expected"
got=`./main`
echo "# got: $got"
if test "$got" = "$expected"; then
    echo "ok - C app"
else
    echo "not ok - C app"
    failure=1
fi
got=`./main.sh`
echo "# got: $got"
if test "$got" = "$expected"; then
    echo "ok - standalone utility"
else
    echo "not ok - standalone utility"
    failure=1
fi
exit $failure
