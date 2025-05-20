<?php
/**\addtogroup component
 * @{
 */




class ComponentUpdateLog extends DataItem
{
	var $table = "component_update_log";
	var $primary_key = "component_update_log_id";
	
	var $fields = array(
			"component_update_log_id"	=>	'Number',
			"component"					=>	'String',
			"version_number"			=>	'Number',
			"date_updated"				=>	'DateTime',
			"description"				=>	'Text',
			"filename"					=>	'String'
	);
	
	
	static function recordUpdate($component, $version_number, $description = "", $fileName)
	{
		$log = new ComponentUpdateLog();
		$log->component = $component;
		$log->version_number = $version_number;
		$log->description = $description;
		$log->filename = $fileName;
		$log->date_updated = now();
		
		$log->save();
	}
	
	static function getLatestVersion($component)
	{
		return Query::create(ComponentUpdateLog, "WHERE component=:component")
			->bind(":component", $component)
			->executeValue("MAX(version_number)");
	}
	
	static function getLogs($component_name)
	{
		return Query::create(ComponentUpdateLog, "WHERE component=:component_name ORDER BY version_number")
			->bind(":component_name", $component_name)
			->execute();
	}
}

/**@}*/?>
