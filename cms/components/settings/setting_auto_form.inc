<?php
Fakoli::usingFeature("auto_form");

class SettingAutoForm extends AutoForm
{
	var $settings;
	var $customSettings = array();
	
	function SettingAutoForm($component)
	{
		$this->settings = Query::create(Settings, "WHERE component=:component ORDER BY name")
				->bind(":component", $component->name)
				->execute();
			
		$componentSettings = new ComponentSettings();

		/*
		 * Translate custom setting types (CheckList and Select)
		 * back into the general field types (String). Then,
		 * after fields are built, create their custom renderer.
		 */
		foreach($this->settings as $setting)
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

		$componentSettings->fields = $fields;
		parent::AutoForm($componentSettings, "POST", "?component_id={$component->component_id}");

		foreach($this->settings as $setting)
		{
			$this->data->set($setting->name, $setting->value);
			if($setting->annotation)
				$this->annotate($setting->name, $setting->annotation);	
		}
		
		
		/*
		 * Create CheckListFieldRenderer or SelectFieldRenderer using
		 * the renderer type specified in the setting type field and
		 * the options field value.
		 */
		if(count($customSettings) > 0)
		{
			foreach($customSettings as $setting)
			{
				$rendererName = Settings::$fieldTypes[$setting->field_type];
				$renderers[$setting->settings_id] = new $rendererName($this, $setting->name, prettify($setting->name, $setting->formatOptionList()));
			}
		}		
			
		$this->submitLabel = "Save Changes";
			
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
		
		echo "<pre>"; print_r($obj); echo "</pre>";
		
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
		return true;
	}
}