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
    while true ; do
        if ! ./generator.py "${test_file}" "${test_count}"; then
            log_error "Failed to run test ${test_file}"
            return 1
        fi
        (( ++test_count ))
        if ! ./main < "${test_file}" > "${answer_main}" ; then
            log_error "Failed to run test ${test_file}"
            return 1
        fi
        if ! ./easy < "${test_file}" > "${answer_easy}" ; then
            log_error "Failed to run test ${test_file}"
            return 1
        fi
        if ! diff -q "${answer_main}" "${answer_easy}" ; then
            log_error "Failed to check test ${test_file}."
            return 1
        fi
        log_info "${test_file}: OK"
    done
}
main "$@"
