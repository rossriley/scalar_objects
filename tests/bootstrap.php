<?php

function str($str) {
    echo "\nWorking on string \"$str\"\n";
    $GLOBALS['_test'] = $str;
}

function arr($arr) {
    echo "\nWorking on array ".print_r($arr,1)."\n\n";
    $GLOBALS['_test'] = $str;
}

function r($methodCode) {
    try {
        $result = eval("return \$GLOBALS['_test']->$methodCode;");
        p($methodCode, $result);
    } catch (Exception $e) {
        echo $methodCode, ":\n", get_class($e), ': ', $e->getMessage(), "\n";
    }
}

function p($name, $result) {
    echo $name, ':', strlen($name) > 50 ? "\n" : ' ';
    var_dump($result);
}

function sep() {
    echo "\n";
}
function head($line) {
    echo "\n\n=====".$line."=====\n";
}