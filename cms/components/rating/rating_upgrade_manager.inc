<?php
/**
 * Manage schema updates for the rating component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andrew Green
 */


Fakoli::using("rating", "component");

class RatingUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Create rating table and rating_statistics view",
		"1.1"	=>	"sql:Added an index to rating table to improve performance"
	);

	function RatingUpgradeManager()
	{
		$this->component_name = "rating";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>