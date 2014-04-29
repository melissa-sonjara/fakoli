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

Fakoli::using("page");

class PageSerializationHandler
{
	function PageSerializationHandler()
	{
	}
	
	function export()
	{
		$xml .= "\n<PageMap>";
		$xml .= SerializationManager::serialize(Page, "ORDER BY page_id");
		$xml .= SerializationManager::serialize(PageModuleXref);
		$xml .= "\n</PageMap>";
		return $xml;
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(Page, $doc, $tx);
		SerializationManager::unserialize(PageModuleXref, $doc, $tx);
	}	
}

class PageManager
{

	static function pageTabs($key)
	{
		$tabs = array(	"Content"	=>	"/admin/page_form",
						"Modules"	=>	"/admin/page_modules"
		);
	
		$qs = ($key) ? "page_id=$key" : "";
		return new TabBar("tabs", $tabs, $qs);
	}
	
	static function displayPage($identifier, &$continue)
	{
		global $page_role;
		
		try
		{
			$page = Query::create(Page, "WHERE identifier=:i")
						->bind(":i", $identifier)
						->executeSingle();
			
			if (!$page->published && !checkRole("admin")) throw new FakoliException("Unrecognized page '$identifier'");

			// JDG 11/2011 - why did the code think the template would not have tpl at the end??
			// avoid adding 2nd "tpl" 
			$page_template = (preg_match("/tpl$/", $page->template)) ? $page->template : "{$page->template}.tpl";
			$pageView = new PageView($page, $page_template);
		
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
	
	static function enumeratePages($items)
	{
		$pages = Query::create(Page, "ORDER BY page_title")->execute();
		
		$items["Pages"] = $pages;
		return $items;
	}
	
	static function deleteRole($del_role)
	{
		$constraint = "WHERE role like '%$del_role%'";
		$pages = Query::create(Page, $constraint)->execute();
		
		RoleManager::deleteRoleFromString($pages, $del_role);
		
		return $del_role;		
	}

	static function registerSearchables($searchables)
	{
		trace("PageManager registerSearchables", 3);
		$searchables["page"] = array(Page);
		return $searchables;
	}
	
	static function registerSolrAdapter()
	{
		SolrManager::registerSolrAdapter(Page, new PageSolrAdapter());
	}
	
	static function registerTaxonomyClasses($classes)
	{
		$classes[] = Page;
		return $classes;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new PageUpgradeManager();
		$mgr->upgrade($version);
	}	
		
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("page", "CMS Pages", new PageSerializationHandler());
		return true;
	}
}
?>