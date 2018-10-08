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

require_once realpath(dirname(__FILE__)."/query.inc");

class GroupedQuery extends AbstractQuery
{
	var $groupBy;
	
	public function __construct($class, $constraints = "", $groupBy = "")
	{
		parent::__construct($class, $constraints);
		$this->groupBy = $groupBy;
	}
		
	function groupBy($groupBy)
	{
		$this->groupBy = $groupBy;
		return $this;
	}
	
	function execute()
	{
		$prototype = new $this->class;
		
		if (!$this->groupBy) $this->groupBy = $prototype->getPrimaryKey();
		
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
	
		if ($this->constraints == "") $this->constraints = "WHERE 1=1"; //TODO - tidy this up some day
	
		$query = "SELECT $fields FROM {$prototype->table} {$this->constraints} ".$prototype->getIdentityConstraint()." $orderBy";;
		trace($query, 3);
		
		$items = array();
		
		$field = $this->groupBy;
		
		$useFormat = preg_match("/^[A-Za-z0-9_]*$/", $field) ? false : true;
		
		try
		{
			$db = ConnectionManager::getConnection();
			
			$result = $db->prepare($query);
			$result->execute($this->params);
	
			while($line = $result->fetch())
			{
				$item = new $this->class;	//Hack to work around PHP's stupid implementation of get_class()
				$item->populate($line);
				$val = ($useFormat) ? $item->format($field) : $item->get($field);
				
				$items[$val][] = $item;
			}
		
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "groupedQuery() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		return $items;
	}
	
	
	/**
	 * Query the database to calculate aggregate values by grouping. The database table associated with
	 * the specified class is used as the source for the data.
	 * @param string $func the value or function to retrieve
	 * @return array an associative array of group values to calculated aggregate values
	 */
	function executeValue($func)
	{
		$prototype = new $this->class;
		
		if ($this->constraints == "") $this->constraints = "WHERE 1=1"; //TODO - tidy this up some day
		$this->constraints .= " ".$prototype->getIdentityConstraint();
		
		$query = "SELECT {$this->groupBy}, $func as result FROM {$prototype->table} {$this->tableAlias} {$this->constraints} GROUP BY {$this->groupBy}";
		
		trace($query, 3);
		
		$values = array();
		
		try
		{
			$db = ConnectionManager::getConnection();
			
			$result = $db->prepare($query);
			$result->execute($this->params);
			
			while ($row = $result->fetch())
			{
				$values[$row[$this->groupBy]] = $row['result'];
			}
			
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "Query::executeValue() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		return $values;
	}
	
	static function create($class, $constraints = "", $groupBy = "")
	{
		return new GroupedQuery($class, $constraints, $groupBy);
	}
}
		

/** Performs a query against the database, returning an array of arrays of DataItem objects of the specified class,
 * grouped by a particular field. If the indexing field has unique values, indexedQuery() might be more appropriate.
 * 
 * @param string $class the name of the class of objects to query
 * @param string $constraints optional constraint clause to apply to the query in the form "WHERE ... [ORDER BY ...]"
 * @param string $field optional the name of the field by which to group the results
 * @param object $filter optional filter used to select subset of the database fields for the result 
 * (See InclusionFilter and ExclusionFilter)
 */
function groupedQuery($class)
{
	$field = $prototype->primary_key;
	
	$filter = null;
	
	$query = new GroupedQuery($class);
	
	if (func_num_args() > 1)
	{
		$query->constraints(func_get_arg(1));
		
		if (func_num_args() > 2)
		{
			$query->groupBy(func_get_arg(2));
			
			if (func_num_args() > 3)
			{
				$query->filter(func_get_arg(3));
			} 
		}
	}
	
	return $query->execute();
}

?>