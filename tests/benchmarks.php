<?php
$mask = "| %20.20s | %-20.20s |\n";
$header = str_pad("",47,'-'). "\n %40.40s \n".str_pad("",47,'-')."\n";
printf($mask, 'Operation', 'Time (s)');

function test($operation, $times) {
    $start = micro_time();
    for($i=0; $i<$times;$i++ ) {
        $operation();
    }
    $end = micro_time();
    return bcsub($end, $start, 6);
}

function micro_time() {
    $temp = explode(" ", microtime());
    return bcadd($temp[0], $temp[1], 6);
}

function run($title, $native, $scalar, $cycles) {
    global $mask, $header;
    printf($header, $title.' - '.number_format($cycles).' Cycles', "");
    printf($mask, 'Native PHP', test($native, $cycles));
    printf($mask, 'Scalar Method', test($native, $cycles));
}



$native = function() {$s = "Hello World";$l = strlen($s);};
$scalar = function() {$s = "Hello World";$l = $s->length();};
run("String Length Test", $native, $scalar, 500000);

$native = function() {$a = [1,2,3,4,5,6];$l = count($a);};
$scalar = function() {$a = [1,2,3,4,5,6];$l = $a->count();};
run("Array Count Test", $native, $scalar, 500000);
