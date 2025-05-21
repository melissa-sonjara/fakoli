<?php
/**\defgroup bookmark Bookmark
 * @{
 */


class BookmarkManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "bookmark";
		$component->description = "Simple internal bookmarking feature allowing users to bookmark pages";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 14;
		$component->enabled = true;

		return $component;
	}

	static function getStyles()
	{
		return array(
				//TODO: Add CSS File References "/components/bookmark/css/..."
		);
	}

	static function getScripts()
	{
		return array(
				"/components/bookmark/js/bookmark_manager.js"
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	'BookmarkManager::upgradeComponent',
					"ComponentScanComplete"	=>	'BookmarkManager::setDefaults'
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>