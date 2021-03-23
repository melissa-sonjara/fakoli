<?php
/**\defgroup captcha Captcha
 * @{
 */


class CaptchaManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "captcha";
		$component->description = "Manages integration with Captcha services that attempt to reduce spam.";
		$component->author = "Andrew Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(CaptchaManager, setDefaults),
					"RegisterSettingsFormExtension"	=>	array(CaptchaManager, registerSettingsFormExtension)
		);
	}
}

/**@}*/?>