<?
class HtmlEditorManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "html_editor";
		$component->description = "Allows extensions and configuration for the HTML editor";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 2;
		$component->enabled = true;

		return $component;
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(HtmlEditorManager, setDefaults),
					"Initialize"			=>	array(HtmlEditorManager, onInitialize)
					//TODO: Add Event Registrations here
		);
	}
}

?>