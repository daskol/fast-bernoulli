#!/bin/sh
#
#   compile-and-test.sh
#
#   This is a simple script that builds all targets and update build
#   configuration and then it runs tests. If there is no fails, then pre-commit
#   check is passed.

function prefix() {
    echo -ne "\033[1;30m[$(date +'%Y-%m-%d %H:%M')]\033[0m"
}

echo -n "$(prefix) Update build configuration..."
cd build/debug
output=$(cmake ../.. \
             -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
             -DCMAKE_BUILD_TYPE=Debug)
retcode=$?

if [ $retcode -ne 0 ]; then
    echo "FAIL"
    echo "$output"
    echo
    echo "ERROR: Failed to update build configuration."
    exit 1
else
    echo "OK"
fi

echo -n "$(prefix) Build all targets..."
output=$(make)
retcode=$?

if [ $retcode -ne 0 ]; then
    echo "FAIL"
    echo "$output"
    echo
    echo "ERROR: Failed to build some target."
    exit 1
else
    echo "OK"
fi

echo -n "$(prefix) Run all tests..."
output=$(src/fast-bernoulli-test --gtest_color=yes)
retcode=$?

if [ $retcode -ne 0 ]; then
    echo "FAIL"
    echo "$output"
    echo
    echo "ERROR: Some test are failed."
    exit 1
else
    echo "OK"
fi

echo "$(prefix) Ok."
