<?php
Fakoli::using("settings");

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the recaptcha_v2 component.
 */

class RecaptchaV2Manager
{
	static $verificationURL = 'https://www.google.com/recaptcha/api/siteverify';
	
	function RecaptchaV2Manager()
	{

	}

	
	static function setDefaults()
	{
		Settings::setDefaultValue("captcha", "recaptcha_site_key", "", String, "", "reCAPTCHA v2");
		Settings::setDefaultValue("captcha", "recaptcha_secrect_key", "", String, "", "reCAPTCHA v2");
	}
	
	static function registerCaptchaProvider()
	{
		CaptchaManager::registerCaptchaProvider("reCAPTCHA v2", RecaptchaV2FieldRenderer);
	}


}

?>