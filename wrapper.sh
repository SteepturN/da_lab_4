#!/usr/bin/env bash

set -o errexit
set -o nounset

log_error() {
    log_ "error" "$@"
}
log_() {
    type=$1
    message=$2
    date_str=; date_str=$(date +'%Y-%m-%d %H:%M:%S')
    echo "[${type}] [${date_str}] ${message}"
}
log_info() {
  log_ "info" "$@"
}
main() {
    test_file=./gen_tests/gentest
    answer_main=./gen_tests/main_ans
    answer_easy=./gen_tests/easy_ans
    test_count=1

    if [[ ! -d "./gen_tests" ]]; then
        mkdir ./gen_tests
    fi
    while true ; do
        # printf "[ %d ]: " "${test_count}"

        if ! ./generator.py "${test_file}" "10"; then
            log_error "Failed to run test ${test_file}"
            return 1
        fi

        # echo -n " test generated -- "

        # if ! /usr/bin/time -f "%S" -o .tmp_gen ./main.bin < "${test_file}" > "${answer_main}" ; then
        if ! ./main.bin < "${test_file}" > "${answer_main}" ; then
            log_error "Failed to run test ${test_file}"
            return 1
        fi

        # echo -n "main: "
        # wc -l "${answer_main}" | sed -z 's/\([[:digit:]]\+\) .*\n/\1/'

        if ! ./easy.bin < "${test_file}" > "${answer_easy}" ; then
            log_error "Failed to run test ${test_file}"
            return 1
        fi

        # printf "\teasy: "
        # wc -l "${answer_easy}" | sed -z 's/\([[:digit:]]\+\) .*\n/\1/'
        # printf "\t"

        if ! diff -q "${answer_main}" "${answer_easy}" ; then
            log_error "Failed to check test ${test_file}."
            return 1
        fi
        # log_info "${test_file}: OK"
        # echo
        (( ++test_count ))

        # clear
        # if [[ -e ${test_count} 2 ]]; then
        #     printf ""
        # fi
    done
}
main "$@"
