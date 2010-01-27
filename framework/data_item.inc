<?php
/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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
 */

require_once realpath(dirname(__FILE__)."/connection_manager.inc");

/*define("String", 1);	
define("Number", 2);
define("Date", 3);
define("HTML", 4);
define("Currency",  5);
define("Boolean", 6);
define("Password", 7);
define("Timestamp", 8);
define("Text", 9);*/

/**
 * Used to place a filter on the contents of a DataItem-derived object.
 * With an ExclusionFilter, the specified fields will be <b>excluded</b> from
 * any subsequent queries or operations.
 * 
 * 
 */
class ExclusionFilter
{
	var $fields = array();
	var $type = "ExclusionFilter";
	
	function ExclusionFilter()
	{
		$args = func_get_args();
		for($i = 0; $i < count($args); ++$i)
		{
			$this->fields[$args[$i]] = true;
		}
	}
	
	function isExcluded($field)
	{
		return (array_key_exists($field, $this->fields));
	}	
	
	function add()
	{
		$args = func_get_args();
		for($i = 0; $i < count($args); ++$i)
		{
			$this->fields[$args[$i]] = true;
		}
	}
		
	function remove()
	{
		$args = func_get_args();
		for($i = 0; $i < count($args); ++$i)
		{
			unset($this->fields[$args[$i]]);
		}
	}
	
	function includeField($field)
	{
		$this->remove($field);
	}
	
	function excludeField($field)
	{
		$this->add($field);
	}
}

/**
 * Used to place a filter on the contents of a DataItem-derived object.
 * With an InclusionFilter, only the specified fields (and the primary key)
 * will be <b>included</b> in any subsequent queries or operations.
 */
class InclusionFilter
{
	var $fields = array();
	var $type = "InclusionFilter";
	
	function InclusionFilter()
	{
		$args = func_get_args();
		for($i = 0; $i < count($args); ++$i)
		{
			$this->fields[$args[$i]] = true;
		}
	}
	
	function isExcluded($field)
	{
		return !(array_key_exists($field, $this->fields));
	}	

	function add()
	{
		$args = func_get_args();
		for($i = 0; $i < count($args); ++$i)
		{
			$this->fields[$args[$i]] = true;
		}
	}
	
	function remove()
	{
		$args = func_get_args();
		for($i = 0; $i < count($args); ++$i)
		{
			unset($this->fields[$args[$i]]);
		}
	}

	function includeField($field)
	{
		$this->add($field);
	}
	
	function excludeField($field)
	{
		$this->remove($field);
	}
}

define("SerializeRelations", 1);
define("SerializeDirectRelations", 2);
define("ProtectHTML", 4);

/**
 * DataItem is the generic base class for database mapped classes. It provides a
 * compact, consistent API for object persistence in the database.
 * 
 * 
 */
class DataItem
{
	var $filter;
	var $_options = 0;
	
	/**
	 * Constructor. DataItems can be constructed in three ways:<br>
	 * <ol>
	 *  <li>Empty objects can be constructed by calling the constructor with no arguments</li>
	 *  <li>Objects can be instantiated from their primary key by passing in a single value</li>
	 *  <li>Objects can be populated from an associative array of field/value pairs [such
	 *      as those returned from mysql_fetch_assoc()]</li>
	 * </ol>
	 */
	function DataItem()
	{
		if (func_num_args() > 0)
		{
			$arg = func_get_arg(0);
			
			if (is_array($arg) )
			{
				if (count($arg) > 1)
				{
					$this->populate($arg);
				}
				else if (count($arg) > 0)
				{
					if (is_array($arg[0]))
					{
						$this->populate($arg[0]);
					}
					else
					{
						$this->load($arg[0]);
					}
				}
			}
			else
			{
				if (func_num_args() > 1)
				{
					$this->filter = func_get_arg(1);
				}
				
				$this->load($arg);
			}
		}
	}

	/**
	 * Set a behavior option flag
	 * @param integer $opt the option to set 
	 */
	function setOption($opt)
	{
		$this->_options |= $opt;
	}
	
	/**
	 * Clear the specified behavior option flag
	 * @param integer $opt the option to clear
	 */
	function clearOptions($opt)
	{
		$this->_options |= $opt;
	}
	
	/**
	 * Retrieves a connection to the database.
	 *
	 * @return a connection to the database
	 */
	function getConnection()
	{
		//TODO: Add transactional support
		return ConnectionManager::getConnection();
	}
	
	/**
	 * Subclasses can override this function to provide additional 
	 * identity constraints to be used when querying or updating. This allows for
	 * easy and safe siloing of data between application users based on session variables.
	 */
	function getIdentityConstraint()
	{
		return "";
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
		foreach ($line as $field => $value)
        {
			//TODO: Figure out why this line causes login to fail for PSL.
        	if (!array_key_exists($field, $this->fields)) continue;
        	if ($this->filter && $this->filter->isExcluded($field)) continue;
        	
            $this->$field = iconv("UTF-8", "CP1252//IGNORE", $value); 
        }
	}
		
	/**
	 * Returned a comma-separated list of the fields for this object (applying the assigned
	 * filter if there is one).
	 */
	function getFieldList($alias = "")
	{
		$list = "";
		$first = true;
		
		foreach($this->fields as $field => $type)
		{
			if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (!$first) $list .= ", ";
			if ($alias)
			{
				$list .= "$alias.";
			}
			$list .= $field;
			$first = false;
		}
		
		return $list;
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
		if (!array_key_exists($field, $this->fields)) return false;
		if ($this->filter && $this->filter->isExcluded($field)) return false;
		return true;
	}
			
	/**
	 * Load the object with the specified primary key
	 * 
	 * @param int id the primary key value to instantiate from
	 */
	function load($id)
	{
		$fields = $this->getfieldList();
		$query = "SELECT $fields FROM {$this->table} WHERE {$this->primary_key}=$id ".$this->getIdentityConstraint();
		trace("DataItem::load($id): $query", 3);
		
		$db = $this->getConnection();
		
		try
		{
			$result = $db->prepare($query);
			$result->execute();
			
			if ($line = $result->fetch())
			{
				$this->populate($line);
			}
			
			unset($result);
		}
		catch(PDOException $e)
		{
			die("DataItem::load() failed - ".$e->getMessage());
		}
	}
		
	/**
	 * Store the object in the database.
	 */
	function save()
	{
		if ($this->exists())
    	{
    		return $this->update();
    	}
    	else
    	{
    		return $this->insert();
    	}
	}
		
	/**
	 * Select the object from the database, based on the value of the primary key field.
	 */
	function select()
	{
		$pk = $this->primary_key;
		$this->load($this->$pk);
	}
	
	/**
	 * Check whether the object exists in the database.
	 */
	function exists($constraint = "")
	{
		
		$pk = $this->primary_key;
		
		if (!$constraint)
		{
			if ($this->$pk == "" || $this->$pk == 0) return false;
			$constraint = "WHERE $pk={$this->$pk}";
		}
		
		$query = "SELECT $pk FROM {$this->table} $constraint";
		trace("DataItem::exists() - $query", 3);
		
		$db = $this->getConnection();
		$exists = false;
		
		try
		{
			$result = $db->prepare($query);
			$result->execute();
			
			if ($line = $result->fetch())
			{
				$exists = true;
			}
			
			unset($result);
		}
		catch(PDOException $e)
		{
			die("DataItem::exists() failed - " . $e->getMessage());
		}
		
		return $exists;
	}
		
	/**
	 * Update the row in the database that corresponds to this object.
	 */
	function update()
	{
		$pk = $this->primary_key;

		$query = "UPDATE {$this->table} SET ";
    		
		$first = true;
		$found = false;
		
		foreach($this->fields as $field => $type)
		{			
			if ($field == $this->primary_key) continue;
        	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (!isset($this->$field) && $this->fields[$field] != Timestamp && $this->fields[$field] != Boolean) continue;
			
			if (!$first) $query .= ", ";
			
			$first = false;
			$found = true;
			
			$query .= "$field=";
			$query .= $this->quoteFieldValue($field, $type);
    	}

    	if (!$found) return true; // No fields to update - bug out quietly
    	
		$query .= " WHERE $pk={$this->$pk} ".$this->getIdentityConstraint();
		
		trace("DataItem::update() - $query", 3);
		
		try
		{
			$db = $this->getConnection();
			
			$success = $db->exec($query);
			if ($success === FALSE)
			{
				trace("DataItem::update failed - $query", 2);
			}
		}
		catch(PDOException $e)
		{
			die("DataItem::update() failed - " . $e->getMessage());		
		}
		
		return $success;
	}

	/**
	 * Returns the properly quoted value of the specified field. Quoting rules
	 * are dependent on the specified type of the field.
	 * 
	 * @param string $field the field to return
	 * @param string $type the type of the field.
	 */
	function quoteFieldValue($field, $type)
	{
		$query = "";

		$db = $this->getConnection();
		
		switch($type)
		{
		case Number:
		case Currency:
		
			if ($this->$field == "" || !is_numeric($this->$field))
			{
				$query .= "0";
			}
			else
			{
				$query .= $this->$field;
			}
			break;
	
		case Date:
		
			$query .= $db->quote($this->reformatToSQLDate($this->$field));
			break;
			
		case Timestamp:
			
			$query .= $db->quote(date("Y-m-d H:i:s"));
			break;
			
		case Boolean:
			
			$query .= ($this->$field) ? "1" : "0";
			break;
			
		case String:
		case Text:
			
		default:
		
			$query .= $db->quote($this->$field);
			break;
		}		

		return $query;			
	}
	
	/**
	 * Format field for XML output
	 */
	function formatFieldForXML($field)
	{
		$type = $this->fields[$field];
		
		$v = "";
		switch($type)
		{
		case HTML:
			
			if ($this->_options & ProtectHTML)
			{
				$v = str_replace( array("<", ">", "&"), array("[_[OPENTAG]_]", "[_[CLOSETAG]_]", "[_[AMPERSAND]_]"), $this->$field);				
				break;
			}
			
		case Number:
		case Date:
		case Timestamp:
		case String:
		case Text:
		default:
		
			$v = htmlspecialchars($this->$field);
			break;
		}
		
		return $v;	
			
	}
	
	/**
	 * Reformats the specified date to be in a format used by the database.
	 * 
	 * @param string $date the date to reformat.
	 */
	function reformatToSQLDate($date)
	{
		trace($date, 3);
		$fields = array();
		if (preg_match("|^(\\d+)[\-/](\\d+)[\-/](\\d{4})$|", $date, $fields))
		{
			$date = $fields[3]."-".$fields[1]."-".$fields[2];
		}
		else if (preg_match("|^(\\d+)/(\\d+)/(\\d{4})\\s+(\\d+:\\d+:\\d+)|", $date, $fields))
		{
			$date = $fields[3]."-".$fields[1]."-".$fields[2]." ".$fields[4];
		}

		return $date;
	}
	
	function reformatFromSQLDate($date)
	{
		$fields = array();
		if (preg_match("|^(\\d{4})-(\\d\\d)-(\\d\\d)$|", $date, $fields))
		{
			$date = $fields[2]."/".$fields[3]."/".$fields[1];
		}
		else if (preg_match("|(\\d{4})-(\\d\\d)-(\\d\\d)\\s+(\\d+:\\d+:\\d+)|", $date, $fields))
		{
			$date = $fields[2]."/".$fields[3]."/".$fields[1]." ".$fields[4];
		}
		
		if ($date == "00/00/0000") $date = "";
		
		return $date;
	}
	
	/**
	 * Insert a new row in the database to store this object.
	 */
	function insert()
	{
		$pk = $this->primary_key;
		
		$first = true;
		
		
		foreach($this->fields as $field => $type)
		{
			if ($field == $pk) continue;
        	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (!isset($this->$field) && $this->fields[$field] != Timestamp && $this->fields[$field] != Boolean) continue;
			
			if (!$first)
			{
				$fields .= ", ";
				$values .= ", ";
			}
			
			$fields .= $field;
			$values .= $this->quoteFieldValue($field, $type);

			$first = false;
		}
					
		$query = "INSERT INTO {$this->table} ($fields) values ($values)";
		
		trace("DataItem::insert() - $query", 3);
		
		$success = false;
		
		try
		{
			$db = $this->getConnection();
			$success = $db->exec($query);
			
			if ($success !== FALSE)
			{
				$this->$pk = $db->lastInsertId();
			}
			else
			{
				$this->$pk = 0;
	 			trace("DataItem::insert() failed - $query", 2);
	 		}
		}	
		catch(PDOException $e)
		{
			die("DataItem::insert() failed - ". $e->getMessage());		
		}
		return $success;						
	}

	/**
	 * Delete the row in the database that corresponds to this object.
	 */
	function delete($constraint = "")
	{
		if ($constraint == "")
		{
			$pk = $this->primary_key;
			$constraint =  "WHERE $pk={$this->$pk} ".$this->getIdentityConstraint();
		}
		
		$query = "DELETE FROM {$this->table} $constraint";
		
		trace($query, 3);
		
		$db = $this->getConnection();
		
		try
		{
			$db->exec($query);
		}
		catch(PDOException $e)
		{
			die ("DataItem::delete() failed - " . $e->getMessage());
		}
	}
		
	/**
	 * Automatically populate the object based on parameters in the $_GET collection.
	 */
	function fromGET()
	{
		foreach($this->fields as $field => $type)
		{
        	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (array_key_exists($field, $_GET))
			{
				$this->$field = $_GET[$field];
			}
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
		foreach($this->fields as $field => $type)
		{
       	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (array_key_exists($field, $_POST))
			{
				$this->$field = $_POST[$field];
			}
			elseif (!array_key_exists($field, $_POST) AND ($type == Boolean))
			{
				$this->$field = 0;
			}
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
		foreach($this->fields as $field => $type)
		{
			if ($this->filter && $this->filter->isExcluded($field)) continue;
			if ($to->$field != $this->$field)
			{
				trace("$field '{$this->$field}' != '{$to->$field}'", 3);
				return false;
			}
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
		foreach($this->fields as $field => $type)
		{
			if ($this->filter && $this->filter->isExcluded($field)) continue;
			$this->$field = $from->$field;
		}
	}
	
	/**
	 * Returns a single item related by the specified foreign key. The foreign
	 * key name must match the primary key name for the specified class. 
	 * 
	 * Use this method to implement one-to-one relations. 
	 * 
	 * @param string $class the name of the related class
	 * @param string $field the name of the foreign key defining the relation.
	 */
	function getRelated($class, $field = "")
	{
		$obj = new $class;
		if ($field == "") 
		{
			$field = $obj->primary_key;
			
		}
		
		if ($this->hasField($field))
		{
			$pk = $obj->primary_key;
			$obj->$pk = $this->$field;
			$obj->select();
		}
		else
		{
			$field = $this->primary_key;
			$obj = querySingle($class, "WHERE $field={$this->$field}");		
		}
		return $obj;
	}
	
	/**
	 * Returns a list of items that are related to this item. Usually the
	 * items will be related by having a foreign key field that matches the
	 * primary key on the calling object. However, another field on the calling
	 * object can be specified if required. You can also optionally specify a
	 * sort order for the results.
	 * 
	 * Use this field to implement one-to-many relationships.
	 * 
	 * @param string $class the name of the related class
	 * @param string $field (optional) the name of the field defining the relation
	 * @param string $orderBy (optional) a sort order clause for the results
	 */
	function getRelatedList($class, $field = "", $orderBy = "")
	{
		$pk = $this->primary_key;
		
		if ($field == "") $field = $pk;
		$value = $this->$field ? $this->$field : $this->$pk;
		$orderBy = preg_replace("/^\s*WHERE\s+/i", "AND ", $orderBy);
		//$this->quoteFieldValue($this->$field, $this->fields[$field])
		$list = query($class, "WHERE $field=$value $orderBy");
		
		return $list;
	}
	
	/**
	 * Returns a list of items related to this item via a cross-reference table.
	 * The cross-reference table must contain the primary keys of each of the related
	 * classes as fields.
	 * 
	 * @param string $class the name of the related class
	 * @param string $xref the name of the cross-reference table
	 * @param string $orderBy (optional) a sort order clause for the results.
	 */
	function crossReference($class, $xref, $orderBy = "", $pkField = "", $xrefField = "")
	{
		$obj = new $class;
		$xref = new $xref;
		
		if ($orderBy != "")
		{
			foreach($obj->fields as $field => $type)
			{
				$orderBy = preg_replace("/\\b".$field."\\b/i", "a.$field", $orderBy);
			}
		}
		
		$orderBy = preg_replace("/^\s*WHERE\s+/i", "AND ", $orderBy);
		
		$xt = $xref->table;
		$xp = ($xrefField != "") ? $xrefField : $obj->primary_key;
		
		$pkField = ($pkField != "") ? $pkField : $this->primary_key;
		$pk = $this->primary_key;
		
		$query = "SELECT a.* FROM {$obj->table} a, $xt x WHERE a.{$obj->primary_key}=x.{$xp} AND x.{$pkField}={$this->$pk} $orderBy";
		
		trace($query, 3);
		
		try
		{
			$db = $this->getConnection();
			
			$result = $db->prepare($query);
			$result->execute();
			
			$list = array();
			while ($line = $result->fetch())
			{
				$obj = new $class;
				$obj->populate($line);
				$list[] = $obj;
			}
			
			unset($result);
		}
		catch(PDOException $e)
		{
			die("DataItem::crossReference() failed - " . $e->getMessage());
		}
		return $list;
	}  
		
	/**
	 * Query the database to calculate an aggregate value. The database table associated with
	 * the instatiated object is used as the source for the data.
	 * 
	 * @param string $func the value or function to retrieve
	 * @param string $constraints optional constraint clause to apply to the query
	 */
	function queryValue($func)
	{
		$constraints = "";
		$value = null;
		
		if (func_num_args() > 1)
		{
			$constraints = func_get_arg(1);
			
			if (func_num_args() > 2)
			{
				$value = func_get_arg(2);
			}
		}
		
		if ($constraints == "") $constraints = "WHERE 1=1"; //TODO - tidy this up some day
		$constraints .= " ".$this->getIdentityConstraint();
		
		$query = "SELECT $func as result FROM {$this->table} $constraints";
		
		trace("DataItem::queryValue: $query", 3);
		try
		{
			$db = $this->getConnection();
			$result = $db->prepare($query);
			$result->execute();
		
			if ($row =$result->fetch())
			{
				$value = $row['result'];
			}
			
			unset($result);
		}
		catch(PDOException $e)
		{
			die("DataItem::queryValue() failed - " . $e->getMessage());
		}
		
		return $value;
			
	}
	
	/**
	 * Generates an XML representation of the object. Filters are honored when determining
	 * which fields are included in the XML. Empty fields are not output (note that a string
	 * field with value "" is not considered empty).
	 */
	function toXML($indent = 0, $path = null)
	{
		trace(get_class($this)."->toXML()", 4);
		
		$xml = str_repeat(" ", $indent) . "<" . get_class($this) . ">\n";
		
		foreach($this->fields as $field => $type)
		{
			if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (isset($this->$field))
			{
				if ($this->$field === "")
				{
					$xml .= str_repeat(" ", $indent) . " <$field/>\n";
				}
				else
				{
					$xml .= str_repeat(" ", $indent) . " <{$field}>".$this->formatFieldForXML($field)."</$field>\n";
				}
				
			}			
		}
		
		$path[get_class($this)] = true;
		trace(implode(" > ", array_keys($path)), 4);
		trace("Options: ".$this->_options, 4);
		
		if (($this->_options & SerializeRelations) || 
			($this->_options & SerializeDirectRelations) && 
			$this->relations)
		{
			if (is_array($this->relations))
			{
				foreach($this->relations as $rel => $relType)
				{
					if (is_array($path) && array_key_exists($relType, $path)) continue;
						
					$obj = $this->$rel();
					if ($obj)
					{
						$xml .= str_repeat(" ", $indent) . " <$rel>\n";
						
						if (is_array($obj))
						{
							foreach($obj as $item)
							{
								if ($this->_options & SerializeRelations)
								{
									$item->setOption(SerializeRelations);
								}
								
								if ($this->_options & ProtectHTML)
								{
									$item->setOption(ProtectHTML);
								}
								
								$xml .= $item->toXML($indent + 2, $path);
							}
						}
						else
						{
							if ($this->_options & SerializeRelations)
							{
								$obj->setOption(SerializeRelations);
							}
						
							if ($this->_options & ProtectHTML)
							{
								$obj->setOption(ProtectHTML);
							}
						
							$xml .= $obj->toXML($indent + 2, $path);
						}
						
						$xml .= str_repeat(" ", $indent) . " </$rel>\n";
					}
					else
					{
						$xml .= str_repeat(" ", $indent) . " <$rel/>\n";
					}
				}
			}
		}	
			
		$xml .= str_repeat(" ", $indent) . "</" . get_class($this) . ">\n";
		
		return $xml;
	}
		
	function format($template = "", $separator = ", ")
	{
		if ($template == "") $template = $this->default_format;
		
		foreach($this->fields as $field => $type)
		{
			$template = str_replace("{".$field."}", $this->$field, $template);
		}

		// Feature enhancement - allow drill-down through relation functions
		
		$matches = array();
		preg_match_all("/\\{(\\w+).(\\w+):?([^}]*)}/", $template, $matches, PREG_SET_ORDER);
		
		foreach($matches as $match)
		{
			$relation = $match[1];
			$field = $match[2];
			$constraint = count($match) == 4 ? $match[3] : null;
			
			
			if ($constraint)
			{
				$relations = $this->$relation($constraint);
				$format = "{".$relation.".".$field.":".$constraint."}";
			}
			else
			{
				$relations = $this->$relation();
				$format = "{".$relation.".".$field."}";
			}
			
			$sub = "";
			
			if (is_array($relations))
			{
				$sub = displayFieldAsList($relations, $field, $separator);
			}
			else
			{
				$sub = $relations->$field;
			}
			
			trace("Replacing {$match[0]} [ $format ] with '$sub'", 3);
			
			$template = str_replace($format, $sub, $template);
		}
		
		return $template;
	}
	
	function prettifyFieldName($field)
	{
		$field = preg_replace("/([a-z])([A-Z0-9])/", "$1 $2", $field);
		$field = str_replace("_", " ", $field);
		$field = ucwords($field);
		
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
		$pk = $target->primary_key;
		
		if (!$field) $field = $pk;
		
		$this->$field = $target->$pk;
	}
}
	
/**
 * Performs a query against the database, returning an array of DataItem objects of the specified class.
 * 
 * @param string $class the name of the class of objects to query
 * @param $constraints optional constraint clause to apply to the query in the form "WHERE ... [ORDER BY ...]"
 * @param $page optional page number to retrieve
 * @param $size optional size of pages
 */
function query($class)
{
	$constraints = "";
	
	$page = -1;
	$size = -1;
	
	$filter = null;
	
	if (func_num_args() > 1)
	{
		$idx = 1;
		
		if (is_object(func_get_arg(1)))
		{
			$filter = func_get_arg(1);
			++$idx;
		}
		
		if (func_num_args() > $idx)
		{
			$constraints = func_get_arg($idx);
			
			if (func_num_args() == $idx + 3)
			{
				$page = func_get_arg($idx + 1);
				$size = func_get_arg($idx + 2);
			}
		}
	}
	
	$prototype = new $class;
	$prototype->filter = $filter;
	
	$order_by_idx = strpos(strtoupper($constraints), "ORDER BY");
	$orderBy = "";	
	if ($order_by_idx !== false)
	{
		$orderBy = substr($constraints, $order_by_idx);
		
		$constraints = substr($constraints, 0, $order_by_idx);
	} 
	
	if ($constraints == "") $constraints = "WHERE 1=1"; //TODO - tidy this up some day
	$constraints .= " ".$prototype->getIdentityConstraint();
	
	if (is_object($filter))
	{
		$query = "SELECT ".$prototype->getFieldList()." FROM {$prototype->table} $constraints $orderBy";
	}
	else
	{
		$query = "SELECT * FROM {$prototype->table} $constraints $orderBy";
	}
	
	trace("DataItem query: $query", 3);
	trace("Page: $page Size: $size", 3);
	$items = array();
	
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
		
		while($line = $result->fetch())
		{
			$item = new $class;	//Hack to work around PHP's stupid implementation of get_class()
			$item->filter = $filter;
			$item->populate($line);
			$items[] = $item;
			
			--$size;
			if ($size == 0) break;
		}
		
		unset($result);
	}
	catch(PDOException $e)
	{
		die("query() failed - " . $e->getMessage());
	}
	
	trace(count($items)." items found", 4);
	return $items;
}

/**
 * Performs a query against the database and returns a matching singleton object. If the query
 * returns more than one object, and error is thrown.
 * 
 * @param string $class the class name of the object to query
 * @param string $constraints optional constraint clause to apply to the query in the form "WHERE ... [ORDER BY ...]"
 */
function querySingle($class)
{
	$constraints = "";
	
	if (func_num_args() > 1)
	{
		$constraints = func_get_arg(1);
	}
	
	$result = query($class, $constraints);
	
	if (count($result) > 1)
	{
		die("Ambiguous singleton query");
	}
	
	if (count($result) == 1)
	{
		return $result[0];
	}
	
	return null;	
}

/**
 * Query the database to calculate an aggregate value. The database table associated with
 * the specified class is used as the source for the data.
 * 
 * @param string $class the class name of the object to query
 * @param string $func the value or function to retrieve
 * @param string $constraints optional constraint clause to apply to the query
 */
function queryValue($class, $func)
{
	$constraints = "";
	$value = null;
	
	if (func_num_args() > 2)
	{
		$constraints = func_get_arg(2);
		
		if (func_num_args() > 3)
		{
			$value = func_get_arg(3);
		}
	}

	$prototype = new $class;
	
	if ($constraints == "") $constraints = "WHERE 1=1"; //TODO - tidy this up some day
	$constraints .= " ".$prototype->getIdentityConstraint();
	
	$query = "SELECT $func as result FROM {$prototype->table} $constraints";
	
	trace($query, 3);
	
	try
	{
		$db = ConnectionManager::getConnection();
		
		$result = $db->prepare($query);
		$result->execute();
		
		if ($row = $result->fetch())
		{
			$value = $row['result'];
		}

		unset($result);
	}
	catch(PDOException $e)
	{
		die("queryValue() failed - " . $e->getMessage());
	}
	
	trace("Value = $value", 3);
	return $value;
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
	
	if (func_num_args() > 1)
	{
		$constraints = func_get_arg(1);
		
		if (func_num_args() > 2)
		{
			$field = func_get_arg(2);
			
			if (funC_num_args() > 3)
			{
				$filter = func_get_arg(3);
			} 
		}
		
		
	}

	$prototype = new $class;
	
	if ($filter)
	{
		$prototype->filter = $filter;
	}
	
	$order_by_idx = strpos(strtoupper($constraints), "ORDER BY");	
	if ($order_by_idx !== false)
	{
		$orderBy = substr($constraints, $order_by_idx);
		
		$constraints = substr($constraints, 0, $order_by_idx);
	} 
	
	$fields = $prototype->getfieldList();
	
	if ($constraints == "") $constraints = "WHERE 1=1"; //TODO - tidy this up some day

	$query = "SELECT $fields FROM {$prototype->table} $constraints ".$prototype->getIdentityConstraint()." $orderBy";
	trace("DataItem indexedQuery(): $query", 3);
	
	$items = array();
	
	try
	{
		$db = ConnectionManager::getConnection();

		$result = $db->prepare($query);
		$result->execute();
		
		while($line = $result->fetch())
		{
			$item = new $class;	//Hack to work around PHP's stupid implementation of get_class()
			$item->populate($line);
			if (array_key_exists($item->$field, $items))
			{
				// Implicitly promote to array if there is a collision
				if (!is_array($items[$item->$field]))
				{
					$items[$item->$field] = array($items[$item->$field]);
				}
				$items[$item->$field][] = $item;
			}
			else
			{
				$items[$item->$field] = $item;
			}
		}
		
		unset($result);
	}
	catch(PDOException $e)
	{
		die("indexedQuery() failed - " . $e->getMessage());
	}
	
	return $items;
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
	
	if (func_num_args() > 1)
	{
		$constraints = func_get_arg(1);
		
		if (func_num_args() > 2)
		{
			$field = func_get_arg(2);
			
			if (func_num_args() > 3)
			{
				$filter = func_get_arg(3);
			} 
		}
	}

	$prototype = new $class;
	
	if ($filter)
	{
		$prototype->filter = $filter;
	}
	
	$order_by_idx = strpos(strtoupper($constraints), "ORDER BY");	
	if ($order_by_idx !== false)
	{
		$orderBy = substr($constraints, $order_by_idx);
		
		$constraints = substr($constraints, 0, $order_by_idx);
	} 
		
	$fields = $prototype->getfieldList();

	if ($constraints == "") $constraints = "WHERE 1=1"; //TODO - tidy this up some day

	$query = "SELECT $fields FROM {$prototype->table} $constraints ".$prototype->getIdentityConstraint()." $orderBy";;
	trace("DataItem groupedQuery(): $query", 3);
	
	$items = array();
	
	try
	{
		$db = ConnectionManager::getConnection();
		
		$result = $db->prepare($query);
		$result->execute();

		while($line = $result->fetch())
		{
			$item = new $class;	//Hack to work around PHP's stupid implementation of get_class()
			$item->populate($line);
			$items[$item->$field][] = $item;
		}
	
		unset($result);
	}
	catch(PDOException $e)
	{
		die("groupedQuery() failed - " . $e->getMessage());	
	}
	
	return $items;
}

/**
 * Reindex an indexed or grouped query result by a different field.
 * 
 * @param array $list an array of DataItem-derived objects, as returned by query(), indexedQuery() or groupedQuery()
 * @param string $field the field by which to reindex the list
 */
function reindexList($list, $field, $autoPromote = true)
{
	$result = array();
	
	foreach($list as $elt)
	{
		if (is_array($elt))
		{
			foreach($elt as $e)
			{
				if (array_key_exists($e->$field, $result))
				{
					if (!is_array($result[$e->$field]))
					{
						$result[$e->$field] = array($result[$e->$field]);
					}
					$result[$e->$field][] = $e;
				}
				else
				{
					$result[$e->$field] = $e;
				}
			}
		}
		else
		{
			if (array_key_exists($elt->$field, $result) && $autoPromote)
			{
				if (!is_array($result[$elt->$field]))
				{
					$result[$elt->$field] = array($result[$elt->$field]);
				}
				$result[$elt->$field][] = $elt;
			}
			else
			{
				$result[$elt->$field] = $elt;
			}
		}
	}
	
	return $result;
}

/**
 * Regroup an indexed or grouped query result by a different field.
 * 
 * @param array $list an array of DataItem-derived objects, as returned by query(), indexedQuery() or groupedQuery()
 * @param string $field the field by which to regroup the list
 */
function regroupList($list, $field)
{
	$result = array();
	
	foreach($list as $elt)
	{
		if (is_array($elt))
		{
			foreach ($elt as $e)
			{
				$result[$e->$field][] = $e;
			}
		}
		else
		{
			$result[$elt->$field][] = $elt;
		}
	}
	
	return $result;
}

/**
 * Remove duplicate DataItems from a list, based on the value of a field.
 * If no field is specified, the items' primary key is used.
 * It is assumed that all the objects in the list are of the same class.
 * A filtered copy of the list is returned. The original list is left
 * unchanged.
 * 
 * @param array $list the list of DataItems from which duplicates are to be removed
 * @param string $field the name of the field to use to determine duplicity.
 * @return a filtered list containing no duplicates.
 */
function removeDuplicates($list, $field = "")
{
	if (count($list) == 0) return $list;
	
	if ($field == "")
	{
		$field = $list[0]->primary_key;
	}
	
	$found = array();
	$filtered = array();
	foreach($list as $item)
	{
		if (!array_key_exists($item->$field, $found))
		{
			$filtered[] = $item;
		}
		
		$found[$item->$field] = true;
	}
	
	return $filtered;
}

function toXML($tag, $objects)
{
	$xml = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<$tag>\n";
	
	foreach($objects as $obj)
	{
		$xml .= $obj->toXML(1);
	}
	$xml .= "</$tag>\n";
	
	return $xml;
}

function displayFieldAsList($items, $field, $separator = ", ")
{
	return formatItems($items, "{".$field."}", $separator);
}	

function formatItems($items, $template, $separator = "")
{
	$list = "";
	$first = true;
	
	foreach($items as $item)
	{
		if (!$first) $list .= $separator;
		$list .= $item->format($template);
		$first = false;
	}
	
	return $list;
}

/**
 * Extract the field values for the specified field from a list of DataItems
 * @param $items the DataItems on which to perform the extraction
 * @param $field the name of the field to extract
 * @return array an array of the field values in the same order as the presented DataItem list.
 */
function extractFieldValues($items, $field)
{
	$out = array();
	foreach($items as $item)
	{
		$out[] = $item->$field;
	}
	return $out;
}

function restoreHTML($text)
{
	$v = str_replace( array("[_[OPENTAG]_]", "[_[CLOSETAG]_]", "[_[AMPERSAND]_]"), array("<", ">", "&"),  $text);
	return $v;
}

class DataQuery
{
	var $type;
	var $constraint;
	var $filter;
	var $sortBy;
	var $sortDir;
	var $page;
	var $pageSize;
	
	function DataQuery($type, $constraint = "", $page = 1, $pageSize = 20, $sortBy = "", $sortDir = "ASC", $filter = null)
	{
		$this->type = $type;
		$this->constraint = $constraint;
		$this->page = $page;
		$this->pageSize = $pageSize;
		$this->sortBy = $sortBy;
		$this->sortDir = $sortDir;
		$this->filter = $filter;
	}
	
	function toQueryString()
	{
	}
}
?>
