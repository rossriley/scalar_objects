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
  | Author: Nikita Popov <nikic@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_scalar_objects.h"

ZEND_DECLARE_MODULE_GLOBALS(scalar_objects)

#ifdef COMPILE_DL_SCALAR_OBJECTS
ZEND_GET_MODULE(scalar_objects)
#endif

#ifdef ZEND_ENGINE_2_5
#define SO_EX_CV(i)     (*EX_CV_NUM(execute_data, i))
#define SO_EX_T(offset) (*EX_TMP_VAR(execute_data, offset))
#else
#define SO_EX_CV(i)     (execute_data)->CVs[(i)]
#define SO_EX_T(offset) (*(temp_variable *) ((char *) execute_data->Ts + offset))
#endif

#define FREE_OP(should_free)                                           \
    if (should_free.var) {                                             \
        if ((zend_uintptr_t)should_free.var & 1L) {                    \
            zval_dtor((zval*)((zend_uintptr_t)should_free.var & ~1L)); \
        } else {                                                       \
            zval_ptr_dtor(&should_free.var);                           \
        }                                                              \
    }

#define FREE_OP_IF_VAR(should_free)                                                 \
    if (should_free.var != NULL && (((zend_uintptr_t)should_free.var & 1L) == 0)) { \
        zval_ptr_dtor(&should_free.var);                                            \
    }

static zval *get_zval_ptr_safe(
	int op_type, const znode_op *node, const zend_execute_data *execute_data
) {
	switch (op_type) {
		case IS_CONST:
			return node->zv;
		case IS_TMP_VAR:
			return &SO_EX_T(node->var).tmp_var;
		case IS_VAR:
			return SO_EX_T(node->var).var.ptr;
		case IS_CV: {
			zval **tmp = SO_EX_CV(node->constant);
			return tmp ? *tmp : NULL;
		}
		default:
			return NULL;
	}
}

static zval *get_object_zval_ptr_safe(
	int op_type, const znode_op *node, const zend_execute_data *execute_data TSRMLS_DC
) {
	if (op_type == IS_UNUSED) {
		return EG(This);
	} else {
		return get_zval_ptr_safe(op_type, node, execute_data);
	}
}

static zval *get_zval_ptr_real(
	int op_type, const znode_op *node, const zend_execute_data *execute_data,
	zend_free_op *should_free, int type TSRMLS_DC
) {
#ifdef ZEND_ENGINE_2_5
	return zend_get_zval_ptr(op_type, node, execute_data, should_free, type TSRMLS_CC);
#else
	return zend_get_zval_ptr(op_type, node, execute_data->Ts, should_free, type TSRMLS_CC);
#endif
}

static zval *get_object_zval_ptr_real(
	int op_type, const znode_op *node, const zend_execute_data *execute_data,
	zend_free_op *should_free, int type TSRMLS_DC
) {
	if (op_type == IS_UNUSED) {
		if (!EG(This)) {
			zend_error(E_ERROR, "Using $this when not in object context");
		}

		should_free->var = 0;
		return EG(This);
	} else {
		return get_zval_ptr_real(op_type, node, execute_data, should_free, type TSRMLS_CC);
	}
}

static zval **get_zval_ptr_ptr_real(
	int op_type, const znode_op *node, const zend_execute_data *execute_data,
	zend_free_op *should_free, int type TSRMLS_DC
) {
#ifdef ZEND_ENGINE_2_5
	return zend_get_zval_ptr_ptr(op_type, node, execute_data, should_free, type TSRMLS_CC);
#else
	return zend_get_zval_ptr_ptr(op_type, node, execute_data->Ts, should_free, type TSRMLS_CC);
#endif
}

static zval **get_object_zval_ptr_ptr_real(
	int op_type, const znode_op *node, const zend_execute_data *execute_data,
	zend_free_op *should_free, int type TSRMLS_DC
) {
	if (op_type == IS_UNUSED) {
		if (!EG(This)) {
			zend_error(E_ERROR, "Using $this when not in object context");
		}

		should_free->var = 0;
		return &EG(This);
	} else {
		return get_zval_ptr_ptr_real(op_type, node, execute_data, should_free, type TSRMLS_CC);
	}
}

static int scalar_objects_method_call_handler(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op *opline = execute_data->opline;
	zend_free_op free_op1, free_op2;
	zval *obj, *method;
	zend_class_entry *ce;
	zend_function *fbc;

	/* First we fetch the ops without refcount changes or errors. Then we check whether we want
	 * to handle this opcode ourselves or fall back to the original opcode. Only once we know for
	 * certain that we will not fall back the ops are fetched for real. */
	obj = get_object_zval_ptr_safe(opline->op1_type, &opline->op1, execute_data TSRMLS_CC);
	method = get_zval_ptr_safe(opline->op2_type, &opline->op2, execute_data);

	if (!obj || Z_TYPE_P(obj) == IS_OBJECT || Z_TYPE_P(method) != IS_STRING) {
		return ZEND_USER_OPCODE_DISPATCH;
	}

	ce = SCALAR_OBJECTS_G(handlers)[Z_TYPE_P(obj)];
	if (!ce) {
		zend_error(E_ERROR, "Call to a member function %s() on a non-object", Z_STRVAL_P(method));
	}

	if (ce->get_static_method) {
		fbc = ce->get_static_method(ce, Z_STRVAL_P(method), Z_STRLEN_P(method) TSRMLS_CC);
	} else {
		fbc = zend_std_get_static_method(
			ce, Z_STRVAL_P(method), Z_STRLEN_P(method),
			opline->op2_type == IS_CONST ? opline->op2.literal + 1 : NULL TSRMLS_CC
		);
	}

	if (!fbc) {
		zend_error(E_ERROR, "Call to undefined method %s::%s()", ce->name, Z_STRVAL_P(method));
	}

	method = get_zval_ptr_real(opline->op2_type, &opline->op2, execute_data, &free_op2, BP_VAR_R TSRMLS_CC);

	{
		zval **obj_ptr = get_object_zval_ptr_ptr_real(
			opline->op1_type, &opline->op1, execute_data, &free_op1, BP_VAR_R TSRMLS_CC
		);

		if (obj_ptr) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(obj_ptr);
			obj = *obj_ptr;
		} else {
			/* I think that we currently can't hit this case because INIT_METHOD_CALL is specd
			 * only for CV and VAR and not for TMP_VAR and CONST. Still leaving it in to be sure. */
			obj = get_object_zval_ptr_real(
				opline->op1_type, &opline->op1, execute_data, &free_op1, BP_VAR_R TSRMLS_CC
			);
		}
	}

	Z_ADDREF_P(obj);

#ifdef ZEND_ENGINE_2_5
	execute_data->call = execute_data->call_slots + opline->result.num;
	execute_data->call->fbc = fbc;
	execute_data->call->called_scope = ce;
	execute_data->call->object = obj;
	execute_data->call->is_ctor_call = 0;
#else
	zend_ptr_stack_3_push(&EG(arg_types_stack), execute_data->fbc, execute_data->object, execute_data->called_scope);

	execute_data->fbc = fbc;
	execute_data->called_scope = ce;
	execute_data->object = obj;
#endif

	FREE_OP(free_op2);
	FREE_OP_IF_VAR(free_op1);

	execute_data->opline++;
	return ZEND_USER_OPCODE_CONTINUE;
}

static int get_type_from_string(const char *str) {
	/* Not all of these types will make sense in practice, but for now
	 * we support all of them. */
	if (!strcasecmp(str, "null")) {
		return IS_NULL;
	} else if (!strcasecmp(str, "bool")) {
		return IS_BOOL;
	} else if (!strcasecmp(str, "int")) {
		return IS_LONG;
	} else if (!strcasecmp(str, "float")) {
		return IS_DOUBLE;
	} else if (!strcasecmp(str, "string")) {
		return IS_STRING;
	} else if (!strcasecmp(str, "array")) {
		return IS_ARRAY;
	} else if (!strcasecmp(str, "resource")) {
		return IS_RESOURCE;
	} else {
		zend_error(E_WARNING, "Invalid type \"%s\" specified", str);
		return -1;
	}
}

ZEND_FUNCTION(register_primitive_type_handler) {
	char *type_str;
	int type_str_len;
	int type;
	zend_class_entry *ce = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sC", &type_str, &type_str_len, &ce) == FAILURE) {
		return;
	}

	type = get_type_from_string(type_str);
	if (type == -1) {
		return;
	}

	if (SCALAR_OBJECTS_G(handlers)[type] != NULL) {
		zend_error(E_WARNING, "Handler for type \"%s\" already exists, overriding", type_str);
	}

	SCALAR_OBJECTS_G(handlers)[type] = ce;
}

ZEND_FUNCTION(get_primitive_type_handlers) {

	zend_class_entry *array_handler;
	zend_class_entry *bool_handler;
	zend_class_entry *float_handler;
	zend_class_entry *int_handler;
	zend_class_entry *null_handler;
	zend_class_entry *string_handler;

	if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    array_handler = SCALAR_OBJECTS_G(handlers)[4];
    bool_handler = SCALAR_OBJECTS_G(handlers)[3];
    float_handler = SCALAR_OBJECTS_G(handlers)[2];
    int_handler = SCALAR_OBJECTS_G(handlers)[1];
    null_handler = SCALAR_OBJECTS_G(handlers)[0];
    string_handler = SCALAR_OBJECTS_G(handlers)[5];
    php_printf("Array %s \nBool: %s \nFloat: %s \nInt: %s \nNull: %s \nString: %s",
    	&array_handler->type,
    	&bool_handler->type,
    	&float_handler->type,
    	&int_handler->type,
    	&null_handler->type,
    	&string_handler->type
    );
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_register_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, "type")
	ZEND_ARG_INFO(0, "class")
ZEND_END_ARG_INFO()

const zend_function_entry scalar_objects_functions[] = {
	ZEND_FE(register_primitive_type_handler, arginfo_register_handler)
	ZEND_FE_END
};

zend_module_entry scalar_objects_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"scalar_objects",
	scalar_objects_functions,
	ZEND_MINIT(scalar_objects),
	ZEND_MSHUTDOWN(scalar_objects),
	ZEND_RINIT(scalar_objects),
	ZEND_RSHUTDOWN(scalar_objects),
	ZEND_MINFO(scalar_objects),
	"0.1",
	ZEND_MODULE_GLOBALS(scalar_objects),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};




/****** Code to manage handlers below here *******/


/***** Methods for base SplScalarObject ********/
static zend_function_entry scalar_object_functions[] = {
    PHP_ME( SplScalarObject, isArray, 	NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isBool, 	NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isFloat, 	NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isInt, 	NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isNull, 	NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isString, 	NULL, ZEND_ACC_PUBLIC )
    PHP_FE_END
};

PHP_METHOD( SplScalarObject, isArray )
{
    RETURN_BOOL(0);
}
PHP_METHOD( SplScalarObject, isBool )
{
    RETURN_BOOL(0);
}
PHP_METHOD( SplScalarObject, isFloat )
{
    RETURN_BOOL(0);
}
PHP_METHOD( SplScalarObject, isInt )
{
    RETURN_BOOL(0);
}
PHP_METHOD( SplScalarObject, isNull )
{
    RETURN_BOOL(0);
}
PHP_METHOD( SplScalarObject, isString )
{
    RETURN_BOOL(0);
}

/****** Methods for SplScalarArray ************/

static zend_function_entry scalar_object_array_functions[] = {
	PHP_ME( SplScalarArray, isArray, NULL, ZEND_ACC_PUBLIC )
	PHP_FE_END
};

PHP_METHOD( SplScalarArray, isArray )
{
    RETURN_BOOL(1);
}

/****** Methods for SplScalarBoolean ************/

static zend_function_entry scalar_object_boolean_functions[] = {
	PHP_ME( SplScalarBoolean, isBool, NULL, ZEND_ACC_PUBLIC )
	PHP_FE_END
};

PHP_METHOD( SplScalarBoolean, isBool )
{
    RETURN_BOOL(1);
}

/****** Methods for SplScalarFloat ************/

static zend_function_entry scalar_object_float_functions[] = {
	PHP_ME( SplScalarFloat, isFloat, NULL, ZEND_ACC_PUBLIC )
	PHP_FE_END
};

PHP_METHOD( SplScalarFloat, isFloat )
{
    RETURN_BOOL(1);
}

/****** Methods for SplScalarInteger ************/

static zend_function_entry scalar_object_integer_functions[] = {
	PHP_ME( SplScalarInteger, isInt, NULL, ZEND_ACC_PUBLIC )
	PHP_FE_END
};

PHP_METHOD( SplScalarInteger, isInt )
{
    RETURN_BOOL(1);
}

/****** Methods for SplScalarNull ************/

static zend_function_entry scalar_object_null_functions[] = {
	PHP_ME( SplScalarNull, isNull, NULL, ZEND_ACC_PUBLIC )
	PHP_FE_END
};

PHP_METHOD( SplScalarNull, isNull )
{
    RETURN_BOOL(1);
}

/****** Methods for SplScalarString ************/

static zend_function_entry scalar_object_string_functions[] = {
	PHP_ME( SplScalarString, isString, NULL, ZEND_ACC_PUBLIC )
	PHP_FE_END
};

PHP_METHOD( SplScalarString, isString )
{
    RETURN_BOOL(1);
}

/**** Class initialisation *****/
static void register_scalar_objects()
{
	zend_class_entry *ce_SplScalarObject;
	zend_class_entry ce_sc, ce_sc_a, ce_sc_b, ce_sc_f, ce_sc_i, ce_sc_n, ce_sc_s;


    INIT_CLASS_ENTRY(ce_sc, "SplScalarObject", scalar_object_functions);
    ce_SplScalarObject = zend_register_internal_class(&ce_sc TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_sc_a, "SplScalarArray", scalar_object_array_functions);
    zend_register_internal_class_ex(&ce_sc_a, ce_SplScalarObject, NULL TSRMLS_CC);
	SCALAR_OBJECTS_G(handlers)[4] = &ce_sc_a;

    INIT_CLASS_ENTRY(ce_sc_b, "SplScalarBoolean", scalar_object_boolean_functions);
    zend_register_internal_class_ex(&ce_sc_b, ce_SplScalarObject, NULL TSRMLS_CC);
	SCALAR_OBJECTS_G(handlers)[3] = &ce_sc_b;


    INIT_CLASS_ENTRY(ce_sc_f, "SplScalarFloat", scalar_object_float_functions);
    zend_register_internal_class_ex(&ce_sc_f, ce_SplScalarObject, NULL TSRMLS_CC);
	SCALAR_OBJECTS_G(handlers)[2] = &ce_sc_f;

	INIT_CLASS_ENTRY(ce_sc_i, "SplScalarInteger", scalar_object_integer_functions);
    zend_register_internal_class_ex(&ce_sc_i, ce_SplScalarObject, NULL TSRMLS_CC);
	SCALAR_OBJECTS_G(handlers)[1] = &ce_sc_i;

    INIT_CLASS_ENTRY(ce_sc_n, "SplScalarNull", scalar_object_null_functions);
    zend_register_internal_class_ex(&ce_sc_n, ce_SplScalarObject, NULL TSRMLS_CC);
	SCALAR_OBJECTS_G(handlers)[0] = &ce_sc_n;

	INIT_CLASS_ENTRY(ce_sc_s, "SplScalarString", scalar_object_string_functions);
    zend_register_internal_class_ex(&ce_sc_s, ce_SplScalarObject, NULL TSRMLS_CC);
	SCALAR_OBJECTS_G(handlers)[6] = &ce_sc_s;




}
ZEND_MINIT_FUNCTION(scalar_objects) {
	zend_set_user_opcode_handler(ZEND_INIT_METHOD_CALL, scalar_objects_method_call_handler);

	register_scalar_objects(TSRMLS_CC);
	register_default_handlers(TSRMLS_CC);
	return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION(scalar_objects)
{
	return SUCCESS;
}

ZEND_RINIT_FUNCTION(scalar_objects)
{
	memset(SCALAR_OBJECTS_G(handlers), 0, SCALAR_OBJECTS_NUM_HANDLERS * sizeof(zend_class_entry *));

	return SUCCESS;
}

ZEND_RSHUTDOWN_FUNCTION(scalar_objects)
{
	return SUCCESS;
}

ZEND_MINFO_FUNCTION(scalar_objects)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Scalar Objects Support", "enabled");
	php_info_print_table_row(2, "Version", "0.1.0 Prototype Stage");
	php_info_print_table_row(2, "Handler Classes", "SplScalarObject, SplScalarArray, SplScalarBoolean, SplScalarFloat, SplScalarInteger, SplScalarNull, SplScalarString");
	php_info_print_table_end();
}

