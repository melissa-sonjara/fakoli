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
 * The DataTransaction class wraps the underlying database's transaction model.
 * DataTransactions automatically start an isolated database transaction upon
 * creation. The DataTransaction class can then be used with Query and DataItem
 * objects. Each DataTransaction instance maintains its own dedicated connection
 * to the database.
 * 
 * @author andy
 *
 */
class DataTransaction
{
	var $conn;
	var $tx;
	
	/**
	 * Creates a new DataTransaciton.
	 */
	function DataTransaction()
	{
		$this->conn = ConnectionManager::newConnection();
		if (!$this->conn->beginTransaction())
		{
			throw new DataItemException("Unable to initiate transaction");
		}
	}
	
	/**
	 * Roll back any changes made during this transaction.
	 */
	function rollback()
	{
		if (!$this->conn)
		{
			throw new DataItemException("Attempt to rollback a previously completed transaction");
		}			
		
		$this->conn->rollback();
		$this->conn = null;
	}
	
	/**
	 * Commit the changes made during this transaction.
	 */
	function commit()
	{
		if (!$this->conn)
		{
			throw new DataItemException("Attempt to commit a previously completed transaction");
		}	
		
		$this->conn->commit();
		$this->conn = null;
	}
	
	/**
	 * Return the database connection for this transaction.
	 * @return PDO database connection
	 */
	function getConnection()
	{
		if (!$this->conn)
		{
			throw new DataItemException("Transaction has already been closed");
		}
		
		return $this->conn;
	}
}

?>