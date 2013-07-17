<?php

/**
 * Serialization handler for Link Libraries and Link Records.
 * @author andy
 *
 */
class LinkLibrarySerializationHandler
{
	function __construct()
	{
	}
	
	function export()
	{
		$xml  = "\n<LinkLibraryMap>";
		$xml .= SerializationManager::serialize(LinkLibrary, "ORDER BY link_library_id");
		$xml .= SerializationManager::serialize(LinkRecord, "ORDER BY link_id");
		$xml .= "</LinkLibraryMap>";

		return $xml;
	}

	function import($doc, $tx)
	{
		SerializationManager::unserialize(LinkLibrary, $doc, $tx);
		SerializationManager::unserialize(LinkRecord, $doc, $tx);
	}
}

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the link_library component.
 */
class LinkLibraryManager
{
	function LinkLibraryManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	function getTabs($key)
	{
		$tabs = array("Library Details"	=>	"/admin/link_library_form",
					  "Links"			=>	"/admin/link_library_links");
	
		$qs = ($key) ? "link_library_id=$key" : "";
		return new TabBar("tabs", $tabs, $qs);
	}
	
	static function buildLinkLibraryForm($library)
	{		
		$form = new AutoForm($library);
		$form->hide("owner_id", "create_date");
		$form->required("name", "identifier");
		$form->allowDelete = true;
		$form->onDelete = array(LinkLibraryManager, deleteLinkLibary);
		

		$readOnlyAccessList = new CheckListFieldRenderer($form, "read_access", "Allow Read Access", SiteRole::getRolesArray());
		$writeOnlyAccessList = new CheckListFieldRenderer($form, "write_access", "Create & Delete Links", SiteRole::getRolesArray());
		
		return $form;
	}
	
	static function deleteLinkLibrary($library)
	{
		trace("Deleting Link Library {$library->link_library_id}", 3);
		
		$tx = new DataTransaction();
		
		try
		{
			$library->joinTransaction($tx);
			
			$link = new LinkRecord();
			$link->joinTransaction($tx);
			$link->delete("WHERE link_library_id={$library->link_library_id}");
			$library->delete();
		
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}	
		
		return true;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new LinkLibraryUpgradeManager();
		$mgr->upgrade($version);
	}

	/**
	 * Respond to fired event DeleteUser
	 *
	 * Delete any records in this component that have dependencies on user object.
	 *
	 * @param obj $user - class SiteUser or custom user class
	 */
	static function deleteUser($user)
	{
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		
		trace("Component link_library is deleting objects dependent on user_id {$user_id}", 3);
	
		$tx = new DataTransaction();
		
		try
		{
			$libraries = Query::create(LinkLibrary, "WHERE owner_id=:owner_id")
				->bind(":owner_id", $user_id)
				->execute();
			
			if(count($libraries))
			{
				foreach($libraries as $library)
				{
					$library->joinTransaction($tx);
					LinkLibraryManager::deleteLinkLibrary($library);
				}
			}
			
			$record = new LinkRecord();
			$record->joinTransaction($tx);
			$record->delete("WHERE owner_id={$user_id}");
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
		
		return $user;
	}
	
	/**
	 * Enumerate the Document objects.
	 * @param  $items collection that the Document items are returned in
	 */
	static function enumerateItems($items)
	{
		$libraries = Query::create(LinkLibrary, "WHERE enabled=1 ORDER BY name")->execute();
	
		$items["Link Libraries"] = $libraries;
		return $items;
	}
	
	static function displayLibrary($identifier, &$continue)
	{
		try
		{
			$library = LinkLibrary::findByIdentifier($identifier);
			$page = ComponentPage::findByIdentifier("link_library", "WHERE enabled=1");
				
			$_GET["library"] = $identifier;
	
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
	
			$page_role = $page->role;
				
			if (!checkRole($page->role))
			{
				LoginManager::storeRedirectPage();
				redirect("/login");
			}
				
			echo $pageView->drawView();
				
			$continue = false;
		}
		catch(DataNotFoundException $e)
		{
				
		}
	
		return $identifier;
	}

	static function registerSearchables($searchables)
	{
		trace("LinkLibraryManager registerSearchables", 3);
		$searchables["link_library"] = array(LinkRecord);
		return $searchables;
	}
	
	static function registerTaxonomyClasses($classes)
	{
		$classes[] = LinkRecord;
		return $classes;
	}

	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("link_library", "Link Libraries", new LinkLibrarySerializationHandler());
		return true;
	}
	
	static function formatLink($link)
	{
		$url = $link->url;
		
		$type = getDocType($url);
		$icon = getDocIcon($url);
		
		return $link->format("<h4>{title}</h4><p>{description}</p><img style='display:inline-block;vertical-align: middle' src='$icon' alt='$type'/> <a href='{url}' target='_blank'>Read More</a>");
	}		
}

?>