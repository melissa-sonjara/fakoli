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

Fakoli::using("settings");
Fakoli::usingFeature("data_tabs");

class SettingsSerializationHandler
{
	function SettingsSerializationHandler()
	{
	}

	function export()
	{
		return SerializationManager::serialize(Settings, "ORDER BY settings_id");
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(Settings, $doc, $tx);
	}	
}

class SettingsManager
{
	static $extensionMap = array();
	
	static function registerExtension($component, $handler)
	{
		SettingsManager::$extensionMap[$component] = $handler;
	}
	
	static function getExtension($component)
	{
		return isset(SettingsManager::$extensionMap[$component]) ? SettingsManager::$extensionMap[$component] : null;
	}
	
	function SettingsManager()
	{
		
	}
	
	/*
	 * The following config settings appear not be be in use in 
	 * any of our projects and thus are not included in application
	 * settings:
	 * 
	 * news_item_count
	 * latest_addition_count
	 * document
	 * tmp
	 * 
	 * The following config settings are not included and will
	 * remain in $config array:
	 * 
	 * homedir
	 * uploadbase
	 * http_host	
	 */
	static function setDefaults()
	{
		trace("SettingsManager::setDefaults", 3);
		
		global $config;
		
		preg_match_all("/([\\w\\d_]+)\.?([^}]*)/", $config["http_host"], $matches, PREG_SET_ORDER);
		if(count($matches) > 0)
		{
			$matches = $matches[0];
			if(count($matches) > 1)
			{
				$path = $matches[1];
				$sitename = prettify($path);
			}
		}

		Settings::setDefaultValue("settings", "prettyUrls", true, "Boolean", "Whether urls are translated to CMS identifiers", "General");		
		Settings::setDefaultValue("settings", "allow_pdf_download", true, "Boolean", "", "General");
		Settings::setDefaultValue("settings", "sitename", $sitename, "String", "", "General");
		Settings::setDefaultValue("settings", "wrap_modules", true, Boolean, "Specifies whether to wrap module output in a div tag, or render them bare", "General");
		Settings::setDefaultValue("settings", "system_notification_email", $config["email_contact"], String, "The email address to which system notifications should be sent", "General");
		Settings::setDefaultValue("settings", "local_IP_address", "", String, "Specifies the server's local IP address for environments (such as Microsoft Azure) where this cannot be determined automatically", "General");
		
		Settings::setDefaultValue("settings", "formCSS", "form", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "labelCSS", "label", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "valueCSS", "value", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "buttonCSS", "button", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "buttonAlignment", "left", "String", "", "AutoForm");
	
		Settings::setDefaultValue("settings", "componentPath", "/fakoli", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "HTMLEditorHideStyleBar", true, "Boolean", "", "AutoForm");
		Settings::setDefaultValue("settings", "HTMLEditorDefaultWidth", "540px", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "HTMLEditorDefaultHeight", "200px", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "javascriptEnabled", true, "Boolean", "", "AutoForm");
		Settings::setDefaultValue("settings", "markRequiredFields", true, "Boolean", "", "AutoForm");
		Settings::setDefaultValue("settings", "onFormatLabel", "addContextHelp", "String", "Callback function for start of field label", "AutoForm");
		Settings::setDefaultValue("settings", "requiredFieldsText", "* indicates required field", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "passwordEnterLabel", "Enter", "String", "The text of the password entry field label", "AutoForm");
		Settings::setDefaultValue("settings", "passwordEnterLabel", "Confirm", "String", "The text of the password confirmation field label", "AutoForm");
		Settings::setDefaultValue("settings", "buttons_at_top", false, "Boolean", "Whether the form's save/cancel buttons appear at the top of the form in addition to the bottom.", "AutoForm");
		Settings::setDefaultValue("settings", "default_layout", "table", String, "Specifies the default layout manager to use when rendering forms", "AutoForm", "table\nsimple\nmobile");
		Settings::setDefaultValue("settings", "interstitialSpinner", "/fakoli/images/spinner.gif", String, "Specifies the default spinner animation to use with interstitials", "AutoForm");

		Settings::setDefaultValue("settings", "font_awesome_URL", "", 'String', "If you want to use a specific version of Font Awesome (for instance if you have a Pro subscription) enter the URL here", "Font Awesome");	
	}
	
	/**
	 * Sets the Fakoli framework configuration variables according
	 * to the settings stored in the database. If a configuration variable
	 * already has a value (i.e. manually specified in the config.inc file)
	 * then the value is not overwritten.
	 * 
	 * The goal of this function is to aid with the migration of application-specific
	 * settings out of configuration files and into the application settings in the database.
	 */
	static function configurationBootstrap()
	{
		global $auto_form_defaults;
		global $fontawesome;
		
		$afDefaults = Cache::get("auto_form_defaults");
		if (!$afDefaults)
		{
			$afDefaults = array();
			$autoFormSettings = Query::create(Settings, "WHERE component='settings' AND category='AutoForm'")
									 ->filter(new InclusionFilter("name", "value"))
									 ->execute();
			
			foreach($autoFormSettings as $setting)
			{
				$afDefaults[$setting->name] = $setting->value;
			}
			
			Cache::put("auto_form_defaults", $afDefaults);
		}
		
		foreach($afDefaults as $name => $value)
		{
			if (!isset($auto_form_defaults[$name])) $auto_form_defaults[$name] = $value;
		}
		
		$fontawesomeURL = Cache::get("font_awesome_URL");
		if (!$fontawesomeURL)
		{
			$fontawesomeURL = Settings::getValue("settings", "font_awesome_URL");
			if (!$fontawesomeURL) $fontawesomeURL = $fontawesome;
			Cache::put("font_awesome_URL", $fontawesomeURL);
		}
		
		$fontawesome = $fontawesomeURL;
	}
	
	static function getSubTabs($component)
	{
		$tabs = null;
		$settingCategories = array();
		
		$categories = Query::create(Settings, "WHERE component = :component && (category != '' OR category != null)")
			->bind("component", $component->name)
			->execute();
			
		if(count($categories) > 0)
		{
			$categories = removeDuplicates($categories, "category");
			if(count($categories) > 1)
			{
				$idx = 1;
				foreach($categories as $category)
				{
					$settingCategory = new SettingsCategory();
					$settingCategory->component_id = $component->component_id;
					$settingCategory->category_id = $idx;
					$settingCategory->component = $component->name;
					$settingCategory->category = $category->category;
					$settingCategories[] = $settingCategory;
					$idx++;
				}

				$tabs = new DataItemTabBar("subtabs", $settingCategories);
			}
		}	
		return $tabs;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new SettingsUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("settings", "Settings and Permissions", new SettingsSerializationHandler());
		return true;
	}		
}