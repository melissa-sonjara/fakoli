<?php
/**\addtogroup process
 * @{
 */


/**
 * Manage schema updates for the process component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("process", "component");

class ProcessUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		//"x.x"	=>	"description"
	);

	function __construct()
	{
		$this->component_name = "process";

		// sets the sqlFile name according to default
		parent::__construct();
	}
}

/**@}*/?>
