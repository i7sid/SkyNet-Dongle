<?php
/**
 * @file
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-07-20
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
    exit;
}
$in = readCSV($argv[1]);

$sum_lat = array();
$sum_lon = array();
$i = array();

// generate output (and write to stdout)
echo "id,mac,longitude,latitude\n";
foreach ($in as $r) {
    if (!$r) continue;
    $lat = ($r[2]);
    $lon = ($r[3]);

    if (!array_key_exists($r[1], $i)) $i[$r[1]] = 0;

    if (!array_key_exists($r[1], $sum_lat)) {
        $sum_lat[$r[1]] = 0;
    }
    $sum_lat[$r[1]] += $lat;
    if (!array_key_exists($r[1], $sum_lon)) {
        $sum_lon[$r[1]] = 0;
    }
    $sum_lon[$r[1]] += $lon;

    $i[$r[1]] += 1;
}

$cnt = 0;

foreach ($sum_lat as $k => $v) {
    echo $cnt . ',' . $k . ',' . ($v / $i[$k]) . ',' . ($sum_lon[$k] / $i[$k]) . "\n";
    $cnt++;
}


function parseCoord($c) {
    $r = '';
    $full = floatval($c);

    $deg = (int)floor($c / 100);
    $min = $c - $deg * 100;

    $frac = $min / 60.0;

    return $deg + $frac;
}
?>
