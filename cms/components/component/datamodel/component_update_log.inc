<?php


class ComponentUpdateLog extends DataItem
{
	var $fields = array(
			"component_update_log_id"	=>	Number,
			"component"					=>	String,
			"version_number"			=>	Number,
			"date_updated"				=>	Timestamp,
			"description"				=>	Text
	);
	
	
	static function recordUpdate($component, $version_number, $description = "")
	{
		$log = new ComponentUpdateLog();
		$log->component = $component;
		$log->version_number = $version_number;
		$log->description = $description;
		$log->save();
	}
	
	static function getLatestVersion($component)
	{
		return Query::create(ComponentUpdateLog, "WHERE component=:component")
			->bind(":component", $component)
			->executeValue("MAX(version_number)");
	}
	
	function ComponentUpdateLog()
	{
		$this->table = "component_update_log";
		$this->primary_key = "component_update_log_id";
		
		$this->DataItem(func_get_args());		
	}
}