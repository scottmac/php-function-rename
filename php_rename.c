/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_rename.h"

const zend_function_entry rename_functions[] = {
    PHP_FE(rename_function, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry rename_module_entry = {
    STANDARD_MODULE_HEADER,
    "rename",
    rename_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "1.0",
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_RENAME
ZEND_GET_MODULE(rename)
#endif

/* {{{ proto bool rename_function(string orig_func_name, string new_func_name)
   Rename a function from its original to a new name. This is mainly useful in
   unittest to stub out untested functions */
PHP_FUNCTION(rename_function)
{
	zend_function *func, *dummy_func;
	char *orig_fname, *new_fname, *lower_orig, *lower_new;
	int orig_fname_len, new_fname_len, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &orig_fname, &orig_fname_len, &new_fname, &new_fname_len) == FAILURE) {
		return;
	}

	// Normalize the source function name to lower case.
	lower_orig = malloc(orig_fname_len+1);
	for (i = 0; i < orig_fname_len; i ++) {
		lower_orig[i] = tolower(orig_fname[i]);
	}
	lower_orig[i] = 0;

	// Normalize the destination function name to lower case.
	lower_new = malloc(new_fname_len + 1);
	for (i = 0; i < new_fname_len; i++) {
		lower_new[i] = tolower(new_fname[i]);
	}
	lower_new[i] = 0;

  if (zend_hash_find(EG(function_table), lower_orig, orig_fname_len + 1, (void **) &func) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s(%s, %s) failed: %s does not exist!"			,
						get_active_function_name(TSRMLS_C),
						lower_orig,  lower_new, lower_orig);
		RETVAL_FALSE;
		goto rename_end;
	}

	if (func->type != ZEND_USER_FUNCTION) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s(%s, %s) failed: \"%s\" is an internal function!"			,
							get_active_function_name(TSRMLS_C),
							lower_orig,  lower_new, lower_orig);
		RETVAL_FALSE;
		goto rename_end;
	}

	if (zend_hash_find(EG(function_table), lower_new, new_fname_len + 1, (void **) &dummy_func) == SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s(%s, %s) failed: %s already exists!"			,
							get_active_function_name(TSRMLS_C),
							lower_orig,  lower_new, lower_new);
		RETVAL_FALSE;
		goto rename_end;
	}

	if (zend_hash_add(EG(function_table), lower_new, new_fname_len + 1, func, sizeof(zend_function), NULL) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s() failed to insert %s into EG(function_table)", get_active_function_name(TSRMLS_C), lower_new);
		RETVAL_FALSE;
		goto rename_end;
	}

	/* NOTE(cjiang): This line of code is our fix for APD's rename_function. */
	function_add_ref(func);

	if (zend_hash_del(EG(function_table), lower_orig, orig_fname_len + 1) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s() failed to remove %s from function table", get_active_function_name(TSRMLS_C), lower_orig);

		zend_hash_del(EG(function_table), lower_new, new_fname_len + 1);
		RETVAL_FALSE;
		goto rename_end;
	}
	RETVAL_TRUE;

rename_end:
	free(lower_orig);
	free(lower_new);
}
/* }}} */