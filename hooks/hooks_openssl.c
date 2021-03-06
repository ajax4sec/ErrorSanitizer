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
    Author: Jakub Botwicz <j.botwicz@samsung.com>
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/

#ifndef ESAN_DISABLE_HOOKS_OPENSSL

#include "hooks_include.h"

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <string.h>

/* EVP_CIPHER_CTX* EVP_CIPHER_CTX_new(void); */

typedef EVP_CIPHER_CTX *(*EVP_CIPHER_CTX_new_func_t)(void);

static EVP_CIPHER_CTX *esan_on_fail_EVP_CIPHER_CTX_new(void)
{
	return NULL;
}

static EVP_CIPHER_CTX *real_EVP_CIPHER_CTX_new(void)
{
	static EVP_CIPHER_CTX_new_func_t EVP_CIPHER_CTX_new_func_ptr = NULL;
	if (NULL == EVP_CIPHER_CTX_new_func_ptr)
		EVP_CIPHER_CTX_new_func_ptr = (EVP_CIPHER_CTX_new_func_t)dlsym(
			RTLD_NEXT, "EVP_CIPHER_CTX_new");
	if (NULL != EVP_CIPHER_CTX_new_func_ptr)
		return (*EVP_CIPHER_CTX_new_func_ptr)();

	exit_failure(ESAN_DLSYM_ERROR,
		     "Error in dlsym - in 'EVP_CIPHER_CTX_new' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void)
{
	if (esan_should_I_fail(__builtin_return_address(0),
			       ESAN_EVP_CIPHER_CTX_NEW)) {
		return esan_on_fail_EVP_CIPHER_CTX_new();
	} else {
		return real_EVP_CIPHER_CTX_new();
	}
}

/**************************************************************************************/
#if OPENSSL_VERSION_NUMBER >= 0x11000000L
/* EVP_MD_CTX* EVP_MD_CTX_create(void); */

typedef EVP_MD_CTX *(*EVP_MD_CTX_create_func_t)(void);

static EVP_MD_CTX *esan_on_fail_EVP_MD_CTX_create(void)
{
	return NULL;
}

static EVP_MD_CTX *real_EVP_MD_CTX_create(void)
{
	static EVP_MD_CTX_create_func_t EVP_MD_CTX_create_func_ptr = NULL;
	if (NULL == EVP_MD_CTX_create_func_ptr)
		EVP_MD_CTX_create_func_ptr = (EVP_MD_CTX_create_func_t)dlsym(
			RTLD_NEXT, "EVP_MD_CTX_create");
	if (NULL != EVP_MD_CTX_create_func_ptr)
		return (*EVP_MD_CTX_create_func_ptr)();

	exit_failure(ESAN_DLSYM_ERROR,
		     "Error in dlsym - in 'EVP_MD_CTX_create' wrapper\n");
}

// parameter names starting with __ are reserved for standard library
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
EVP_MD_CTX *EVP_MD_CTX_create(void)
{
	if (esan_should_I_fail(__builtin_return_address(0),
			       ESAN_EVP_MD_CTX_CREATE)) {
		return esan_on_fail_EVP_MD_CTX_create();
	} else {
		return real_EVP_MD_CTX_create();
	}
}
#endif /* OPENSSL_VERSION_NUMBER */
#endif /* ESAN_DISABLE_HOOKS_OPENSSL */
