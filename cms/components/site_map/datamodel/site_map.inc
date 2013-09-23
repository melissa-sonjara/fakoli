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

/*
 * Description: data model classes for site hierarchy page
 * chart. Site map or chart can be used for breadcrumbs
 * and site help.
 *
 * author Janice Gallant for Sonjara, Inc.
 * 
 * date: 9/30/10
 * 
 */

class SiteMap extends DataItem
{
	var $fields = array(
					"site_map_id"			=>	Number,
					"url"					=>	String,
					"page_title"			=>	String,
					"sort_order"			=>	String,
					"role"					=>	String,
					"parent_url"			=>	String,
					"published"				=>	Boolean
	);
	
	function Parent()
	{
		$parents = Query::create(SiteMap, "WHERE url=:parent_url")
			->bind("parent_url", $this->parent_url)
			->execute();	
		
		return count($parents) ? $parents[0] : null;
	}
	
	function Children($constraint = "")
	{
		$query = "WHERE parent_url=:url";
		if($constraint)
		{
			$query .= preg_replace("/^WHERE/i", " AND", $constraint);
		}
		return Query::create(SiteMap, $query)
			->bind(":url", $this->url)
			->execute();
	}
		
	
	/**
	 * Is the given url a sibling of this SiteMap obj.
	 * 
	 * @param String $url
	 * @return boolean
	 */
	function isSibling($url)
	{
		$sibling = false;

		$siteMap = SiteMap::findSiteMapPage($url);
		
		$sibling = ($siteMap AND ($siteMap->parent_url == $this->parent_url)) ? true : false;
		
		return $sibling;
	}
	
	static function findSiteMapPage($url)
	{
		if(!$url) return null;
		$siteMaps = Query::create(SiteMap, "WHERE url=:url")
			->bind(":url", $url)
			->execute();
		
		return count($siteMaps) ? $siteMaps[0] : null;
	}
	
	// allow delete if the page has no sub or child pages
	function allowDelete()
	{
		$count = Query::create(SiteMap, "WHERE parent_url=:url")
			->bind(":url", $this->url)
			->executeValue("COUNT(1)");
		
		return ($count > 0) ? false : true;
	}
	
	function isLeaf()
	{
		return $this->allowDelete();
	}
	
	/*
	 * For select field renderer
	 */
	static function getSiteMapPageList()
	{
		$siteMapPages = query(SiteMap, "ORDER BY url");
				
		if(count($siteMapPages) > 0)
		{
			foreach($siteMapPages as $sPage)
			{
				$pageList[$sPage->url] = $sPage->url . ": " . $sPage->page_title;
			}
		}
		
		$pageList[""] = "";
		return $pageList;
	}
		
	function SiteMap()
	{
		$this->table = "site_map";
		$this->primary_key = "site_map_id";

		$this->DataItem(func_get_args());
	}
}
