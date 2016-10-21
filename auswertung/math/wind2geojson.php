<?php
/**
 * @file
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-08-22
 */

require_once('shared.inc.php');


function drawWindLine($x, $y, $speed, $dir, $props) {
    // calculate target point for wind direction marker
    $r = $speed * 0.0650;     // in km
    $alpha = $dir;
    $alpha = (360 - $alpha + 90) % 360;
    $dx = $r * cos(deg2rad($alpha));
    $dy = $r * sin(deg2rad($alpha));

    // source: https://www.kompf.de/gps/distcalc.html
    #$target_x = $x + ($dx / (111.3 * cos(deg2rad($y))));
    $target_x = $x + ($dx / 111.3);
    $target_y = $y + ($dy / 111.3);

    $r = '';
    $prop_string = '';

    foreach ($props as $k => $v) {
        $prop_string .= ',
            "' . $k . '": "' . $v . '"';
    }

    $r .= '
        ,
        {   "type": "Feature",
            "geometry": {
                "type": "LineString",
                "coordinates": [
                    [' . $x . ', ' . $y . '], ['.$target_x.', '.$target_y.']
                ]
            },
            "properties": {
                "wind_speed": ' . $speed . ',
                "wind_dir": ' . $dir . $prop_string . '
            }
        }' . "\n";

    return $r;

}


// print usage
if ($argc < 4 || $argv[1] == '-h') {
    _e("Usage: " . $argv[0] . " in_file.csv pos_station1.csv pos_station2.csv\n");
    exit;
}
$in         = readCSV($argv[1]);
$in_pos1    = readCSV($argv[2]);
$in_pos2    = readCSV($argv[3]);

// read stations
$stations = array();
foreach ($in_pos1 as $line) {
    if (count($line) < 2) continue;
    $stations[$line[1]] = $line;
}
foreach ($in_pos2 as $line) {
    if (count($line) < 2) continue;
    $stations[$line[1]] = $line;
}

// header
$s1 = array_values($stations)[1];
$s2 = array_values($stations)[2];
echo '
wind_data.push(
{ "type": "FeatureCollection", "features": [
    {   "type": "Feature",
        "geometry": {"type": "Point", "coordinates": ['.$s1[2].', '.$s1[3].']},
        "properties": {"mac": "'.$s1[1].'" , "point_type" : "base_station" }
    },
    {   "type": "Feature",
        "geometry": {"type": "Point", "coordinates": ['.$s2[2].', '.$s2[3].']},
        "properties": {"mac": "'.$s2[1].'" , "point_type" : "base_station" }
    }
';

$id = 0;
$cnt = 0;
$ret_intersect = '';

// read wind measurements
foreach($in as $line) {
    if (count($line) < 2) continue;
    $time = strtotime($line[0]);

    // wind direction station 1 (live)
    if (!empty($line[1])) {
        $speed = $line[4];
        $dir = $line[5];
        if (empty($speed)) $speed = 0;
        if (empty($dir)) $dir = 0;

        $p = array(
            'mac'       => $line[1],
            'time'      => $time,
            'item_type' => 'wind_raw',
            'item_id'   => $id
        );
        echo drawWindLine($line[2], $line[3], $speed, $dir, $p);

        $id++;
    }
    
    // wind direction station 2 (live)
    if (!empty($line[9])) {
        $speed = $line[12];
        $dir = $line[13];
        if (empty($speed)) $speed = 0;
        if (empty($dir)) $dir = 0;

        $p = array(
            'mac'       => $line[1],
            'time'      => $time,
            'item_type' => 'wind_raw',
            'item_id'   => $id
        );
        echo drawWindLine($line[10], $line[11], $speed, $dir, $p);

        $id++;
    }

    // wind direction station 1 (5 min)
    if (!empty($line[1])) {
        $speed = $line[6];
        $dir = $line[23];
        if (empty($speed)) $speed = 0;
        if (empty($dir)) $dir = 0;

        $p = array(
            'mac'       => $line[1],
            'time'      => $time,
            'item_type' => 'wind_5min',
            'item_id'   => $id
        );
        echo drawWindLine($line[2], $line[3], $speed, $dir, $p);

        $id++;
    }

    // wind direction station 2 (5 min)
    if (!empty($line[9])) {
        $speed = $line[14];
        $dir = $line[26];
        #if (empty($speed)) $speed = 0;
        #if (empty($dir)) $dir = 0;
        if (empty($speed) === false && empty($dir) === false) {
            $p = array(
                'mac'       => $line[9],
                'time'      => $time,
                'item_type' => 'wind_5min',
                'item_id'   => $id
            );
            echo drawWindLine($line[10], $line[11], $speed, $dir, $p);

            $id++;
        }
    }

    // wind direction station 1 (15 min)
    if (!empty($line[1])) {
        $speed = $line[7];
        $dir = $line[24];
        if (empty($speed)) $speed = 0;
        if (empty($dir)) $dir = 0;

        $p = array(
            'mac'       => $line[1],
            'time'      => $time,
            'item_type' => 'wind_15min',
            'item_id'   => $id
        );
        echo drawWindLine($line[2], $line[3], $speed, $dir, $p);

        $id++;
    }

    // wind direction station 2 (15 min)
    if (!empty($line[9])) {
        $speed = $line[15];
        $dir = $line[27];
        if (empty($speed)) $speed = 0;
        if (empty($dir)) $dir = 0;

        $p = array(
            'mac'       => $line[9],
            'time'      => $time,
            'item_type' => 'wind_15min',
            'item_id'   => $id
        );
        echo drawWindLine($line[10], $line[11], $speed, $dir, $p);

        $id++;
    }

    // wind intersection (updraft)
    if (!empty($line[17])) {
        #        if ($cnt > 1000) continue;
        #var_dump(floatval($line[17]));
        if ((floatval($line[17]) > 11.39 && floatval($line[17]) < 11.72) &&
            (floatval($line[18]) > 48.95 && floatval($line[18]) < 49.39)) {

            if ($cnt > 0) $ret_intersect .= ',';

            $ret_intersect .= '
                {   "type": "Feature",
                    "geometry": {"type": "Point", "coordinates": ['.$line[17].', '.$line[18].']},
                    "properties": {
                        "point_type" : "intersection_point",
                        "height": "0",
                        "time": "' . $time . '",
                        "item_id": "' . $cnt . '"
                    }
                }' . "\n";
            $id++;
            $cnt++;
        }

        if ((floatval($line[19]) > 11.39 && floatval($line[19]) < 11.72) &&
            (floatval($line[20]) > 48.95 && floatval($line[20]) < 49.39)) {

            if ($cnt > 0) $ret_intersect .= ',';

            $ret_intersect .= '
                {   "type": "Feature",
                    "geometry": {"type": "Point", "coordinates": ['.$line[19].', '.$line[20].']},
                    "properties": {
                        "point_type" : "intersection_point",
                        "height": "' . $line[21] . '",
                        "time": "' . $time . '",
                        "item_id": "' . $cnt . '"
                    }
                }' . "\n";
            $id++;
            $cnt++;
        }

    }

}
echo '
]});
';

echo '
wind_data.push(
    { "type": "FeatureCollection", "features": [
        ' . $ret_intersect . '
]});
';




