<?php
Fakoli::using("settings");

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the mobile component.
 */
class MobileSettingsManager
{
	function MobileSettingsManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("mobile_settings", "mobile_content_mode", "Deliver Full Content", String, "Sets the content delivery mode for mobile content", "", "Deliver Full Content\nUse Mobile Template\nDedicated Mobile Section");
		Settings::setDefaultValue("mobile_settings", "mobile_content_section", "", String, "The identifier of the section providing dedicated mobile content");
		Settings::setDefaultValue("mobile_settings", "send_mobile_content_to_tablets", false, Boolean);
	}

	static function onInitialize()
	{
		$mode = Settings::getValue("mobile_settings", "mobile_content_mode");

		if ($mode != "Dedicated Mobile Section") return;
		
		global $section;
		
		$mobileSection = Settings::getValue("mobile_settings", "mobile_content_section");
		
		if ($mobileSection->section != $section->section && MobileSettingsManager::useMobile())
		{
			$uri = $mobileSection->section == "/" ? "/" : "/{$mobileSection->section}/";
			redirect($uri);
		}
	}
	
	static function overrideTemplate($template, &$continue)
	{
		global $site;
		
		if (!MobileSettingsManager::useMobile()) return false;
		
		$mode = Settings::getValue("mobile_settings", "mobile_content_mode");
		$mode = ($mode == "Use Mobile Template");
		$continue = !$mode;
		return $mode ? $site->mobile_template : $template;
	}
	
	static function useMobile()
	{
		$mobileTablets = Settings::getValue("mobile_settings", "send_mobile_content_to_tablets");
		
		$detect = new MobileDetect();
		
		if ($detect->isTablet() && $mobileTablets) return true;
		return $detect->isMobile(); 
	}
}

?>