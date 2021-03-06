# Copyright (c) 2018 - 2020, Samsung Electronics Co., Ltd. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#
# Author: Ernest Borowski <e.borowski@samsung.com>
# Author: Mateusz Nosek <m.nosek@samsung.com>
#
ifeq (${ESAN_PATH},)
	ESAN_PATH := $(shell realpath ./../)
endif

CC ?= clang

CFLAGS = ${CFLAGS_LOCAL}
LDFLAGS = ${LDFLAGS_LOCAL}
ifeq (,$(findstring -I${ESAN_PATH}/include,$(CFLAGS)))
CFLAGS += -I${ESAN_PATH}/include
endif

ESAN_LIB := ${ESAN_PATH}/error_sanitizer_preload.so

TESTS   = $(wildcard test_*.c) $(wildcard neg_test_*.c)
EXEC    = $(TESTS:.c=.elf)
RESULTS = $(TESTS:.c=.txt) neg_test_sigaction.txt

############################################################################

all: $(RESULTS)
	$(MAKE) clean_test_files

clean:
	rm -f $(EXEC)
	$(MAKE) clean_test_files

clean_test_files:
	rm -f *.cur_input
	rm -f *.esn_input

############################################################################

%.elf: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

test_fail_status.elf: test_fail_status.c ${ESAN_LIB}
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

test_fail_status.txt: test_fail_status.elf ${ESAN_LIB}
	echo "Executing: $< test."
	cp esan_always_succeed.map "$<.cur_input"
	LD_PRELOAD=${ESAN_LIB} ./$< "$<.cur_input"
	rm -f "$<.cur_input"

test_empty_map.txt: test_empty_map.elf ${ESAN_LIB}
	echo "Executing: $< test."
	touch "esan_empty.map.$<.cur_input"
	LD_PRELOAD=${ESAN_LIB} ./$< "esan_empty.map.$<.cur_input" 2>&1 \
	| grep -qE '^error_sanitizer\.c \| parse_map:3(14|17) \| Unable to read input data\. bitmap size: 0$$'
	rm -f "esan_empty.map.$<.cur_input"

test_SIG%.txt: test_SIG%.elf ${ESAN_LIB}
	echo "Executing: $< test."
	cp esan_always_succeed.map "esan_always_succeed.map.$<.cur_input"
	LD_PRELOAD=${ESAN_LIB} ./$< "esan_always_succeed.map.$<.cur_input" 2>&1 \
	| grep -vE '^(in_library|error_sanitizer)\.c' - | diff SIGtest_results.txt -
	rm -f "esan_always_succeed.map.$<.cur_input"
	rm -f "esan_always_succeed.map.$<.esn_input"

neg_test_execute_once.elf: neg_test_execute_once.c ${ESAN_LIB}
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

ifneq (,$(findstring -DDEBUG,$(CFLAGS)))
neg_test_execute_once.txt: neg_test_execute_once.elf ${ESAN_LIB}
	echo "Executing: $< test."
	cp esan_always_succeed.map "$<.cur_input"
	LD_PRELOAD=${ESAN_LIB} ./$< "$<.cur_input" 2>&1 \
	| grep -vE '^(in_library|error_sanitizer)\.c' - | diff neg_test_execute_once_results.txt -
	rm -f "$<.cur_input"
else
neg_test_execute_once.txt: neg_test_execute_once.elf  ${ESAN_LIB}
	echo "Executing: $< test."
	cp esan_always_succeed.map "$<.cur_input"
	LD_PRELOAD=${ESAN_LIB} ./$< "$<.cur_input"
	rm -f "$<.cur_input"
endif

sigaction_test_hook.so: sigaction_test_hook.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -shared

neg_test_sigaction.txt: test_empty_map.elf sigaction_test_hook.so
	cp esan_always_succeed.map "$<.cur_input"
	SIGACTION_FAIL=0 LD_PRELOAD=./$(word 2, $^):${ESAN_PATH}/error_sanitizer_preload.so ./$< "$<.cur_input" 2>&1 \
	| grep -x "esan_signal.c | esan_signal_initialize:5[5-6] | Unable to set signal handlers!"
	SIGACTION_FAIL=1 LD_PRELOAD=./$(word 2, $^):${ESAN_PATH}/error_sanitizer_preload.so ./$< "$<.cur_input" 2>&1 \
	| grep -x "esan_signal.c | esan_signal_initialize:5[5-6] | Unable to set signal handlers!"
	SIGACTION_FAIL=2 LD_PRELOAD=./$(word 2, $^):${ESAN_PATH}/error_sanitizer_preload.so ./$< "$<.cur_input" 2>&1 \
	| grep -x "esan_signal.c | esan_signal_destructor:6[6-7] | Unable to restore signal handlers!"
	rm -f "$<.cur_input"
