<?php

class PageManager
{
	static function displayPage($identifier, &$continue)
	{
		global $page_role;
		
		try
		{
			$page = Query::create(Page, "WHERE identifier=:i")
						->bind(":i", $identifier)
						->executeSingle();
			
			if (!$page->published && !checkRole("admin")) throw new FakoliException("Unrecognized page '$identifier'");
			$pageView = new PageView($page, "{$page->template}.tpl");
		
			$page_role = $page->role;
			
			if (!checkRole($page->role))
			{
				$_SESSION["login_redirect"] = "/$identifier";
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
	
	static function enumeratePages(&$items)
	{
		$pages = Query::create(Page, "ORDER BY page_title")->execute();
		
		$items["Pages"] = $pages;
		return $items;
	}
}
?>