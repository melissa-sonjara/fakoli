<?php
/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

require_once realpath(dirname(__FILE__)."/cache.inc");

/**
 * 
 * 
 * The ConnectionManager class provides the common point of entry by
 * which DataItems can access the global database connection via the 
 * getConnection() method. The global connection can be manually released
 * via the releaseConnection() method. If needed, additional connections
 * to the database can be created using the newConnection() method. 
 * These additional connections can be released simply by setting the variable
 * used to store them to null. 
 */
class ConnectionManager
{
	static $dsn;
	static $user;
	static $password;
	static $conn;
	
	/**
	 * Retrieves a reference to the global database connection. If no connection
	 * to the database has yet been made, one will be created as a result of this
	 * call.
	 */
	static function getConnection()
	{
		try
		{
			if (!ConnectionManager::$conn)
			{
				$startTime = microtime(true);
				ConnectionManager::$conn = new PDO(ConnectionManager::$dsn, ConnectionManager::$user, ConnectionManager::$password);
				ConnectionManager::$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
				$endTime = microtime(true);
				trace("Connection opened in ".number_format($endTime-$startTime, 3)." seconds", 3);
					
			}
			return ConnectionManager::$conn;
		}
		catch(PDOException $e)
		{
			trace("Database connection failed - " . $e->getMessage(), 1);
			throw new DataItemException("Database connection failed - " . $e->getMessage());
		}
	}
	
	/**
	 * Returns a new connection to the database. This is separate from the global
	 * connection.
	 */
	static function newConnection()
	{
		try
		{
			$startTime = microtime(true);
			$conn = new PDO(ConnectionManager::$dsn, ConnectionManager::$user, ConnectionManager::$password);
			$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
			$endTime = microtime(true);
			trace("New connection opened in ".number_format($endTime-$startTime, 3)." seconds", 3);
			return $conn;
		}
		catch(PDOException $e)
		{
			trace("Database connection failed - " . $e->getMessage(), 1);
			throw new DataItemException("Database connection failed - " . $e->getMessage());
		}
	}
	
	/**
	 * Releases the global connection to the database.
	 */
	static function releaseConnection()
	{
		ConnectionManager::$conn = null;
	}
	
	/**
	 * Quote a string value based on the character set of the global connection
	 * 
	 * @param string $str the string to quote
	 * @return string the string, wrapped in quotes, with other quote characters escaped
	 */
	static function quote($str)
	{
		return ConnectionManager::getConnection()->quote($str);
	}
	
	/**
	 * Escapes a string based on the character set of the global connection.
	 * This is a kludge - try not to use it. I'm adding it for backward compatibility
	 * only when migrating to PHP 7 where mysql_escape_string() is no longer available.
	 * @param string the string to escape
	 * @return string the string with quote characters escaped
	 */
	static function escape($str)
	{
		$quoted = ConnectionManager::getConnection()->quote($str);
		$escaped = preg_replace("/^'(.*)'$/", "$1", $quoted);
		trace("ESCAPE: $str -> $quoted -> $escaped", 1);
		return $escaped;
	}
	
	/**
	 * Determine the version of the connected database.
	 * @return string the version of the connected database.
	 */
	static function getVersion()
	{
		$version = Cache::get("sql_version");
		if (!$version)
		{
			$conn = ConnectionManager::getConnection();
			$version = $conn->query('select version()')->fetchColumn();
			Cache::put("sql_version", $version);
		}
		
		return $version;
	}
}