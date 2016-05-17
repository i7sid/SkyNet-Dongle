<?php
/**
 * @file
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-05-14
 */

require_once './config/config.php';

$db = new PDO('mysql:host=' . $config['db']['host'] .
                ';dbname=' . $config['db']['name'] .
                ';charset=utf8mb4', $config['db']['user'],
                $config['db']['password']);

// print usage
if ($argc < 2 || $argv[1] == '-h') {
    echo "Usage: " . $argv[0] . " exp_id\n";
    exit;
}

// generate query
$sql = "SELECT * FROM entities e" .
    " JOIN stations s ON e.station_id = s.id " .
    " WHERE " .
    " experiment_id = '".$argv[1]."' AND entity_type_id = '1';";

$sum_lat = array();
$sum_lon = array();

// execute query
$stmt = $db->query($sql);
$rows = $stmt->fetchAll(PDO::FETCH_ASSOC);

// generate output (and write to stdout)
echo "id,mac,latitude,longitude\n";
$i = 0;
foreach ($rows as $r) {
    $a = explode(':', $r['value']);

    $lat = parseCoord($a[1]);
    $lon = parseCoord($a[3]);

    if ($a[0] == 'S') {
        $lat *= -1;
    }
    if ($a[2] == 'W') {
        $lon *= -1;
    }

    if (!array_key_exists($r['mac'], $sum_lat)) {
        $sum_lat[$r['mac']] = 0;
    }
    $sum_lat[$r['mac']] += $lat;
    if (!array_key_exists($r['mac'], $sum_lon)) {
        $sum_lon[$r['mac']] = 0;
    }
    $sum_lon[$r['mac']] += $lon;

    $i++;
}

$cnt = $i;
$i = 0;

foreach ($sum_lat as $k => $v) {
    echo $i . ',' . $k . ',' . ($v / $cnt) . ',' . ($sum_lon[$k] / $cnt) . "\n";
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
