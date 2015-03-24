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

class Site extends DataItem
{
	private static $currentSite = null;
	
	// Fields
	var $fields = array("site_id"				=> Number,
						"site_name"				=> String,
						"domain"				=> String,
						"home_page"				=> String,
						"description"			=> Text,
						"meta_tag_description"	=> Text,
						"meta_tag_keyword"		=> String,				
						"theme"					=> String,
						"default_template"		=> String,
						"print_template"		=> String,
						"popup_template"		=> String,
						"mobile_template"		=> String,
						"enabled"				=> Boolean,
						"default_site"			=> Boolean);
	
// Relations
	
	var $relations = array(	"Pages"				=>	Page,
							"Articles"			=>	Article,
							"Events"			=>	Event,
							"Fora"				=>	Forum,
							"ForumSummaries"	=>	ForumSummary,
							"Sections"			=>	Section);
	
	function Pages($constraint = "")
	{
		return $this->getRelatedList(Page, "", $constraint);
	}
	
	function Articles($constraint = "")
	{
		return $this->crossReference(Article, ArticleSiteXref, $constraint);
	}
	
	function Events($constraint = "")
	{
		return $this->crossReference(Event, EventSiteXref, $constraint);
	}
	
	function Fora($constraint = "")
	{
		return $this->crossReference(Forum, ForumSiteXref, $constraint);
	}
	
	function ForumSummaries($constraint = "")
	{
		return $this->crossReference(ForumSummary, ForumSiteXref, $constraint);
	}
	
	function Sections($constraint = "")
	{
		return $this->getRelatedList(Section, "", $constraint);
	}
	
	/**
	 * Returns the Site object that describes the currently active site (i.e. the site
	 * associated with the domain the user is currently accessing).
	 * @throws FakoliException
	 */
	static function getSite()
	{
		global $config;
		
		if (Site::$currentSite)
		{
			return Site::$currentSite;
		}
		
		$domain = $_SERVER['HTTP_HOST'];
		trace("Accessed via domain: $domain", 3);
		
		$sites = Query::create(Site, "WHERE domain=:d and enabled=1 ORDER BY default_site DESC")
				->bind(":d", $domain)
				->execute();
		if (count($sites) > 0) 
		{
			trace("Matching Site record found", 3);
			Site::$currentSite = $sites[0];
			
			// Store in $config
			$config['http_host'] = $sites[0]->domain;
			
			return Site::$currentSite;
		}
		
		$sites = Query::create(Site, "WHERE enabled=1 ORDER BY default_site DESC, site_name ASC")->execute();
		
		if (count($sites) > 0) 
		{
			Site::$currentSite = $sites[0];
			trace("Using default Site record with domain ".Site::$currentSite->domain, 3);			
			return Site::$currentSite;
		}
		
		throw new FakoliException("No site defined");
	}
	
	static function getSites()
	{
		return Query::create(Site, "ORDER BY site_name")->execute();
	}
	
	function getThemeDirectory()
	{
		return SiteManager::getThemeDirectory($this->theme);
	}
	
	// Constructor
	function Site()
	{
		$this->primary_key = "site_id";
		$this->table = "site";
		
		$this->default_format = "{site_name}";
		$this->DataItem(func_get_args());
	}
}
	

?>