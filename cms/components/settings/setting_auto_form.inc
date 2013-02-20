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

Fakoli::usingFeature("auto_form");

class SettingAutoForm extends AutoForm
{
	var $settings;
	var $customSettings = array();
	
	function SettingAutoForm($component)
	{
		$this->component = $component;
		
		$this->settings = Query::create(Settings, "WHERE component=:component ORDER BY category, name")
				->bind(":component", $component->name)
				->execute();

		$this->buildForm($component->component_id);
	}
	
	function buildForm($component_id = 0)
	{
		$componentSettings = new ComponentSettings();

		foreach($this->settings as $setting)
		{
			$fields[$setting->name] = ($setting->field_type) ? $setting->field_type : "String";
			if ($setting->category) $this->group($setting->category, $setting->name);
		}

		$componentSettings->fields = $fields;
		parent::AutoForm($componentSettings, "POST", "?component_id={$component_id}");

		foreach($this->settings as $setting)
		{
			$this->data->set($setting->name, $setting->value);
			if($setting->annotation)
				$this->annotate($setting->name, $setting->annotation);

			/*
			 * Create custom renderers. If option is not
			 * a custom renderer name, then assume it is
			 * a set of options for SelectFieldRenderer.
		 	*/
			if($setting->options == "FileUploadFieldRenderer")
			{
				$renderers[$setting->settings_id] = new FileUploadFieldRenderer($this, $setting->name, prettify($setting->name), uploadSettingFile);
				$this->annotate($setting->name, "<a href='/action/settings/download?setting_id={$setting->settings_id}'>{$setting->value}</a>");
				
			}
			else if ($setting->options == "Permissions")
			{
				$renderer = new ChecklistFieldRenderer($this, $setting->name, $setting->annotation ? $setting->annotation : prettify($setting->name), SiteRole::getRolesArray());
				if ($setting->annotation) $this->annotate($setting->name, "");
				$renderer->horizontal = true;
				$renderer->nobr = true;
			}			
			else if($setting->options)
			{
				$optionList = $setting->formatOptionList();
				if(isset($optionList))
					$renderers[$setting->settings_id] = new SelectFieldRenderer($this, $setting->name, prettify($setting->name), $optionList);
			}
		}

		$this->submitLabel = "Save Changes";					
	}
	
	function writeSettings($settings)
	{
		foreach($settings as $setting)
		{
			if(array_key_exists($setting->field_type, Settings::$customTypes))
			{
				$type = Settings::$customTypes[$setting->field_type];
				$customSettings[] = $setting;
			}
			else
				$type = ($setting->field_type) ? $setting->field_type : "String";
				
			$fields[$setting->name] = $type;
		}
		
		return $fields;
	}
	
	function save()
	{
		$this->patchUpReferences();

		$this->configureValidators();

		$obj =& $this->data;
		$obj->fromPOST();
		
		if (isset($_POST["#FORMID#"]))
		{
			$this->id = $_POST["#FORMID#"];
		}

		$this->msg = $this->validator->validate();
		if ($this->msg != "") return false;

		$this->preProcessFields($pk, $filter);
	
		$indexedSettings = reindexList($this->settings, "name");
		
		foreach($obj->fields as $name => $type)
		{
			if(array_key_exists($name, $indexedSettings))
			{
				$setting = $indexedSettings[$name];
				$setting->value = $obj->$name;
				$setting->filter = new InclusionFilter("value");
				$setting->save();
				Cache::put("setting_{$setting->component}_{$setting->name}", $setting);
				
			}
		}
		Cache::invalidate("auto_form_defaults");
		Cache::invalidate("config");
		
		trace("Settings auto form returning true", 3);
		
		ComponentManager::fireEventTo($this->component->name, "ClearCache");
		
		
		return true;
	}
}

function uploadSettingFile($field, $setting)
{
	global $config;

	trace("uploadSettingFile called for $field", 3);

	if (!$_FILES[$field])
	{
		trace("No upload record for $field", 2);
		return false;
	}
	if ($_FILES[$field]["name"]=="")
	{
		trace("Upload name is empty", 2);
		return false;
	}

	/* Copy across the uploaded file */

	trace("Upload Base: {$config['uploadbase']}", 3);
	trace("Upload Directory: {$config['uploaddir']}", 3);

	$dir = $config["uploaddir"];
	$name = $_FILES[$field]["name"];
	$file = "$dir/$name";

	trace ("Uploading file to {$config['uploadbase']}/$file", 3);

	if (!file_exists("{$config['uploadbase']}/$dir"))
	{
		// If the directory does not exist, create it
		mkdir("{$config['uploadbase']}/$dir");
	}
	else if (file_exists("{$config['uploadbase']}/$file"))
	{
		// If a previous copy of the file already exists, remove it
		unlink("{$config['uploadbase']}/$file");
	}

	move_uploaded_file($_FILES[$field]["tmp_name"], "{$config['uploadbase']}/$file");
	chmod("{$config['uploadbase']}/$file", 0755);

	$setting->$field = "$file";
	 
	return true;
}


/*
 * Shows all settings on one page.
 */
class SettingPageAutoForm extends SettingAutoForm
{
	function SettingPageAutoForm($settings)
	{
		$this->settings = $settings;
		$this->buildForm();
	}	
}