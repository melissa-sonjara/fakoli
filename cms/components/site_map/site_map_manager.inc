<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/**
 * Site Map Manager builds a hierarchical map of the site.
 *
 * Steps:
 * 
 * 1) Retrieve all pages that are pupblished into an array. 
 * 
 * 2) Retrieve all component pages that are enabled and add them 
 * to the array if the identifier does not already exist. Pages
 * can override component pages.
 * 
 * 3) Retrieve the global menu. Search for matches of its identifiers
 * with ones in the pages array. If found, set the parent identifier
 * to match the parent of the menu page.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 *
 * 10/8/2010
 */

Fakoli::using("component", "page", "menu", "site_map");

/**
 * Scans the application home directory and PHP include path
 * and builds the component and administration page map that
 * describe the configuration of the application..
 */

class SiteMapManager
{
	var $pages = array();
	var $map = array();
	var $currentContent = array();
	
	static $titleFields = array(
		"Blogs"					=>	"title",
		"Calendars"				=>	"name",
		"Document Libraries"	=>	"name",
		"Component Pages"		=>	"page_title",	
		"Forums"				=>	"title",
		"Image Galleries"		=>	"gallery_name",
		"Link Libraries"		=>	"name",
		"Pages"					=>	"page_title",
		"Video Galleries"		=>	"gallery_name",
			);
	
	/**
	 * Creates a new ComponentManager object
	 */
	function __construct()
	{
		$this->map = IndexedQuery::create(SiteMap, "", "url")->execute();
		
		if(!is_array($this->map))
		{
			$this->map = array();			
		}
	}
	
	static function updateSiteMap()
	{
		trace("SiteMapManager processing method updateSiteMap", 3);
		
		$mgr = new SiteMapManager();
		$mgr->update();
	}
	
	function update()
	{
		$this->saveNewContentToSiteMap();
		$this->removeOldContentFromSiteMap();
		$this->setHierarchyFromMenuItems();
	}

	/*
	 * Before deleting a page...
	 * 
	 * If the page is a leaf, then we don't need to
	 * do anything.
	 * 
	 * If the page has a child and parent, link all
	 * the children to its parent.
	 * 
	 * If the page has no parent but does have children,
	 * set the parent_url of its children to blank.
	 */
	function deleteSiteMapPage($siteMapPage)
	{
		$parent = $siteMapPage->Parent();

		$children = $siteMapPage->Children();
		if(count($children) > 0)
		{
			foreach($children as $child)
			{
				$child->parent_url = ($parent AND $parent->url) ? 
					$parent->url : "";
				$child->save();
			}
		}		
		$siteMapPage->delete();
	}
	
	/**
	 * Default the hierarchy of pages to the global menu's
	 * hierarchy.
	 */
	function setHierarchyFromMenuItems()
	{
		$indexedMap = $this->map;
		
		$menuItems = GroupedQuery::create(MenuItem, "WHERE menu_id IN (SELECT menu_id FROM menu WHERE identifier='global')", "url")
			->execute();
		
		if(!count($menuItems)) return;
		
		foreach($menuItems as $url => $urlItems)
		{
			if(array_key_exists($url, $indexedMap))
			{
				$siteMapPage = $indexedMap[$url];
				
				if(!is_array($urlItems)) $urlItems = array($urlItems);
				
				foreach($urlItems as $menuItem)
				{
					$parent = $menuItem->Parent();
					if(!$parent || $parent->url == $siteMapPage->url) continue;
					
					if($parent AND $parent->url != $siteMapPage->parent_url)
					{
						$siteMapPage->parent_url = $parent->url;
						$siteMapPage->published = true;
						$siteMapPage->sort_order = $menuItem->sort_order;
						$siteMapPage->filter = new InclusionFilter("parent_url", "published");
						$siteMapPage->save();
						break;
					}		
				}
			}
		}
	}		
	
	/**
	 * Check if the SiteMap contains records of content that
	 * has been removed from the site.
	 */
	function removeOldContentFromSiteMap()
	{
		foreach($this->map as $url => $siteMapPage)
		{
			if(array_search($url, $this->currentContent) === FALSE)
			{
				$this->deleteSiteMapPage($siteMapPage);
			}
		}
	}
	

	/**
	 * Given an instance of a CMS content item,
	 * get the title.
	 */
	function getTitle($content, $type, $item)
	{	
		if ($content->override_page_title) 
		{
			$title = $content->override_page_title;
		}
		else if(array_key_exists($type, SiteMapManager::$titleFields))
		{
			$field = SiteMapManager::$titleFields[$type];
			$title = $item->$field;
		}
		else
		{
			$title = prettify($item->identifier);
		}

		return $title;
	}
	
	/**
	 * Gather all CMS content and if an item is not already in the site
	 * map, add it.
	 */
	function saveNewContentToSiteMap()
	{
		$itemsByType = array();
		$itemsByType = ComponentManager::fireEvent("EnumerateItems", $itemsByType);
	
		$sections = Query::create(Section, "ORDER BY section")->execute();
		
		foreach($sections as $section)
		{
			$content = $section->Content("ORDER BY identifier");
			$contentByIdentifier = reindexList($content, "identifier");
				
			// e.g., type is Component Pages and items are all component pages 
			foreach($itemsByType as $type => $items)
			{
				foreach($items as $item)
				{
					if (!$item->identifier || !array_key_exists($item->identifier, $contentByIdentifier)) continue;
						
					trace(get_class($item). " - ".$item->identifier, 3);
					
					$content = $contentByIdentifier[$item->identifier];
					$url = $this->getLink($section, $item->identifier);
					$page_title = $this->getTitle($content, $type, $item);
					
					$siteMapPage = new SiteMap();			
					if(array_key_exists($url, $this->map))
					{
						$siteMapPage = $this->map[$url];
						if($page_title != $siteMapPage->page_title)
						{
							$siteMapPage->page_title = $page_title;
							$siteMapPage->filter = new InclusionFilter("page_title");
							$siteMapPage->save();
						}	
					}
					else
					{
						$siteMapPage->url = $url;
						$siteMapPage->published = true;
						$siteMapPage->page_title = $page_title;
						$siteMapPage->save();
						$this->map[$siteMapPage->url] = $siteMapPage;
					}
		
					$this->currentContent[] = $siteMapPage->url;
				}
			}					
		}
	}
	
	function categorizeItemTypes($itemsByType)
	{
		$types = array();
	
		foreach($itemsByType as $type => $items)
		{
			foreach($items as $item)
			{
				$types[$item->identifier] = $type;
			}
		}
	
		return $types;
	}
	
	function getLink($section, $identifier)
	{
		if ($section->section == "/")
			$url = "/$identifier";
		else
			$url = "/{$section->section}/$identifier";
	
		return $url;
	}
	
	
	static function upgradeComponent($version)
	{
		$mgr = new SiteMapUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("site_map", "Site Map", new SiteMapSerializationHandler());
		return true;
	}	
}


class SiteMapSerializationHandler
{
	function __construct()
	{
	}

	function export()
	{
		$xml .= "\n<SiteMapMap>";
		$xml .= SerializationManager::serialize(SiteMap, "ORDER BY site_map_id");
		$xml .= "\n</SiteMapMap>";
		return $xml;
	}

	function import($doc, $tx)
	{
		SerializationManager::unserialize(SiteMap, $doc, $tx);
	}
}


class SiteMapHelper
{
	/**
	 * 
	 * @param obj $siteMapPage class SiteMap
	 */
	static function formatURL($siteMapPage)
	{
		if(!$siteMapPage->isLeaf())
		{
			return $siteMapPage->format("<b>{url}</b>");
		}
		
		return $siteMapPage->format("{url}");
	}
}

?>