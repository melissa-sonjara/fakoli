<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the developer_tools component.
 */

class DeveloperToolsManager
{
	function DeveloperToolsManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function registerIcons($icons)
	{
		$icons["Developer Tools"] = "code";
		return $icons;
	}
	
	static function formatDefault($item)
	{
		return $item->format();
	}
	
	static function formatSections($item)
	{
		$sectionContent = Query::create(SectionContent, "WHERE identifier=:i")
							   ->bind(":i", $item->identifier)
							   ->execute();
		
		$map = formatItems($sectionContent, "{Section.section}", ", ");
		if (!$map) $map = "<em>Not mapped</em>";
		return $map;
	}
	
	
	static function formatComponent($item)
	{
		return $item->component;
	}
	
	static function formatPriority($item)
	{
		return $item->priority;
	}
	
	static function formatHandler($item)
	{
		if (is_array($item->handler)) return implode("::", $item->handler);
		return $item->handler;
	}
}

?>