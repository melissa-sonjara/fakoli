<?php
require_once realpath(dirname(__FILE__))."/data_item.inc";
require_once realpath(dirname(__FILE__))."/join.inc";

class CompositeDataItem extends AbstractDataItem
{
	var $__objects;
	
	function CompositeDataItem()
	{
		$__objects = array();	
	}
	
	function add()
	{
		$args = func_get_args();
		
		foreach($args as $cl)
		{
			$obj = new $cl();
			$pk = $obj->getPrimaryKey();
			
			if (count($this->__objects))
			{
				$found = false;
				
				foreach($this->__objects as $o)
				{
					if ($o->hasField($pk))
					{
						$found = true;
						break;
					}
				}
				
				if (!$found)
				{
					throw new DataItemException("$cl does not have a direct foreign key relationship with another class in ".get_class($this));
				}
			}
			
			$this->__objects[] = $obj;
			$this->$cl = $obj;
		}
	}

	function cast($class)
	{
		if ($this->$class) return $this->$class;
		
		throw new DataItemException("Cannot cast from ".get_class($this)." to $class");
	}
	
	/**
	 * Populates the object using the supplied associative array (field -> value).
	 * Only values that match the definition of the sub-classed object will be copied.
	 * Fields that have been excluded by the use of an InclusionFilter or ExclusionFilter
	 * will also not be populated.
	 * 
	 * @param array $line the array of values to populate the object with. 
	 */
	function populate($line)
	{
		// Load all component objects from the outside in.
		
		$this->__objects[0]->populate($line);
		$num = count($this->__objects);
		
		for($i = 1; $i < num; ++$i)
		{
			$pk = $this->__objects[$i];
			$key = $obj->get($pk);
			$obj->load($key);
		}
	}
		
	/**
	 * Returned a comma-separated list of the fields for this object (applying the assigned
	 * filter if there is one).
	 */
	function getFieldList($alias = "")
	{
		$fields[] = array();
		
		// Create a composite field list from the inside out.
		$num = count($this->__objects);
		for($i = $num - 1; $i >= 0; ++$i)
		{
			$of = $this->__objects[$i]->fields;
			$fields = array_merge($fields, $of);			
		}
		
		return implode(", ", array_keys($fields));
	}
	
	/**
	 * Return an array of field names for this object filtered by any active filter.
	 * @return array
	 */
	function getFieldArray()
	{
		$fields[] = array();
		
		// Create a composite field list from the inside out.
		$num = count($this->__objects);
		for($i = $num - 1; $i >= 0; --$i)
		{
			$of = $this->__objects[$i]->fields;
			$fields = array_merge($fields, $of);			
		}
		
		return array_keys($fields);
	}
	
	/**
	 * Retrieve the field type list for this object. 
	 */
	function getFields()
	{
		$fields = array();
		// Create a composite field list from the inside out.
		$num = count($this->__objects);
		for($i = $num - 1; $i >= 0; --$i)
		{
			$of = $this->__objects[$i]->getFields();
			$fields = array_merge($fields, $of);			
		}
		
		return $fields;
	}

	/**
	 * Returns true if this DataItem contains a field with the specified name
	 * and that field is not excluded by a filter.
	 *
	 * @param string $field
	 * @return true if this field is present and not filtered, false otherwise.
	 */
	function hasField($field)
	{
		foreach($this->__objects as $obj)
		{
			if ($obj->hasField($field)) return true;
		}
		
		return false;
	}	
	
	/**
	 * Returns true if this DataItem contains a relation with the specified name.
	 *
	 * @param string $relation
	 * @return true if this relation is present and not filtered, false otherwise.
	 */
	function hasRelation($relation)
	{
		foreach($this->__objects as $obj)
		{
			if ($obj->hasRelation($relation)) return true;
		}
		
		return false;
	}

	/**
	 * Retrieves the primary key field name
	 */
	function getPrimaryKey()
	{
		// Primary key field for the composite object is the primary key field of the outermost object.
		
		return $this->__objects[0]->getPrimaryKey();
	}
	
	/**
	 * Retrieves a list of all the primary keys used for an object as an array
	 * @return array all the primary keys for this object's constituent parts
	 */
	function getPrimaryKeyList()
	{
		$pk = array();
		
		foreach($this->__objects as $obj)
		{
			$pk = array_merge($pk, $obj->getPrimaryKeyList());
		}
		
		return $pk;
	}	
	
	/**
	 * Retrieve the value for the specified field.
	 * @param string $field the field to retrieve
	 * @return mixed the value of that field
	 */
	function get($field)
	{
		foreach($this->__objects as $obj)
		{
			if ($obj->hasField($field)) return $obj->get($field);
		}
	
		return null;
	}
	
	/**
	 * Set the value of the specified field
	 * @param $field the field to set
	 * @param $value the value to which the field is to be set
	 */
	function set($field, $value)
	{
		foreach($this->__objects as $obj)
		{
			if ($obj->hasField($field)) $obj->set($field, $value);
		}
	}
	
	/**
	 * Retrieves the data type of the specified field
	 * @param $field the field in question
	 * @return string the data type of the specified field
	 */
	function getType($field)
	{
		foreach($this->__objects as $obj)
		{
			if ($obj->hasField($field)) return $obj->getType($field);
		}
		
		return null;
	}	
	
	/**
	 * Returns the filter set on this object. 
	 */
	function getFilter()
	{
		return $this->__objects[0]->getFilter();
	}

	/**
	 * Sets the filter on this object.
	 * @param $filter the filter
	 */
	function setFilter($filter)
	{
		foreach($this->__objects as $obj)
		{
			$obj->setFilter($filter);
		}
	}
		
	/**
	 * Retrieve the list of field aliases
	 * @return array the alias list for this object
	 */
	function getFieldAliases()
	{
		$aliases = array();
		
		// Create a composite field list from the inside out.
		$num = count($this->__objects);
		for($i = $num - 1; $i >= 0; --$i)
		{
			$of = $this->__objects[$i]->getFieldAliases();
			if ($of)
			{
				$aliases = array_merge($aliases, $of);
			}			
		}
		
		return $aliases;
	}
	
	/**
	 * Load the object with the specified primary key
	 * 
	 * @param int id the primary key value to instantiate from
	 */
	function load($id)
	{
		// Load from the outside in, chaining on the
		// primary key/foreign key relationship at each level
		
		$num = count($this->__objects);
		
		for($i = 0; $i < $num; ++$i)
		{
			$this->__objects[$i]->load($id);
			
			if ($i == $num - 1) continue;

			$id = $this->__objects[$i]->get($this->__objects[$i+1]->getPrimaryKey());
		}
	}
		
	function loadFromBase($id)
	{
		$num = count($this->__objects);
		
		$this->__objects[$num - 1]->load($id);
		
		for($i = $num - 2; $i >= 0; --$i)
		{
			trace("BOOGER!", 3);
			$pk = $this->__objects[$i+1]->getPrimaryKey();
			$cl = get_class($this->__objects[$i]);
			$obj = querySingle($cl, "WHERE $pk=".$this->__objects[$i+1]->get($pk));
			$this->__objects[$i] = $obj;
			$this->$cl = $obj;
		} 
	}
	
	/**
	 * Store the object in the database.
	 */
	function save()
	{
		// Save from the inside out, chaining on the
		// primary key/foreign key relationship at each level
		$num = count($this->__objects);
		
		for($i = $num - 1; $i >= 0; --$i)
		{
			if ($this->__objects[$i]->hasField("composite_class"))
			{
				$this->__objects[$i]->set("composite_class", get_class($this));
			}
			
			$this->__objects[$i]->save();
			if ($i != 0)
			{
				$pk = $this->__objects[$i]->getPrimaryKey();
				$this->__objects[$i - 1]->set($pk, $this->__objects[$i]->get($pk));
			}
		}
	}
	
	/**
	 * Select the object from the database, based on the value of the primary key field.
	 */
	function select()
	{
		// Select from the outside in, chaining on the
		// primary key/foreign key relationship at each level
		
		$this->__objects[0]->select();
		
		$num = count($this->__objects);
		
		for($i = 1; $i < $num; $i)
		{
			$pk = $this->__objects[$i]->getPrimaryKey();
			$this->__objects[$i]->set($pk, $this->__objects[$i - 1]->get($pk));
			$this->__objects[$i]->select();
		}
	}
	
	/**
	 * Check whether the object exists in the database.
	 */
	function exists($constraint = "")
	{
		return $this->__objects[0]->exists($constraint);
	}
	
	/**
	 * Update the row in the database that corresponds to this object.
	 */
	function update()
	{
		foreach($this->__objects as $obj)
		{
			$obj->update();
		}
	}
	
	/**
	 * Insert a new row in the database to store this object.
	 */
	function insert()
	{
		// Insert and chain primary/foreign keys, from the inside out
	
		$num = count($this->__objects);
		
		for($i = $num - 1; $i >= 0; --$i)
		{
			$this->__objects[$i]->insert();
			if ($i > 0)
			{
				$pk = $this->__objects[$i]->getPrimaryKey();
				$this->__objects[$i - 1]->set($pk, $this->__objects[$i]->get($pk));
			}
		}
	}
		
	/**
	 * Delete the row in the database that corresponds to this object.
	 */
	function delete($constraint = "")
	{
		if ($constraint)
		{
			throw new DataItemException("CompositeDataItem::delete() does not currently support constraint clauses");
		}
		
		$num = count($this->__objects);
		
		for($i = 0; $i < $num; ++$i)
		{
			$this->__objects[$i]->delete();
		}
	}
		
	/**
	 * Retrieves the distinct values in the database for the specified field 
	 * across the specified set of records.
	 * @param string $field
	 * @param boolean $sorted
	 * @param string $constraint
	 * @return array
	 */
	function distinctValues($field, $sorted = false, $constraint = "")
	{
		foreach($this->__objects as $obj)
		{
			if ($obj->hasField($field))
			{
				return $obj->distinctValues($field, $sorted, $constraint);
			}
		}
		
		throw new DataItemException("Could not find field '$field' in any component objects");
	}
	
	/**
	 * Automatically populate the object based on parameters in the $_GET collection.
	 */
	function fromGET()
	{
		foreach($this->__objects as $obj)
		{
			$obj->fromGET();
		}
	}	
	
	/**
	 * Automatically populate the object based on parameters in the $_POST collection.
	 * If the key does not exist in $_POST and the type is Boolean, that means
	 * that the user unchecked a checkbox and we need to assign 0 to the
	 * field.
	 */
	function fromPOST()
	{
		foreach($this->__objects as $obj)
		{
			$obj->fromPOST();
		}
	}	
		
	/**
	 * Automatically populate the object based on parameters in either the $_GET or $_POST collection,
	 * depending on the method used to access the page.
	 */
	function fromREQUEST()
	{
		foreach($this->__objects as $obj)
		{
			$obj->fromREQUEST();
		}
	}	
	
	/**
	 * Compare this object to another object. If all the fields match,
	 * the method returns true, otherwise false. If a filter is in place on
	 * the source object, then only the filtered fields are compared.
	 * 
	 * @param object $to the object with which to compare this object
	 */
	function compare($to)
	{
		if (get_class($to) != get_class($this)) return false;
		$num = count($this->__objects);
		
		for($i = 0; $i < $num; ++$i)
		{
			if (!$this->__objects[$i]->compare($to->__objects[$i])) return false;
		}
		
		return true;
	}
		
	/**
	 * Copies values from another object, field by field. If a filter
	 * is in place on the source object, then only the filtered fields are copied.
	 * 
	 * @param object from the object from which data is to be copied
	 */
	function copy($from)
	{
		if (get_class($to) != get_class($this)) return false;
		
		$num = count($this->__objects);
		
		for($i = 0; $i < $num; ++$i)
		{
			$this->__objects[$i]->copy($from->__objects[$i]);
		}
	}	
			
	/**
	 * Generates an XML representation of the object. Filters are honored when determining
	 * which fields are included in the XML. Empty fields are not output (note that a string
	 * field with value "" is not considered empty).
	 */
	function toXML($indent = 0, $path = null)
	{
		$xml = str_repeat(" ", $indent) . "<" . get_class($this) . ">\n";
		foreach ($this as $field => $val)
		{
			if (is_object($val))
			{
				$xml .= $val->toXML($indent + 1);
			}
		}
		
		$xml .=  str_repeat(" ", $indent) . "</" . get_class($this) . ">\n";
		
		return $xml;
	}

	
	function format($template = "", $separator = ",")
	{
		$matches = array();
		
		preg_match_all("/\\{([\\w_]+)\.([^}]+)}/", $template, $matches, PREG_SET_ORDER);
		
		foreach($matches as $match)
		{
			$class = $match[1];
			$subtemplate = $match[2];
	
			$value = $this->$class->format("{".$subtemplate."}", $separator);
			
			$template = str_replace($match[0], $value, $template);
		}
		
		return $template;
	}
		
	
	function prettifyFieldName($field)
	{
		foreach($this->__objects as $obj)
		{
			if ($obj->hasField($field)) return $obj->prettifyFieldName($field);
		}
		
		return $field;
	}
	
		
	function prettifyClassName($plural = false)
	{
		$c = ($this->pretty_class_name) ? $this->pretty_class_name : get_class($this);
		$c = preg_replace(array("/([a-z])([A-Z0-9])/",
								"/_/"),
						  array("$1 $2",
						  		" "),
						  $c);
		$c = ucwords($c);
		
		if ($plural)
		{
			pluralize($c);
		}
		
		return $c;
	}

	/**
	 * Link this object to the specified target by setting corresponding field
	 * to the value of the target's primary key.
	 *
	 * @param DataItem $target the DataItem to which the relationship will be created
	 * @param string $field the field to set for the relationship. If not specified,
	 * the primary key of the target object is used for the foreign key field name.
	 */
	function relateTo($target, $field = "")
	{
		if ($field == "") $field = $target->getPrimaryKey();
		
		foreach($this->__objects as $obj)
		{
			if ($obj->hasField($field))
			{
				$obj->relateTo($target, $field);
				return;
			}
		}
	}
	
		
	function query($constraints = "", $page = -1, $size = -1)
	{
		$join = new InnerJoin();
		foreach($this->__objects as $obj)
		{
			$join->add(get_class($obj));
		}
		
		$joinResults = $join->query($constraints, $page, $size);
		
		$results = array();
		
		foreach($joinResults as $result)
		{
			$obj = clone($this);
			foreach($obj->__objects as $target)
			{
				$cl = get_class($target);
				$target->copy($result->$cl);
			}
			
			$results[] = $obj;
		}
		
		return $results;
	}
	
		
	/** Performs a query against the database, returning an array of CompositeDataItem objects of the specified class,
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
	
	/** Performs a query against the database, returning an array of arrays of CompositeDataItem objects of the specified class,
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

?>