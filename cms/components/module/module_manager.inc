<?php

class ModuleManager
{
	function ModuleManager()
	{
		
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ModuleUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("module", "Modules", new SimpleSerializationHandler(Module));
		return true;
	}
}