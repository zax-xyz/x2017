#!/bin/bash

# ANSI colour codes
RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e $BLUE--- TESTING objdump_x2017 ---$NC

passed=0
total=0

for t in tests/*.asm; do
	let total++
	base=${t%.asm}

	echo -n Testing $base... 
	out=$(diff --color=always <(./objdump_x2017 $base.x2017) $t) 

	if [ $? ]; then
		let passed++
		echo -e $GREEN PASSED$NC
	else
		echo -e $RED FAILED$NC
		echo "$out"
	fi
done;

echo -e Passed $ORANGE$passed/$total$NC tests

echo

echo -e $BLUE--- TESTING vm_x2017 ---$NC

passed=0
total=0

for t in tests/*.out; do
	let total++
	base=${t%.out}

	echo -n Testing $base... 
	out=$(diff --color=always <(./vm_x2017 $base.x2017) $t) 

	if [ $? ]; then
		let passed++
		echo -e $GREEN PASSED$NC
	else
		echo -e $RED FAILED$NC
		echo "$out"
	fi
done;

echo -e Passed $ORANGE$passed/$total$NC tests
