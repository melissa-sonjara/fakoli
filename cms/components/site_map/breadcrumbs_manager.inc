<?php
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
	static function getBreadcrumbs($identifier, $qs)
	{				
		$component = querySingle(Component, "WHERE name = 'site_map'");

		if(!$component OR $component->enabled == false)
			return;

		$currentSiteMapPage = Query::create(SiteMap, "WHERE identifier=:i")
						->bind(":i", $identifier)
						->executeSingle();
	
		trace("Breadcrumbs:: currentSiteMapPage id {$currentSiteMapPage->identifier}", 3);
		
		if(!$currentSiteMapPage)
			return;

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
				
		return $breadcrumbs;
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