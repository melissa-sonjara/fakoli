<?php
/**\addtogroup module
 * @{
 */



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
	
	/**
	 * This utility method adds a code module record for the specified PHP code file. This can be used within 
	 * upgrade scripts to programmatically deploy modules.
	 * 
	 * @param string $title human-readable name for the module
	 * @param string $php_code_file file path to the module code, relative either to the home directory or to a component directory if a component is specified
	 * @param string $component (optional) name of the component containing the module code
	 * @param boolean $global (optional) true if the module is global in scope
	 * @param string $global_position (optional) position to display at for global modules. This is required if $global is true.
	 * @param string $css_class (optional) CSS class to be applied to the module when renderered
	 * @return Module
	 */
	static function addCodeModule($title, $php_code_file, $component = null, $global = false, $global_position = "", $css_class = "")
	{
		$file = ($component) ? "{$component},{$php_code_file}" : $php_code_file;
		
		$module = new Module();
		$module->title = $title;
		$module->content_type = "Code";
		$module->php_code_file = $file;
		$module->global = $global;
		$module->global_position = $global_position;
		$module->css_class = $css_class;
		$module->save();
		return $module;
	}
}
/**@}*/?>