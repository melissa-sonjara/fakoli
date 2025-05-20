<?php
/**\addtogroup scroll
 * @{
 */


/**
 * Manage schema updates for the scroll component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("scroll", "component");

class ScrollUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Added scroll and scroll_part tables",
		"1.1"	=>	"sql:Added styles and image_id fields to scroll_part table"
	);

	function __construct()
	{
		$this->component_name = "scroll";

		// sets the sqlFile name according to default
		parent::__construct();
	}
}

/**@}*/?>
