<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

$page = array();

$sys['version'] = '0.0.0';
$sys['commit']  = '#DEVELOP#';

define('REV_FILE_PATH', __DIR__.'/../../pkg.rev');
define('VERSION_FILE_PATH', __DIR__.'/../../pkg.version');


if (@is_readable(REV_FILE_PATH)) {
    $sys['commit'] = trim(@file_get_contents(REV_FILE_PATH));
}

if (@is_readable(VERSION_FILE_PATH)) {
    $sys['version'] = trim(@file_get_contents(VERSION_FILE_PATH));
}

?>
