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

 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 10/9/10
 * 
 ***************************************************************/

Fakoli::using("site_map");


class Breadcrumbs
{	
	static function getBreadcrumbs($page)
	{
		global $breadcrumbs;

		$qs = $_SERVER['QUERY_STRING'];
		$identifier = $_REQUEST["identifier"];
		
		$qs = preg_replace("/identifier=[\\w_]+&*/", "", $qs);
		$qs = preg_replace("/_style=\\w+&*/", "", $qs);
		
		$currentSiteMapPages = query(SiteMap, "WHERE identifier = '{$identifier}'");
		if(count($currentSiteMapPages) > 0)
			$currentSiteMapPage = $currentSiteMapPages[0];
		else
			return $page;
			
		trace("Breadcrumbs:: currentSiteMapPage id {$currentSiteMapPage->identifier}", 3);
		
		if(!$currentSiteMapPage)
			return $page;
			
		$upTreePages = Breadcrumbs::getUpTreePages($currentSiteMapPage);
	
		/*
		 * No need to show breadcrumbs if there is just one single
		 * node in the tree path.
		 */
		if(count($upTreePages) > 1)
		{
			$downTreePages = array_reverse($upTreePages);
			
			$idx = 0;	
			$last = count($downTreePages) - 1;
			$breadcrumbs = "";
	
			foreach($downTreePages as $downTreePage)
			{
				// Only need qs if this is not the first page
				// and not the last. Last page is just title, not link
				if($idx < $last)
				{
					$link = ($idx > 0 AND $qs) ? "$downTreePage->identifier?$qs" : $downTreePage->identifier;
					$breadcrumbs .= "<a href=\"{$link}\">{$downTreePage->page_title}</a> &raquo;\n";
				}
				else
					$breadcrumbs .= $downTreePage->page_title . "\n";
			
				$idx++;
			}
		}
				
		return $page;
	}
			
	static function getUpTreePages($currentSiteMapPage)
	{
		if($currentSiteMapPage)
		{	
			$upTreePages = array();	
			$currPage = $currentSiteMapPage;
	
			// walk up the tree to get all parents of the page
			while(isset($currPage))
			{
				array_push($upTreePages, $currPage);
				$currPage = $currPage->Parent();
			} 
		}
		return $upTreePages;
	}	
}

?>