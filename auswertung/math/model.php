<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

require_once('shared.inc.php');

$dist_thresh = 1500;


// print usage
if ($argc < 17 || $argv[1] == '-h') {
    echo "Usage: " . $argv[0] . ' ' .
        'wind-speed-raw.csv wind-dir-raw.csv ' .
        'wind-speed-5min.csv wind-dir-5min.csv' .
        'wind-speed-15min.csv wind-dir-15min.csv ' .
        'wind-speed-1min.csv wind-dir-1min.csv ' .
        'wind-speed-30min.csv wind-dir-30min.csv ' .
        'wind-speed-diff.csv  ' .
        'wind-speed-dir-diff.csv  ' .
        'wind-speed-vector.csv  ' .
        'wind-speed-raw.csv wind-dir-raw.csv ' .
        'wind-speed-5min.csv wind-dir-5min.csv ' .
        'wind-speed-15min.csv wind-dir-15min.csv ' .
        'wind-speed-1min.csv wind-dir-1min.csv ' .
        'wind-speed-30min.csv wind-dir-30min.csv ' .
        'wind-speed-diff.csv ' .
        'wind-speed-dir-diff.csv  ' .
        'wind-speed-vector.csv  ' .
        'station-1-pos.csv station-2-pos.csv' .
        "\n";
    exit;
}
$in_speed_raw    = readCSV($argv[1]);
$in_dir_raw      = readCSV($argv[2]);
$in_speed_5min   = readCSV($argv[3]);
$in_dir_5min     = readCSV($argv[4]);
$in_speed_15min  = readCSV($argv[5]);
$in_dir_15min    = readCSV($argv[6]);
$in_speed_1min   = readCSV($argv[7]);
$in_dir_1min     = readCSV($argv[8]);
$in_speed_30min  = readCSV($argv[9]);
$in_dir_30min    = readCSV($argv[10]);
$in_speed_diff   = readCSV($argv[11]);
$in_dir_diff     = readCSV($argv[12]);
$in_vector       = readCSV($argv[13]);
$in2_speed_raw   = readCSV($argv[14]);
$in2_dir_raw     = readCSV($argv[15]);
$in2_speed_5min  = readCSV($argv[16]);
$in2_dir_5min    = readCSV($argv[17]);
$in2_speed_15min = readCSV($argv[18]);
$in2_dir_15min   = readCSV($argv[19]);
$in2_speed_1min  = readCSV($argv[20]);
$in2_dir_1min    = readCSV($argv[21]);
$in2_speed_30min = readCSV($argv[22]);
$in2_dir_30min   = readCSV($argv[23]);
$in2_speed_diff  = readCSV($argv[24]);
$in2_dir_diff    = readCSV($argv[25]);
$in2_vector      = readCSV($argv[26]);
$pos_station1    = readCSV($argv[27]);
$pos_station2    = readCSV($argv[28]);

$data = array();

// read wind speed data
foreach ($in_speed_raw as $l) {
    if (count($l) < 2) continue;
    $tp = new TimePoint($l[0]);
    $tp->station1 = new Measurement();
    $tp->station1->speed = $l[1];

    $data[$l[0]] = $tp;
}


// read wind dir data
foreach ($in_dir_raw as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->dir = mirrorWind($l[1]);
}

// read history
foreach ($in_speed_5min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->speed_5min = $l[1];
}
foreach ($in_dir_5min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->dir_5min = mirrorWind($l[1]);
}
foreach ($in_speed_15min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->speed_15min = $l[1];
}
foreach ($in_dir_15min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->dir_15min = mirrorWind($l[1]);
}
foreach ($in_speed_1min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->speed_1min = $l[1];
}
foreach ($in_dir_1min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->dir_1min = mirrorWind($l[1]);
}
foreach ($in_speed_30min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->speed_30min = $l[1];
}
foreach ($in_dir_30min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->dir_30min = mirrorWind($l[1]);
}
foreach ($in_speed_diff as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->speed_diff = $l[1];
}
foreach ($in_dir_diff as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->dir_diff = mirrorWind($l[1]);
}
foreach ($in_vector as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station1->v_a_speed = $l[2];
    $m->station1->v_a_dir = mirrorWind($l[3]);
}


// read station 2 wind speed data
foreach ($in2_speed_raw as $l) {
    if (count($l) < 2) continue;

    if (array_key_exists($l[0], $data)) {
        $tp = $data[$l[0]];
    }
    else {
        $tp = new TimePoint($l[0]);
        $data[$l[0]] = $tp;
    }

    $tp->station2 = new Measurement();
    $tp->station2->speed = $l[1];

}

// read wind dir data
foreach ($in2_dir_raw as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->dir = mirrorWind($l[1]);
}


foreach ($in2_speed_5min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->speed_5min = $l[1];
}
foreach ($in2_dir_5min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->dir_5min = mirrorWind($l[1]);
}
foreach ($in2_speed_15min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->speed_15min = $l[1];
}
foreach ($in2_dir_15min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->dir_15min = mirrorWind($l[1]);
}
foreach ($in2_speed_1min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->speed_1min = $l[1];
}
foreach ($in2_dir_1min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->dir_1min = mirrorWind($l[1]);
}
foreach ($in2_speed_30min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->speed_30min = $l[1];
}
foreach ($in2_dir_30min as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->dir_30min = mirrorWind($l[1]);
}
foreach ($in2_speed_diff as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->speed_diff = $l[1];
}
foreach ($in2_dir_diff as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->dir_diff = mirrorWind($l[1]);
}
foreach ($in2_vector as $l) {
    if (!array_key_exists($l[0], $data)) continue;

    $m = $data[$l[0]];
    $m->station2->v_a_speed = $l[2];
    $m->station2->v_a_dir = mirrorWind($l[3]);
}


// sort all data by time
ksort($data);

// read position data
$pos1       = $pos_station1[1];
$pos1_x     = $pos1[2];
$pos1_y     = $pos1[3];

foreach ($data as $tp) {
    if ($tp->station1 != null) {
        $tp->station1->mac      = $pos1[1];
        $tp->station1->pos_x    = $pos1_x;
        $tp->station1->pos_y    = $pos1_y;
    }
}

$pos2       = $pos_station2[1];
$pos2_x     = $pos2[2];
$pos2_y     = $pos2[3];

foreach ($data as $tp) {
    if ($tp->station2 != null) {
        $tp->station2->mac      = $pos2[1];
        $tp->station2->pos_x    = $pos2_x;
        $tp->station2->pos_y    = $pos2_y;
    }
}

$pos_mean_x = 0.5 * ($pos2_x + $pos1_x);
$pos_mean_y = 0.5 * ($pos2_y + $pos1_y);

// calculate scores for single stations
$s4l = 0;
$s5l = 0;
$tp->score3 = 'NA';
foreach ($data as $tp) {
    if ($tp->station1 !== null) {
        $tp->score4 = singleStationScore($tp->station1);
        #$tp->score3 = singleStationScore2($tp->station1);
    }
    else {
        $tp->score4 = 'NA';
        #$tp->score3 = 'NA';
    }
    if ($tp->station2 !== null) {
        $tp->score5 = singleStationScore($tp->station2);
    }
    else {
        $tp->score5 = 'NA';
    }
}



// calculate thermal point on earth, in the air and the overall score

$tp1 = null;
$tp2 = null;
$h = 100;    // test height: 100m

foreach ($data as $tp) {
    if ($tp->station1 !== null) {
        $tp1 = $tp;
    }
    if ($tp->station2 !== null) {
        $tp2 = $tp;
    }

    if (!$tp1 || !$tp2) continue;

    // now check if last_* data too old
    $diff = abs($tp1->time->getTimestamp() - $tp2->time->getTimestamp());

    // check if time between last two measurements differs too much
    if ($diff >= 150) {
        $tp->score2 = 0;
        continue;
    }

    #// some shortcuts
    #$dir1 = $tp1->station1->dir_5min;
    #$dir2 = $tp2->station2->dir_5min;

    #// dir* is direction FROM that the wind comes, so add 180°
    #$dir1 = ($dir1 + 180) % 360;
    #$dir2 = ($dir2 + 180) % 360;

    // transform wind from polar to cartesian
#    $dir1_15 = (round($tp1->station1->dir_15min) + 180) % 360;
#    $dir2_15 = (round($tp2->station2->dir_15min) + 180) % 360;
#    $dir1_5 = (round($tp1->station1->dir_5min) + 180) % 360;
#    $dir2_5 = (round($tp2->station2->dir_5min) + 180) % 360;
#    $x_u1 = ($tp1->station1->speed_15min)^2 * cos(deg2rad($dir1_15));
#    $y_u1 = ($tp1->station1->speed_15min)^2 * sin(deg2rad($dir1_15));
#    $x_u2 = ($tp2->station2->speed_15min)^2 * cos(deg2rad($dir2_15));
#    $y_u2 = ($tp2->station2->speed_15min)^2 * sin(deg2rad($dir2_15));
#    $x_1 = ($tp1->station1->speed_5min)^2 * cos(deg2rad($dir1_5));
#    $y_1 = ($tp1->station1->speed_5min)^2 * sin(deg2rad($dir1_5));
#    $x_2 = ($tp2->station2->speed_5min)^2 * cos(deg2rad($dir2_5));
#    $y_2 = ($tp2->station2->speed_5min)^2 * sin(deg2rad($dir2_5));
#
#    // remember: CURRENT = LONG + UPDRAFT
#    $x_a1 = $x_1 - $x_u1;
#    $y_a1 = $y_1 - $y_u1;
#    $x_a2 = $x_2 - $x_u2;
#    $y_a2 = $y_2 - $y_u2;
#
#    // transform back to polar coordinates
#    $dir1 = (450 - rad2deg(atan2($y_a1, $x_a1))) % 360;
#    $dir2 = (450 - rad2deg(atan2($y_a2, $x_a2))) % 360;
#    #$dir1 = (360 - rad2deg(atan2($y_a1, $x_a1)) + 90) % 360;
#   $dir2 = (360 - rad2deg(atan2($y_a2, $x_a2)) + 90) % 360;

    #$dir1 = $tp1->station1->dir_diff;
    #$dir2 = $tp2->station2->dir_diff;
    $dir1 = $tp1->station1->v_a_dir;
    $dir2 = $tp2->station2->v_a_dir;
    
    #$dir1 = (540 - $dir1) % 360;
    #$dir2 = (540 - $dir2) % 360;


    #$dir1 = $tp1->station1->dir;
    #$dir2 = $tp2->station2->dir;

    // debug
    #_e($tp1->time->format('Y-m-d H:i:s') . ' : ');
    #_e($tp1->station1->dir . ':' . $tp1->station1->dir_5min . ' : ');
    #_e($tp2->station2->dir . ':' . $tp2->station2->dir_5min . "\n");


    $x1 = $tp1->station1->pos_x;
    $y1 = $tp1->station1->pos_y;
    $x2 = $tp2->station2->pos_x;
    $y2 = $tp2->station2->pos_y;

    /*
    // calculate tangens only once
    $tan1 = tan(deg2rad($dir1));
    $tan2 = tan(deg2rad($dir2));

    if ($tan1 == $tan2) continue;
    $t2 = ($x1 - $x2 + ($y2 - $y1) * $tan1) / ($tan2 - $tan1);
    $t1 = $y2 + $t2 - $y1;
    #_e($tp->time->format('Y-m-d H:i:s') . ':' . $t1 . ':' . $t2 . "\n");

    if ($t1 >= 0 && $t2 >= 0) {
        $tp->thermal_point_x = $tp2->station2->pos_x + $t2 * $tan2;
        $tp->thermal_point_y = $tp2->station2->pos_y + $t2;
    }*/

    $rad1 = deg2rad($dir1);
    $rad2 = deg2rad($dir2);
    $sin1 = sin($rad1);
    $sin2 = sin($rad2);
    $cos1 = cos($rad1);
    $cos2 = cos($rad2);

    $t1 = -1;
    $t2 = -1;
    $det = $sin2 * $cos1 - $sin1 * $cos2;
    $dist = $dist_thresh;

    if ($det != 0) { // they are parallel!
        $t2 = (($x1 - $x2) * $cos1 - $sin1 * ($y1 - $y2)) / $det;
        $t1 = - ($sin2 * ($y1 - $y2) - ($x1 - $x2) * $cos2) / $det;

        // do not throw a "Division by zero" error
        #if ($cos2 == 0 ||  ($sin1 - ($sin2 / $cos2) * $cos1) == 0) continue;

        #$t1 = ($x2 - $x1 + ($sin2 / $cos2) * ($y1 - $y2)) / ($sin1 - ($sin2 / $cos2) * $cos1);
        #$t2 = ($y1 - $y2 + $cos1 * $t1) / $cos2;

        #$t = new DateTime("2016-07-08 12:44:46");
        #if ($t == $tp->time) {
        #    _e("DEBUG!!!\n");
        #    _e($tp1);
        #    _e($tp2);
        #    _e($dir1 . "  |  " . $dir2 . "\n");
        #    _e($x1 . ", " . $y1 . "  |  " . $x2 . ", " . $y2 . "\n");
        #    _e($sin1 . ", " . $cos1 . "  |  " . $sin2 . ", " . $cos2 . "\n");
        #    _e($det . " | " . $t1 . " | " . $t2 . "\n");
        #}

        if ($t1 >= 0 && $t2 >= 0) {
            $tp->thermal_point_x = $tp2->station2->pos_x + $t2 * $sin2;
            $tp->thermal_point_y = $tp2->station2->pos_y + $t2 * $cos2;

            // calc distance from mid point between stations
            $dist = distanceFromCoordinates($pos_mean_y, $pos_mean_x,
                $tp->thermal_point_y, $tp->thermal_point_x);

            // only proceed if thermal point is too far away
            if ($dist <= $dist_thresh) {
                $tp->score2 = 5;
                $tp->score3 = (1 / ($dist_thresh / 50)) * ($dist_thresh - $dist);


                // now calc thermal point on pilot's height

                $p = new XYZ($tp->thermal_point_x, $tp->thermal_point_y, 0);
                $v_long = 0;
                $dir_long = 0;
                polarVectorMean($tp1->station1->speed_15min, $tp1->station1->dir_15min,
                    $tp2->station2->speed_15min,  $tp2->station2->dir_15min,
                    $v_long, $dir_long);

                #$dir_long = ($dir_long + 180) % 360;
                $v_therm = 1; // in m/s
                $thermal_air = calcWindOffset($p, $h, $v_long, $dir_long, $v_therm);

                $tp->thermal_point_air_x = $thermal_air->x;
                $tp->thermal_point_air_y = $thermal_air->y;
                $tp->thermal_point_air_z = $thermal_air->z;
            }
            else {
                $tp->thermal_point_y = 0;
                $tp->thermal_point_x = 0;
                $tp->score2 = 0;
                $tp->score3 = -1;
            }
        }
        else {
            $tp->score2 = 0;
            $tp->score3 = -1;
        }
    }


    // now calc updraft probability score
    $s = 0;
    #$s2 = 0.2;
    #$s = $tp1->score4;

    if ($tp1->score4 == 'NA' || $tp2->score5 == 'NA') {
        $s = 'NA';
        #$s2 = 'NA';
    }
    else {
        $s += $tp1->score4;
        $s += $tp2->score5;
        $s *= 0.5;
        #$s2 *= $tp1->score4;
        #$s2 *= $tp2->score5;
    }

    #if ($dist <= $dist_thresh) {
    #    $s += ($dist_thresh - $dist) / 66;
    #}

    #$s += $tp1->station1->speed_diff;
    #$s += $tp2->station2->speed_diff;
    #$s += abs($tp1->station1->dir_diff) * 0.1;
    #$s += abs($tp2->station2->dir_diff) * 0.1;



    // penalty for centrum against wind direction
    #if ($t1 < 0) {
    #    $s -= 5;
    #}
    #if ($t2 < 0) {
    #    $s -= 5;
    #}

    #$s -= ;

    $tp->score = $s;


    // Score Variante 2
    $s2 = 0;
    if ($t1 >= 0 && $t2 >= 0) {
        $s2 = 20;
    }


}

foreach ($data as $tp) {
    echo $tp->time->format('Y-m-d H:i:s') . ',';
    if ($tp->station1 !== null) {
        echo $tp->station1->mac . ',' .                 // CSV: 1
            $tp->station1->pos_x . ',' .                // CSV: 2
            $tp->station1->pos_y . ',' .                // CSV: 3
            #$tp->station1->speed . ',' .               // CSV: 4
            #$tp->station1->speed_diff . ',' .          // CSV: 4
            $tp->station1->v_a_speed . ',' .            // CSV: 4
            $tp->station1->dir_diff . ',' .             // CSV: 5
            #$tp->station1->dir_5min . ',' .            // CSV: 5
            #$tp->station1->dir . ',' .                 // CSV: 5
            $tp->station1->speed . ',' .                // CSV: 6
            #$tp->station1->speed_5min . ',' .          // CSV: 6
            $tp->station1->speed_15min . ',' .          // CSV: 7
            $tp->station1->speed_diff . ',';            // CSV: 8
    }
    else {
        echo ',,,,,,,,';
    }

    if ($tp->station2 !== null) {
        echo $tp->station2->mac . ',' .                 // CSV: 9
            $tp->station2->pos_x . ',' .                // CSV: 10
            $tp->station2->pos_y . ',' .                // CSV: 11
            #$tp->station2->speed_diff . ',' .          // CSV: 12
            $tp->station2->v_a_speed . ',' .            // CSV: 12
            #$tp->station2->speed . ',' .               // CSV: 12
            $tp->station2->dir_diff . ',' .             // CSV: 13
            #$tp->station2->dir_5min . ',' .            // CSV: 13
            #$tp->station2->dir . ',' .                 // CSV: 13
            $tp->station2->speed      . ',' .           // CSV: 14
            #$tp->station2->speed_5min . ',' .          // CSV: 14
            $tp->station2->speed_15min . ',' .          // CSV: 15
            $tp->station2->speed_diff . ',';            // CSV: 16
    }
    else {
        echo ',,,,,,,,';
    }
    echo $tp->thermal_point_x . ',' .                   // CSV: 17
        $tp->thermal_point_y . ',';                     // CSV: 18
    echo $tp->thermal_point_air_x . ',' .               // CSV: 19
        $tp->thermal_point_air_y . ',' .                // CSV: 20
        $tp->thermal_point_air_z . ',' .                // CSV: 21
        $tp->score . ',';                               // CSV: 22


    // more directions
    if ($tp->station1 !== null) {
        echo
            #$tp->station1->dir_5min . ',' .            // CSV: 23
            $tp->station1->dir . ',' .                  // CSV: 23
            $tp->station1->dir_15min . ',' .            // CSV: 24
            #$tp->station1->dir_diff . ',';             // CSV: 25
            $tp->station1->v_a_dir . ',';               // CSV: 25
    }
    else {
        echo ',,,';
    }

    if ($tp->station2 !== null) {
        echo
            #$tp->station2->dir_5min . ',' .            // CSV: 26
            $tp->station2->dir . ',' .                  // CSV: 26
            $tp->station2->dir_15min . ',' .            // CSV: 27
            #$tp->station2->dir_diff . ',';             // CSV: 28
            $tp->station2->v_a_dir . ',';               // CSV: 28
    }
    else {
        echo ',,,';
    }

    echo $tp->score2 . ',';                             // CSV: 29
    echo $tp->score3 . ',';                             // CSV: 30
    echo $tp->score4 . ',';                             // CSV: 31
    echo $tp->score5 . ',';                             // CSV: 32
    echo '0';

    echo "\n";
}





?>
