<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

require_once('shared.inc.php');
require_once('php-igc-master/PHP_IGC.php');

$station1_lon = 11.522680474573;
$station1_lat = 49.187706224573;
$station1_alt = 537;
$station2_lon = 11.522574107083;
$station2_lat = 49.196462426253;
$station2_alt = 524;


// print usage
if ($argc < 2 || $argv[1] == '-h') {
    _e("Usage: " . $argv[0] . ' ' .
        'input.igc' .
        "\n");
    exit;
}

$igc        = new PHP_IGC($argv[1]);

$a1 = new XYZ($station1_lon, $station1_lat, $station1_alt);
$a2 = new XYZ($station2_lon, $station2_lat, $station2_alt);

echo "Time,Dist-to-3b67,Dist-to-85c0\n";
foreach ($igc->records as $r) {
    if (get_class($r) != 'IGC_B_Record') continue;

    $b = new XYZ(floatval($r->longitude['decimal_degrees']),
        floatval($r->latitude['decimal_degrees']),
        floatval($r->gps_altitude));
    $d1 = getDistance3d($a1, $b);
    $d2 = getDistance3d($a2, $b);
    echo $r->time_array['h'] . ':' .
        $r->time_array['m'] . ':' .
        $r->time_array['s'] . ',' .
        $d1 . "," .
        $d2 . "\n";
    #var_dump($r); exit;
}


?>
