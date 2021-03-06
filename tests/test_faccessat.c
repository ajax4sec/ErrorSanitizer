/*
    Copyright (c) 2018 - 2020, Samsung Electronics Co., Ltd. All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

    Author: Ernest Borowski <e.borowski@samsung.com>
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include "esan_wrapper.h"
#include <fcntl.h>
#include <unistd.h>

int main()
{
	FILE *file;
	const char *path = "./faccessat.test";
	file = fopen(path, "a");
	if (!file)
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "Creating temporary file failed!");
	fclose(file);
	/* race condition here... I know */
	if (0 != faccessat(AT_FDCWD, path, F_OK, 0)) {
#ifdef ESAN_FAIL_TEST
		log("faccessat successfully failed.");
#else
		exit_failure(ESAN_TESTS_LIBRARY_FUNCTION_ERROR,
			     "Faccessat failed!");
#endif
	} else {
#ifdef ESAN_FAIL_TEST
		exit_failure(ESAN_TESTS_FAILURE,
			     "faccessat should have failed, but didn't.");
#else
		log("faccessat succeed.");
#endif
	}

	return 0;
}
