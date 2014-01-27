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
?>
--EXPECTF--
bool(true)
bool(true)