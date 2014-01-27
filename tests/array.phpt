--TEST--
Testing SplScalarArray.
--SKIPIF--
<?php if (!extension_loaded('scalar_objects')) echo 'skip'; ?>
--FILE--
<?php
$test = [1,2,3,4,5];
var_dump($test->isArray());
?>
--EXPECTF--
bool(false)