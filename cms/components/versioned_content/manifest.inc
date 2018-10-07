<?
class VersionedContentManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "versioned_content";
		$component->description = "Provides support for content versioning, version management and approval";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 12;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array(
				//TODO: Admin Menu Item definitions
		);
	}

	static function getScripts()
	{
		return array(
			"/components/versioned_content/js/versioned_content_manager.js"
		);
	}

	static function getStyles()
	{
		return array("/components/versioned_content/css/versioned_content.css");
	}
	
	static function subscribeToEvents()
	{
		return array("Initialize"				 =>	array(VersionedContentManager, onInitialize),
					 "upgradeComponent"			=>	array(VersionedContentManager, upgradeComponent),
					 "ComponentScanComplete"	=>	array(VersionedContentManager, setDefaults),
					 "RenderVersioningControls"	=>	array(VersionedContentManager, onDrawInlineControls)
					//TODO: Add Event Registrations here
		);
	}
}

?>