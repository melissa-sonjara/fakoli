<?php
Fakoli::using("settings", "section");

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
		global $isResource;
		global $isAction;
		
		if ($isResource || $isAction) return;
		
		$mode = Settings::getValue("mobile_settings", "mobile_content_mode");

		if ($mode != "Dedicated Mobile Section") return;
		
		trace ("Using Dedicated Mobile Section", 3);
		
		$section = checkIdentifier($_GET["section"]);
		
		$mobileSection = Settings::getValue("mobile_settings", "mobile_content_section");
		
		if ($mobileSection == "") return;
		
		trace("Mobile: $mobileSection vs $section", 3);
		
		if ($mobileSection != $section && MobileSettingsManager::useMobile())
		{
			$uri = ($mobileSection == "/") ? "/" : "/{$mobileSection}/";
			
			trace("Redirecting to mobile section at $uri", 3);
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
		$out = $mode ? $site->mobile_template : $template;
		
		trace("OverrideTemplate: $template -> $out", 3);
		return $out;
	}
	
	static function useMobile()
	{
		if (isset($_SESSION["useMobile"])) return $_SESSION["useMobile"];
		
		$mobileTablets = Settings::getValue("mobile_settings", "send_mobile_content_to_tablets");
		
		$detect = new MobileDetect();
		
		$mobile = false;
		
		if ($detect->isTablet() && $mobileTablets) 
		{
			$mobile = true;
		}
		else $mobile = $detect->isMobile(); 
		
		trace("useMobile: ".($mobile?'true':'false'), 3);
		
		return $mobile;
	}
}

?>