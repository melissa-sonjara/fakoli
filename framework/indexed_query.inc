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

class IndexedQuery
{
	var $class;
	var $constraints;
	var $filter;
	var $params;
	var $indexBy;
	
	function IndexedQuery($class, $constraints = "", $indexBy = "")
	{
		$this->class = $class;
		$this->constraints = $constraints;
		$this->indexBy = $indexBy;
	}
	
	function constraints($constraints)
	{
		$this->constraints = $constraints;
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

	function indexBy($indexBy)
	{
		$this->indexBy = $indexBy;
		return $this;
	}
	
	function execute()
	{
		$prototype = new $this->class;
	
		if (!$this->indexBy) $this->indexBy = $prototype->getPrimaryKey();
		
		if ($this->filter)
		{
			$prototype->filter = $this->filter;
		}
		
		$order_by_idx = strpos(strtoupper($this->constraints), "ORDER BY");	
		if ($order_by_idx !== false)
		{
			$orderBy = substr($this->constraints, $order_by_idx);
			
			$this->constraints = substr($this->constraints, 0, $order_by_idx);
		} 
		
		$fields = $prototype->getFieldList();
		
		if ($this->constraints == "") $this->constraints = "WHERE 1=1"; 
		$query = "SELECT $fields FROM {$prototype->table} {$this->constraints} ".$prototype->getIdentityConstraint()." $orderBy";
		trace("DataItem indexedQuery(): $query", 3);
		
		$items = array();
		
		$field = $this->indexBy;
		
		try
		{
			$db = ConnectionManager::getConnection();
	
			$result = $db->prepare($query);
			$result->execute($this->params);
			
			while($line = $result->fetch())
			{
				$item = new $this->class;
				$item->populate($line);
				$val = $item->get($field);
				
				if (array_key_exists($val, $items))
				{	
					// Implicitly promote to array if there is a collision
					if (!is_array($items[$val]))
					{
						$items[$val] = array($items[$val]);
					}
					$items[$val][] = $item;
				}
				else
				{
					$items[$val] = $item;
				}
			}
			
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "indexedQuery() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		return $items;	
	}
	
	static function create($class, $constraints = "", $indexBy = "")
	{
		return new IndexedQuery($class, $constraints, $indexBy);
	}
}


/** Performs a query against the database, returning an array of DataItem objects of the specified class,
 * indexed by a particular field. If more than one item is found that matches the same value of the indexing field,
 * the results entry is promoted to an array automatically. However, in cases where this is common, using
 * groupedQuery() might simplify your calling code.
 * 
 * @param string $class the name of the class of objects to query
 * @param string $constraints optional constraint clause to apply to the query in the form "WHERE ... [ORDER BY ...]"
 * @param string $field optional the name of the field by which to index the results
 * @param object $filter optional filter used to select subset of the database fields for the result 
 * (See InclusionFilter and ExclusionFilter)
 */
function indexedQuery($class)
{
	$field = $prototype->primary_key;
	$filter = null;
	
	$query = new IndexedQuery($class);
	
	if (func_num_args() > 1)
	{
		$query->constraints(func_get_arg(1));
		
		if (func_num_args() > 2)
		{
			$query->indexBy(func_get_arg(2));
			
			if (func_num_args() > 3)
			{
				$query->filter(func_get_arg(3));
			} 
		}		
	}

	return $query->execute();
}
?>