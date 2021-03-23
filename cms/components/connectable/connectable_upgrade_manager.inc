<?php
/**\addtogroup connectable
 * @{
 */


/**
 * Manage schema updates for the connectable component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andrew Green
 */


Fakoli::using("connectable", "component");

class ConnectableUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Created connection_record table and associated indices"
	);

	function ConnectableUpgradeManager()
	{
		$this->component_name = "connectable";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

/**@}*/?>