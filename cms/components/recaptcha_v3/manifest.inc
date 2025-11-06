<?php
/**\defgroup recaptcha_v2 Recaptcha V 2
 * @{
 */


class RecaptchaV3Manifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "recaptcha_v3";
		$component->description = "Captcha Provider for reCAPTCHA version 3";
		$component->author = "Melissa Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function subscribeToEvents()
	{
		return array(
				"ComponentScanComplete"		=>	array(RecaptchaV3Manager, setDefaults),
				"RegisterCaptchaProvider"	=>	array(RecaptchaV3Manager, registerCaptchaProvider)
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>