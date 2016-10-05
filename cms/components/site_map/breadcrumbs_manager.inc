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

/**************************************************************
 * 
 * Title: breadcrumbs.inc
 * 
 * Description: Display location (not path) breadcrumbs
 * based on the calling script's identifier and its match
 * to the site hierarchy chart as specified in site_map
 * table.
 * 
 * To add breadcrumbs to your pages:
 * 1) Add {position:breadcrumbs} to your template, if not already there.
 * 2) Create code module "Breadcrumbs" linked to site_map module
 * breadcrumbs, set to global position "breadcrumbs"
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 10/9/10
 * 
 ***************************************************************/

Fakoli::using("site_map");


class Breadcrumbs
{	
	var $breadcrumbs;
	var $qs;
	var $url;
	
	static $overrideCurrentPage = null;
	function __construct()
	{		
		$qs = getCleanQueryString();
		$this->url = $this->getLink();
		
		trace("Breadcrumb url $this->url", 3);
		$this->qs = preg_replace("/_style=\\w+&*/", "", $qs);
		
		$this->breadcrumbs = $this->getTrail();
	}
	
	/**
	 * Gather the breadcrumb trail.
	 * 1) Find the page the user is on in the SiteMap table
	 * 2) Traverse the site map tree to find the root node
	 * 3) Walk down the tree to format the trail from the root to the current page
	 * for display.
	 */
	function getTrail()
	{
		$currentSiteMapPage = $this->findPage();
		trace("Breadcrumbs:: currentSiteMapPage id {$currentSiteMapPage->url}", 3);
		
		if(!$currentSiteMapPage)
		{
			return "";
		}
			
		$upTreePages = $this->walkUpTree($currentSiteMapPage);
		
		$breadcrumbs = $this->walkDownTree($upTreePages);
		
		return $breadcrumbs;
	}
	

	/**
	 * Get the server's url and query the SiteMap
	 * table for a match.
	 * 
	 * @return instance of SiteMap or null
	 */
	function findPage()
	{
		$url = (Breadcrumbs::$overrideCurrentPage) ? Breadcrumbs::$overrideCurrentPage: $this->url;
		
		$pages = Query::create(SiteMap, "WHERE url =:url")
			->bind(":url", $url)
			->execute();
		
		return (count($pages)) ? $pages[0] : null;
	}
			
	/**
	 * Traverse up the tree through each parent url to find
	 * the root.
	 * 
	 * @param obj $currentSiteMapPage instance of SiteMap
	 * @return array of SiteMap objects
	 */
	function walkUpTree($currentSiteMapPage)
	{
		$upTreePages = array();	
		if(!$currentSiteMapPage) return $upTreePages;
		
		$currPage = clone $currentSiteMapPage;
		// walk up the tree to get all parents of the page
		while(isset($currPage))
		{
			array_push($upTreePages, $currPage);
			$currPage = $currPage->Parent();
		} 

		return $upTreePages;
	}

	/**
	 * No need to show breadcrumbs if there is just one single
	 * node in the tree path.
	 * 
	 * @param array $upTreePages array of SiteMap objects
	 * @return String breadcrumbs
	 */
	function walkDownTree($upTreePages)
	{
		$breadcrumbs = "";
		
		if(count($upTreePages) < 2) return $breadcrumbs;
		
		$downTreePages = array_reverse($upTreePages);
		
		$idx = 0;
		$last = count($downTreePages) - 1;

		foreach($downTreePages as $downTreePage)
		{
			// Only need qs if this is not the first page
			// and not the last. Last page is just title, not link
			if($idx < $last)
			{
				$link = ($idx > 0 AND $this->qs) ? "$downTreePage->url?{$this->qs}" : $downTreePage->url;
				$title = $downTreePage->page_title;
				$breadcrumbs .= "<a href=\"{$link}\">{$title}</a> &raquo;\n";
			}
			else
			{
				$breadcrumbs .= $downTreePage->page_title . "\n";
			}
				
			$idx++;
		}

		return $breadcrumbs;
	}
	
	function getLink($identifier)
	{
		global $section;
		
		$identifier = $_REQUEST["identifier"];
		
		if ($section->section == "/")
			$url = "/$identifier";
		else
			$url = "/{$section->section}/$identifier";
	
		return $url;
	}

}

?>