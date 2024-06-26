<?php
/**\addtogroup mobile_settings
 * @{
 */


Fakoli::using("settings", "section", "menu");

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
		Settings::setDefaultValue("mobile_settings", "use_native_form_controls", false, Boolean, "Use native HTML 5 controls for date and time fields (when available on mobile device)");
	}

	static function onInitialize()
	{
		global $isResource;
		global $isAction;
		global $auto_form_defaults;

		$identifier = checkIdentifier($_GET["identifier"]);
				
		$mode = Settings::getValue("mobile_settings", "mobile_content_mode");
		$useNative = Settings::getValue("mobile_settings", "use_native_form_controls");

		$useMobile = MobileSettingsManager::useMobile();
		$isTablet = MobileSettingsManager::isTablet();
		
		MenuManager::setDisplayFlag("mobile", $useMobile);
		MenuManager::setDisplayFlag("tablet", $isTablet);
		
		if ($useMobile && !$isTablet)
		{
			$auto_form_defaults["default_layout"] = "mobile";
		}
		
		if ($useMobile && $useNative)
		{
			$detect = new MobileDetect();
			if ($detect->isiPhone() || $detect->isiPad())
			{
				Fakoli::usingFeature("auto_form");
				AutoForm::registerFieldRendererClass(DateTime, NativeDateTimeFieldRenderer);
				AutoForm::registerFieldRendererClass(Date, NativeDateFieldRenderer);
			}
		}
		
		if ($isResource || $isAction || $mode == "Deliver Full Content") return;
		
		if ($mode != "Dedicated Mobile Section" || !$useMobile) return;
		
		trace ("Using Dedicated Mobile Section", 4);
		
		$section = checkIdentifier($_GET["section"]);
		
		$mobileSection = Settings::getValue("mobile_settings", "mobile_content_section");
		
		if ($mobileSection == "") return;
		
		trace("Mobile: $mobileSection vs $section", 4);
		
		if ($mobileSection != $section)
		{
			if ($useMobile)
			{
				$mobileSectionObject = Section::findSection($mobileSection);
				if ($mobileSectionObject->getContent($identifier))
				{
					$pattern = ($section == "") ? "#^/#" : "#/^{$section}/#";
					$uri = preg_replace($pattern, "/{$mobileSection}/", $_SERVER["REQUEST_URI"]);
				}
				else
				{
					
					$uri = ($mobileSection == "/") ? "/" : "/{$mobileSection}/";
				}
				
				trace("Redirecting to mobile section at $uri", 3);
				redirect($uri);
			}
		}
		else
		{
			if (!MobileSettingsManager::isTablet())
			{
				$auto_form_defaults["default_layout"] = "mobile";
			}
		}
	}
	
	static function overrideTemplate($template, &$continue)
	{
		global $site;
		
		if (!MobileSettingsManager::useMobile()) return $template;
		
		$mode = Settings::getValue("mobile_settings", "mobile_content_mode");
		$mode = ($mode == "Use Mobile Template");
		$continue = !$mode;
		$out = $mode ? $site->mobile_template : $template;
		
		trace("OverrideTemplate: $template -> $out", 4);
		return $out;
	}
	
	static function useMobile()
	{
		if (isset($_SESSION["useMobile"])) return $_SESSION["useMobile"];
		
		$mobileTablets = Settings::getValue("mobile_settings", "send_mobile_content_to_tablets");
		
		$detect = new MobileDetect();
		
		$mobile = false;
		
		if ($detect->isTablet()) 
		{
			$mobile = $mobileTablets;
		}
		else $mobile = $detect->isMobile(); 
		
		trace("useMobile: ".($mobile?'true':'false'), 4);
		
		return $mobile;
	}
	
	static function isTablet()
	{		
		$detect = new MobileDetect();
		return $detect->isTablet();
	}
}

/**@}*/?>