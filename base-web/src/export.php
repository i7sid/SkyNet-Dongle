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
if ($argc < 4 || $argv[1] == '-h') {
    echo "Usage: " . $argv[0] . " station_id exp_id entity_id\n";
    exit;
}

// generate query
$sql = "SELECT * FROM entities WHERE station_id = '".$argv[1]."' AND " .
    " experiment_id = '".$argv[2]."' AND entity_type_id = '$argv[3]';";

// execute query
$stmt = $db->query($sql);
$rows = $stmt->fetchAll(PDO::FETCH_ASSOC);

// generate output (and write to stdout)
$i = 0;
foreach ($rows as $r) {
    echo '"' . $r['timestamp'] . '","' . $r['value'] . '"' . "\n";
    #echo $i . ',' . $r['value'] . "\n";
    $i++;
}

?>
