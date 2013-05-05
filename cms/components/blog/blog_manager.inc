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

Fakoli::using("component");

class BlogSerializationHandler
{
	function BlogSerializationHandler()
	{
	}
	
	
	function export()
	{
		$xml  = "\n<ArticleMap>";
		$xml .= SerializationManager::serialize(Article, "ORDER BY article_id");
		$xml .= SerializationManager::serialize(Blog, "ORDER BY blog_id");
		$xml .= "</ArticleMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(Article, $doc, $tx);
		SerializationManager::unserialize(Blog, $doc, $tx);		
	}		
}

class BlogManager
{
	static function displayBlog($identifier, &$continue)
	{
		try
		{
			$blog = Blog::findByIdentifier($identifier);
			$page = ComponentPage::findByIdentifier("blog", "WHERE enabled=1");
			$_GET["blog"] = $identifier;
		
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		
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
	
	static function enumerateItems($items)
	{
		$blogs = Query::create(Blog, "ORDER BY title")->execute();
		
		$items["Blogs"] = $blogs;
		return $items;
	}
	
	static function buildBlogForm($blog, $redirect, $blogView)
	{
		$form = new AutoForm($blog);
		$form->hide("owner_id", "created_date");
		$form->button("Cancel", $redirect);
		
		$blogTypes = array_keys($blogView->handlers);
		$blogTypes = array_combine($blogTypes, $blogTypes);
		
		$orderSelect = new SelectFieldRenderer($form, "default_article_order", "Default Article Order", array("DESC" => "Newest to Oldest", "ASC" => "Oldest to Newest", "SORT" => "Sorted Order"));
		$readSelect = new CheckListFieldRenderer($form, "read_access", "Read Access", SiteRole::getRolesArray());
		$readSelect->setSize(350, 150, 10);
		$writeSelect = new CheckListFieldRenderer($form, "write_access", "Write Access", SiteRole::getRolesArray());
		$writeSelect->setSize(350, 150, 10);
		$typeSelect = new SelectFieldRenderer($form, "blog_type", "Blog Type", $blogTypes);
		$imageSelect = new ImageSelectFieldRenderer($form, "image_id", false);
		$form->alias("image_id", "Image");
		$form->alias("enable_rss_feed", "Enable RSS Feed");
		$form->alias("max_rss_articles", "Max # of Articles in RSS Feed");	
		$form->annotate("max_rss_articles", "(A value of zero means no limit)");
		return $form;
	}
	
	static function blogTabs($blog_id)
	{
		$tabs = array(	
				"Blog Definition"	=>	"blog_form",
				"Articles"			=>	"blog_articles"
				);

		$bar = new TabBar("tabs", $tabs, "blog_id=$blog_id");
	
		return $bar;
	}
	
	static function buildBlogArticleForm($article, $blog)
	{
		$form = new AutoForm($article);
		$form->required("title");
		$form->allowDelete = true;
		$redirect = "/admin/blog_articles?blog_id={$blog->blog_id}";
		$form->button("Cancel", $redirect);
		$imageSelect = new ImageSelectFieldRenderer($form, "image_id");
		$form->alias("image_id", "Associated Image");
		$form->readonly("created_date");
		$form->hide("author_id"); //TODO - allow author selection/re-assignment?
		$form->hide("article_type");
		
		$sites = query(Site, "ORDER BY site_name");
		$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Sites", $sites, "site_name", ArticleSiteXref);

		$form->getRenderer("archive_date")->validator->required = false;
		
		return $form;
	}
	
	static function getStyles()
	{
		$styles .= "<link href=\"/fakoli/css/tabs.css\" rel=\"stylesheet\"/>\n";
		
		return $styles;
	}	

	static function upgradeComponent($version)
	{
		$mgr = new BlogUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	/**
	 * Respond to fired event DeleteUser
	 *
	 * Delete any records in this component that have dependencies on user object.
	 *
	 * @param obj $user - class SiteUser or custom user class
	 */
	static function deleteUser($user)
	{
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		
		trace("Component blog is deleting objects dependent on user_id {$user_id}", 3);
	
		$blog = new Blog();
		$blog->delete("WHERE owner_id={$user_id}");
	
		return $user;
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("blog", "Articles and Blogs", new BlogSerializationHandler());
		return true;
	}
}

?>