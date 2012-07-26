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

class SiteMapManager
{

	/**
	 * Creates a new ComponentManager object
	 */
	function SiteMapManager()
	{
	}
	
	/**
	 * Retrieve all pages. Search for new pages that are not
	 * in the site map. Get the module pages first because
	 * they can override component pages.
	 *
	 */
	function getModuleAndComponentPageUpdates()
	{
		$indexedMap = $this->getIndexedMap();
			
		$this->pages = query(Page, "WHERE published = 1");
		$indexedMap = $this->getPageUpdates($this->pages, $indexedMap);
		
		$this->componentPages = query(ComponentPage, "WHERE enabled = 1");
		$this->getPageUpdates($this->componentPages, $indexedMap);
	}
	
	function getIndexedMap()
	{
		$siteMapPages = query(SiteMap, "", "identifier");
		
		$indexedMap = array();
		if(count($siteMapPages) > 0)
			$indexedMap = reindexList($siteMapPages, "identifier");
		
		return $indexedMap;
	}
	
	function getPageUpdates($pages, $indexedMap)
	{
		if(count($pages) > 0)
		{
			foreach($pages as $l_page)
			{
				$found = $indexedMap[$l_page->identifier];
				if(!$found)
					$indexedMap = $this->savePage($l_page, $indexedMap);
				else
					$this->updatePage($found, $l_page);
			}
		}
		
		return $indexedMap;
	}
	
	function savePage($l_page, $indexedMap)
	{
		$found = new SiteMap();
		$found->identifier = $l_page->identifier;
		$found->page_title = $l_page->page_title;
		$found->role = $l_page->role;
		$found->save();
		$indexedMap[$l_page->identifier] = $found;
		
		return $indexedMap;
	}
	
	function updatePage($found, $l_page)
	{
		if($found->page_title == $l_page->page_title AND $found->role == $l_page->role)
			return;
			
		$found->page_title = $l_page->page_title;
		$found->role = $l_page->role;
		$found->filter = new InclusionFilter("page_title", "role");
		$found->save();	
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
	 * set the parent_identifier of its children to blank.
	 */
	function deleteSiteMapPage($siteMapPage)
	{
		$parent = $siteMapPage->parent();

		$children = $siteMapPage->Children();
		if(count($children) > 0)
		{
			foreach($children as $child)
			{
				$child->parent_identifier = ($parent AND $parent->identifier) ? 
					$parent->identifier : "";
				$child->save();
			}
		}		
		$siteMapPage->delete();
	}
	
	
	function setHierarchyFromMenuItems()
	{
		$indexedMap = $this->getIndexedMap();
		
		$menuItems = query(MenuItem, "WHERE published = 1 ORDER BY menu_id DESC", "identifier");
		if(count($menuItems) > 0)
		{
			foreach($menuItems as $menuItem)
			{
				if(array_key_exists($menuItem->identifier, $indexedMap))
				{
					$siteMapPage = $indexedMap[$menuItem->identifier];
					$parent = $menuItem->Parent();
					if($parent AND $parent->identifier != $siteMapPage->identifier)
					{
						$siteMapPage->parent_identifier = $parent->identifier;
					}
						
					$siteMapPage->sort_order = $menuItem->sort_order;
					$siteMapPage->published = true;
					$siteMapPage->filter = new InclusionFilter("parent_identifier", "sort_order", "published");
					$siteMapPage->save();
				}
			}
		}		
	}
	

	function checkForRemovedModuleAndComponentPages()
	{
		$mPages = query(Page, "WHERE published = 1");
		if(count($mPages) == 0) $mPages = array();
		$cPages = query(ComponentPage, "WHERE enabled = 1");
		if(count($cPages) == 0) $cPages = array();
		$sitePages = array_merge($mPages, $cPages);
		
		if(count($sitePages) > 0)
		{
			$indexed = reindexList($sitePages, "identifier");
		}	
			
		$siteMapPages = query(SiteMap);
		foreach($siteMapPages as $siteMapPage)
		{
			if(!array_key_exists($siteMapPage->identifier, $indexed))
			{
				$this->deleteSiteMapPage($siteMapPage);
			}			
		}
	}
	
	
	/**
	 * Scans the application home directory and PHP include path
	 * and builds the component and administration page map that
	 * describe the configuration of the application..
	 */
	function scan()
	{
		$this->getModuleAndComponentPageUpdates();
		$this->checkForRemovedModuleAndComponentPages();
		$this->setHierarchyFromMenuItems();		
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new SiteMapUpgradeManager();
		$mgr->upgrade($version);
	}	
}
?>