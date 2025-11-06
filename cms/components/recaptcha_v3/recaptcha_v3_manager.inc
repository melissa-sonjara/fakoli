<?php
/**\addtogroup recaptcha_v2
 * @{
 */


Fakoli::using("settings");

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the recaptcha_v2 component.
 */

class RecaptchaV3Manager
{
	static $verificationURL = 'https://www.google.com/recaptcha/api/siteverify';
	
	function __construct()
	{

	}

	
	static function setDefaults()
	{
		Settings::setDefaultValue("captcha", "recaptcha_v3_site_key", "", String, "", "reCAPTCHA v3", "", 1);
		Settings::setDefaultValue("captcha", "recaptcha_v3_secret_key", "", String, "", "reCAPTCHA v3", "", 2);
	}
	
	static function registerCaptchaProvider()
	{
		CaptchaManager::registerCaptchaProvider("reCAPTCHA v3", RecaptchaV3FieldRenderer);
	}


}

/**@}*/?>