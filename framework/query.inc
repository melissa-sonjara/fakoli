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
 * Query provides an object-oriented interface for database queries.
 * By instantiating Query objects you can specify a full or partial 
 * set of criteria and parameters, and then subsequently execute the query
 * to retrieve the results. You can pass around the constructed Query 
 * object and execute it multiple times, allowing for lazy evaluation
 * and other performance enhancing tricks.
 * @author andy
 *
 */
class Query
{
	var $class;
	var $constraints;
	var $page;
	var $size;
	var $filter;
	var $params;
	var $tableAlias;
	
	/**
	 * Creates a new Query object.
	 * @param string $class the DataItem class to be queried
	 * @param string $constraints [optional] constraint clause for the query
	 * @return a new Query object
	 */
	function Query($class, $constraints = "")
	{
		$this->class = $class;
		$this->constraints = $constraints;
	
		$this->page = -1;
		$this->size = -1;
		
		$this->filter = null;
		$this->params = array();
		$this->tableAlias = "";
	}

	/**
	 * Sets the constraint clause for the Query
	 * @param $constraints the constraint clause;
	 * @return Query reference to the Query object, to allow for call chaining.
	 */
	function constraints($constraints)
	{
		$this->constraints = $constraints;
		return $this;
	}
	
	/**
	 * Sets the page number and size for constraining the result set by page.
	 * By default results are not paged.
	 * @param $page integer the page number
	 * @param $size integer the number of items per page
	 * @return Query reference to the Query object, to allow for call chaining.
	 */
	function page($page, $size)
	{
		$this->page = $page;
		$this->size = $size;
		return $this;
	}
	
	/**
	 * Sets a filter to constrain the fields retrieved when the query is executed.
	 * @param $filter Object the InclusionFilter or ExclusionFilter to use
	 * @return Query reference to the Query object, to allow for call chaining.
	 */
	function filter($filter)
	{
		$this->filter = $filter;
		return $this;
	}
	
	/**
	 * Sets the bound parameters array. Generally it is more convenient to call bind()
	 * but this method can be useful if the query is being constructed programmatically.
	 * @param $params the array of placeholders and parameters to bind
	 * @return Query reference to the Query object, to allow for call chaining.
	 */
	function params($params)
	{
		$this->params = $params;
		return $this;
	}
	
	/**
	 * Sometimes when you are creating abonimable SQL constraints you might need to
	 * refer back to the main object table by a specific alias. In those cases, set
	 * the table alias for the main object using this message.
	 * 
	 * @param $alias String the table alias name
	 * @return Query reference to the Query object, to allow for call chaining.
	 */
	function tableAlias($alias)
	{
		$this->tableAlias = $alias;
		return $this;
	}
	
	/**
	 * Binds placeholders to parameter values.
	 * @return Query reference to the Query object, to allow for call chaining.
	 */
	function bind()
	{
		$num = func_num_args();
		for($i = 0; $i < $num; $i += 2)
		{
			$this->params[func_get_arg($i)] = func_get_arg($i + 1);
		}
		
		return $this;
	}
	
	/**
	 * Return true if the query matches one or more rows in the database.
	 */
	function exists()
	{
		$prototype = new $this->class;
		$pk = $prototype->getPrimaryKey();
		
		$query = "SELECT $pk FROM {$prototype->table} {$this->tableAlias} {$this->constraints} LIMIT 1";
		
		$exists = false;
		
		try
		{
			$db = ConnectionManager::getConnection();
			
			$result = $db->prepare($query);
			$result->execute($this->params);
			
			if ($result->fetch()) $exists = true;
			
			unset($result);
		}
		catch(PDOException $e)
		{
			throw new DataItemException($e->getMessage());
		}
		
		return $exists;
				 
	}
	
	/**
	 * Executes the query and returns an array of DataItems containing the results
	 * (the class of items returned is as specified in the Query constructor).
	 * @return array an array of DataItems containing the results.
	 */
	function execute()
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
		
		if (is_object($this->filter))
		{
			$query = "SELECT ".$prototype->getFieldList()." FROM {$prototype->table} {$this->tableAlias} {$this->constraints} $orderBy";
		}
		else
		{
			$query = "SELECT * FROM {$prototype->table} {$this->tableAlias} {$this->constraints} $orderBy";
		}
		
		trace("$query", 3);
		trace("Page: $this->page Size: $this->size", 3);
		$items = array();
		
		$size = $this->size;
		
		try
		{
			$db = ConnectionManager::getConnection();
			
			$result = $db->prepare($query);
			$result->execute($this->params);
			
			if ($this->page > 0)
			{
				$count = ($this->page - 1) * $this->size;
				while($count--)
				{
					$result->fetch();
				}
			}
			
			while($line = $result->fetch())
			{
				$item = new $this->class;
				$item->filter = $this->filter;
				$item->populate($line);
				$items[] = $item;
				
				--$size;
				if ($size == 0) break;
			}
			
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "query() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		trace(count($items)." items found", 3);
		return $items;
	}
	
	/**
	 * Executes the query and returns a singleton result row. If no results match
	 * then a DataNotFoundException is thrown. If more than one row matches, then
	 * a DataItemException is thrown.
	 * @return DataItem the matching DataItem from the database
	 */
	function executeSingle()
	{
		$results = $this->execute();
		if (count($results) == 1)
		{
			return $results[0];
		}
		
		if (count($results) == 0)
		{
			throw new DataNotFoundException();
		}		
		else
		{
			throw new DataItemException("Ambiguous singleton query");
		}
	}

	/**
	 * Query the database to calculate an aggregate value. The database table associated with
	 * the specified class is used as the source for the data.
	 * @param string $func the value or function to retrieve
	 */
	function executeValue($func)
	{
		$prototype = new $this->class;
		
		if ($this->constraints == "") $this->constraints = "WHERE 1=1"; //TODO - tidy this up some day
		$this->constraints .= " ".$prototype->getIdentityConstraint();
			
		$query = "SELECT $func as result FROM {$prototype->table} {$this->tableAlias} {$this->constraints}";
		
		trace($query, 3);
		
		try
		{
			$db = ConnectionManager::getConnection();
			
			$result = $db->prepare($query);
			$result->execute($this->params);
			
			if ($row = $result->fetch())
			{
				$value = $row['result'];
			}
	
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "Query::executeValue() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		return $value;
	}
	
	
	/**
	 * Static factory method to create a new Query. 
	 * @param string $class the DataItem class to be queried
	 * @param string $constraints [optional] constraint clause for the query
	 * @return a new Query object
	 */
	static function create($class, $constraints = "")
	{
		return new Query($class, $constraints);
	}
}

/**
 * Performs a query against the database, returning an array of DataItem objects of the specified class.
 * 
 * @param string $class the name of the class of objects to query
 * @param $constraints optional constraint clause to apply to the query in the form "WHERE ... [ORDER BY ...]"
 * @param $filter optional filter to apply to the query
 * @param $page optional page number to retrieve
 * @param $size optional size of pages
 */
function query($class)
{
	$query = new Query($class);

	if (func_num_args() > 1)
	{
		$idx = 1;
		
		if (is_object(func_get_arg(1)))
		{
			$query->filter($filter);
			++$idx;
		}
		
		if (is_array(func_get_arg($idx)))
		{
			$query->params(func_get_arg($idx++));
		}
		
		if (func_num_args() > $idx)
		{
			$query->constraints(func_get_arg($idx));
			
			if (func_num_args() == $idx + 3)
			{
				$query->page(func_get_arg($idx + 1), func_get_arg($idx + 2));
			}
		}
	}
	
	return $query->execute();
}

?>