#!/bin/sh
TIMEFORMAT='%U'

print_point() {
    echo "m0=$m0 m12=$m12 a0=$a0 tan(beta)=$tan_beta sign(mu)=$sign_mu"
}

measure_time() {
    if test $# -lt 1 ; then
        echo "measure_time: no arguments given"
        error="1"
    fi

    # time=`time `
    _start_time=`date +%s.%N`
    $*
    error="$?"
    _stop_time=`date +%s.%N`
    time=$(echo "$_stop_time - $_start_time" | bc | awk '{printf "%f", $0}')
}

valid_spectrum() {
    if test $# -ne 1 ; then
        echo "valid_spectrum: only one argument accepted"
        error="1"
        return 1;
    fi

    _slha_file="$1"

    if test "x${_slha_file}" = "x" ; then
        error="1"
        return 1;
    fi
    if ! test -r "${_slha_file}" ; then
        error="1"
        return 1;
    fi

    _is_valid=`awk -f valid_spectrum.awk ${_slha_file}`
    if test "x$_is_validx" = "xyesx" ; then
        error="0"
        return 0; # ok
    else
        error="1"
        return 1; # not ok
    fi
}

# directory of this script
BASEDIR=$(dirname $0)

fs_path="/home/avoigt/tu/research/FlexibleSUSY/models/MSSM/run_MSSM.x"
ss_path="/home/avoigt/packages/softsusy-3.3.9/softpoint.x"
sp_path="/home/avoigt/packages/SPheno-3.2.3/bin/SPheno"
spmssm_path="/home/avoigt/packages/SPheno-3.2.3/bin/SPhenoMSSM"

random_float="${BASEDIR}/random_float.x"
random_sign="${BASEDIR}/random_sign.x"

slha_template="${BASEDIR}/mssm_generic.slha2"

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

echo "# m0/GeV | m12/GeV | tan(beta) | sign(mu) | A0/GeV" \
    " | Softsusy time/s | Softsusy error" \
    " | FlexibleSUSY time/s | FlexibleSUSY error" \
    " | SPheno time/s | SPheno error" \
    " | SPhenoMSSM time/s | SPhenoMSSM error"

while [ true ]
do
    # create point
    m0=`$random_float 50 1000`
    m12=`$random_float 50 1000`
    a0=`$random_float -1000 1000`
    tan_beta=`$random_float 1 100`
    sign_mu=`$random_sign`

    rm -f out.spc SPheno.spc SPheno.spc.MSSM SPheno.out Messages.out

    slha_file="${BASEDIR}/${slha_template}.point"
    error="0"

    cp $slha_template $slha_file
    echo "Block MINPAR\n"     \
         "   1   $m0\n"       \
         "   2   $m12\n"      \
         "   3   $tan_beta\n" \
         "   4   $sign_mu\n"  \
         "   5   $a0"         \
        >> $slha_file

    measure_time $ss_path leshouches < $slha_file > out.spc 2> /dev/null
    ss_time="$time"
    if ! test "x$error" = "x0" ; then
        valid_spectrum out.spc
    fi
    ss_error="$error"

    measure_time $fs_path --slha-input-file=$slha_file --slha-output-file=out.spc > /dev/null 2>&1
    fs_time="$time"
    if ! test "x$error" = "x0" ; then
        valid_spectrum out.spc
    fi
    fs_error="$error"

    measure_time $sp_path $slha_file > /dev/null 2>&1
    sp_time="$time"
    if ! test "x$error" = "x0" ; then
        valid_spectrum SPheno.spc
    fi
    sp_error="$error"

    measure_time $spmssm_path $slha_file > /dev/null 2>&1
    spmssm_time="$time"
    if ! test "x$error" = "x0" ; then
        valid_spectrum SPheno.spc.MSSM
    fi
    spmssm_error="$error"

    echo "$m0 \t $m12 \t $tan_beta \t $sign_mu \t $a0" \
        " \t $ss_time \t $ss_error" \
        " \t $fs_time \t $fs_error" \
        " \t $sp_time \t $sp_error" \
        " \t $spmssm_time \t $spmssm_error"
done
