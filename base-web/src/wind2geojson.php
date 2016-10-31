<?php
/**
 * @file
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-08-22
 */


// set timezone for strtotime
date_default_timezone_set('UTC');


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
if ($argc < 3 || $argv[1] == '-h') {
    _e("Usage: " . $argv[0] . " in_wind_file.csv in_pos_file.csv\n");
    exit;
}
$in = readCSV($argv[1]);
$pos_file = readCSV($argv[2]);

// read stations
$stations = array();
foreach ($pos_file as $line) {
    if (count($line) < 2) continue;
    $stations[$line[1]] = $line;
}

// header
$s = array_values($stations)[1];
echo '
wind_data.push(
{ "type": "FeatureCollection", "features": [
    {   "type": "Feature",
        "geometry": {"type": "Point", "coordinates": ['.$s[2].', '.$s[3].']},
        "properties": {"mac": "'.$s[1].'"}
    }
';


// read wind measurements
foreach($in as $line) {
    if (count($line) < 2) continue;
    $s = $stations[$line[1]];
    $speed = $line[2];
    $dir = $line[3];

    // calculate target point
    $r = $speed * 0.0250;     // in km
    $alpha = $dir;
    $alpha = (360 - $alpha + 90) % 360;
    $dx = $r * cos(deg2rad($alpha));
    $dy = $r * sin(deg2rad($alpha));

    // source: https://www.kompf.de/gps/distcalc.html
    $target_x = $s[2] + ($dx / (111.3 * cos(deg2rad($s[3]))));
    $target_y = $s[3] + ($dy / 111.3);

    $time = strtotime($line[0]);

    echo '
    ,{   "type": "Feature",
        "geometry": {
            "type": "LineString",
            "coordinates": [
                ['.$s[2].', '.$s[3].'], ['.$target_x.', '.$target_y.']
            ]
        },
        "properties": {
            "wind_speed": ' . $speed . ',
            "wind_dir": ' . $dir . ',
            "time": "' . $time . '",
            "mac": "' . $line[1] . '"
        }
    }' . "\n";
}
echo '
]});
';


function parseCoord($c) {
    $r = '';
    $full = floatval($c);

    $deg = (int)floor($c / 100);
    $min = $c - $deg * 100;

    $frac = $min / 60.0;

    return $deg + $frac;
}
?>
