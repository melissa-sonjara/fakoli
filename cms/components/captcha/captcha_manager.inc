<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the captcha component.
 */

class CaptchaManager
{
	static $providers = array();
	
	function CaptchaManager()
	{
		ComponentManager::fireEvent("RegisterCaptchaProvider");
	}

	function addCaptchaToForm($form)
	{
		$provider = Settings::getValue("captcha", "captcha_provider");
		if (!$provider) return;
		
		$renderer = new $provider($form);		
	}
	
	static function registerCaptchaProvider($name, $class)
	{
		CaptchaManager::$providers[$class] = $name;
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("captcha", "captcha_provider", "", String, "Select the Captcha provider to use on your site", "CAPTCHA Provider");
	}
	
	static function registerSettingsFormExtension()
	{
		SettingsManager::registerExtension("captcha", array(CaptchaManager, settingsExtension));		
	}
		
	static function settingsExtension($form)
	{
		$mgr = new CaptchaManager();
		$providerSelect = new SelectFieldRenderer($form, "captcha_provider", "Captcha Provider", CaptchaManager::$providers);
		$providerSelect->defaultBlank = true;
	}
}

?>