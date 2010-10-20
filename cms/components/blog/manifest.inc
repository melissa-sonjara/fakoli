<?php

class BlogManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "blog";
		$component->description = "Basic blog functionality for Fakoli/CMS";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 2;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getAdminMenu()
	{
		return array("Web Site Content"	=> array
			(

				"Blogs"	=>	array("page"	=>	"/admin/blogs",
										  "role"	=>	"admin",
								  		  "weight"	=>	9)
			)
		);
	}	
	
	static function getStyles()
	{
		return array("/fakoli/css/blog.css");
	}
	 
	static function subscribeToEvents()
	{
		return array("ResolveIdentifier" => array(BlogManager, displayBlog),
					 "EnumerateItems"	 => array(BlogManager, enumerateItems),
					 "RegisterBlogViewHandlers" => array(BlogView, registerStandardHandlers));
	}
}

?>