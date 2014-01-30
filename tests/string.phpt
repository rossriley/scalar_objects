--TEST--
Testing SplScalarString.
--SKIPIF--
<?php if (!extension_loaded('scalar_objects')) echo 'skip'; ?>
--FILE--
<?php
error_reporting(E_ALL);
require __DIR__ . '/bootstrap.php';

head("Type Checks");
str("Hello World");
r('isString()');
r('isArray()');
r('isBool()');
r('isFloat()');
r('isInt()');
r('isNull()');
head("Method Tests");
r('length()');

?>
--EXPECTF--

-----Type Checks-----
Working on string "Hello World"
isString(): bool(true)
isArray(): bool(false)
isBool(): bool(false)
isFloat(): bool(false)
isInt(): bool(false)
isNull(): bool(false)


-----Method Tests-----
length(): int(11)
