<?php
class CustomReportManager
{
	private static $managerClassMap = null;
	
	static function registerManagerClass($target, $managerClass, $description = "")
	{
		CustomReportManager::$managerClassMap[$target] = array("class" => $managerClass, "description" => $description);
	}
	
	static function registerManagerClasses()
	{
		if (is_array(CustomReportManager::$managerClassMap)) return;
		ComponentManager::fireEvent("RegisterCustomReportManagers");
	}
	
	static function getManagerClass($target)
	{
		CustomReportManager::registerManagerClasses();
		$record = CustomReportManager::$managerClassMap[$target];
		return ($record) ? $record["class"] : null;
	}
	
	static function getManagerDescription($target)
	{
		CustomReportManager::registerManagerClasses();
		$record = CustomReportManager::$managerClassMap[$target];
		return ($record) ? $record["description"] : null;
	}
	
	static function createManager($target)
	{
		$class = CustomReportManager::getManagerClass($target);
		return ($class) ? new $class : null;
	}
	
	static function getTargets()
	{
		CustomReportManager::registerManagerClasses();
		if (!count(CustomReportManager::$managerClassMap)) return null;
		
		$targets = array_keys(CustomReportManager::$managerClassMap);
		sort($targets);
		return $targets;
	}
	
	static function getTarget($manager)
	{
		CustomReportManager::registerManagerClasses();
		if (!count(CustomReportManager::$managerClassMap)) return "";
		
		$cl = get_class($manager);
		foreach(CustomReportManager::$managerClassMap as $target => $record)
		{
			if ($record["class"] == $cl) return $target;
		}
		
		return "";
	}
}