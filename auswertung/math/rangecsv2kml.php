<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

require_once('shared.inc.php');


// print usage
if ($argc < 4 || $argv[1] == '-h') {
    _e("Usage: " . $argv[0] . ' ' .
        'input-gps.csv input-r1.csv input-r2.csv ' . "\n");
    exit;
}

$in         = readCSV($argv[1]);
$in1        = readCSV($argv[2]);
$in2        = readCSV($argv[3]);
$track      = '';
$waypoints  = '';
$utc        = new DateTimeZone('UTC');


foreach ($in as $l) {
    if (count($l) < 2) continue;
    $d = new DateTime($l[0], $utc);
    if (!$d) continue;

    $track .= "
            <trkpt lat=\"" . $l[3] . "\" lon=\"" . $l[2] . "\">
                <ele>" . $l[4] . "</ele>
                <time>" . $d->format('Y-m-d\TH:i:s\Z') . "</time>
            </trkpt>";

    #$r .= "
    #    <when>" . $d->format('Y-m-d\TH:i:s.000\Z') . "</when>
    #    <gx:coord>" . $l[2] . ' ' .  $l[3] . ' ' . $l[4] . "</gx:coord>
    #";
}


$waypoints .= getWaypoints($in2, $in, 1);
$waypoints .= getWaypoints($in1, $in, 2);



function getWaypoints($csv_in, $gps_in, $dongle) {
    global $utc;
    $waypoints = '';

    foreach ($csv_in as $l) {
        if (count($l) < 2) continue;
        $d = new DateTime($l[0], $utc);
        if (!$d) continue;

        $gps = getPrevElem($gps_in, $l[0]);
        if (!$gps) continue;

        $waypoints .= "
            <wpt lat=\"" . $gps[3] . "\" lon=\"" . $gps[2] . "\">
                <ele>" . $gps[4] . "</ele>
                <name><![CDATA[" . $dongle . "]]></name>
                <type><![CDATA[Crossing]]></type>
                <time>" . $d->format('Y-m-d\TH:i:s\Z') . "</time>
            </wpt>";

    }

    return $waypoints;
}


echo <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<gpx version="1.0">
    <name>Reichweitentest GPS-Track</name>
    <trk>
        <name></name><number>1</number>
        <trkseg>$track
        </trkseg>
    </trk>
$waypoints
</gpx>

XML;


/*
 * */


/*
    <trk>
        <name>Reichweitentest</name><number>1</number>
        <trkseg>
            $r
        </trkseg>
    </trk>
 */

/*
echo <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<kml xmlns="http://www.opengis.net/kml/2.2"
xmlns:gx="http://www.google.com/kml/ext/2.2"
xmlns:atom="http://www.w3.org/2005/Atom">
<Document>
<open>1</open>
<visibility>1</visibility>
<name><![CDATA[Test]]></name>
<atom:author><atom:name><![CDATA[Michael Zapf]]></atom:name></atom:author>
<Schema id="schema">
<gx:MultiTrack>
<Placemark>
<gx:Track>

XML;

echo $r;

echo <<<XML

</gx:Track>
</Placemark>
</gx:MultiTrack>
</Document>
</kml>

XML;
*/


function getPrevElem($haystack, $needle) {
    $last = null;
    foreach ($haystack as $h) {
#        _e($h[0] . '  |  ' . $needle . "\n");
        if ($h[0] > $needle) return $last;
        $last = $h;
    }
    return $last;
}


?>
