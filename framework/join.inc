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
 * JoinResult is an empty placeholder class. Instances of 
 * JoinResult are used to aggregate the results of the joined 
 * query. Each table that was joined is added as a sub-object of
 * the JoinResult.
 */
class JoinResult
{
	var $_xmlTagName = "join";
	var $_classes;
	
	function JoinResult($tag = "join")
	{
		$this->_xmlTagName = $tag;
		$this->_classes = array();
	}
	
	/**
	 * Serializes the JoinResult to XML.
	 * 
	 * @param integer $indent the initial indentation level for the output.
	 */
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
	
	/**
	 * Formats the contents of the JoinResult using the supplied template.
	 * The DataItems contained in the JoinResult can be accessed via the
	 * usual relation traversal syntax {ClassName.field}. This method allows
	 * JoinResult objects to be used in DataListViews and other display controls.
	 * 
	 * @param $template the template for the formatted output
	 */
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

		$matches = array();
		
		preg_match_all("/\\{([\\w\\d_]+)::([^}]*)}/", $template, $matches, PREG_SET_ORDER);

		foreach($matches as $match)
		{
			$sub = call_user_func(array($match[1], $match[2]), $this);
			$template = str_replace($match[0], $sub, $template);
		}
		
		return $template;
	}
}

class JoinResultIterator implements Iterator, Countable
{
	var $join;
	var $constraints;
	var $query;
	var $result;
	var $current = null;
	var $position = -1;
	var $item;
	
	public function __construct($join, $constraints)
	{
		$this->join = $join;
		$this->constraints = $constraints;
		$this->item = new JoinResult();
		for($i = 0; $i < count($join->classes); ++$i)
		{
			$class = $join->classes[$i];
			$this->item->$class = new $class;	
		}

		$this->query = $join->generateQuery($constraints);
	}
	
	public function count()
	{
		$this->rewind();
		trace("Number of Rows: {$this->rowCount}", 4);
		
		return $this->rowCount;
	}
	
	public function rewind()
	{
		if ($this->position == 0) return;
		
		$this->current = null;
		$this->position = 0;
		
		if (!$this->query)
		{
			$this->query = $this->join->generateQuery($this->constraints);		
		}
		
		try
		{
			$db = ConnectionManager::getConnection();
			$this->result = $db->prepare($this->query);
			$this->result->execute();
			$this->rowCount = $this->result->rowCount();
			$this->current = $this->result->fetch();
		}
		catch(PDOException $e)
		{
			throw new FakoliException($e->getMessage());
		}
	}
	
	public function current()
	{
		for($i = 0; $i < count($this->join->classes); ++$i)
		{
			$class = $this->join->classes[$i];
			$this->item->$class->populate($this->current, $this->join->aliases[$this->item->$class->table]);
		}

		foreach($this->join->pseudos as $class => $lookup)
		{
			foreach($this->join->aliases as $table => $prefix)
			{
				$keyAlias = "{$prefix}.{$lookup['index']}";
				
				$key = $this->current[$keyAlias];
				
				if (array_key_exists($key, $lookup['items']))
				{
					$this->item->$class = $lookup['items'][$key];
					break;
				}
			}
		}
		
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
 * AbstractJoin is the abstract base class from which concrete join classes
 * can be derived in order to provide support for different styles of relational
 * join.
 * 
 * @author andy
 *
 */
abstract class AbstractJoin
{
	var $classes = array();			///< The names of the DataItem classes being joined
	var $protos = array();			///< Prototype instances of the DataItem classes being joined
	var $xrefs = array();			///< Cross-reference classes (i.e. classes that are part of the join, but not the result)

	var $xrefFieldMaps = array();	///< Cross-reference field map (to support cross-referencing through fields with differing names)
	
	var $aliases = array();			///< Field aliases for output
	var $xrefAliases = array();     ///< Cross-reference table aliases
	var $constraints = array();		///< Class-specific constraints add via the constrain() method
	
	var $unique = false;			///< true to output distinct rows only
	var $tag = "join";				///< XML tag name

	var $excludedKeys = array();
	var $pseudos = array();			///< Pseudo-join lookup tables
	
	/**
	 * Adds a class to the join definition. Classes added using this method
	 * are included in the eventual result set.
	 * @param string $class the name of the class to add to the join
	 * @param Filter $filter (optional) InclusionFilter or ExclusionFilter object defining the sub-set of fields to include
	 */
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
		
		return $this;
	}
	
	/**
	 * Sets an InclusionFilter or ExclusionFilter on the specified class.
	 * @param $class the class to be filtered
	 * @param $filter InclusionFilter or ExclusionFilter object defining the sub-set of fields to include
	 */
	function setFilter($class, $filter)
	{
		$this->protos[$class]->filter = $filter;
		return $this;
	}

	/**
	 * Sets a query constraint on the given class
	 * @param string $class the class to be constrained
	 * @param string $constraint the SQL constraint to be applied
	 */
	function constrain($class, $constraint)
	{
		$this->constraints[$class] = $constraint;
	}
	
	
	/**
	 * By default joins are generated following the "most restrictive search" principle.
	 * This means that all matching foreign key/primary key pairs are used in the join constraint.
	 * You can use this method to override this behavior for a member of the join, specifying
	 * that a given key should not be used as part of the join constraint.
	 *  
	 * @param string $class
	 * @param string $key
	 */
	function excludeKeyFromJoin($class, $key)
	{
		if (!isset($this->excludedKeys[$class]))
		{
			$this->excludedKeys[$class] = array();
		}
		
		$this->excludedKeys[$class][$key] = true;
		
		trace("*** Excluding $key on $class", 3);
		return $this;
	}
	
	/**
	 * Adds a cross-reference class to the join definition. Classes added using this 
	 * method are NOT included in the eventual result set.
	 * @param $xref
	 * @param $xrefFieldMaps
	 */
	function xref($xref, $xrefFieldMaps = null)
	{
		$x = new $xref;
		
		$this->xrefs[] = $x;
		
		if ($xrefFieldMaps)
		{
			$this->xrefFieldMaps[$x->table] = $xrefFieldMaps;
		}
	}
	
	function pseudo($class, $items, $indexField)
	{
		$this->pseudos[$class] = array("index" => $indexField, "items" => $items);
	}
	
	abstract function generateQuery($constraint = "", $count = false);

	function count($constraints = "", $countExpr = true)
	{
		$query = $this->generateQuery($constraints, $countExpr);
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
			throw FakoliException($e->getMessage());
		}
		
		return $value;		
	}

	/**
	 * Performs a query against the database returning an array of JoinResult objects.
	 * @param string $constraints
	 * @param integer $page
	 * @param integer $size
	 */
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
				$item = new JoinResult($this->tag);
				
				for($i = 0; $i < count($this->classes); ++$i)
				{
					$class = $this->classes[$i];
					$item->$class = new $class;	
					$item->$class->populate($line, $this->aliases[$item->$class->table]);
				
					foreach($this->pseudos as $class => $lookup)
					{
						foreach($this->aliases as $table => $prefix)
						{
							$keyAlias = "{$prefix}.{$lookup['index']}";
					
							$key = $line[$keyAlias];
							
							if (array_key_exists($key, $lookup['items']))
							{
								$item->$class = $lookup['items'][$key];
								break;
							}
						}
					}
					
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
			throw new FakoliException($e->getMessage());
		}
	}
	
	/**
	 * Performs a query against the database returning an iterator of JoinResult objects.
	 * @param string $constraints
	 */
	function iteratedQuery($constraints = "")
	{
		return new JoinResultIterator($this, $constraints);
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
				$item = new JoinResult($this->tag);
				
				for($i = 0; $i < count($this->classes); ++$i)
				{
					$class = $this->classes[$i];
					$item->$class = new $class;	
					$item->$class->populate($line, $this->aliases[$item->$class->table]);

					foreach($this->pseudos as $class => $lookup)
					{
						foreach($this->aliases as $table => $prefix)
						{
							$keyAlias = "{$prefix}.{$lookup['index']}";
					
							$key = $line[$keyAlias];
					
							if (array_key_exists($key, $lookup['items']))
							{
								$item->$class = $lookup['items'][$key];
								break;
							}
						}
					}
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
			throw new FakoliException($e->getMessage());
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
		
		trace("groupedQuery(): Group by $idxClass $field", 3);
			
		$query = $this->generateQuery($constraints);		
			
		try
		{
			$db = ConnectionManager::getConnection();
			$result = $db->prepare($query);
			$result->execute();
						
			$items = array();
			
			while($line = $result->fetch())
			{
				$item = new JoinResult($this->tag);
				
				for($i = 0; $i < count($this->classes); ++$i)
				{
					$class = $this->classes[$i];
					$item->$class = new $class;	
					$item->$class->populate($line, $this->aliases[$item->$class->table]);

					foreach($this->pseudos as $class => $lookup)
					{
						foreach($this->aliases as $table => $prefix)
						{
							$keyAlias = "{$prefix}.{$lookup['index']}";
					
							$key = $line[$keyAlias];
					
							if (array_key_exists($key, $lookup['items']))
							{
								$item->$class = $lookup['items'][$key];
								break;
							}
						}
					}
				}
	
				$idx = $item->$idxClass->$field;
				$items[$idx][] = $item;
			}
			
			unset($result);	
		
			return $items;
		}
		catch(PDOException $e)
		{
			throw new FakoliException($e->getMessage());
		}
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
 * Results are returned as an array of JoinResult composite objects.
 * The matching row from each object in the join as added as a sub-object
 * of the JoinResult with the field name being the class name of the 
 * object.
 * 
 * 
 */
class InnerJoin extends AbstractJoin
{
	
	/**
	 * Create a new InnerJoin object.
	 */
	function InnerJoin($tag = "join")
	{
		$this->tag = $tag;
	}
	
	function generateQuery($constraints = "", $countOnly = false)
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
			
			if (array_key_exists($class, $this->constraints))
			{
				$c = $this->constraints[$class];
				
				foreach($proto->getFields() as $field => $type)
				{
					$c = preg_replace("/([\s\(])(".$field.")\\b/", "$1t$i.$2", $c);
				}

				$c = "(".preg_replace("/^\\s*(WHERE\\s+|AND\\s+|OR\\s+)/i", "", $c).")";
				
				$this->constraints[$class] = $c;			
			}
			
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
		
		$classConstraints = implode(" AND ", array_values($this->constraints));

		if (strncmp($constraints, "WHERE", 5))
		{
			if ($classConstraints) $classConstraints = " AND ".$classConstraints;
			$constraints = "WHERE 1=1 $classConstraints $constraints";
		}
		else
		{
			if ($classConstraints) $classConstraints = $classConstraints." AND ";
			$constraints = preg_replace("/^\\s*WHERE/i", "WHERE $classConstraints", $constraints);
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
				
				if ($this->excludedKeys[$d][$q->primary_key] ||
				    $this->excludedKeys[$d][$p->primary_key])
				{
					continue;
				}
				
				trace($p->primary_key." ".array_key_exists($p->primary_key, $q->getFields()), 5);
				trace($q->primary_key." ".array_key_exists($q->primary_key, $p->getFields()), 5);
				
				if (array_key_exists($p->primary_key, $q->getFields()))
				{
					$join .= " AND t$i.{$p->primary_key}=t$j.{$p->primary_key}";
				}
				else if (array_key_exists($q->primary_key, $p->getFields()))
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
			foreach($p->getFields() as $field => $type)
			{
				$constraints = preg_replace("/([\s\(])(".$field.")\\b/", "$1t$i.$2", $constraints);
			}
			
			foreach($p->calculatedFields as $field => $expr)
			{
				foreach($p->getFields() as $f => $type)
				{
					$expr = preg_replace("/([\s\(])(".$f.")\\b/", "$1t$i.$2", $expr);
				}
			
				$constraints = preg_replace("/([\s\(])(".$field.")\\b/", " $expr", $constraints);
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
		else if (preg_match("/^(.*?)(LIMIT.*)/i", $constraints, $matches))
		{
			$constraints = $matches[1];
			$orderBy = $matches[2];
		}
		
		if ($countOnly) $fields = (is_bool($countOnly)) ? "COUNT(1) as result" : "COUNT($countOnly) as result";
				
		$query = "SELECT $fields FROM $tables $constraints $join $orderBy";
		
		trace("InnerJoin::generateQuery() - $query", 3);
		
		return $query;
	}
}





class LeftOuterJoin extends AbstractJoin
{
	
	/**
	 * Create a new LeftOuterJoin object.
	 */
	function LeftOuterJoin($tag = "join")
	{
		$this->tag = $tag;
	}
	
	function generateQuery($constraints = "", $countOnly = false)
	{
		$tables = "";
		$fields = "";
		$alias  = "";
		
		trace(print_r($this->constraints, true), 3);
		
		for($i = 0; $i < count($this->classes); ++$i)
		{
			$class =  $this->classes[$i];
			$proto =& $this->protos[$class];
			 
			$alias = "t$i";
			$this->aliases[$proto->table] = $alias;
			
			if ($tables) $tables .= ", ";
			$tables .= "{$proto->table} $alias";
			
			if (array_key_exists($class, $this->constraints))
			{
				$c = $this->constraints[$class];
				
				foreach($proto->getFields() as $field => $type)
				{
					$c = preg_replace("/([\s\(])(".$field.")\\b/", "$1t$i.$2", $c);
				}

				$c = "(".preg_replace("/^\\s*(WHERE\\s+|AND\\s+|OR\\s+)/i", "", $c).")";
				
				$this->constraints[$class] = $c;			
			}

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
		
		$classConstraints = implode(" AND ", array_values($this->constraints));

		if (strncmp($constraints, "WHERE", 5))
		{
			if ($classConstraints) $classConstraints = " AND ".$classConstraints;
			$constraints = "WHERE 1=1 $classConstraints $constraints";
		}
		else
		{
			if ($classConstraints) $classConstraints = $classConstraints." AND ";
			$constraints = preg_replace("/^\\s*WHERE/i", "WHERE $classConstraints", $constraints);
		}
				
		$join = "";
		$join = $this->protos[$this->classes[0]]->table . " t0";
		
		// Build Left Outer Join
		
		$joined = array();
			
		for($i = 0; $i < count($this->classes) - 1; ++$i)
		{
			$c = $this->classes[$i];
			$p = $this->protos[$c];
			
			
			for ($j = $i+1; $j < count($this->classes); ++$j)
			{
				$d = $this->classes[$j];
				$q = $this->protos[$d];
				

				
				trace($p->primary_key." ".array_key_exists($p->primary_key, $q->getFields()), 5);
				trace($q->primary_key." ".array_key_exists($q->primary_key, $p->getFields()), 5);
				
				if (array_key_exists($p->primary_key, $q->getFields()))
				{
					if ($this->excludedKeys[$d][$p->primary_key] ||
				    	$this->excludedKeys[$c][$p->primary_key])
					{
						continue;
					}
					
					//$join .= " AND t$i.{$p->primary_key}=t$j.{$p->primary_key}";
					$idx = "{$j}-{$p->primary_key}";
					if ($joined[$idx])
					{
						continue;
					}
					
					if ($joined[$j])
					{
						$join .= " AND t$i.{$p->primary_key}=t$j.{$p->primary_key}";
					}
					else
					{
						$join .= " LEFT OUTER JOIN {$q->table} t$j ON t$i.{$p->primary_key}=t$j.{$p->primary_key}";
					}
					
					$joined[$j] = true;
					$joined[$idx] = true;
					
				}
				else if (array_key_exists($q->primary_key, $p->getFields()))
				{
					if ($this->excludedKeys[$d][$q->primary_key] ||
				    	$this->excludedKeys[$c][$q->primary_key])
					{
						continue;
					}
					
					$idx = "{$j}-{$q->primary_key}";
					if ($joined[$idx])
					{
						continue;
					}

					//$join .= " AND t$i.{$q->primary_key}=t$j.{$q->primary_key}";
					if ($joined[$j])
					{
						$join .= " AND t$i.{$q->primary_key}=t$j.{$q->primary_key}";
					}
					else
					{
						$join .= " LEFT OUTER JOIN {$q->table} t$j ON t$i.{$q->primary_key}=t$j.{$q->primary_key}";
					}
					
					$joined[$j] = true;
					$joined[$idx] = true;
				}
				
			}
			
		}
		
		// Alias main table fields
		
		for($i = 0; $i < count($this->classes); ++$i)
		{
			$c = $this->classes[$i];
			$p = $this->protos[$c];
			foreach($p->getFields() as $field => $type)
			{
				$constraints = preg_replace("/([\s\(])(".$field.")\\b/", "$1t$i.$2", $constraints);
			}
										
			foreach($p->calculatedFields as $field => $expr)
			{
				foreach($p->getFields() as $f => $type)
				{
					$expr = preg_replace("/([\s\(])(".$f.")\\b/", "$1t$i.$2", $expr);
				}
			
				$constraints = preg_replace("/([\s\(])(".$field.")\\b/", " $expr", $constraints);
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
					
					$join .= " LEFT OUTER JOIN {$xref->table} $xa ON $xa.{$xk}=$ta.{$pk}";
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
		else if (preg_match("/^(.*?)(LIMIT.*)/i", $constraints, $matches))
		{
			$constraints = $matches[1];
			$orderBy = $matches[2];
		}
		
		if ($countOnly) $fields = (is_bool($countOnly)) ? "COUNT(1) as result" : "COUNT($countOnly) as result";
		
		$query = "SELECT $fields FROM $join $constraints $orderBy";
		
		trace("LeftOuterJoin::generateQuery() - $query", 3);
		
		return $query;
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
