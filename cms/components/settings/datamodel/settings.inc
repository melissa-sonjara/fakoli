<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::usingFeature("cache");

class Settings extends DataItem
{
	// Fields
	var $fields = array("settings_id"	=>	Number,
						"component"		=>	String,
						"name"			=>	String,
						"value"			=>	String);
	
	// relationships
	var $relations = array();
	
	function Settings()
	{
		$this->primary_key = "settings_id";
		$this->table = "settings";
		
		$this->DataItem(func_get_args());
	}
	
	static function getValue($component, $name)
	{
		$setting = (Cache::get("setting_{$component}_{$name}"));
		
		if (!$setting)
		{
			try
			{
				$setting = Query::create(Settings, "WHERE component=:component AND name=:name")
								->bind(":component", $component, ":name", $name)
								->executeSingle();
				Cache::put("setting_{$component}_{$name}", $setting);
			}
			catch(DataNotFoundException $e)
			{
				return null;
			}		
		}
		
		return $setting->value;
	}
	
	static function setValue($component, $name, $value)
	{
		$setting = Settings::getValue($component, $name);
		if (!$setting)
		{
			$setting = new Settings();
			$setting->component = $component;
			$setting->name = $name;
		}
		
		$setting->value = $value;
		$setting->save();
		Cache::put("setting_{$component}_{$name}", $setting);
	}
	
	static function setDefaultValue($component, $name, $value)
	{
		$setting = new Settings();
		if ($setting->exists("WHERE component='$component' AND name='$name'")) return;
		Settings::setValue($component, $name, $value);
	}
	
	static function getComponents()
	{
		return Query::create(Component, "WHERE name in (SELECT DISTINCT component FROM settings) ORDER BY name")->execute();
	}					
}


?>