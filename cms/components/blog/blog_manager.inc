<?php

Fakoli::using("component");

class BlogManager
{
	static function displayBlog($identifier, &$continue)
	{
		try
		{
			$page = ComponentPage::findByIdentifier("blog", "WHERE enabled=1");
			$_GET["blog"] = $identifier;
		
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		
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
	
	static function enumerateItems(&$items)
	{
		$blogs = Query::create(Blog, "ORDER BY title")->execute();
		
		$items["Blogs"] = $blogs;
		return $items;
	}
}

?>