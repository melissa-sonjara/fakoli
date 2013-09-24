<?php
/*
 * Created on Apr 12, 2007
 *
 * To change the template for this generated file go to
 * Window - Preferences - PHPeclipse - PHP - Code Templates
 */

require_once "framework/connection_manager.inc";

ConnectionManager::$dsn = "mysql:dbname=%{DB_SCHEMA};host=%{DB_HOST}";
ConnectionManager::$user = "%{DB_USER}";
ConnectionManager::$password = "%{DB_PASSWORD}";
	 
?>
