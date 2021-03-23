<?php
/**\addtogroup sharing
 * @{
 */


/**
 * Manage schema updates for the sharing component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("sharing", "component");

class SharingUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.1"	=>	"sql:Created share_token and share_access tables",
		"1.2"	=>	"sql:Added last_updated and last_updated_by_id field to share_token table"
	);

	function SharingUpgradeManager()
	{
		$this->component_name = "sharing";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

/**@}*/?>