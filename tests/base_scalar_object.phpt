--TEST--
Testing SplScalarObject.
--SKIPIF--
<?php if (!extension_loaded('scalar_objects')) echo 'skip'; ?>
--FILE--
<?php
$test = new SplScalarObject();
var_dump($test->isArray());
var_dump($test->isBool());
var_dump($test->isFloat());
var_dump($test->isInt());
var_dump($test->isNull());
var_dump($test->isString());
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)