--TEST--
Testing SplScalarArray.
--SKIPIF--
<?php if (!extension_loaded('scalar_objects')) echo 'skip'; ?>
--FILE--
<?php
$test = [1,2,3,4,5];
var_dump($test->isArray());
$test2 = ["key1"=>"val1"];
var_dump($test2->isArray());
var_dump($test2->isBool());
var_dump($test2->isFloat());
var_dump($test2->isInt());
var_dump($test2->isNull());
var_dump($test2->isString());
?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)