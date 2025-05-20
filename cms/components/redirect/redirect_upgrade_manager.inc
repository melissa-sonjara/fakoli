<?php
/**\addtogroup redirect
 * @{
 */


/**
 * Manage schema updates for the redirect component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("redirect", "component");

class RedirectUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Added redirect table",
		"1.1"	=>	"sql:Added override field to redirect table",
		"1.2"	=>	"sql:Added exemption_flag and exemption_roles fields to redirect table",
		"1.3"	=>	"sql:Added preserve_query_string flag to redirect table"
	);

	function __construct()
	{
		$this->component_name = "redirect";

		// sets the sqlFile name according to default
		parent::__construct();
	}
}

/**@}*/?>
