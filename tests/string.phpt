--TEST--
Testing SplScalarString.
--SKIPIF--
<?php if (!extension_loaded('scalar_objects')) echo 'skip'; ?>
--FILE--
<?php
$test = "Hello World";
var_dump($test->isString());
var_dump($test->isArray());
var_dump($test->isBool());
var_dump($test->isFloat());
var_dump($test->isInt());
var_dump($test->isNull());
var_dump($test->length());
?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
int(11)