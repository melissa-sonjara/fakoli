<?php
/**************************************************************

 Copyright (c) 2007-2012 Sonjara, Inc

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

require_once realpath(dirname(__FILE__)."/query.inc");

/**
 * DataItemIterator is a memory-efficient iterator class that can be used
 * when rendering large data sets. With this iterator DataItems are retrieved 
 * from the database one at a time, rather than retrieving the full result set 
 * into memory all at once. It is a forward-only iterator, meaning
 * it can be used in foreach(...) loops, but not for random access to 
 * the results. This class is not intended to be used directly by the programmer.
 * 
 * @author andy
 */
class DataItemIterator implements Iterator, Countable
{
	var $query;
	var $result;
	var $current = null;
	var $position = -1;
	var $item;
	
	public function __construct($query)
	{
		$this->query = $query;
		$this->item = new $this->query->class;		
	}
	
	public function count()
	{
		$this->rewind();
		trace("Row Count: {$this->query->rowCount}", 4);
		return $this->query->rowCount;		
	}
	
	public function rewind()
	{
		if ($this->position == 0) return;
		
		$this->current = null;
		$this->position = 0;
		
		try
		{
			$this->result = $this->query->_runQuery();
			$this->current = $this->result->fetch();
		}
		catch(PDOException $e)
		{
			throw new FakoliException($e->getMessage());
		}
	}
	
	public function current()
	{
		$this->item->populate($this->current);
		return $this->item;
	}
	
	public function valid()
	{
		return ($this->current != null);
	}
	
	public function next()
	{
		$this->current = $this->result->fetch();
		++$this->position;
	}
	
	public function key()
	{
		return $this->position;
	}
	
}

/**
 * IteratedQuery provides a memory-efficient way to query and return large
 * data sets. Whereas the default Query class and the IndexedQuery and
 * GroupedQuery classes all pull the full result set into memory when the
 * query is executed, IteratedQuery leaves the connection to the database
 * open and retrieves the results one row at a time. The results are returned
 * wrapped in a DataItemIterator, allowing them to be used in foreach(...)
 * statements, etc.
 * 
 * In general it is recommended that IteratedQuery only be used in situations 
 * where the result would take up too much memory to be processed using a 
 * standard Query object.
 * 
 * @author andy
 *
 */
class IteratedQuery extends AbstractQuery
{	
	public function __construct($class, $constraints = "")
	{
		parent::__construct($class, $constraints);
	}

	function _runQuery()
	{
		$prototype = new $this->class;
		$prototype->filter = $this->filter;
	
		$order_by_idx = strpos(strtoupper($this->constraints), "ORDER BY");
		$orderBy = "";	
		
		if ($order_by_idx !== false)
		{
			$orderBy = substr($this->constraints, $order_by_idx);
			
			$this->constraints = substr($this->constraints, 0, $order_by_idx);
		} 
		
		if ($this->constraints == "") $this->constraints = "WHERE 1=1"; //TODO - tidy this up some day
		$this->constraints .= " ".$prototype->getIdentityConstraint();
		
		$query = "SELECT ".$prototype->getFieldList()." FROM {$prototype->table} {$this->tableAlias} {$this->constraints} $orderBy";
		
		trace("$query", 3);
		
		try
		{
			$db = ConnectionManager::getConnection();
			
			$result = $db->prepare($query);
			$result->execute($this->params);
			$this->rowCount = $result->rowCount();
			return $result;
		}
		catch(PDOException $e)
		{
			throw new FakoliException($e->getMessage());
		}
	}
	
	function execute()
	{
		return new DataItemIterator($this);
	}
	
	static function create($class, $constraints = "")
	{
		return new IteratedQuery($class, $constraints);
	}
}


/**
 * iteratedQuery() provides a memory-efficient way to query and return large
 * data sets. Whereas the default query() function and the indexedQuery() and
 * groupedQuery() functions all pull the full result set into memory when the
 * query is executed, iteratedQuery() leaves the connection to the database
 * open and retrieves the results one row at a time. The results are returned
 * wrapped in a DataItemIterator, allowing them to be used in foreach(...)
 * statements, etc.
 * 
 * One limitation of the returned resultset is that count() will not return
 * an accurate count of the number of items, since this cannot be determined
 * reliably until all items have been read. In general it is recommend that
 * iteratedQuery() only be used in situations where the result would take up 
 * too much memory to be processed using a standard Query object.
 * 
 * @param string $class the name of the class of objects to query
 * @param string $constraints optional constraint clause to apply to the query in the form "WHERE ... [ORDER BY ...]"
 * @param object $filter optional filter used to select subset of the database fields for the result 
 * (See InclusionFilter and ExclusionFilter)
 */
function iteratedQuery($class)
{
	$query = new IteratedQuery($class);
	
	if (func_num_args() > 1)
	{
		$query->constraints(func_get_arg(1));
		
		if (func_num_args() > 2)
		{
			$query->filter(func_get_arg(3));
		}		
	}

	return $query->execute();
}
?>