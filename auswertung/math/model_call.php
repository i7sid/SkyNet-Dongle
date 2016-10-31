<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

require_once(__DIR__ . '/shared.inc.php');

if ($argc < 2 || $argv[1] == '-h') {
    echo "Usage: " . $argv[0] . ' directory' .
        "\n";
    exit;
}

$bases = array();
$files = scandir($argv[1]);

foreach ($files as $file) {
    if (substr($file, -4) != '.csv') continue;
    if (substr($file, 4, 1) != '-') continue;

    if (substr($file, 5, 4) == 'wind') {
        $base = $argv[1] . '/' . substr($file, 0, 29);
        if (!in_array($base, $bases)) {
            $bases[] = $base;
        }
    }
}

if (count($bases) < 2) {
    _e("No enough source files in given directory.\n");
    exit;
}

$argc = 19;
$i = 0;
$argv[$i++] = 'model.php';

// output file names
foreach ($bases as $b) {
    $argv[$i++] = $b . '-raw.csv';
    $argv[$i++] = $b . '-dir-raw.csv';
    $argv[$i++] = $b . '-5min.csv';
    $argv[$i++] = $b . '-dir-5min.csv';
    $argv[$i++] = $b . '-15min.csv';
    $argv[$i++] = $b . '-dir-15min.csv';
    $argv[$i++] = $b . '-1min.csv';
    $argv[$i++] = $b . '-dir-1min.csv';
    $argv[$i++] = $b . '-30min.csv';
    $argv[$i++] = $b . '-dir-30min.csv';
    $argv[$i++] = $b . '-diff.csv';
    $argv[$i++] = $b . '-dir-diff.csv';
    $argv[$i++] = $b . '-vector.csv';
}

$argv[$i++] = str_replace('-wind-', '-pos-', $bases[0]) . '-mean.csv';
$argv[$i++] = str_replace('-wind-', '-pos-', $bases[1]) . '-mean.csv';

// now call model via include
require_once(__DIR__ . '/model.php');

?>
