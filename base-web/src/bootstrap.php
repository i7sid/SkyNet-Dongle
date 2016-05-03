<?php
/**
 * @package bootstrap
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */


require_once __DIR__."/config/system.php";
require_once __DIR__."/config/config.php";


// bootstrap DOCTRINE2
use Doctrine\ORM\Tools\Setup;
use Doctrine\ORM\EntityManager;

require_once __DIR__."/../vendor/autoload.php";

// Create a simple "default" Doctrine ORM configuration for Annotations
$isDevMode = true;
$modelpaths = array(__DIR__.'/models');
$doctrine_config = Setup::createAnnotationMetadataConfiguration($modelpaths, $isDevMode);


// database configuration parameters
$conn = array(
    'driver'    => 'pdo_mysql',
    'host'      => $config['db']['host'],
    'user'      => $config['db']['user'],
    'password'  => $config['db']['password'],
    'dbname'    => $config['db']['name'],
    'driverOptions'    => array(1002 => 'SET NAMES utf8')
);

// obtaining the entity manager
$em = EntityManager::create($conn, $doctrine_config);



// todo own autoloader
#require_once __DIR__.'/models/step.php';

// exceptions
#require_once __DIR__.'/exceptions/success.php';
?>
