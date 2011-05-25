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
						"annotation"	=>	Text,
						"value"			=>	String,
						"field_type"	=>	String
					);
	
	// relationships
	var $relations = array();
	
	static $fieldTypes = array(
				"String"	=>	"String",
				"Number"	=>	"Number",
				"Boolean"	=>	"Boolean",
				"Date"		=>	"Date",
				"Currency"	=>	"Currency"
				//"Select"	=>	"SelectFieldRenderer",
				//"CheckList"	=>	"CheckListFieldRenderer"
	); 
	
	/*
	 * Not sure if it makes sense to allow these application
	 * setting types. Wait and see if it is needed.
	 */
	static $customTypes = array(
				//"Select"	=>	String,
				//"CheckList"	=>	String
	);
	
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

	
	static function setValue($component, $name, $value, $field_type, $annotation)
	{
		$setting = Settings::getValue($component, $name);
		if (!$setting)
		{
			$setting = new Settings();
			$setting->component = $component;
			$setting->name = $name;
			$setting->field_type = $field_type;
			$setting->annotation = $annotation;
		}
		
		$setting->value = $value;
		$setting->save();
		Cache::put("setting_{$component}_{$name}", $setting);
	}
	
	/*
	 * field_type: Number, String, Boolean
	 */
	static function setDefaultValue($component, $name, $value, $field_type = "String", $annotation = "")
	{
		$setting = new Settings();
		if ($setting->exists("WHERE component='$component' AND name='$name'")) return;
		Settings::setValue($component, $name, $value, $field_type, $annotation);
	}
	
	static function formatOptionList()
	{
		if(!$this->options)
			return;
			
		$options = explode("\n", $this->options);
		
		$optionList = array();
		foreach($options as $option)
		{
			$optionList[$option] = $option;
		}
		
		return $optionList;
	}
	
	static function getComponents()
	{
		return Query::create(Component, "WHERE name in (SELECT DISTINCT component FROM settings) ORDER BY name")->execute();
	}					
}

/*
 * Dummy data model to render a component's settinsg
 * as a form, using the "type" field of the setting
 * record to determine the renderer.
 * 
 * The fields array is built in the settings.inc 
 * script and given to AutoForm.
 */
class ComponentSettings extends DataItem
{
	var $fields = array();

	function ComponentSettings()
	{
		$this->primary_key = "settings_id";
		$this->table = "settings";
		
		$this->DataItem(func_get_args());
	}
	
	
}

?>