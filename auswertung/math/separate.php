<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

require_once('shared.inc.php');


// print usage
if ($argc < 3 || $argv[1] == '-h') {
    _e("Usage: " . $argv[0] . ' ' .
        'input-wind.csv MAC-output.csv ' . "\n" .
        '  where MAC will be replaced by each station\'s mac address.' .
        "\n");
    exit;
}

$in1        = readCSV($argv[1]);
$outbase    = $argv[2];

if (strpos($outbase, 'MAC') === false) {
    _e('Second parameter must contain MAC as a placeholder.' . "\n");
    exit;
}

$fds = array();
$seen_dates = array();

foreach ($in1 as $l) {
    if (count($l) < 2) continue;

    $mac = str_replace(':', '', $l[1]);

    if (!array_key_exists($mac, $fds)) {
        $fds[$mac] = fopen(str_replace('MAC', $mac, $outbase), 'w');
        $seen_dates[$mac] = array();
    }

    if (in_array($l[0], $seen_dates[$mac])) {
        _e("Datetime double: " . $l[0] . " in " . $mac . "\n");
        exit;
    }
    $seen_dates[$mac][] = $l[0];

    $r = '';
    #@fputcsv($fds[$mac], $l, ',', '');
    foreach ($l as $i) {
        $r .= $i . ',';
    }
    $r = rtrim($r, ',');
    $r .= "\n";
    fputs($fds[$mac], $r);
}


foreach ($fds as $fd) {
    fclose($fd);
}
?>
