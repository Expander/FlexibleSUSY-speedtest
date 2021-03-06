#!/bin/sh
TIMEFORMAT='%U'
LC_NUMERIC=C
LC_COLLATE=C

print_point() {
    echo "m0=$m0 m12=$m12 a0=$a0 tan(beta)=$tan_beta sign(mu)=$sign_mu"
}

measure_time() {
    if test $# -lt 1 ; then
        echo "Error: measure_time: no arguments given"
        error="1"
    fi

    _start_time=`date +%s.%N`
    $*
    error="$?"
    _stop_time=`date +%s.%N`
    time=$(echo "$_stop_time - $_start_time" | bc | awk '{printf "%f", $0}')
}

valid_spectrum() {
    if test $# -ne 1 ; then
        echo "Error: valid_spectrum: only one argument accepted"
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

    _is_valid=`awk -f ${BASEDIR}/valid_spectrum.awk ${_slha_file}`
    if test "x${_is_valid}x" = "xyesx" ; then
        error="0"
        return 0; # ok
    else
        error="1"
        return 1; # not ok
    fi
}

# directory of this script
BASEDIR=$(dirname $0)

fs_path="./run_CMSSM.x"
fsnofv_path="./run_CMSSMNoFV.x"
ss_path="./softpoint.x"
sp_path="./SPheno"
spmssm_path="./SPhenoMSSM"

random_float="${BASEDIR}/random_float.x"
random_sign="${BASEDIR}/random_sign.x"

slha_template="${BASEDIR}/mssm_generic.slha2"

if ! test -x $fs_path ; then
    echo "Error: FlexibleSUSY CMSSM executable not found: $fs_path"
    exit 1
fi
if ! test -x $fsnofv_path ; then
    echo "Error: FlexibleSUSY CMSSMNoFV executable not found: $fsnofv_path"
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

fs_version=$($fs_path --version)
ss_version=$($ss_path --version 2>&1)

printf "# CPU: $(cat /proc/cpuinfo | grep "model name" | head -n 1)\n"
printf "# CPU cores: $(nproc)\n"
printf "# SS = ${ss_version} (no FV)\n"
printf "# FS = ${fs_version} (FV)\n"
printf "# SP = SPheno 3.3.7 (no FV)\n"
printf "# SSP = SPheno 3.3.7 with SARAH 4.5.8 (FV)\n"
printf "\n"
printf "# %20s %20s %20s %20s %20s" "m0/GeV" "m12/GeV" "tan(beta)" "sign(mu)" "A0/GeV"
printf "%20s %20s" "SS time/s" "SS error"
printf "%20s %20s" "FS time/s" "FS error"
printf "%20s %20s" "SP time/s" "SP error"
printf "%20s %20s" "SSP time/s" "SSP error"
printf "%20s %20s" "FS-NoFV time/s" "FS-NoFV error"
printf "\n"

while [ true ]
do
    # create point
    m0=`$random_float 50 2000`
    m12=`$random_float 50 2000`
    a0=`$random_float -1000 1000`
    tan_beta=`$random_float 1 50`
    sign_mu=`$random_sign`

    rm -f out.spc SPheno.spc SPheno.spc.MSSM SPheno.out Messages.out

    slha_file="${BASEDIR}/${slha_template}.point"
    error="0"

    cp $slha_template $slha_file
    echo "Block MINPAR"     >> $slha_file
    echo "   1   $m0"       >> $slha_file
    echo "   2   $m12"      >> $slha_file
    echo "   3   $tan_beta" >> $slha_file
    echo "   4   $sign_mu"  >> $slha_file
    echo "   5   $a0"       >> $slha_file

    measure_time $ss_path leshouches < $slha_file > out.spc 2> /dev/null
    ss_time="$time"
    if test "x$error" = "x0" ; then
        valid_spectrum out.spc
    fi
    ss_error="$error"

    measure_time $fs_path --slha-input-file=$slha_file --slha-output-file=out.spc > /dev/null 2>&1
    fs_time="$time"
    if test "x$error" = "x0" ; then
        valid_spectrum out.spc
    fi
    fs_error="$error"

    measure_time $fsnofv_path --slha-input-file=$slha_file --slha-output-file=out.spc > /dev/null 2>&1
    fsnofv_time="$time"
    if test "x$error" = "x0" ; then
        valid_spectrum out.spc
    fi
    fsnofv_error="$error"

    measure_time $sp_path $slha_file > /dev/null 2>&1
    sp_time="$time"
    if test "x$error" = "x0" ; then
        valid_spectrum SPheno.spc
    fi
    sp_error="$error"

    measure_time $spmssm_path $slha_file > /dev/null 2>&1
    spmssm_time="$time"
    if test "x$error" = "x0" ; then
        valid_spectrum SPheno.spc.MSSM
    fi
    spmssm_error="$error"

    printf "  %20e %20e %20e %20i %20e" "$m0" "$m12" "$tan_beta" "$sign_mu" "$a0"
    printf "%20g %20i" "$ss_time" "$ss_error"
    printf "%20g %20i" "$fs_time" "$fs_error"
    printf "%20g %20i" "$sp_time" "$sp_error"
    printf "%20g %20i" "$spmssm_time" "$spmssm_error"
    printf "%20g %20i" "$fsnofv_time" "$fsnofv_error"
    printf "\n"
done
