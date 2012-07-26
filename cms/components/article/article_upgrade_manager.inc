<?php
/*
 * Manage Article component upgrades/updates.
 * 
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 6/2/2011
 * 
 */

Fakoli::using("article", "component");

class ArticleUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
			"1.0"		=>	"sql:Initial Installation",
			"1.1"		=>	"sql:Added sort_order field to article table"
	);
	
	function ArticleUpgradeManager()
	{
		$this->component_name = "article";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}