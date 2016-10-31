<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 * @license MIT
 */

function _e($msg) {
    file_put_contents('php://stderr', $msg);
}

function readCSV($csvFile){
    $file_handle = fopen($csvFile, 'r');
    while (!feof($file_handle) ) {
        $line_of_text[] = fgetcsv($file_handle, 1024);
    }
    fclose($file_handle);
    return $line_of_text;
}


// print usage
if ($argc < 2 || $argv[1] == '-h') {
    echo "Usage: " . $argv[0] . " in_file.csv\n";
    exit(-1);
}

$in = readCSV($argv[1]);
$out = fopen('php://stdout', 'w');

$c = 0;

usort($in, function ($a, $b) { return strcmp($a[0], $b[0]); });

foreach ($in as $l) {
    $c++;
    if (!$l) continue;
    fputcsv($out, $l);
}

_e("Processed " . $c . " lines.\n");
exit(0);
?>
