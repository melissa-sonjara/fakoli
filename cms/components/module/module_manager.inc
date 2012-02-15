<?php

class ModuleSerializationHandler
{
	function ModuleSerializationHandler()
	{
	}
	
	function export()
	{
		$modules = Query::create(Module, "ORDER BY module_id")->execute();
		$xml .= toXML("ModuleList", $modules, "");
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		trace("== Importing Modules", 3);
		
		$moduleNodes = $doc->getElementsByTagName("Module");
	
		$modules = array();
		
		foreach($moduleNodes as $node)
		{
			$module = new Module();
			$module->fromXML($node);
			$modules[] = $module;
		}
		
		$m = new Module();
		$m->joinTransaction();
		$m->deleteAll();
		$m->disablePrimaryKey();
		
		foreach($modules as $module)
		{
			$module->_disablePK = true;
			$module->joinTransaction($tx);
			$module->save();
		}
			
		$m->enablePrimaryKey();
	}	
}

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
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("module", "Modules", new ModuleSerializationHandler());
		return true;
	}
}