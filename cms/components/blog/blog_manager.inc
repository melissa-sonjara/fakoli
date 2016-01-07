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
	
	static function inlineEditingControls($blog)
	{

		if (Settings::getValue("settings", "enable_inline_editing") && Settings::checkPermission("settings", "editor_roles"))
		{
			echo "<p id='inline_editing'><a href='#' onclick='new BlogManager().editBlog({$blog->blog_id}); return false;'>Edit</a>&nbsp;&nbsp;<a href='#' onclick='new ArticleManager().editArticle(\"\", {$blog->blog_id}); return false;'>Add an Article</a></p>";
		}		
	}
	
	static function enumerateItems($items)
	{
		$blogs = Query::create(Blog, "ORDER BY title")->execute();
		
		$items["Blogs"] = $blogs;
		return $items;
	}
	

	static function registerSearchables($searchables)
	{
		trace("BlogManager registerSearchables", 3);
		$searchables["blog"] = array(Blog);
		return $searchables;
	}
	
	static function registerSolrAdapter()
	{
		SolrManager::registerSolrAdapter(Blog, new BlogSolrAdapter());
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
	
	static function setDefaults()
	{
		Settings::setDefaultValue("blog", "allow_subscription_options", "0", Boolean, "Whether to allow users to choose subscription type options. Cron job must be setup for the website to call handler file 'send_digest'.");
		Settings::setDefaultValue("blog", "weekly_digest_day", "Mon", String, "The day of the week when the weekly digest will be sent for sites that allow that option.", "", "Sun\nMon\nTue\nWed\nThu\nFri\nSat");
		Settings::setDefaultValue("blog", "subscribe_button_format", "Subscribe to {title}", String, "Format string to use for subscribe buttons");
		
		BlogManager::setDefaultEmailTemplates();
		BlogManager::setDefaultMergeCodes();
	}
	
	/**
	 * For blog, need default templates:
	 * blog_subscription
	 * blog_published
	 * blog_unsubscribe_confirmation
	 */
	static function setDefaultEmailTemplates()
	{
		global $config;

		$sitename = $config["sitename"];
		
		$message = "This is an automated message from {$sitename}. You have successfully subscribed to the {Blog.title}.<br><br>To unsubscribe, <a href='{getUnsubscribeUrl()}'>click here</a> or follow this link: {getUnsubscribeUrl()}.";
		EmailTemplate::setDefaultTemplate("blog_subscription", "{subscriber_email}", "{$sitename} Blog Subscription: {Blog.title}", $message, "BlogSubscriber");
		
		$message = "Greetings. This is an automated message from {$sitename}. A new post has been published on {Blog.title}. Click the link below to read it.<br><br>{getBlogUrl()}?article_id=[article_id]<br><br>You are receiving this message because you subscribed to {Blog.title}. To unsubscribe, please follow this link: {getUnsubscribeUrl()}";
		EmailTemplate::setDefaultTemplate("blog_published", "{subscriber_email}", "{$sitename} post: {Blog.title}", $message, "BlogSubscriber");
	
		$message = "This is an automated message from {$sitename}. You have successfully unsubscribed to the {Blog.title}.";
		EmailTemplate::setDefaultTemplate("blog_unsubscribe_confirmation", "{subscriber_email}", "{$sitename} Blog Subscription: {Blog.title}", $message, "BlogSubscriber");
	
	}
	
	/**
	 * Merge codes:
	 * blog_title
	 * article_id
	 * blog_identifier
	 * unsubscribe_url
	 * blog_url
	 * 
	 */	
	static function setDefaultMergeCodes()
	{
		MergeCode::setDefaultMergeCode("blog_title", "Blog.title", "BlogSubscriber", "Blog Title");
		MergeCode::setDefaultMergeCode("article_id", "", "BlogSubscriber", "ID of the article whose publication is the cause of the subscriber email being sent. This value is provided in the calling code b/c it can't be accessed through the blog subscriber.");
		MergeCode::setDefaultMergeCode("blog_identifier", "Blog.identifier", "BlogSubscriber", "The CMS page identifier of the blog.");
		MergeCode::setDefaultMergeCode("unsubscribe_url", "getUnsubscribeUrl", "BlogSubscriber", "The url to unsubscribe from a blog.");
		MergeCode::setDefaultMergeCode("blog_url", "getBlogUrl", "BlogSubscriber", "The url to view blog posts.");
	}

	static function registerTaxonomyClasses($classes)
	{
		$classes[] = Blog;
		return $classes;
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
	
	/**
	 * Custom save handlerf or blog_subscription_form. Avoid saving duplicate
	 * subscriptions (same email and blog_id).
	 * 
	 * @param unknown $form
	 */
	static function saveBlogSubscription($form)
	{
		$blogSubscriber = $form->data;
		
		$subscriptions = Query::create(BlogSubscriber, "WHERE subscriber_email=:email AND blog_id=:blog_id")
			->bind(":email", $blogSubscriber->subscriber_email, ":blog_id", $blogSubscriber->blog_id)
			->execute();
		
		$subscription = (count($subscriptions)) ? $subscriptions[0] : null;
		
		if($subscription && $subscription->subscription_type != $blogSubscriber->subscription_type)
		{
			$subscription->subscription_type = $blogSubscriber->subscription_type;
			$subscription->filter = new InclusionFilter("subscription_type");
			$subscription->save();

			$form->data = $subscription;
		}
		else if(!$subscription)
		{
			if (!$blogSubscriber->subscription_token) 
			{
				$blogSubscriber->subscription_token = BlogSubscriptionManager::generateToken();
			}
			$blogSubscriber->save();
		}
		
		return true;
	}
	
	static function sendDigests()
	{
		$mgr = new BlogSubscriptionManager();
		
		$mgr->sendDigests();
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("blog", "Articles and Blogs", new BlogSerializationHandler());
		return true;
	}
	
	static function registerScheduledTaskWorkers()
	{
		ScheduledTaskManager::registerWorker("blog", "Send Digests", array(BlogManager, sendDigests));
	}
}

?>