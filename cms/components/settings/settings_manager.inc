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

class SettingsManager
{
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
			$tracePath = "/share/". $path . "_trace.log";
		}

		Settings::setDefaultValue("settings", "prettyUrls", true, "Boolean", "Whether urls are translated to CMS identifiers", "General");		
		Settings::setDefaultValue("settings", "allow_pdf_download", true, "Boolean", "", "General");
		Settings::setDefaultValue("settings", "sitename", $sitename, "String", "", "General");
		
		Settings::setDefaultValue("settings", "trace_path", $tracePath, "String", "File path to trace output log", "General");
		Settings::setDefaultValue("settings", "trace_level", 3, "Number", "Lowest to output only the most serious errors; highest for all", "General", "1\n2\n\n3\n4\n5");
		Settings::setDefaultValue("settings", "trace_detail", "TraceDetailHigh", "String", "The level of detail to be output", "General", "TraceDetailFull\nTraceDetailHigh\nTraceDetailMedium\nTraceDetailNormal");
		
		Settings::setDefaultValue("settings", "formCSS", "form", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "labelCSS", "label", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "valueCSS", "value", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "buttonCSS", "button", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "buttonAlignment", "left", "String", "", "AutoForm");
	
		Settings::setDefaultValue("settings", "componentPath", "/fakoli", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "HTMLEditorHideStyleBar", true, "Boolean", "", "AutoForm");
		Settings::setDefaultValue("settings", "HTMLEditorDefaultWidth", "540px", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "HTMLEditorDefaultWidth", "200px", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "javascriptEnabled", true, "Boolean", "", "AutoForm");
		Settings::setDefaultValue("settings", "markRequiredFields", true, "Boolean", "", "AutoForm");
		Settings::setDefaultValue("settings", "onFormatLabel", "addContextHelp", "String", "Callback function for start of field label", "AutoForm");
		Settings::setDefaultValue("settings", "requiredFieldsText", "* indicates required field", "String", "", "AutoForm");
		Settings::setDefaultValue("settings", "passwordEnterLabel", "Enter", "String", "The text of the password entry field label", "AutoForm");
		Settings::setDefaultValue("settings", "passwordEnterLabel", "Confirm", "String", "The text of the password confirmation field label", "AutoForm");
		Settings::setDefaultValue("settings", "buttons_at_top", false, "Boolean", "Whether the form's save/cancel buttons appear at the top of the form in addition to the bottom.", "AutoForm");
		
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
}