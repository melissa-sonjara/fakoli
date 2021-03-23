<?php
/**\addtogroup sharing
 * @{
 */



interface ShareHandler
{
	public function getTitle($key);
	public function draw($key);
}

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the sharing component.
 */

class SharingManager
{
	static $shareHandlerMap = null;
	
	function SharingManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function getShareHandler($type)
	{
		if (!SharingManager::$shareHandlerMap)
		{
			SharingManager::$shareHandlerMap = array();
			ComponentManager::fireEvent("RegisterShareHandlers");
		}
		
		if (!isset(SharingManager::$shareHandlerMap[$type]))
		{
			throw new FakoliException("Unknown share handler");
		}
		
		return SharingManager::$shareHandlerMap[$type];
	}

	static function registerShareHandler($type, $handler)
	{
		SharingManager::$shareHandlerMap[$type] = $handler;
	}
	
	static function registerSectionContentManager()
	{
		SectionContentManagerMap::registerManager("Sharing", new SharingContentManager());
	}
	
	static function drawShare($token)
	{
		global $user;
		
		$share = ShareToken::getShare($token);
		if (!$share->active)
		{
			throw new FakoliException("This shared item is no longer available");
		}
		
		$handler = SharingManager::getShareHandler($share->item_type);
		$handler->draw($share->item_key);
		
		$access = new ShareAccess();
		$access->share_id = $share->share_id;
		$access->user_id = $user ? $user->user_id : 0;
		$access->access_timestamp = now();
		$access->ip_address = $_SERVER["REMOTE_ADDR"];
		$access->save();
	}
	
	/**
	 * Creates a ShareToken for the specified item
	 * @param DataItem $item the item to be shared
	 */
	static function shareItem($item)
	{
		global $user;
		
		$type = get_class($item);
		SharingManager::getShareHandler($type); // Trigger an exception if the item is not a registered type
		
		$token = new ShareToken();
		$token->token = plainGUID();
		$token->item_type = $type;
		$token->item_key = $item->getPrimaryKeyValue();
		$token->user_id = $user->user_id;
		$token->created_date = now();
		$token->last_updated = now();
		$token->last_updated_by_id = $user->user_id;
		$token->active = true;
		$token->save();
		
		return $token;
	}
	
	static function embedSharedItem($template)
	{
		$match = [];
		while(preg_match("/\\[share:(.*?)\\]/", $template, $match))
		{
			$id = $match[1];
			
			$token = ShareToken::getShare($id);
			
			if (!$token) continue;
			
			ob_start();
			
			$handler = SharingManager::getShareHandler($token->item_type);
			$handler->draw($token->item_key);
			
			$out = ob_get_contents();
			ob_end_clean();
			$template = str_replace($match[0], $out, $template);
		}
		
		return $template;
	}
		
	static function getURL($token)
	{
		global $config;
		
		$sharing = Query::create('Section', "WHERE section_type='Sharing'")->execute();
		if (count($sharing) == 0) return "No Sharing Section";
		$sharing = $sharing[0];
		
		return "https://".$config['http_host']."/".$sharing->section."/".$token->token;
	}
	
	static function formatStatusToggle($token)
	{
		if ($token->active)
		{
			$out = $token->format("<a href='#' data-share_id='{share_id}' class='share_enabled' onclick='SharingManager.toggleStatus(this); return false;'><i class='icon-ok'></i> Enabled</a>");
		}
		else
		{
			$out = $token->format("<a href='#' data-share_id='{share_id}' class='share_disabled' onclick='SharingManager.toggleStatus(this); return false;'><i class='icon-cancel'></i> Disabled</a>");
		}
		
		return $out;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new SharingUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>