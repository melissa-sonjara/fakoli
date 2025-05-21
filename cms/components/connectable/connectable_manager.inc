<?php
/**\addtogroup connectable
 * @{
 */



interface Connectable
{
	function getConnectableIcon();
	function getConnectableDisabledIcon();
	function getConnectableConstraint();
	function getConnectableLink($source);
	function getConnectableTitle();
	function getConnectableHome();
	function populateSelectionTable($table);
}

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the connectable component.
 */

class ConnectableManager
{
	static $connectables = array();	///< Connectable type map defining which items can be connected together
	static $classLookup = array();	///< Look-up table of connectable classes from their primary keys
	static $contextRouter = "";
	
	function __construct()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function onInitialize()
	{
		ComponentManager::fireEvent("RegisterConnectables");
	}
	
	/**
	 * Register a connectable class and the list of classes it can connect with
	 * @param string $connectable the connectable class
	 * @param array $targets array of target classes to which it can be connected
	 */
	static function registerConnectable($connectable, $targets)
	{
		ConnectableManager::$connectables[$connectable] = $targets;
		$obj = new $connectable;
		ConnectableManager::$classLookup[$obj->getPrimaryKey()] = $connectable;	
	}

	/**
	 * Create and populate a Connectable DataItem based on its primary key as passed in
	 * on the requesting URL.
	 * @return Connectable 
	 */
	static function getConnectableFromRequest()
	{
		foreach (ConnectableManager::$classLookup as $primaryKey => $class)
		{
			if (array_key_exists($primaryKey, $_GET))
			{
				$id = checkNumeric($_GET[$primaryKey]);
				$obj = new $class;
						
				if ($id) $obj->load($id);
				return $obj;
			}
		}
		
		return null;
	}
	
	/**
	 * Safely instantiate a Connectable.
	 * @param string $class
	 * @param int $id
	 * @throws FakoliException
	 * @return Connectable
	 */
	static function instantiate($class, $id)
	{
		if (!array_key_exists($class, ConnectableManager::$connectables))
		{
			throw new FakoliException(htmlSafe($class)." is not a registered Connectable");
		}
		
		checkNumeric($id);
		
		$obj = new $class;
		if ($id) $obj->load($id);
		return $obj;
	}
	
	/**
	 * Determine whether two objects or classes can connect based on the registered connectable map.
	 * @param mixed $source either a Connectable DataItem or its class name
	 * @param mixed $target either a Connectable DataItem or its class name
	 * @return boolean true if the items can be connected, false otherwise
	 */
	static function canConnect($source, $target)
	{
		if (is_object($source)) $source = get_class($source);
		if (is_object($target)) $target = get_class($target);
		
		$targets = ConnectableManager::getTargetClasses($source);
		
		foreach($targets as $t)
		{
			if ($t == $target) return true;
		}
		
		return false;
	}
	
	/**
	 * Check whether a connection exists between the given source and target objects
	 * @param DataItem $source the source object
	 * @param DataItem $target the target object
	 */
	static function connectionExists($source, $target)
	{
		return Query::create('ConnectionRecord', "WHERE source_class=:sc AND source_id=:si AND target_class=:tc AND target_id=:ti")
					 ->bind(":sc", get_class($source),
					 		":si", $source->get($source->getPrimaryKey()),
					 		":tc", get_class($target),
					 		":ti", $target->get($target->getPrimaryKey()))
					 ->exists();
	}
	
	/**
	 * Add a connection between the given source and target objects. If the target object
	 * registered itself as connectable to the source object, then a back-link is automatically
	 * inserted.
	 * @param DataItem $source the source object
	 * @param DataItem $target the target object
	 */
	static function addConnection($source, $target)
	{
		if (ConnectableManager::connectionExists($source, $target)) return;
		
		$sourceDefn = ConnectableManager::$connectables[get_class($source)];
		$targetDefn = ConnectableManager::$connectables[get_class($target)];
		
		$sourceClass = get_class($source);
		$targetClass = get_class($target);
		
		if (!$sourceDefn || !$targetDefn)
		{
			throw new FakoliException("Attempt to link an object that is not a registered Connectable");
		}
		 
		if (!ConnectableManager::canConnect($sourceClass, $targetClass))
		{
			throw new FakoliException("Cannot create a link between a $sourceClass and a $targetClass");
		}

		$record = new ConnectionRecord();
		$record->source_class = $sourceClass;
		$record->source_id = $source->get($source->getPrimaryKey());
		$record->target_class = $targetClass;
		$record->target_id = $target->get($target->getPrimaryKey());
		
		$record->save();
		
		if (ConnectableManager::canConnect($targetClass, $sourceClass))
		{
			// Also insert back-link
			$record = new ConnectionRecord();
			$record->source_class = $sourceClass;
			$record->source_id = $target->get($target->getPrimaryKey());
			$record->target_class = $targetClass;
			$record->target_id = $source->get($source->getPrimaryKey());
			
			$record->save();
		}		
	}
	
	/**
	 * Remove any connection that exists between the source and target object
	 * (including back-links if they were created).
	 * @param DataItem $source the source object
	 * @param DataItem $target the target object
	 */
	static function removeConnection($source, $target)
	{
		$record = new ConnectionRecord();
		$sourceID = checkNumeric($source->get($source->getPrimaryKey()));
		$targetID = checkNumeric($target->get($target->getPrimaryKey()));
		
		$sourceClass = get_class($source);
		$targetClass = get_class($target);
		
		$record->delete("WHERE (source_class='{$sourceClass}' AND source_id={$sourceID} AND target_class='{$targetClass}' AND target_id={$targetID}') " .
						"OR (source_class='{$targetClass}' AND source_id={$targetID} AND target_class='{$sourceClass}' AND target_id={$sourceID})");
	}
	
	/**
	 * Remove any connection that exists between the source and objects of the specified target class
	 * (including back-links if they were created).
	 * @param DataItem $source the source object
	 * @param DataItem $targetClass the target class
	 */
	static function removeConnections($source, $targetClass)
	{
		$record = new ConnectionRecord();
		$sourceClass = get_class($source);
		$sourceID = checkNumeric($source->get($source->getPrimaryKey()));

		$record->delete("WHERE (source_class='{$sourceClass}' AND source_id={$sourceID} AND target_class='{$targetClass}') " .
						"OR (source_class='{$targetClass}' AND target_class='{$sourceClass}' AND target_id={$sourceID})");
	}
	
	/**
	 * Remove all the connections to and from the given object. Use this method
	 * when the object is being deleted from the database to ensure that you don't 
	 * end up with stale connections.
	 * @param DataItem $object the object for which connections are to be removed
	 */
	static function removeAllConnections($object)
	{
		$record = new ConnectionRecord();
		$objectID = $object->get($object->getPrimaryKey());
		$class = get_class($object);
		$record->delete("WHERE (source_class='{$class}' AND source_id={$objectID}) OR (target_class='{$class}' AND target_id={$objectID})");
	}
	
	/**
	 * Return an array of connectable target classes for the specified item
	 * @param Connectable $sourceItem the source DataItem (must implement the Connectable interface)
	 * @return Array of class names
	 * @throws FakoliException
	 */
	static function getTargetClasses($sourceItem)
	{
		$sourceClass = is_object($sourceItem) ? get_class($sourceItem) : $sourceItem;
		
		if (!array_key_exists($sourceClass, ConnectableManager::$connectables))
		{
			throw new FakoliException("$sourceClass is not a registered connectable class");
		}
		
		return ConnectableManager::$connectables[$sourceClass];
		
	}
	

	/**
	 * Returns the connected items for the specified source that are of the specified class
	 * 
	 * @param Connectable $sourceItem the source DataItem (must implement the Connectable interface)
	 * @param string $targetClass The class of DataItem Connectable objects that are to be returned
	 * @param string $constraint additional constraints to be applied
	 * @throws FakoliException
	 */
	static function getConnectedItems($sourceItem, $targetClass, $constraint = "")
	{
		$query = ConnectableManager::generateConnectedQuery($sourceItem, $targetClass, $constraint);		
		return Query::create($targetClass, $query)
					->bind(":sc", get_class($sourceItem), ":si", $sourceItem->get($sourceItem->getPrimaryKey()), ":tc", $targetClass)
					->execute();
	}
	
	/**
	 * Returns the number of connected items for the specified source that are of the specified class
	 * 
	 * @param Connectable $sourceItem the source DataItem (must implement the Connectable interface)
	 * @param string $targetClass The class of DataItem Connectable objects that are to be returned
	 * @param string $constraint additional constraints to be applied
	 * @throws FakoliException
	 */
	static function getConnectedItemCount($sourceItem, $targetClass, $constraint = "")
	{
		$query = ConnectableManager::generateConnectedQuery($sourceItem, $targetClass, $constraint);		
		return Query::create($targetClass, $query)
					->bind(":sc", get_class($sourceItem), ":si", $sourceItem->get($sourceItem->getPrimaryKey()), ":tc", $targetClass)
					->executeValue("COUNT(1)");
	}
	
	private static function generateConnectedQuery($sourceItem, $targetClass, $constraint)
	{
		$constraint = preg_replace("/^\\s*WHERE/i", " AND", $constraint);
		
		$sourceClass = get_class($sourceItem);
		
		$targetClasses = ConnectableManager::getTargetClasses($sourceItem);
		
		trace(print_r($targetClasses, true), 3);
		
		if (!ConnectableManager::canConnect($sourceClass, $targetClass))
		{
			throw new FakoliException("$sourceClass cannot be connected to $targetClass");
		}
		
		$obj = new $targetClass;
		$pk = $obj->getPrimaryKey();
		
		return "WHERE $pk in (SELECT target_id FROM connection_record WHERE source_class=:sc AND source_id=:si AND target_class = :tc) $constraint";
	}
	static function upgradeComponent($version)
	{
		$mgr = new ConnectableUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>
