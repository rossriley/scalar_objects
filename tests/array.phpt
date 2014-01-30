--TEST--
Testing SplScalarArray.
--SKIPIF--
<?php if (!extension_loaded('scalar_objects')) echo 'skip'; ?>
--FILE--
<?php
error_reporting(E_ALL);
require __DIR__ . '/bootstrap.php';

head("Type Checks");
arr([1,2,3,4,5]);
r('isString()');
r('isArray()');
r('isBool()');
r('isFloat()');
r('isInt()');
r('isNull()');
head("Method Tests");
r('count()');
?>
--EXPECTF--
-----Type Checks-----

Working on: Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
)
isString(): bool(false)
isArray(): bool(true)
isBool(): bool(false)
isFloat(): bool(false)
isInt(): bool(false)
isNull(): bool(false)


-----Method Tests-----
count(): int(5)
