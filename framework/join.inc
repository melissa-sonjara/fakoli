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

require_once realpath(dirname(__FILE__)."/data_item.inc");

/**
 * InnerJoinResult is an empty placeholder class. Instances of 
 * InnerJoinResult are used to aggregate the results of the joined 
 * query. Each table that was joined is added as a sub-object of
 * the InnerJoinResult.
 * 
 * 
 */
class InnerJoinResult
{
	var $_xmlTagName = "join";
	var $_classes;
	
	function InnerJoinResult($tag = "join")
	{
		$this->_xmlTagName = $tag;
		$this->_classes = array();
	}
	
	function toXML($indent = 0)
	{
		$xml = str_repeat(" ", $indent) . "<" . $this->_xmlTagName . ">\n";
		foreach ($this as $field => $val)
		{
			if (is_object($val))
			{
				$xml .= $val->toXML($indent + 1);
			}
		}
		
		$xml .=  str_repeat(" ", $indent) . "</" . $this->_xmlTagName . ">\n";
		
		return $xml;
	}
	
	function format($template)
	{
		$matches = array();
		
		if (count($this->_classes) == 1 && preg_match("/^[\\w_]+/"))
		{
			$cl = $this->_classes[0];
			return $this->$cl->$format;
		}
		
		preg_match_all("/\\{([\\w_]+)\.([^}]+)}/", $template, $matches, PREG_SET_ORDER);
		
		foreach($matches as $match)
		{
			$class = $match[1];
			$subtemplate = $match[2];
	
			$value = $this->$class->format("{".$subtemplate."}");
			
			$template = str_replace($match[0], $value, $template);
		}
		
		return $template;
	}
}

/**
 * This class is used to programmatically perform inner join queries
 * across multiple objects. The InnerJoin will automatically generate
 * the SQL for the join based on the primary key field names of each
 * object. In order for an object to be successfully joined, it must
 * contain a primary key with the same name as a foreign key in another
 * participating object (or a foreign key that matches another object's
 * primary key).
 * 
 * Results are returned as an array of InnerJoinResult composite objects.
 * The matching row from each object in the join as added as a sub-object
 * of the InnerJoinResult with the field name being the class name of the 
 * object.
 * 
 * 
 */
class InnerJoin
{
	var $classes = array();
	var $protos = array();
	var $xrefs = array();

	var $xrefFieldMaps = array();
	
	var $aliases = array();
	var $xrefAliases = array();
	
	var $unique = false;
	var $tag = "join";
	
	/**
	 * Create a new InnerJoin object.
	 */
	function InnerJoin($tag = "join")
	{
		$this->tag = $tag;
	}
	
	function add($class)
	{
		$this->classes[] = $class;
		
		$this->protos[$class] = new $class;
		
		if (func_num_args() > 1)
		{
			$filter = new InclusionFilter();

			if (func_get_arg(1) !== false)
			{
				for($i = 1; $i < func_num_args(); ++$i)
				{
					$filter->add(func_get_arg($i));
				}
			}
					
			$this->protos[$class]->filter = $filter;
		}
	}
	
	function setFilter($class, $filter)
	{
		$this->protos[$class]->filter = $filter;
	}
	
	function xref($xref, $xrefFieldMaps = null)
	{
		$x = new $xref;
		
		$this->xrefs[] = $x;
		
		if ($xrefFieldMaps)
		{
			$this->xrefFieldMaps[$x->table] = $xrefFieldMaps;
		}
	}
	
	function generateQuery($constraints = "")
	{
		$tables = "";
		$fields = "";
		$alias  = "";
		for($i = 0; $i < count($this->classes); ++$i)
		{
			$class =  $this->classes[$i];
			$proto =& $this->protos[$class];
			 
			$alias = "t$i";
			$this->aliases[$proto->table] = $alias;
			
			if ($tables) $tables .= ", ";
			$tables .= "{$proto->table} $alias";
			
			$f = $proto->getFieldList($alias); 
			if ($fields && $f) $fields .=", ";
			$fields .= $f;
		}
		
		for($i = 0; $i < count($this->xrefs); ++$i)
		{
			$xref = $this->xrefs[$i];
			
			$alias = "x$i";
			$this->xrefAliases[$xref->table] = $alias;
			
			if ($tables) $tables .= ", ";
			$tables .= "{$xref->table} $alias";
		}
		
		if ($this->unique) $fields = "DISTINCT $fields";
		
		if (strncmp($constraints, "WHERE", 5))
		{
			$constraints = "WHERE 1=1 $constraints";
		}
		
		$join = "";
		
		// Build Inner Join
		
		for($i = 0; $i < count($this->classes) - 1; ++$i)
		{
			$c = $this->classes[$i];
			$p = $this->protos[$c];
			
			for ($j = $i+1; $j < count($this->classes); ++$j)
			{
				$d = $this->classes[$j];
				$q = $this->protos[$d];
				
				trace($p->primary_key." ".array_key_exists($p->primary_key, $q->fields), 3);
				trace($q->primary_key." ".array_key_exists($q->primary_key, $p->fields), 3);
				
				if (array_key_exists($p->primary_key, $q->fields))
				{
					$join .= " AND t$i.{$p->primary_key}=t$j.{$p->primary_key}";
				}
				else if (array_key_exists($q->primary_key, $p->fields))
				{
					$join .= " AND t$i.{$q->primary_key}=t$j.{$q->primary_key}";
				}
				
			}
			
		}
		
		// Alias main table fields
		
		for($i = 0; $i < count($this->classes); ++$i)
		{
			$c = $this->classes[$i];
			$p = $this->protos[$c];
			foreach($p->fields as $field => $type)
			{
				$constraints = preg_replace("/([\s\(])(".$field.")\\b/", "$1t$i.$2", $constraints);
			}
		}
		
		// Handle XRefs
		
		foreach($this->xrefs as $xref)
		{
			$map = $this->xrefFieldMaps[$xref->table];
			
			foreach($this->protos as $p)
			{
				$pk = $p->primary_key;
				$xk = ($map && array_key_exists($pk, $map)) ? $map[$pk] : $pk;
					
				if ($xref->hasField($xk))
				{
					$xa = $this->xrefAliases[$xref->table];
					$ta = $this->aliases[$p->table];
					
					$join .= " AND $xa.{$xk}=$ta.{$pk}";
				}
			}
		}
		
		$matches = array();

		$orderBy = "";
		if (preg_match("/^(.*?)(ORDER\\s+BY.*)/i", $constraints, $matches))
		{
			$constraints = $matches[1];
			$orderBy = $matches[2];
		}
		
		$query = "SELECT $fields FROM $tables $constraints $join $orderBy";
		
		trace("InnerJoin::generateQuery() - $query", 3);
		
		return $query;
	}
	
	function query($constraints = "", $page = -1, $size = -1)
	{
		$query = $this->generateQuery($constraints);		
		
		try
		{
			$db = ConnectionManager::getConnection();
			$result = $db->prepare($query);
			$result->execute();	
			
			if ($page > 0)
			{
				$count = ($page - 1) * $size;
				while($count--)
				{
					$result->fetch();
				}
			}
				
			$items = array();
			
			while($line = $result->fetch())
			{
				$item = new InnerJoinResult($this->tag);
				
				for($i = 0; $i < count($this->classes); ++$i)
				{
					$class = $this->classes[$i];
					$item->$class = new $class;	//Hack to work around PHP's stupid implementation of get_class()
					$item->$class->populate($line);
				
					--$size;
					if ($size == 0) break;
				}
	
				$items[] = $item;
			}
			
			unset($result);	
			
			return $items;	
		}
		catch(PDOException $e)
		{
			die("InnerJoin::query() failed - ".$e->getMessage());
		}
	}
	
		
	/** Performs a query against the database, returning an array of DataItem objects of the specified class,
	 * indexed by a particular field. If more than one item is found that matches the same value of the indexing field,
	 * the results entry is promoted to an array automatically. However, in cases where this is common, using
	 * groupedQuery() might simplify your calling code.
	 * 
	 * @param string $constraints optional constraint clause to apply to the query in the form "WHERE ... [ORDER BY ...]"
	 * @param string $field optional the name of the field by which to index the results
	 * (See InclusionFilter and ExclusionFilter)
	 */
	function indexedQuery($constraints = "", $indexBy = "")
	{
		$field = "";
		$idxClass = "";
		
		if ($indexBy != "")
		{
			list($idxClass, $field) = explode(".", $indexBy);	
		}
		else
		{
			$idxClass = $this->class[0];
			$obj = new $class;
			$field = $obj->primary_key;
		}
			
		$query = $this->generateQuery($constraints);		
		
		try
		{
			$db = ConnectionManager::getConnection();
			$result = $db->prepare($query);
			$result->execute();
			
			$items = array();
			
			while($line = $result->fetch())
			{
				$item = new InnerJoinResult($this->tag);
				
				for($i = 0; $i < count($this->classes); ++$i)
				{
					$class = $this->classes[$i];
					$item->$class = new $class;	//Hack to work around PHP's stupid implementation of get_class()
					$item->$class->populate($line);
				}
	
				$idx = $item->$idxClass->$field;
				if (array_key_exists($idx, $items))
				{
					// Implicitly promote to array if there is a collision
					if (!is_array($items[$idx]))
					{
						$items[$idx] = array($items[$idx]);
					}
					$items[$idx][] = $item;
				}
				else
				{
					$items[$idx] = $item;
				}			
			}
			
			unset($result);	
			
			return $items;
		}
		catch(PDOException $e)
		{
			die("InnerJoin::indexedQuery() failed - ".$e->getMessage());	
		}
	}
	
	/** Performs a query against the database, returning an array of arrays of DataItem objects of the specified class,
	 * grouped by a particular field. If the indexing field has unique values, indexedQuery() might be more appropriate.
	 * 
	 * @param string $constraints optional constraint clause to apply to the query in the form "WHERE ... [ORDER BY ...]"
	 * @param string $field optional the name of the field by which to group the results
	 */
	function groupedQuery($constraints = "", $indexBy = "")
	{
		$field = "";
		$idxClass = "";
		
		if ($indexBy != "")
		{
			list($idxClass, $field) = explode(".", $indexBy);	
		}
		else
		{
			$idxClass = $this->class[0];
			$obj = new $class;
			$field = $obj->primary_key;
		}
		
		trace("InnerJoin::groupedQuery(): Group by $idxClass $field", 3);
			
		$query = $this->generateQuery($constraints);		
			
		try
		{
			$db = ConnectionManager::getConnection();
			$result = $db->prepare($query);
			$result->execute();
						
			$items = array();
			
			while($line = $result->fetch())
			{
				$item = new InnerJoinResult($this->tag);
				
				for($i = 0; $i < count($this->classes); ++$i)
				{
					$class = $this->classes[$i];
					$item->$class = new $class;	//Hack to work around PHP's stupid implementation of get_class()
					$item->$class->populate($line);
				}
	
				$idx = $item->$idxClass->$field;
				$items[$idx][] = $item;
			}
			
			unset($result);	
		
			return $items;
		}
		catch(PDOException $e)
		{
			die("InnerJoin::groupedQuery() failed - ".$e->getMessage());
		}
	}
	
}

/**
 * Extracts the specified object from the combined join results,
 * returning an array of the specified objects n the same order
 * they were returned by the join query.
 *
 * @param unknown_type $class the class of objects to extract
 * @param unknown_type $results an array containing the results of a join query.
 * @return unknown an array of the specified objects extracted from the join query results array.
 */
function extractJoinResults($class, $results)
{
	$arr = array();
	
	if (!$results) return $arr;
	
	foreach($results as $result)
	{
		$arr[] = $result->$class;
	}
	
	return $arr;
}

function extractGroupedJoinResults($class, $results)
{
	$arr = array();
	
	if (!$results) return $arr;
	
	foreach(array_keys($results) as $key)
	{
		$arr[$key] = extractJoinResults($class, $results[$key]);
	}
	
	return $arr;
}
?>
