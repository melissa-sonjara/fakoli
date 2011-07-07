<?php

class ReportManagerManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "report_manager";
		$component->description = "General purpose manager for configurable reporting screens";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 5;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getScripts()
	{
		return array("/fakoli/report_manager/js/report_manager.js");
	}
}

?>