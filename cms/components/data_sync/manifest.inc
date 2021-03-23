<?php
/**\defgroup data_sync Data Sync
 * @{
 */


class DataSyncManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "data_sync";
		$component->description = "Generic Data Import Framework";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getStyles()
	{
		return array(
				"/components/data_sync/css/data_sync.css"
		);
	}
		
	static function getScripts()
	{
		return array(
				"/components/data_sync/js/data_sync_manager.js"
		);
	}
	
	static function getAdminMenu()
	{
		return array
		(
				"Site Configuration"	=> array
				(
						"Data Import"	=>	array("page"		=>	"/admin/data_import",
												  "role"		=>	"admin",
												  "weight"	=>	100,
												  "icon"		=>	"database")
				)
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"				=>	array(DataSyncManager, upgradeComponent),
					"RegisterSerializationHandler"	=>	array(DataSyncManager, registerSerializationHandler),
					"ComponentScanComplete"			=>	array(DataSyncManager, setDefaults),
					"RegisterDataSyncAdapter"		=>	array(DataSyncManager, registerCSVAdapter)
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>