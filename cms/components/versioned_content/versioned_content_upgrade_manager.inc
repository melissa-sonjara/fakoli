<?php
/**\addtogroup versioned_content
 * @{
 */


/**
 * Manage schema updates for the versioned_content component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("versioned_content", "component");

class VersionedContentUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Created content_version table",
		"1.1"	=>	"sql:Delete content_version records with 0 value primary keys that resulted from a bug"
	);

	function VersionedContentUpgradeManager()
	{
		$this->component_name = "versioned_content";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

/**@}*/?>