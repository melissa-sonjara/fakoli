<?
class TermsAndConditionsManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "terms_and_conditions";
		$component->description = "Terms and Conditions display and approval control";
		$component->author = "Andrew Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	var $allow_sessionless_handlers = true;
	
	static function getStyles()
	{
		return array("/components/terms_and_conditions/css/terms_and_conditions.css");
	}
	
	static function getScripts()
	{
		return array(
				"/components/terms_and_conditions/js/terms_and_conditions.js"
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(TermsAndConditionsManager, setDefaults)
					//TODO: Add Event Registrations here
		);
	}
}

?>