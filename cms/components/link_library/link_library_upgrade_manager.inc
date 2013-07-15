<?php
/**
 * Manage schema updates for the link_library component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("link_library", "component");

class LinkLibraryUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.1" => "sql:Create tables",
		"1.2" => "sql:Add enabled flag to link_library",
		"1.3" => "sql:Added hidden field to link_library"
	);

	function LinkLibraryUpgradeManager()
	{
		$this->component_name = "link_library";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>