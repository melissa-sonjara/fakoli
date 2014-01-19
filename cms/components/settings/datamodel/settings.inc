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

Fakoli::usingFeature("cache", "field_renderers");

/**
 * The Settings class provides components with a common API for specifying application
 * settings and configuration values. These settings are stored in the application
 * database and can be modified through the site administration screens.
 * 
 * @author andy
 */
class Settings extends DataItem
{
	// Fields
	var $fields = array("settings_id"	=>	Number,
						"component"		=>	String,
						"category"		=>	String,
						"name"			=>	String,
						"annotation"	=>	Text,
						"options"		=>	Text,
						"value"			=>	String,
						"field_type"	=>	String,
						"weight"		=>	Number
					);
	
	// relationships
	var $relations = array();
	
	static $fieldTypes = array(
				"String"	=>	"String",
				"Number"	=>	"Number",
				"Boolean"	=>	"Boolean",
				"Date"		=>	"Date",
				"Currency"	=>	"Currency",
				"Text"		=>	"Text"
	); 
	
	
	function Settings()
	{
		$this->primary_key = "settings_id";
		$this->table = "settings";
		
		$this->DataItem(func_get_args());
	}
	
	static function getSetting($component, $name)
	{
		try
		{
			$setting = Query::create(Settings, "WHERE component=:component AND name=:name")
							->bind(":component", $component, ":name", $name)
							->executeSingle();
			Cache::put("setting_{$component}_{$name}", $setting->value);
			return $setting;
		}
		catch(DataNotFoundException $e)
		{
			return null;
		}		
	}
	
	/**
	 * Retrieve the value of the specified Setting
	 * @param string $component the name of the component that owns the setting
	 * @param string $name the name of the setting
	 */
	static function getValue($component, $name)
	{
		$value = Cache::get("setting_{$component}_{$name}");
		if ($value !== null) return $value;
		
		try 
		{
			$setting = Query::create(Setting, "WHERE component=:c and name=:n")
						->filter(new InclusionFilter("value"))
					    ->bind(":c", $component, ":n", $name)
						->executeSingle();
			Cache::put("setting_{$component}_{$name}", $setting->value);
			return $setting->value;
		}
		catch (DataNotFoundException $e) 
		{
			return null;
		}
	}
	
	/**
	 * Sets the value of the given component setting. 
	 * 
	 * @param string component the name of the component that owns the setting
	 * @param string $name the name of the setting (this will be automatically prettified when presented to the user)
	 * @param mixed  $value the default value for the setting. The value will be overwritten, even if previously set
	 * @param string $field_type the type of field renderer to use to display the setting in the admin interface
	 * @param string $annotation any help annotation to display on the form (optional)
	 * @param string $category the sub-category within the form to which the setting belongs. Settings with the same
	 *               category name are grouped together into fieldsets.
	 * @param number $weight a weighting factor to control display order on the Application Settings form. Controls
	 *               are sorted (within categories) first by weight, then alphabetically (optional).
	 */
	static function setValue($component, $name, $value, $field_type, $annotation, $category, $options, $weight = 0)
	{
		$setting = Settings::getSetting($component, $name);
		if (!$setting)
		{
			$setting = new Settings();
			$setting->component = $component;
			$setting->name = $name;
			$setting->field_type = $field_type;
			$setting->annotation = $annotation;
			$setting->options = $options;
			$setting->category = $category;
			$setting->weight = $weight;
		}
		
		$setting->value = $value;
		$setting->save();
		Cache::put("setting_{$component}_{$name}", $setting->value);
	}
	
	/**
	 * Sets the default value of the given component setting. Creating settings for a component using this
	 * method will generate a tab under the Application Settings area of the site administration interface,
	 * allowing your users to control your component's behavior.
	 * 
	 * @param string component the name of the component that owns the setting
	 * @param string $name the name of the setting (this will be automatically prettified when presented to the user)
	 * @param mixed  $value the default value for the setting. If the setting has already been defined, the value
	 *                will not be overwritten
	 * @param string $field_type the type of field renderer to use to display the setting in the admin interface
	 * @param string $annotation any help annotation to display on the form (optional)
	 * @param string $category the sub-category within the form to which the setting belongs. Settings with the same
	 *               category name are grouped together into fieldsets.
	 * @param number $weight a weighting factor to control display order on the Application Settings form. Controls
	 *               are sorted (within categories) first by weight, then alphabetically (optional).
	 */
	static function setDefaultValue($component, $name, $value, $field_type = "String", $annotation = "", $category = "", $options = "", $weight = 0)
	{
		$setting = Settings::getSetting($component, $name);
		if ($setting) 
		{
			$setting->annotation = $annotation;
			$setting->category = $category;
			$setting->options = $options;
			$setting->weight = $weight;
			$setting->save();
			return;
		}
		
		Settings::setValue($component, $name, $value, $field_type, $annotation, $category, $options, $weight);
	}
	
	/**
	 * Set a Permission value indicating which SiteRoles have the given permission
	 * @param string $component the name of the component that owns the permission
	 * @param string $name the unique name of the permission
	 * @param string $annotation short description of the permission 
	 * @param string $value comma-separated list of roles that have the permission
	 * @param int $weight optional weight parameter to control display order
	 */
	static function setPermission($component, $name, $annotation, $value, $weight)
	{
		Settings::setValue($component, $name, $value, "String", $annotation, "Permissions", "Permissions", $weight);		
	}
	
	/**
	 * Set a default Permission value indicating which SiteRoles have the given permission
	 * @param string $component the name of the component that owns the permission
	 * @param string $name the unique name of the permission
	 * @param string $annotation short description of the permission 
	 * @param string $value comma-separated list of roles that have the permission
	 * @param int $weight optional weight parameter to control display order
	 */
	static function setDefaultPermission($component, $name, $annotation, $value, $weight)
	{
		Settings::setDefaultValue($component, $name, $value, "String", $annotation, "Permissions", "Permissions", $weight);		
	} 
	
	/**
	 * Check whether a user has a specific permission
	 * @param string $component the component that owns the permission
	 * @param string $name the unique name of the permission
	 * @param object $account (optional) user account to test. If not specified the currently logged in user is tested.
	 */
	static function checkPermission($component, $name, $account = null)
	{
		$roles = Settings::getValue($component, $name);
		
		return checkRole($roles, $account);
	}
	
	/**
	 * Asserts whether the current user has a specific permission,
	 * optionally redirecting to a specified location if they do not.
	 * @param string $component the component that owns the permission
	 * @param string $name the unique name of the permission
	 * @param string $redirect URL to redirect to on failure
	 */
	static function assertPermission($component, $name, $redirect = "")
	{
		$roles = Settings::getValue($component, $name);
		
		Fakoli::assertRole($roles, $redirect);
	}

	/**
	 * Check all the permissions specified in the given string. If the user
	 * belongs to a role with ONE of the specified permissions, return true.
	 * If the user does not, return false.
	 * 
	 * @param string $permissions permission list in the form "[component]:[permission],[component]:[permission]..."
	 * @param object $account (optional) user account to test. If not specified the currently logged in user is tested.
	 */
	static function checkPermissions($permissions, $account = null)
	{
		if ($permissions == "") return true;
		
		$perms = explode(",", $permissions);
		
		foreach($perms as $perm)
		{
			list($component, $permission) = explode(":", $perm);
			if (Settings::checkPermission($component, $permission, $account)) return true;
		}
	
		return false;
	}

	function formatOptionList()
	{
		if(!$this->options)
			return null;
			
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

	static function createPermissionsFieldRenderer($form, $field, $label = "")
	{
		$permissions = Query::create(Settings, "WHERE category='Permissions' ORDER BY component, annotation")->execute();
		
		if (count($permissions) == 0) 
		{
			$form->hide($field);			
			return null;
		}
		
		$options = array();
		
		foreach($permissions as $p)
		{
			$c = prettify($p->component);
			
			if (!array_key_exists($c, $options)) $options[$c] = array();
			$options[$c]["{$p->component}:{$p->name}"] = $p->annotation;
		}
		
		$renderer = new CheckListFieldRenderer($form, $field, $label, $options, true);
		$renderer->setSize(400, 150);
		return $renderer;			
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