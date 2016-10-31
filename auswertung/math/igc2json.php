<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

require_once('shared.inc.php');

// print usage
if ($argc < 2 || $argv[1] == '-h') {
    _e("Usage: " . $argv[0] . ' ' .
        'input.igc' .
        "\n");
    exit;
}

$in = $argv[1];

$lines = file($in);


echo "flights.push(\n";

$i = 0;
foreach ($lines as $l) {
    if ($i == 0) {
        echo '    "';
    }
    echo trim($l, "\r\n\t ") . '\n';

    $i++;
    if ($i == 30) {
        echo '" +' . "\n";
        $i = 0;
    }
}

if ($i != 0) {
        echo '" +' . "\n";
}

echo "'');\n";

