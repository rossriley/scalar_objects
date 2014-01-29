#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "php_scalar_objects.h"
#include "scalar_object_handlers.h"

/****** Code to manage handlers below here *******/


/***** Methods for base SplScalarObject ********/
static zend_function_entry scalar_object_functions[] = {
    PHP_ME( SplScalarObject, isArray,   NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isBool,    NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isFloat,   NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isInt,     NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isNull,    NULL, ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarObject, isString,  NULL, ZEND_ACC_PUBLIC )
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_spl_scalar_string_length, 0, 0, 1)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

static const zend_function_entry scalar_object_string_functions[] = {
    PHP_ME( SplScalarString, isString,  NULL,                           ZEND_ACC_PUBLIC )
    PHP_ME( SplScalarString, length,    arginfo_spl_scalar_string_length, ZEND_ACC_PUBLIC )
    PHP_FE_END
};



PHP_METHOD( SplScalarString, isString )
{
    RETURN_BOOL(1);
}

PHP_METHOD( SplScalarString, length )
{

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    zval* strcontext = getThis();


    RETURN_LONG(Z_STRLEN_P(strcontext));
}

/**** Class initialisation *****/
void register_scalar_objects(TSRMLS_CC)
{
    zend_class_entry *ce_SplScalarObject;
    zend_class_entry ce_sc, ce_sc_a, ce_sc_b, ce_sc_f, ce_sc_i, ce_sc_n, ce_sc_s;


    INIT_CLASS_ENTRY(ce_sc, "SplScalarObject", scalar_object_functions);
    ce_SplScalarObject = zend_register_internal_class(&ce_sc TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_sc_a, "SplScalarArray", scalar_object_array_functions);
    zend_register_internal_class_ex(&ce_sc_a, ce_SplScalarObject, NULL TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_sc_b, "SplScalarBoolean", scalar_object_boolean_functions);
    zend_register_internal_class_ex(&ce_sc_b, ce_SplScalarObject, NULL TSRMLS_CC);


    INIT_CLASS_ENTRY(ce_sc_f, "SplScalarFloat", scalar_object_float_functions);
    zend_register_internal_class_ex(&ce_sc_f, ce_SplScalarObject, NULL TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_sc_i, "SplScalarInteger", scalar_object_integer_functions);
    zend_register_internal_class_ex(&ce_sc_i, ce_SplScalarObject, NULL TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_sc_n, "SplScalarNull", scalar_object_null_functions);
    zend_register_internal_class_ex(&ce_sc_n, ce_SplScalarObject, NULL TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_sc_s, "SplScalarString", scalar_object_string_functions);
    zend_register_internal_class_ex(&ce_sc_s, ce_SplScalarObject, NULL TSRMLS_CC);


}

void attach_scalar_class_handlers(TSRMLS_CC) {
    zend_class_entry **ar_han;
    zend_class_entry **bo_han;
    zend_class_entry **fl_han;
    zend_class_entry **in_han;
    zend_class_entry **nu_han;
    zend_class_entry **st_han;

    zend_lookup_class("SplScalarArray", strlen("SplScalarArray"), &ar_han TSRMLS_DC);
    SCALAR_OBJECTS_G(handlers)[get_type_from_string("array")] = *ar_han;

    zend_lookup_class("SplScalarBoolean", strlen("SplScalarBoolean"), &bo_han TSRMLS_DC);
    SCALAR_OBJECTS_G(handlers)[get_type_from_string("bool")] = *bo_han;

    zend_lookup_class("SplScalarFloat", strlen("SplScalarFloat"), &fl_han TSRMLS_DC);
    SCALAR_OBJECTS_G(handlers)[get_type_from_string("float")] = *fl_han;

    zend_lookup_class("SplScalarInteger", strlen("SplScalarInteger"), &in_han TSRMLS_DC);
    SCALAR_OBJECTS_G(handlers)[get_type_from_string("int")] = *in_han;

    zend_lookup_class("SplScalarNull", strlen("SplScalarNull"), &nu_han TSRMLS_DC);
    SCALAR_OBJECTS_G(handlers)[get_type_from_string("null")] = *nu_han;

    zend_lookup_class("SplScalarString", strlen("SplScalarString"), &st_han TSRMLS_DC);
    SCALAR_OBJECTS_G(handlers)[get_type_from_string("string")] = *st_han;
}




