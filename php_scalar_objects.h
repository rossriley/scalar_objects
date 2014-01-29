/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SCALAR_OBJECTS_H
#define PHP_SCALAR_OBJECTS_H


extern zend_module_entry scalar_objects_module_entry;
#define phpext_scalar_objects_ptr &scalar_objects_module_entry

#ifdef PHP_WIN32
#	define PHP_SCALAR_OBJECTS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SCALAR_OBJECTS_API __attribute__ ((visibility("default")))
#else
#	define PHP_SCALAR_OBJECTS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif
#include "scalar_object_handlers.h"


int get_type_from_string(const char *str);

ZEND_MINIT_FUNCTION(scalar_objects);
ZEND_MSHUTDOWN_FUNCTION(scalar_objects);
ZEND_RINIT_FUNCTION(scalar_objects);
ZEND_RSHUTDOWN_FUNCTION(scalar_objects);
ZEND_MINFO_FUNCTION(scalar_objects);

#define SCALAR_OBJECTS_MAX_HANDLER  IS_RESOURCE
#define SCALAR_OBJECTS_NUM_HANDLERS (SCALAR_OBJECTS_MAX_HANDLER + 1)

ZEND_BEGIN_MODULE_GLOBALS(scalar_objects)
	zend_class_entry *handlers[SCALAR_OBJECTS_NUM_HANDLERS];
ZEND_END_MODULE_GLOBALS(scalar_objects)

#ifdef ZTS
#define SCALAR_OBJECTS_G(v) TSRMG(scalar_objects_globals_id, zend_scalar_objects_globals *, v)
extern int scalar_objects_globals_id;
#else
#define SCALAR_OBJECTS_G(v) (scalar_objects_globals.v)
extern zend_scalar_objects_globals scalar_objects_globals;
#endif

#if ZEND_MODULE_API_NO >= 20121204
#define ZEND_ENGINE_2_5
#endif

#endif
