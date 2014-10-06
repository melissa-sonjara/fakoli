<?php
/**
 * Manage schema updates for the rss_feed component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Fiifi Baidoo
 */


Fakoli::using("rss_feed", "component");

class RssFeedUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		//"x.x"	=>	"description"
	);

	function RssFeedUpgradeManager()
	{
		$this->component_name = "rss_feed";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>