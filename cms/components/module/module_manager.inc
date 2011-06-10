<?php

class ModuleManager
{
	function ModuleManager()
	{
		
	}
	
	static function upgradeComponent()
	{
		$mgr = new MenuUpgradeManager();
		$mgr->upgrade();
	}	
}