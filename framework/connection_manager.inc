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
				trace("Opening Connection", 3);
				ConnectionManager::$conn = new PDO(ConnectionManager::$dsn, ConnectionManager::$user, ConnectionManager::$password);
				ConnectionManager::$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
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
			trace("Opening Connection", 3);
			$conn = new PDO(ConnectionManager::$dsn, ConnectionManager::$user, ConnectionManager::$password);
			$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
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
}