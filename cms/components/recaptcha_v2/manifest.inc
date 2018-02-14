<?
class RecaptchaV2Manifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "recaptcha_v2";
		$component->description = "Captcha Provider for reCAPTCHA version 2";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function subscribeToEvents()
	{
		return array(
				"ComponentScanComplete"		=>	array(RecaptchaV2Manager, setDefaults),
				"RegisterCaptchaProvider"	=>	array(RecaptchaV2Manager, registerCaptchaProvider)
					//TODO: Add Event Registrations here
		);
	}
}

?>