<?php

class ModuleManager
{
	function ModuleManager()
	{
		
	}
	
	static function upgradeComponent()
	{
		$mgr = new ModuleUpgradeManager();
		$mgr->upgrade();
	}	
}