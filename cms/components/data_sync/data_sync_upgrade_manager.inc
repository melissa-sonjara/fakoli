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
		"1.1"	=>	"sql:Added required column to data_import_field_mapping",
		"1.2"	=>	"sql:Added grouping column to data_import_field_mapping",
		"1.3"	=>	"sql:Added data_import_profile table",
		"1.4"	=>	"sql:Added class field to data_import_profile",
		"1.5"	=>	"sql:Added notes field to data_import_field_mapping",
		"1.6"	=>	"sql:Fixed missing default value for notes field on data_import_field_mapping table"
	);

	function DataSyncUpgradeManager()
	{
		$this->component_name = "data_sync";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>