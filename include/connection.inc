<?php
/*
 * Created on Apr 12, 2007
 *
 * To change the template for this generated file go to
 * Window - Preferences - PHPeclipse - PHP - Code Templates
 */

require_once realpath(dirname(__FILE__)."/../framework/connection_manager.inc");

ConnectionManager::$dsn = "mysql:dbname=sonjara_cms;host=localhost";
ConnectionManager::$user = "sonjara_cms";
ConnectionManager::$password = "@lcp4tw!";

//$conn = array(
//	"dbhost"   => "localhost",
//	"dbuser"   => "projects",
//	"dbpasswd" => "!happy!",
//	"dbname"   => "projects",
//	 );
	 
?>
