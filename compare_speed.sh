#!/bin/sh

print_point() {
    echo "m0=$m0 m12=$m12 a0=$a0 tan(beta)=$tan_beta sign(mu)=$sign_mu"
}

# directory of this script
BASEDIR=$(dirname $0)

fs_path="/home/avoigt/tu/research/FlexibleSUSY/models/MSSM/run_MSSM.x"
ss_path="/home/avoigt/packages/softsusy-3.3.9/softpoint.x"
sp_path="/home/avoigt/packages/SPheno-3.2.3/bin/SPheno"
spmssm_path="/home/avoigt/packages/SPheno-3.2.3/bin/SPhenoMSSM"

random_float="./random_float.x"
random_sign="./random_sign.x"

if ! test -x $fs_path ; then
    echo "Error: FlexibleSUSY executable not found: $fs_path"
    exit 1
fi
if ! test -x $ss_path ; then
    echo "Error: Softsusy executable not found: $ss_path"
    exit 1
fi

if ! test -x $sp_path ; then
    echo "Error: SPheno executable not found: $sp_path"
    exit 1
fi

if ! test -x $spmssm_path ; then
    echo "Error: SPhenoMSSM executable not found: $spmssm_path"
    exit 1
fi

if ! test -x $random_float ; then
    echo "Error: $random_float not found"
    exit 1
fi

if ! test -x $random_sign ; then
    echo "Error: $random_sign not found"
    exit 1
fi

while [ true ]
do
    # create point
    m0=`$random_float 50 1000`
    m12=`$random_float 50 1000`
    a0=`$random_float -1000 1000`
    tan_beta=`$random_float 1 100`
    sign_mu=`$random_sign`

    print_point
done
