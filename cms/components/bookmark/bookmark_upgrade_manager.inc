<?php
/**
 * Manage schema updates for the bookmark component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("bookmark", "component");

class BookmarkUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Initial Version"
	);

	function BookmarkUpgradeManager()
	{
		$this->component_name = "bookmark";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>