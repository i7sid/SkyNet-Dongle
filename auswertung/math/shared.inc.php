<?php
/**
 * Shared methods and functionality.
 *
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

// set timezone for strtotime
date_default_timezone_set('UTC');

function _e($msg) {
    if (is_string($msg)) {
        file_put_contents('php://stderr', $msg);
    }
    else if (is_object($msg)) {
        $r = var_export($msg, true);
        $r .= "\n";
        file_put_contents('php://stderr', $r);
    }
}

function readCSV($csvFile) {
    $file_handle = fopen($csvFile, 'r');
    if (!$file_handle) throw new Exception("Could not open file  " . $csvFile . "\n");
    while (!feof($file_handle) ) {
        $line_of_text[] = fgetcsv($file_handle, 1024);
    }
    fclose($file_handle);
    return $line_of_text;
}

function parseCoord($c) {
    $r = '';
    $full = floatval($c);

    $deg = (int)floor($c / 100);
    $min = $c - $deg * 100;

    $frac = $min / 60.0;

    return $deg + $frac;
}

/**
 * @brief   Rotates the given angle (in degrees°)  by 180°.
 */
function mirrorWind($a) {
    return ((180 + $a) % 360);
}


function polarVectorMean($speed1, $dir1, $speed2, $dir2, &$speedr, &$dirr) {
    $rad1 = deg2rad($dir1);
    $rad2 = deg2rad($dir2);
    $x1 = $speed1 * $speed1 * sin($rad1);
    $y1 = $speed1 * $speed1 * cos($rad1);
    $x2 = $speed2 * $speed2 * sin($rad2);
    $y2 = $speed2 * $speed2 * cos($rad2);

    $x = 0.5 * ($x1 + $x2);
    $y = 0.5 * ($y1 + $y2);

    $speedr = sqrt(sqrt($x*$x + $y*$y));
    $dirr = (450 - rad2deg(atan2($y, $x))) % 360;
}


/**
 * @param   $p          Point on earth
 * @param   $h          Height of pilot (in meters)
 * @param   $v_long     Long term averaged wind speed
 * @param   $dir_long   Long term averaged wind direction
 * @param   $v_therm    Thermal speed (vertical speed of ascending air)
 */
function calcWindOffset($p, $h, $v_long, $dir_long, $v_therm) {
    $t = $h / $v_therm;
    //$t = meters2degrees($t);

    // calculate long term wind speed vector
    $rad = deg2rad($dir_long);
    $vux = ($v_long) * sin($rad);
    $vuy = ($v_long) * cos($rad);

    return new XYZ(
        $p->x + meters2degrees($t * $vux),
        $p->y + meters2degrees($t * $vuy),
        $p->z + $t * $v_therm
    );
}


/**
 * @brief   Converts the given distance from meters into coordinates (EPSG:4326) degrees.
 * @note    Only works on very small distances as it discards the fact that earth is spherical.
 * @param   coords  Distance in meters
 */
function meters2degrees($m) {
    // rough estimation: 1° is about 111111m
    return ($m / 111111);
}

/**
 * @brief   Calculates the distance from coordinates (EPSG:4326) degrees in meters.
 * @note    Only works on very small distances as it discards the fact that earth is spherical.
 * @source  http://www.kompf.de/gps/distcalc.html
 */
function distanceFromCoordinates($lat1, $lon1, $lat2, $lon2) {
    $dx = 111300 * cos(deg2rad(($lat1 + $lat2) / 2 * 0.01745)) * ($lon1 - $lon2);
    $dy = 111300 * ($lat1 - $lat2);

    return sqrt($dx * $dx + $dy * $dy);
}




function singleStationScore($station) {
    $s = 0;

    #$s += abs($station->speed_diff);
    #$s += abs($station->speed_5min - $station->speed_15min) * 3.6; #* 0.9;
    ##$s += abs($station->dir_5min - $station->dir_15min);


    #$s += abs($station->dir_5min - $station->dir_15min) * 0.6; #* 0.1;
    #$s *= abs($station->speed_diff);
    #$s *= 6;


    $s += abs($station->speed_5min - $station->speed_15min) * 0.1;
    $s += abs($station->dir_diff) * 0.9;

    return $s;
}
function singleStationScore2($station) {
    $s = 0;

#    $s *= 3;
    $s += abs($station->speed_diff);# * 2;

    return $s;
}


class TimePoint {
    public $time                = null;
    public $station1            = null;
    public $station2            = null;
    public $thermal_point_x     = 0;
    public $thermal_point_y     = 0;
    public $thermal_point_air_x = 0;
    public $thermal_point_air_y = 0;
    public $thermal_point_air_z = 0;
    public $score               = 0;
    public $score2              = 0;
    public $score3              = -1;
    public $score4              = 0;
    public $score5              = 0;

    public function __construct($t) {
        $this->time = new DateTime($t);
    }
}

class Measurement {
    public $speed           = 0;
    public $dir             = 0;
    public $speed_1min      = 0;
    public $dir_1min        = 0;
    public $speed_5min      = 0;
    public $dir_5min        = 0;
    public $speed_15min     = 0;
    public $dir_15min       = 0;
    public $speed_30min     = 0;
    public $dir_30min       = 0;
    public $speed_diff      = 0;
    public $dir_diff        = 0;
    public $v_a_speed       = 0;
    public $v_a_dir         = 0;

    public $mac             = 0;
    public $pos_x           = 0;
    public $pos_y           = 0;
}

class XYZ {
    public $x   = 0;
    public $y   = 0;
    public $z   = 0;

    public function __construct($x, $y, $z) {
        $this->x = $x;
        $this->y = $y;
        $this->z = $z;
    }

    public function add($b) {
        $this->x += $b->x;
        $this->y += $b->y;
        $this->z += $b->z;
    }

    public function mult($t) {
        $this->x *= $t;
        $this->y *= $t;
        $this->z *= $t;
    }
}


?>
