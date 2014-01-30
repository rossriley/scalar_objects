#ifndef PHP_SCALAR_OBJECTS_HANDLERS
#define PHP_SCALAR_OBJECTS_HANDLERS

#include "php_scalar_objects.h"

void register_scalar_objects(TSRMLS_CC);
void attach_scalar_class_handlers(TSRMLS_CC);


PHP_METHOD( SplScalarObject, isArray );
PHP_METHOD( SplScalarObject, isBool );
PHP_METHOD( SplScalarObject, isBool );
PHP_METHOD( SplScalarObject, isFloat );
PHP_METHOD( SplScalarObject, isInt );
PHP_METHOD( SplScalarObject, isNull );
PHP_METHOD( SplScalarObject, isString );


PHP_METHOD( SplScalarArray, isArray );
PHP_METHOD( SplScalarArray, count );

PHP_METHOD( SplScalarBoolean, isBool );

PHP_METHOD( SplScalarFloat, isFloat );

PHP_METHOD( SplScalarInteger, isInt );

PHP_METHOD( SplScalarNull, isNull );

PHP_METHOD( SplScalarString, isString );
PHP_METHOD( SplScalarString, length );


#endif