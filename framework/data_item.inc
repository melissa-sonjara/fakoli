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

//require_once realpath(dirname(__FILE__)."/connection_manager.inc");
require_once realpath(dirname(__FILE__)."/abstract_data_item.inc");
require_once realpath(dirname(__FILE__)."/query.inc");
require_once realpath(dirname(__FILE__)."/indexed_query.inc");
require_once realpath(dirname(__FILE__)."/grouped_query.inc");
require_once realpath(dirname(__FILE__)."/iterated_query.inc");
require_once realpath(dirname(__FILE__)."/json_query.inc");

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
 * DataItem is the generic base class for database mapped classes. It provides a
 * compact, consistent API for object persistence in the database.
 * 
 * 
 */
class DataItem extends AbstractDataItem
{
	static $_eventMap = array();
	
	var $_tx = null;
	var $_pkDisabled = false;

   // When adding new renderers, must also update
    // AutoForm $fieldRendererMap	
	static $dataTypeRendererMap = array(
		"Boolean"		=>	BooleanTypeRenderer,	
		"Currency"		=>	CurrencyTypeRenderer,
		"Currency3"		=>	CurrencyTypeRenderer,
		"Date"			=>	DateTypeRenderer,
		"DateTime"		=>	DateTimeTypeRenderer,
		"Time"			=>	TimeTypeRenderer,
		"Number"		=>	NumberTypeRenderer,	
		"String"		=>	StringTypeRenderer,
		"Text"			=>	TextTypeRenderer,
		"HTML"			=>	TextTypeRenderer,
		"Timestamp"		=>	TimestampTypeRenderer,
		"Timezone"		=>	TimezoneTypeRenderer,
		"PhoneNumber"	=>	PhoneNumberTypeRenderer,
		"DateOfBirth"	=>	DateTypeRenderer
	);
	
	
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
	
	function __sleep()
	{
		// No need to persist class variables, such as field metadata
		
		$f = array_keys($this->getFields());
		array_push($f, "primary_key", "table", "filter");
		return $f;
	}
	
	/**
	 * Register an event handler for a specific event and DataItem class.
	 * DataItem events are fired by DataItems at various points in the data
	 * lifecycle (such as inserting, updating, deleting, etc.)
	 * 
	 * @param string $class the DataItem class to which the event handler is to be attached
	 * @param string $event the name of the event
	 * @param callable $handler the event handler (a callable function) 
	 */
	static function registerEventHandler($class, $event, $handler)
	{
		$eventKey = "{$class}_{$event}";
		
		if (!array_key_exists($eventKey, DataItem::$_eventMap))
		{
			
			DataItem::$_eventMap[$eventKey]= array();
		}
		
		array_push(DataItem::$_eventMap[$eventKey], $handler);
	}
	
	/**
	 * Fires the specified event to all registered handlers. The firing object is passed
	 * as a parameter to all the registered handlers.
	 * @param string $event the event to be fired
	 */
	function fireEvent($event)
	{
		$eventKey = get_class($this)."_{$event}";
		if (!array_key_exists($eventKey, DataItem::$_eventMap))
		{
			return;
		}
		
		trace("===== DataItem event key = $eventKey", 3);
		$handlers = DataItem::$_eventMap[$eventKey];
		
		if (!is_array($handlers)) return;
		foreach($handlers as $handler)
		{
			if (!is_callable($handler))
			{
				trace(get_class($this)." {$event} handler is invalid", 2);
			}
			else 
			{
				call_user_func_array($handler, array($this));
			}
		}
	}
	
	/**
	 * Join the DataItem to the specified DataTransaction.
	 * @param DataTransaction $tx the transaction to join
	 */
	function joinTransaction($tx)
	{
		$this->_tx = $tx;
	}
	
	/**
	 * Retrieves the current DataTransaction.
	 */
	function getTransaction()
	{
		return $this->_tx;
	}
	
	/**
	 * Cast this object to another class. For simple DataItems this checks that the class
	 * is the same as the supplied class and returns a reference if so. If not an exception is
	 * thrown.
	 * @param string $class
	 * @return instance of the specified class, if compatible
	 * @throws DataItemException
	 */
	function cast($class)
	{
		if (get_class($this) != $class)
		{
			throw new DataItemException("Cannot cast ".get_class($this)." to $class.");
		}
		
		return $this;
	}
	
	function disablePrimaryKey()
	{
		if (!$this->_disablePK)
		{
			$conn = $this->getConnection();
			$conn->prepare("ALTER TABLE `{$this->table}` DISABLE KEYS")->execute();
			$this->_disablePK = true;
		}			
	}
	
	function enablePrimaryKey()
	{
		if ($this->_disablePK)
		{
			$conn = $this->getConnection();
			$conn->prepare("ALTER TABLE `{$this->table}` ENABLE KEYS")->execute();
			$this->_disablePK = false;
		}			
	}
	
	function getHiddenFields()
	{
		return $this->hiddenFields;
	}
	
	/**
	 * Populates the object using the supplied associative array (field -> value).
	 * Only values that match the definition of the sub-classed object will be copied.
	 * Fields that have been excluded by the use of an InclusionFilter or ExclusionFilter
	 * will also not be populated.
	 * 
	 * @param array $line the array of values to populate the object with. 
	 */
	function populate($line, $alias = false)
	{
		global $config;
		
		// NOTE - structured for speed, since the general case is no aliasing
		// therefore we want to reduce the number of tests of the alias 
		
		$fields = $this->getFields();
		
    	if ($alias)
        {
        	$offset = strlen($alias) + 1;
        	
	    	foreach ($line as $field => $value)
	        {
        		if (!startsWith($field, $alias)) continue;
        		$field = substr($field, $offset);
        	
	        	if (!array_key_exists($field, $fields)) continue;
	        	if ($this->filter && $this->filter->isExcluded($field)) continue;
	        	
	        	if (!isset($config["no_charset_conversions"]) || $config["no_charset_conversions"])
	        	{
	        		$this->$field = $value;
	        	}
	        	else
	        	{
	            	$this->$field = iconv("UTF-8", "CP1252//IGNORE", $value);
	        	} 
	       	}
        }
        else
        {
			foreach ($line as $field => $value)
	        {
	        	if (!array_key_exists($field, $fields)) continue;
	        	if ($this->filter && $this->filter->isExcluded($field)) continue;
	        	
	        	if (!isset($config["no_charset_conversions"]) || $config["no_charset_conversions"])
	        	{
	        		$this->$field = $value;
	        	}
	        	else
	        	{
	            	$this->$field = iconv("UTF-8", "CP1252//IGNORE", $value);
	        	} 
	        } 
        }
        
        if (isset($this->calculatedFields))
        {
        	foreach($this->calculatedFields as $field => $expr)
        	{
        		if ($alias) 
        		{
        			$this->$field = $line[$alias.".".$field];
        		}
        		else
        		{
        			$this->$field = $line[$field];
        		}
        	}
        }
        
        $this->unpack();
        $this->fireEvent("onPopulate");
	}
		
	/**
	 * Retrieve the field type list for this object. 
	 */
	function getFields()
	{
		if ($this->fields) return $this->fields;
		$class = get_class($this);
		if ($class::$fields) return $class::$fields;
		
		throw new FakoliException("DataItem does not contain any fields");
	}
	
	/**
	 * Override the type for the specified field
	 * @param string $field
	 * @param string $type
	 */
	function overrideFieldType($field, $type)
	{
		if (!is_array($this->fields))
		{
			throw new FakoliException("Cannot override field types for statically defined DataItems");
		}
		
		$this->fields[$field] = $type;
	}
	
	/**
	 * Returned a comma-separated list of the fields for this object (applying the assigned
	 * filter if there is one).
	 */
	function getFieldList($alias = "")
	{
		$list = "";
		$first = true;
		
		foreach($this->getFields() as $field => $type)
		{
			if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (!$first) $list .= ", ";
			if ($alias)
			{
				$list .= "$alias.";
			}
			$list .= $field;
			if ($alias)
			{
				$list .= " as `$alias.$field`";
			}
			$first = false;
		}
		
		if (isset($this->calculatedFields))
		{
			foreach($this->calculatedFields as $field => $expr)
			{
				if ($this->filter && $this->filter->isExcluded($field)) continue;
				if (!$first) $list .= ", ";
				
				if ($alias)
				{
					foreach($this->getFields() as $f => $type)
					{
						$expr = preg_replace('/([\s\(])('.$f.')\b/', "$1{$alias}.$2", $expr);
						$expr = preg_replace('/\b'.$this->table.'\.'.$f.'\b/', "{$alias}.{$f}", $expr);
					}
				}
				
				$list .= $expr . " as `";
				if ($alias)
				{
					$list .= $alias.".";
				}
				$list .= $field."`";
				
			}
		}
		
		return $list;
	}
	
	/**
	 * Return an array of field names for this object filtered by any active filter.
	 * @return array
	 */
	function getFieldArray()
	{
		$arr = array();
		foreach($this->getFields() as $field => $type)
		{
			if ($this->filter && $this->filter->isExcluded($field)) continue;
			$arr[] = $field;
		}
		
		return $arr;
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
		if (!array_key_exists($field, $this->getFields())) return false;
		if ($this->filter && $this->filter->isExcluded($field)) return false;
		return true;
	}
	
	/**
	 * Returns true if this DataItem contains a relation with the specified name.
	 *
	 * @param string $relation
	 * @return true if this relation is present and not filtered, false otherwise.
	 */
	function hasRelation($relation)
	{
		return (!array_key_exists($relation, $this->relations)) ? false : true;
	}
			
	/**
	 * Retrieves the primary key field name
	 */
	function getPrimaryKey()
	{
		return $this->primary_key;
	}
	
	/**
	 * Retrieves a list of all the primary keys used for an object as an array
	 * @return array all the primary keys for this object's constituent parts
	 */
	function getPrimaryKeyList()
	{
		return array($this->primary_key);
	}
	
	/**
	 * Retrieve the value for the specified field.
	 * @param string $field the field to retrieve
	 * @return mixed the value of that field
	 */
	function get($field)
	{
		if (!$field) throw new FakoliException("Cannot access empty field");
		return $this->$field;
	}
	
	/**
	 * Set the value of the specified field
	 * @param $field the field to set
	 * @param $value the value to which the field is to be set
	 */
	function set($field, $value)
	{
		if (!$field) throw new FakoliException("Cannot access empty field");
		$this->$field = $value;
	}
	
	/**
	 * Retrieves the data type of the specified field
	 * @param $field the field in question
	 * @return string the data type of the specified field
	 */
	function getType($field)
	{
		$fields = $this->getFields();
		return $fields[$field];
	}
	
	/**
	 * Returns the filter set on this object. 
	 */
	function getFilter()
	{
		return $this->filter;
	}

	/**
	 * Sets the filter on this object.
	 * @param $filter the filter
	 */
	function setFilter($filter)
	{
		$this->filter = $filter;
	}	
	
	/**
	 * Retrieve the list of field aliases
	 * @return array the alias list for this object
	 */
	function getFieldAliases()
	{
		return $this->fieldAliases;
	}
	
	/**
	 * Retrieve the list of field annotations
	 * @return array the annotation list for this object
	 */
	function getFieldAnnotations()
	{
		return $this->fieldAnnotations;
	}
	
	
	/**
	 * The pack() method is called prior to persisting a DataItem to storage.
	 * It can be used to perform a preprocessing step prior to storage, such as
	 * formatting sub-objects into a JSON, XML or other serialized format and storing them in a field.
	 */
	function pack()
	{
	}
	
	/**
	 * The unpack() method is called after populating a DataItem.
	 * It can be used to unpack serialized sub-object representations so they are can
	 * be used directly by your application.
	 */
	function unpack()
	{
	}
	
	/**
	 * Load the object with the specified primary key
	 * 
	 * @param int id the primary key value to instantiate from
	 */
	function load($id)
	{
		if ($this->cacheLookup($id)) return;
		 
		$fields = $this->getFieldList();
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
			$err = "DataItem::load() failed - ".$e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
	}
		

	/**
	 * Retrieves the currently stored state of the DataItem from the database
	 * and returns it as a new object. This can be used for comparisons when updating
	 * a DataItem, for instance to trigger related updates when a field value is changed. 
	 * 
	 * @return DataItem a DataItem populated from the database based on the current item's 
	 * primary key. The returned DataItem has the same filter and DataTransaction as the 
	 * parent item.
	 */
	function old()
	{
		$pk = $this->getPrimaryKeyValue();
		if (!$pk) return null;
		
		$cl = get_class($this);
		$n = new $cl;
		$n->filter = $this->filter;
		$n->joinTransaction($this->getTransaction());
		$n->load($pk);
		return $n;	
	}
	
	/**
	 * Creates an outer CompositeDataItem from its base component. In order
	 * for this method to succeed, the base component table must have a 'composite_class' 
	 * field that is populated with the class name of the correct CompositeDataItem class.
	 * @return CompositeDataItem the containing CompositeDataItem for this base component.
	 */
	function loadComposite()
	{
		if (!$this->hasField("composite_class") || !$this->composite_class)
		{
			throw new DataItemException("Cannot create composite from ".get_class($this)." as required composite cannot be determined");
		}
		
		$composite = new $this->composite_class;
		$composite->loadFromBase($this->get($this->getPrimaryKey()));
		
		return $composite;
	}

	/**
	 * Store the object in the database.
	 */
	function save()
	{
	    if ($this->cacheLocal)
		{
			Cache::invalidate(get_class($this)."_cache");
		}

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
			$err = "DataItem::exists() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		return $exists;
	}
		
	/**
	 * Update the row in the database that corresponds to this object.
	 */
	function update()
	{
		$this->pack();
		
		$pk = $this->primary_key;

		$query = "UPDATE {$this->table} SET ";
    		
		$first = true;
		$found = false;
		
		foreach($this->getFields() as $field => $type)
		{			
			if ($field == $this->primary_key) continue;
        	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (!isset($this->$field) && $this->getType($field) != Timestamp && $this->getType($field) != Boolean) continue;
			
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
			$this->fireEvent("beforeUpdate");
			
			$db = $this->getConnection();
			
			$success = $db->exec($query);
			if ($success === FALSE)
			{
				trace("DataItem::update failed - $query", 2);
			}
			else
			{
				$this->fireEvent("onUpdate");
			}
		}
		catch(PDOException $e)
		{
			$err = "DataItem::update() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);		
		}
		
		return $success;
	}
	
	/**
	 * Executes an explicit update command against the database. This enables
	 * you to perform updates on the object using database functions, allowing
	 * for atomic operations, etc. For example, you might make such a call as
	 * $obj->updateExplicit("SET count=count+1"); The object is refreshed from
	 * the database automatically after the update.
	 * @param string $updates the update command to be run
	 * @param array $params optional array for bound parameters
	 * @return boolean TRUE if the update was successful, FALSE if not.
	 */
	function updateExplicit($updates, $params = null)
	{
		$pk = $this->primary_key;
		
		$query = "UPDATE {$this->table} $updates";

		if ($this->get($pk))
		{
			$query .= " WHERE {$pk}=".$this->get($pk);
		}
		
		trace("DataItem::update() - $query", 3);
		
		try
		{
			$db = $this->getConnection();
			$stmt = $db->prepare($query);
			$success = $stmt->execute($params);
			if ($success === FALSE)
			{
				trace("DataItem::updateExplicit failed - $query", 2);
			}
			if ($this->get($pk)) $this->select();
		}
		catch(PDOException $e)
		{
			$err = "DataItem::updateExplicit() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		return $success;
	}
	
	/**
	 * Insert a new row in the database to store this object.
	 */
	function insert()
	{
		$this->pack();
		
		$pk = $this->primary_key;
		
		$first = true;
		
		$timestampField = null;
		
		foreach($this->getFields() as $field => $type)
		{
			if ($field == $pk && !$this->_disablePK) continue;
        	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (!isset($this->$field) && $this->getType($field) != Timestamp && $this->getType($field) != Boolean) continue;
			
			if ($type == Timestamp)
			{
				$timestampField = $field;
			}
			
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
			$this->fireEvent("beforeInsert");

			$db = $this->getConnection();
			$success = $db->exec($query);
			
			if ($success !== FALSE)
			{
				$this->$pk = $db->lastInsertId();
				//AJG - Set the timestamp field value if the insert succeeded
				if ($timestampField) 
				{
					$this->$timestampField = $this->__timestamp;
					unset($this->__timestamp);
				}
				
				$this->fireEvent("onInsert");
			}
			else
			{
				$this->$pk = 0;
	 			trace("DataItem::insert() failed - $query", 2);
	 		}
		}	
		catch(PDOException $e)
		{
			$err = "DataItem::insert() failed - ". $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
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
			$fire = true;
			$pk = $this->primary_key;
			$constraint =  "WHERE $pk={$this->$pk} ".$this->getIdentityConstraint();
		}
		
		$query = "DELETE FROM {$this->table} $constraint";
		
		trace($query, 3);
		
		$db = $this->getConnection();
		
		try
		{
			if ($fire) $this->fireEvent("onPreDelete");
			$db->exec($query);
			if ($fire) $this->fireEvent("onPostDelete");
		}
		catch(PDOException $e)
		{
			$err = "DataItem::delete() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
	}
		
	/**
	 * Delete all the rows in the database that correspond to this class.
	 */
	function deleteAll()
	{
		//$query = "TRUNCATE TABLE {$this->table}";
		$query = "DELETE FROM {$this->table}; ALTER TABLE {$this->table} AUTO_INCREMENT=1";
		$db = $this->getConnection();
				
		trace($query, 3);
		
		try
		{
			$db->exec($query);
			$this->fireEvent("onDeleteAll");
		}
		catch(PDOException $e)
		{
			$err = "DataItem::deleteAll() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}	
	}

	/**
	 * Check if the table for this class exists in the database.
	 */
	function tableExists()
	{
		$query = "SHOW TABLES LIKE '{$this->table}'";
		$db = $this->getConnection();
				
		trace($query, 3);
		
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
			$err = "DataItem::tableExists() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}

		return $exists;
	}	
	
	/**
	 * Persists a duplicate of the DataItem to the database under a new primary key.
	 * Foreign keys are not updated. Use this as a way to create a quick copy of an existing
	 * object, or to import a serialized object into the data store under a new primary key.
	 * For utility, the previous value of the primary key is returned.
	 * 
	 * @return int the previous value of the primary key
	 */
	function duplicate()
	{
		$pk = $this->primary_key;
		$old = $this->get($pk);
		
		unset($this->$pk);
		$this->save();
		return $old;
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
		$query = "SELECT DISTINCT $field from {$this->table} $constraint";
		
		trace($query, 3);
		
		$db = $this->getConnection();
		
		try
		{
			$result = $db->prepare($query);
			$result->execute();
			
			$values = array();
			
			while($line = $result->fetch())
			{
				$values[] = $line[$field];
			}
			
			if ($sorted)
			{
				sort($values);
			}
					
			unset($result);
			
			return $values;
		}
		catch(PDOException $e)
		{
			$err = "DataItem::distinctValues() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
	}
	
	/**
	 * Automatically populate the object based on parameters in the $_GET collection.
	 */
	function fromGET()
	{
		foreach($this->getFields() as $field => $type)
		{
        	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (array_key_exists($field, $_GET))
			{
				$this->set($field, $_GET[$field]);
			}
		}
		
		$this->unpack();
	}
	
	/**
	 * Automatically populate the object based on parameters in the $_POST collection.
	 * If the key does not exist in $_POST and the type is Boolean, that means
	 * that the user unchecked a checkbox and we need to assign 0 to the
	 * field.
	 */
	function fromPOST()
	{
		foreach($this->getFields() as $field => $type)
		{
       	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (array_key_exists($field, $_POST))
			{
				$this->set($field, $_POST[$field]);
			}
			elseif (!array_key_exists($field, $_POST) AND ($type == Boolean))
			{
				$this->$field = 0;
			}
		}
		
		$this->unpack();
	}
	
	/**
	 * Automatically populate the object based on parameters in either the $_GET or $_POST collection,
	 * depending on the method used to access the page.
	 */
	function fromREQUEST()
	{
		switch($_SERVER["REQUEST_METHOD"])
		{
		case "POST":
			$this->fromPOST();
			break;
			
		case "GET":
		default:
			$this->fromGET();
		}
	}
	
	/**
	 * Automatically populate the object based on a custom set of parameters
	 * (such as a filtered $_POST collection)
	 */
	function fromDataSet($params)
	{
		foreach($this->getFields() as $field => $type)
		{
    	   	if ($this->filter && $this->filter->isExcluded($field)) continue;
			if (array_key_exists($field, $params))
			{
				$this->$field = $params[$field];
			}
			elseif (!array_key_exists($field, $params) AND ($type == Boolean))
			{
				$this->$field = 0;
			}
		}
		
		$this->unpack();
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
		foreach($this->getFields() as $field => $type)
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
		foreach($this->getFields() as $field => $type)
		{
			if ($this->filter && $this->filter->isExcluded($field)) continue;
			$this->set($field, $from->$field);
		}
		
		if (is_array($this->calculatedFields))
		{
			foreach($this->calculatedFields as $field => $expr)
			{
				$this->set($field, $from->$field);
			}
		}	
	}
	
	function cacheLookup($id)
	{
		if ($this->cacheLocal)
		{
			$cache = get_class($this)."_cache";
			$items = Cache::get($cache);
			if (!is_array($items))
			{
				$items = IndexedQuery::create(get_class($this), "", $this->primary_key)->execute();
				Cache::put($cache, $items);
			}
			if (array_key_exists($id, $items))
			{
				trace("Using local cache $cache for item $id", 5);
				$this->copy($items[$id]);
				return true;
			}
		}
		
		return false;
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
		
		if (array_key_exists($field, $this->getFields()))
		{
			$pk = $obj->primary_key;
			$val = $this->get($field);
			
			if (!$val) return null;
			
			$obj->$pk = $val;
			
			$cache = "_{$class}_{$field}_{$val}";
			if (isset($this->$cache))
			{
				$obj->copy($this->$cache);
			}
			else
			{
				$obj->select();
				$this->$cache = $obj;
			}			
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
			foreach($obj->getFields() as $field => $type)
			{
				$orderBy = preg_replace("/\\b".$field."\\b/i", "a0.$field", $orderBy);
			}
		}
		
		$orderBy = preg_replace("/^\s*WHERE\s+/i", "AND ", $orderBy);
		
		$xt = $xref->table;
		$xp = ($xrefField != "") ? $xrefField : $obj->primary_key;
		
		$pkField = ($pkField != "") ? $pkField : $this->primary_key;
		$pk = $this->primary_key;
		
		$fieldList = $obj->getFieldList("a0");
		
		$query = "SELECT {$fieldList} FROM {$obj->table} a0, $xt x WHERE a0.{$obj->primary_key}=x.{$xp} AND x.{$pkField}={$this->$pk} $orderBy";
		
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
				$obj->populate($line, "a0");
				$list[] = $obj;
			}
			
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "DataItem::crossReference() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
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
			$err = "DataItem::queryValue() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
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
		
		foreach($this->getFields() as $field => $type)
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
		
	function fromXML($node)
	{
		if ($node->nodeType != XML_ELEMENT_NODE || $node->tagName != get_class($this)) return;
		
		$kids = $node->childNodes;
		$numChildren = $kids->length;
		
		for($i = 0; $i < $numChildren; ++$i)
		{
			$n = $kids->item($i);
			
			if ($n->nodeType != XML_ELEMENT_NODE) continue;
			$field = $n->tagName;
			$this->set($field, $n->nodeValue);
		}
	}
	
	function toJSON()
	{
		$out = array();
				
		foreach($this->getFields() as $field => $type)
		{
			if ($this->filter && $this->filter->isExcluded($field))
			{
				continue;
			}
			
			$val = str_replace("\\'", "'", jsSafe($this->get($field)));
			
			$out[] = "\"{$field}\": \"".$val."\"";			
		}	
		
		return "{".implode(", ", $out)."}";		
	}
	
	/**
	 * Substitute values for field names in a string, 
	 * with the fields formatted using their type's 
	 * default format or according to the formatting
	 * template sting passed in.
	 * 
	 * example template string:
	 * "{program_name} {start_date} {number_students}"
	 * 
	 * 1. data type format specifications:
	 * The start_date field can have formatting specifications 
	 * such as "{start_date:long} for long format. For more
	 * information on formatting specifications following the colon,
	 * check comments in DataTypeRenderer classes.
	 * 
	 * 2. Default value specification:
	 * syntax:  {field|alternate}
	 * example: {name|N/A}
	 * 
	 * In the above example "N/A" would be output if the name
	 * field is empty or null. This will also work with numeric fields with
	 * a value of 0 and booleans with false. Very useful for places where
	 * you are linking to a form by name or title.
	 * 
	 * 3. Inline static method call:
	 * syntax:  {class::method}
	 * example: {MyHelper::getComplicatedOutput}
	 * This works the same as passing array(MyHelper, getComplicatedOutput)
	 * to a DataListView. It calls the static method getComplicatedOutput()
	 * on the MyHelper class, passing the current object in as a parameter.
	 * The new inline format means that you can use it as part of more
	 * complicated expressions like:
	 * "{name} - {MyHelper::getComplicatedOutput}"
	 * 
	 * 4. Retrieve field value through relations.
	 *  
	 * Fields can also be retrieved through a relation from
	 * the dataitem obj that calls this format function.
	 * 
	 * 5. Local method calls
	 * 
	 * Syntax: {method()}
	 * Example: {getFullName()}
	 * 
	 * This format calls the specified method on the object and substitutes
	 * the returned value into the output. Parameters cannot be passed.
	 * 
	 * for example:
	 * "{MyRelation.event_name}"
	 * 
	 * Variables used:
	 * $format - stores the pattern inside the braces {} for 
	 * search & replace substitution in the template string. 
	 * 
	 * $fieldFormat - the requested formatting for the field, if
	 * any.
	 * 
	 * $sub - the value to be substituted or replacement
	 * value in the tempalte string.
	 * 
	 */
	function format($template = "", $separator = ", ")
	{
		if ($template == "") $template = $this->default_format;
		if (is_array($template) && is_callable($template))
		{
			return call_user_func($template, $this);
		}

		foreach($this->getFields() as $field => $type)
		{
			$template = str_replace("{".$field."}", $this->formatFieldValue($field), $template);
		}

		if (isset($this->calculatedFields))
		{
			foreach($this->calculatedFields as $field => $expr)
			{
				$template = str_replace("{".$field."}", $this->$field, $template);			
			}
		}

		if (isset($this->_decorations))
		{
			foreach($this->_decorations as $field => $value)
			{
				$template = str_replace("{".$field."}", $value, $template);			
			}
		}
		
		// Allow drill-down through relation functions
		$template = $this->formatThroughRelation($template, $separator);
		
		$template = $this->formatField($template);
		
		// Simple method calls - no parameter passing
		// ??Is this block ever executed??
		
		$matches = array();
		preg_match_all("/\\{([\\w_]+)\\(\\)\\}/", $template, $matches, PREG_SET_ORDER);
		
		foreach($matches as $match)
		{
			$format = $match[0];
			$method =  $match[1];
			
			$value = $this->$method();
			
			$template = str_replace($format, $value, $template);
		}
		
		return $template;
	}
		
	/*
	 * Call formatFieldValue on a field with
	 * formatting specification given.
	 * 
	 * e.g.: 
	 * {amount:2}
	 * {start_date:long}
	 */
	function formatField($template)
	{
		$matches = array();
		
		preg_match_all("/\\{([\\w\\d_]+)(::?|\\^|\\(\\)|\\|)?([^}]*)}/", $template, $matches, PREG_SET_ORDER);

		foreach($matches as $match)
		{
			$field = $match[1];
			// JDG 3/12 fix issue with call to function in  DataItem in format like "{getTitle()}" with no separator;
			$separator = ($match[2]) ? $match[2] : ":";
		
			$fieldFormat = $match[3];
			trace("formatField, template $template, field $field, separator $separator and fieldFormat $fieldFormat", 5);
			
			$format = "{".$field.$separator.$fieldFormat."}";
			
			switch($separator)
			{
			case "|":
				if (!$this->get($field)) 
				{
					$sub = $fieldFormat;
				}
				else
				{
					$sub = $this->formatFieldValue($field);
				}
				break;
				
			case "^":
				if (!$this->get($field))
				{
					$sub = $this->format("{".$fieldFormat."}");
				}
				else
				{
					$sub = $this->formatFieldValue($field);
				}
				break;
				
			case "::":
				
				$sub = call_user_func(array($field, $fieldFormat), $this);
				break;
				
			case "()":
				
				$sub = $this->$field();
				break;
				
			case ":":
			default:
				$sub = $this->formatFieldValue($field, $fieldFormat);
			}
			
			$template = str_replace($format, $sub, $template);
		}
		return $template;
	}
	
	/*
	 * Format a field value that is accessed through a relation.
	 * The relation can return multiple items.
	 * e.g.,
	 * 
	 * {Program.program_name}
	 * {Event.start_date} or {Event.start_date:long}
	 * {Events.event_name}
	 */
	function formatThroughRelation($template, $separator = ", ")
	{
		$matches = array();
		preg_match_all("/\\{([\\w\\d_]+)\\.([\\w\\d_\\.\\(\\)]+):?([^}]*)}/", $template, $matches, PREG_SET_ORDER);
		
		foreach($matches as $match)
		{
			$relation = $match[1];
			$field = $match[2];
			
			if(count($match) == 4 && preg_match("/^where|order/i", $match[3]))
				$constraint = $match[3];
			elseif(count($match) == 4)
				$fieldTemplate = $match[3];

			trace("function format: formatThroughRelations template $template field $field relation $relation constraint $constraint", 5);
			
			if ($constraint)
			{
				$relations = $this->$relation($constraint);
				$format = "{".$relation.".".$field.":".$constraint."}";
			}
			elseif($fieldTemplate)
			{
				$relations = $this->$relation();
				$format = "{".$relation.".".$field.":".$fieldTemplate."}";
			}
			else
			{
				$relations = $this->$relation();
				$format = "{".$relation.".".$field."}";
			}
			
			$sub = "";
			
			if (is_array($relations))
			{
				$formatTemplate = ($fieldTemplate) ? "{".$field .":".$fieldTemplate . "}" : "{".$field."}";
				$sub = formatItems($relations, $formatTemplate, $separator);
	
			}
			else if ($relations)
			{
				if ($fieldTemplate) $fieldTemplate = ":{$fieldTemplate}";
				if ($relations) $sub = $relations->format("{{$field}{$fieldTemplate}}");
			}
			
			trace("format using relations: Replacing {$match[0]} [ $format ] with '$sub'", 5);
			
			$template = str_replace($format, $sub, $template);
		}
		
		return $template;
	}
	
	
	function prettifyFieldName($field)
	{
		if (isset($this->fieldAliases) && array_key_exists($field, $this->fieldAliases))
		{
			return $this->fieldAliases[$field];
		}
		
		$field = preg_replace("/([a-z])([A-Z0-9])/", "$1 $2", $field);
		$field = str_replace("_", " ", $field);
		$field = ucwords($field);
		
		return $field;
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
		throw new DataItemException("Ambiguous singleton query");
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
		$err = "queryValue() failed - " . $e->getMessage();
		trace($err, 2);
		throw new DataItemException($err);
	}
	
	return $value;
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

/**
 * Simple field comparator to support sorting arrays of DataItems
 * by the values in a specified field.
 */
class DataItemFieldComparator
{
	var $field;
	
	function __construct($field)
	{
		$this->field = $field;
	}
	
	function compare($a, $b)
	{
		return ($a->get($this->field) < $b->get($this->field)) ? -1 : 1;
	}
}

/**
 * Sorts an array of DataItems based on the values in the specified field
 * @param array $list an array of DataItems
 * @param string $field the field to sort by
 * @return the sorted list
 */
function sortList(&$list, $field)
{
	$cmp = new DataItemFieldComparator($field);
	
	usort($list, array($cmp, "compare"));
	
	return $list;
}

/**
 * Utility method to update sort order for the selected class of objects based
 * on an array that maps primary keys to sort order values. This method is designed
 * to make drag and drop sorting easier to implement.
 * @param string $class the class of DataItem that is being resorted
 * @param string $field the field that contains the sort order value
 */
function updateSortOrder($class, $field = "sort_order", $params = null)
{
	if (!$params) $params = $_GET;
	$obj = new $class;
	$pk = $obj->getPrimaryKey();
	
	$tx = new DataTransaction();
	
	try
	{
		foreach($params[$pk] as $id => $sort_order)
		{
			checkNumeric($id);
			checkNumeric($sort_order);
	
			$item = new $class();
			$item->joinTransaction($tx);
	
			$item->filter = new InclusionFilter($pk, $field);
	
			$item->load($id);
			$item->set($field, $sort_order);
			$item->save();
		}
	
		$tx->commit();
	}
	catch(Exception $e)
	{
		$tx->rollback();
		throw $e;
	}
}

function toXML($tag, $objects, $header = null)
{
	global $config;
	
	if ($header === null)
	{
		$default_charset = $config["default_charset"];
		if (!$default_charset) $default_charset = "iso-8859-1";
		$header = "<?xml version=\"1.0\" encoding=\"{$default_charset}\"?>";
	}
	
	$xml = "$header\n<$tag>\n";
	
	foreach($objects as $obj)
	{
		$xml .= $obj->toXML(1);
	}
	$xml .= "</$tag>\n";
	
	return $xml;
}

function toJSON($items)
{
	$out = array();
	
	foreach($items as $item)
	{
		$out[] = $item->toJSON();
	}
	
	return "[".implode(", ", $out)."]";
}

function toJSONGrouped($items)
{
	$out = array();

	foreach($items as $key => $group)
	{
		$out[] = "\"$key\": ".toJSON($group);
	}
	
	return "{".implode(", ", $out)."}";
}

function fromJSON($class, $json)
{
	$data = json_decode($json);
	
	if (is_object($data))
	{
		$obj = new $class;
		$obj->copy($data);
		return $obj;
	}
	else if (is_array($data))
	{
		foreach($data as $datum)
		{
			$obj = new $class;
			$obj->copy($datum);
			$out[] = $obj;
		}
		
		return $out;
	}
	
	return null;
}

function displayFieldAsList($items, $field, $separator = ", ")
{
	return formatItems($items, "{".$field."}", $separator);
}	

/**
 * Format a list of DataItems using the specified templated. The list
 * is returned as a string with the specifed separator between records.
 * @param array $items the DataItems to format
 * @param string $template the format template
 * @param string $separator the separator to use between records
 * @return string
 */
function formatItems($items, $template, $separator = "")
{
	$list = "";
	$first = true;
	
	foreach($items as $item)
	{
		if (!$item) continue;
		if (!$first) $list .= $separator;
		$list .= $item->format($template);
		$first = false;
	}
	
	return $list;
}

function extractItems($items, $indexFormat, $valueFormat)
{
	$list = array();
	foreach($items as $item)
	{
		$list[$item->format($indexFormat)] = $item->format($valueFormat);
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
		$out[] = $item->get($field);
	}
	return $out;
}

/**
 * Extract the field values for the specified field from a list of DataItems and return
 * them in an array indexed by the primary key values of the source DataItems
 * @param $items the DataItems on which to perform the extraction
 * @param $field the name of the field to extract
 * @return array an array of the field values indexed by the primary key values from the DataItem list.
 */
function extractIndexedFieldValues($items, $field)
{
	$out = array();
	foreach($items as $item)
	{
		$out[$item->get($item->getPrimaryKey())] = $item->get($field);
	}
	
	return $out;
}


/**
 * Find the first item in the array that matches the supplied value for the specified field.
 * @param array $items the items to search
 * @param string $field the name of the field to match
 * @param mixed $value the value that must be matched
 * @return DataItem the first matching object, or null if no match is found
 */
function findItem($items, $field, $value)
{
	foreach($items as $item)
	{
		if ($item->get($field) == $value) return $item;
	}
	
	return null;
}

/**
 * Find the index of the first item in the array that matches the supplied value for the specified field.
 * @param array $items the items to search
 * @param string $field the name of the field to match
 * @param mixed $value the value that must be matched
 * @return mixed the index of the first matching object, or false if no match was found
 */

function findItemIndex($items, $field, $value)
{
	$idx = 0;
	
	foreach($items as $item)
	{
		if ($item->get($field) == $value) return $idx;
		++$idx;
	}
	
	return false;
}

function restoreHTML($text)
{
	$v = str_replace( array("[_[OPENTAG]_]", "[_[CLOSETAG]_]", "[_[AMPERSAND]_]"), array("<", ">", "&"),  $text);
	return $v;
}

/**
 * Dump a raw representation of a list of DataItems to CSV (one column per field, default format only)
 * @param $class class name of the items to be dumped
 * @param $items the items to dump
 * @param $filepath the output file path
 */
function dumpCSV($class, $items, $filepath, $filter = null, $append = false)
{
	$obj = new $class;
	$obj->filter = $filter;

	$fields = $obj->getFieldArray();
	
	if ($append)
	{
		$fp = fopen($filepath, "a");
	}
	else
	{
		$fp = fopen($filepath, "w");	
		fputcsv($fp, $fields);
	}
	
	foreach($items as $item)
	{
		$vals = array();
		foreach($fields as $field)
		{
			$vals[] = $item->get($field);
		}
		fputcsv($fp, $vals);
	}
	fclose($fp);
}
