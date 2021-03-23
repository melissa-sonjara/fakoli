<?php
/**\addtogroup developer_tools
 * @{
 */


Fakoli::using("developer_tools", "section");

class ContentInventoryHelper
{
	var $showAll = false;
	var $map;
	
	function __construct($showAll)
	{
		$this->showAll = $showAll;
		$this->map = null;
	}
	
	function onStartRow($item)
	{
		$map = DeveloperToolsManager::formatSections($item);
		
		if (!$this->showAll)
		{
			return ($map != "<em>Not mapped</em>");
		}
		
		return ($map != "<em>Not mapped</em>") ? "mapped" : "unmapped";
	}
}
/**@}*/?>