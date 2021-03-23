<?php
/**\addtogroup versioned_content
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the versioned_content component.
 */

class VersionedContentManager
{
	function VersionedContentManager()
	{

	}

	static $versionedContentClasses = array();
	
	static function onInitialize()
	{
		ComponentManager::fireEvent("RegisterVersionedContent");
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("settings", "enable_content_versioning", false, Boolean, "Enables content versioning for supported content items", "Content Versioning");
	}

	static function enableVersioning($form)
	{
		if (!Settings::getValue("settings", "enable_content_versioning")) return;
		
		$target = &$form->data;
		
		ContentVersion::loadDraft($target);
		$form->onDrawStart = array(VersionedContentManager, "onDrawForm");
		$form->onSavePreProcess = array(VersionedContentManager, 'onSavePreProcess');
		
	}
	
	static function registerVersionedContent()
	{
		for($i = 0; $i < func_num_args(); ++$i)
		{
			VersionedContentManager::$versionedContentClasses[] = func_get_arg($i);
		}
	}
	
	static function onSavePreProcess($form)
	{
		$target = $form->data;
		ContentVersion::saveDraft($target);
		VersionedContentManager::revert($target);
		return true;
	}
	
	static function onDrawForm($form)
	{
		$target = $form->data;
		$versionControls = VersionedContentManager::renderVersioningControls($form->data, true);
		if ($versionControls)
		{
			echo "<p class='version_info'>$versionControls</p>";
		}
	}
	
	static function onDrawInlineControls($item)
	{
		return VersionedContentManager::renderVersioningControls($item, false);
	}
	
	/**
	 * Revert the versioned fields on specified target to the current live values 
	 * @param DataItem $target the target content object
	 */
	static function revert($target)
	{
		$class = get_class($target);
		$reversion = new $class;
		$id = $target->getPrimaryKeyValue();
		if (!$id) return;
		
		$reversion->load($id);
		
		foreach($reversion->versioned_fields as $field)
		{
			$target->set($field, $reversion->get($field));
		}
	}
	
	static function approve($target)
	{
		$contentVersion = ContentVersion::getVersion($target, "draft");
		if (!$contentVersion) throw new FakoliException("No draft found for item");
		$contentVersion->approve($target);
	}
		
	static function selectDisplayVersion($target, $param = "version")
	{
		if (!isset($_GET[$param])) return $target;
		$version = $_GET[$param];
		
		if ($version == 'draft')
		{
			ContentVersion::loadDraft($target);
			return;
		}
		
		checkNumeric($version);
		
		ContentVersion::loadVersion($target, $version);
	}
		
	
	static function renderVersioningControls($item, $formMode = false)
	{
		if (!Settings::getValue("settings", "enable_content_versioning")) return;
		
		if ($formMode)
		{
			$version = "draft";
		}
		else
		{
			$param = "version";
			
			$version = $_GET[$param];
			if (!$version) $version = "";
			
			$params = array();
			
			$qs = getCleanQueryString();
			parse_str($qs, $params);
			
			unset($params[$param]);
		}
		
		$liveQs = http_build_query($params);
		$params[$param] = "draft";
		$draftQs = http_build_query($params);
		$hasDraft = ContentVersion::hasDraft($item);
		
		if (!$hasDraft && $formMode)
		{
			$version = "";
		}
		
		$out = "";
		
		if ($hasDraft && !$formMode)
		{
			$draftSelected = ($version == "draft") ? " selected" : "";
			$liveSelected = ($version == "") ? " selected" : "";
			
			$out .= " <a class='pushbutton first{$liveSelected}' href='?{$liveQs}'>Live</a><a class='pushbutton last{$draftSelected}' href='?{$draftQs}'> Draft</a>";
		}
		
		$selectedVersion = ContentVersion::getVersion($item, $version);
		if ($selectedVersion)
		{
			$mgr = new UserManager();
			$owner = $mgr->getUserFullName($selectedVersion->LastModifiedBy());
			
			$out .= $selectedVersion->format("<span class='version'>Version {version_number} Last Edited By {$owner} on {last_modified}</span>");
		}
		
		if ($version == "draft" && $hasDraft)
		{
			$class = get_class($item);
			$pkval = $item->getPrimaryKeyValue();
			$out .= "<a class='approve' href='#' onclick='VersionedContentManager.approve(\"{$class}\", {$pkval}); return false'>Approve</a>";
			$out .= "&nbsp;<a class='revert' href='#' onclick='VersionedContentManager.revert(\"{$class}\", {$pkval}); return false'>Discard Changes</a>";
		}
		return $out;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new VersionedContentUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>