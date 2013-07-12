<?php
/**
 * Manage schema updates for the data_sync component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("data_sync", "component");

class DataSyncUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Added data_import_field_mapping table",
		"1.1"	=>	"sql:Added required column to data_import_field_mapping"
	);

	function DataSyncUpgradeManager()
	{
		$this->component_name = "data_sync";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>