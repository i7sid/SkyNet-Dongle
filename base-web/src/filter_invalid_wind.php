<?php
/**
 * Short description for filter_invalid.php
 *
 * @package filter_invalid
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

$legit_stations = array('5d:5d', '70:86');
$c = 0;
$ec = 0;
$last_time = null;
foreach ($in as $l) {
    $c++;

    try {
        $d = new DateTime($l[0]);
        if ($d == $last_time) throw new Exception('time duplicate: ' . $l[0]);
        $last_time = $d;

        // check date
        if ($l[0] !== $d->format("Y-m-d H:i:s")) {
            throw new Exception('date invalid: ' . $l[0]);
        }

        // check speed and direction
        $speed = floatval($l[2]);
        if ($speed < 0) throw new Exception('speed invalid: ' . $l[2]);
        $dir = intval($l[3]);
        if ($dir < 0 || $dir > 360) throw new Exception('direction invalid: ' . $l[3]);

        // check station mac
        if (!in_array($l[1], $legit_stations)) throw new Exception('station invalid: ' . $l[1]);
        fputcsv($out, $l);
    }
    catch (Exception $ex) {
        $ec++;
        _e($ex->getMessage() . "\n");
    }
}

_e("Processed " . $c . " lines.\n");
_e("Ignored   " . $ec . " lines.\n");
exit(0);
?>
