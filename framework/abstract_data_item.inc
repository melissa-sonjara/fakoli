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

class DataItemException extends Exception {}
class DataNotFoundException extends Exception {}

require_once realpath(dirname(__FILE__)."/transaction.inc");

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
 * Abstract base class for all DataItem implementations. 
 * @author andy
 *
 */
abstract class AbstractDataItem
{
	var $filter;
	var $_options = 0;
	var $_tx = null;
	
	/**
	 * Cast this object to another class. This facility is provided so that
	 * it is possible to cast from a CompositeDataItem to one of its constituents
	 * to provide support for polymorphism in those cases. 
	 * @param string $class
	 * @return instance of the specified class, if compatible
	 * @throws DataItemException
	 */
	abstract function cast($class);
	
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
		$tx = $this->getTransaction();
		
		if ($tx)
		{
			return $tx->getConnection();
		}
		else
		{ 
			return ConnectionManager::getConnection();
		}
	}
	
	/**
	 * Join the DataItem to the specified DataTransaction.
	 * @param DataTransaction $tx the transaction to join
	 */
	abstract function joinTransaction($tx);
	
	/**
	 * Retrieves the current DataTransaction.
	 */
	abstract function getTransaction();
	
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
	abstract function populate($line);
		
	/**
	 * Retrieve the field type list for this object. 
	 */
	abstract function getFields();
	
	/**
	 * Returns a comma-separated list of the fields for this object (applying the assigned
	 * filter if there is one).
	 */
	abstract function getFieldList($alias = "");
	
	/**
	 * Return an array of field names for this object filtered by any active filter.
	 * @return array
	 */
	abstract function getFieldArray();
	
	/**
	 * Returns true if this DataItem contains a field with the specified name
	 * and that field is not excluded by a filter.
	 *
	 * @param string $field
	 * @return true if this field is present and not filtered, false otherwise.
	 */
	abstract function hasField($field);
	
	/**
	 * Returns true if this DataItem contains a relation with the specified name.
	 *
	 * @param string $relation
	 * @return true if this relation is present and not filtered, false otherwise.
	 */
	abstract function hasRelation($relation);

	/**
	 * Retrieves the primary key field name
	 */
	abstract function getPrimaryKey();
	
	/**
	 * Retrieves a list of all the primary keys used for an object as an array
	 * @return array all the primary keys for this object's constituent parts
	 */
	abstract function getPrimaryKeyList();
	
	/**
	 * Retrieve the value for the specified field.
	 * @param string $field the field to retrieve
	 * @return mixed the value of that field
	 */
	abstract function get($field);
	
	/**
	 * Set the value of the specified field
	 * @param $field the field to set
	 * @param $value the value to which the field is to be set
	 */
	abstract function set($field, $value);
	
	/**
	 * Retrieves the data type of the specified field
	 * @param $field the field in question
	 * @return string the data type of the specified field
	 */
	abstract function getType($field);
	
	/**
	 * Returns the filter set on this object. 
	 */
	abstract function getFilter();

	/**
	 * Sets the filter on this object.
	 * @param $filter the filter
	 */
	abstract function setFilter($filter);

	/**
	 * Retrieve the list of field aliases
	 * @return array the alias list for this object
	 */
	abstract function getFieldAliases();
	
	/**
	 * Load the object with the specified primary key
	 * 
	 * @param int id the primary key value to instantiate from
	 */
	abstract function load($id);
		
	/**
	 * Store the object in the database.
	 */
	abstract function save();
		
	/**
	 * Select the object from the database, based on the value of the primary key field.
	 */
	abstract function select();
	
	/**
	 * Check whether the object exists in the database.
	 */
	abstract function exists($constraint = "");
	
	/**
	 * Update the row in the database that corresponds to this object.
	 */
	abstract function update();

	/**
	 * Delete the row in the database that corresponds to this object.
	 */
	abstract function delete($constraint = "");
	
	/**
	 * Delete all the rows in the database that correspond to this class.
	 */
	abstract function deleteAll();

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
		
		$val = $this->get($field);
		
		switch($type)
		{
		case Number:
		case Currency:
		
			if ($val == "" || !is_numeric($val))
			{
				$query .= "0";
			}
			else
			{
				$query .= $val;
			}
			break;
	
		case Date:
		case DateTime:
			
			$query .= ($val) ? $db->quote($this->reformatToSQLDate($val)) : "NULL";
			break;
			
		case Timestamp:
			
			$query .= $db->quote(date("Y-m-d H:i:s"));
			break;
			
		case Boolean:
			
			$query .= ($val) ? "1" : "0";
			break;
			
		case String:
		case Text:
			
		default:
		
			$query .= $db->quote($val);
			break;
		}		

		return $query;			
	}
	
	function formatFieldValue($field)
	{
		$type = $this->getType($field);
		$data = $this->get($field);
		
		$val = ""; 
		switch($type)
		{
		case Number:
		
			if ($data == "" || !is_numeric($data))
			{
				$val = "0";
			}
			else
			{
				$val = $data;
			}
			break;
	
		case Currency:
					
			if ($data == "" || !is_numeric($data))
			{
				$val = "0";
			}
			else
			{
				$val = number_format($data, 2);
			}
			break;
						
		case Date:
		
			if (!$data) return "";
			$val = formatDateShort($data);
			break;
			
		/*case DateTime:
			if (!$data) return "";
			$val = formatDateShort($data);
			break;*/
			
		case Timestamp:
			
			$val = date("Y-m-d H:i:s");
			break;
			
		case Boolean:
			
			$query .= ($data) ? "True" : "False";
			break;
			
		case String:
		case Text:
			
		default:
		
			$val = $data;
			break;
		}		

		return $val;					
	}
	
	/**
	 * Format field for XML output
	 */
	function formatFieldForXML($field)
	{
		$type = $this->getType($field);
		$data = $this->get($field);
		
		$v = "";
		switch($type)
		{
		case HTML:
			
			if ($this->_options & ProtectHTML)
			{
				$v = str_replace( array("<", ">", "&"), array("[_[OPENTAG]_]", "[_[CLOSETAG]_]", "[_[AMPERSAND]_]"), $data);				
				break;
			}
			
		case Number:
		case Date:
		case Timestamp:
		case String:
		case Text:
		default:
		
			$v = htmlspecialchars($data);
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
	abstract function insert();
		
	/**
	 * Retrieves the distinct values in the database for the specified field 
	 * across the specified set of records.
	 * @param string $field
	 * @param boolean $sorted
	 * @param string $constraint
	 * @return array
	 */
	abstract function distinctValues($field, $sorted = false, $constraint = "");
	
	/**
	 * Automatically populate the object based on parameters in the $_GET collection.
	 */
	abstract function fromGET();
	
	/**
	 * Automatically populate the object based on parameters in the $_POST collection.
	 * If the key does not exist in $_POST and the type is Boolean, that means
	 * that the user unchecked a checkbox and we need to assign 0 to the
	 * field.
	 */
	abstract function fromPOST();
	
	/**
	 * Automatically populate the object based on parameters in either the $_GET or $_POST collection,
	 * depending on the method used to access the page.
	 */
	abstract function fromREQUEST();
	
	/**
	 * Compare this object to another object. If all the fields match,
	 * the method returns true, otherwise false. If a filter is in place on
	 * the source object, then only the filtered fields are compared.
	 * 
	 * @param object $to the object with which to compare this object
	 */
	abstract function compare($to);
	
	/**
	 * Copies values from another object, field by field. If a filter
	 * is in place on the source object, then only the filtered fields are copied.
	 * 
	 * @param object from the object from which data is to be copied
	 */
	abstract function copy($from);
			
	/**
	 * Generates an XML representation of the object. Filters are honored when determining
	 * which fields are included in the XML. Empty fields are not output (note that a string
	 * field with value "" is not considered empty).
	 */
	abstract function toXML($indent = 0, $path = null);
			
	abstract function format($template = "", $separator = ", ");
	
	abstract function prettifyFieldName($field);
		
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
			$c = pluralize($c);
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
	abstract function relateTo($target, $field = "");
}
?>