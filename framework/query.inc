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

class Query
{
	var $class;
	var $constraints;
	var $page;
	var $size;
	var $filter;
	var $params;
	
	function Query($class, $constraints = "")
	{
		$this->class = $class;
		$this->constraints = $constraints;
	
		$this->page = -1;
		$this->size = -1;
		
		$this->filter = null;
		$this->params = array();
	}

	function constraints($constraints)
	{
		$this->constraints = $constraints;
		return $this;
	}
	
	function page($page, $size)
	{
		$this->page = $page;
		$this->size = $size;
		return $this;
	}
	
	function filter($filter)
	{
		$this->filter = $filter;
		return $this;
	}
	
	function params($params)
	{
		$this->params = $params;
		return $this;
	}
	
	function bind()
	{
		$num = func_num_args();
		for($i = 0; $i < $num; $i += 2)
		{
			$this->params[func_get_arg($i)] = func_get_arg($i + 1);
		}
		
		return $this;
	}
	
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
			$query = "SELECT ".$prototype->getFieldList()." FROM {$prototype->table} {$this->constraints} $orderBy";
		}
		else
		{
			$query = "SELECT * FROM {$prototype->table} {$this->constraints} $orderBy";
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