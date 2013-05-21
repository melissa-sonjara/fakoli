<?php
Fakoli::using("component", "comment", "blog");

class ArticleManager
{
	function ArticleManager()
	{
		
	}
	
	static function buildArticleForm($article)
	{
		if (!$article->article_id)
		{
			global $user;
			
			$mgr = new UserManager();
			$article->author = (method_exists($mgr, getUserFullName)) ? $mgr->getUserFullName($user) : "";
		}

		$form = new AutoForm($article);
		$form->required("title");
		$form->allowDelete = true;
		$imageSelect = new ImageSelectFieldRenderer($form, "image_id");
		$form->alias("image_id", "Associated Image");
		$form->readonly("created_date");
		$form->hide("author_id"); //TODO - allow author selection/re-assignment?
		$articleTypeSelect = new SelectFieldRenderer($form, "article_type", "Article Type", Blog::getArticleTypeOptions());
		//$articleTypeSelect->allowAddEntry();

		$form->getRenderer("message")->height = "480px";
		$form->getRenderer("teaser")->height = "120px";
		
		$sites = query(Site, "ORDER BY site_name");
	
		$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Sites", $sites, "site_name", ArticleSiteXref);

		$form->getRenderer("archive_date")->validator->required = false;
		
		
		return $form;
	}
	
	/**
	 * When an article is published, send an email to instant
	 * notification subscribers.
	 * 
	 * @param obj $article class Article
	 * @param String $email_template_name (e.g., 'blog_published')
	 * 
	 * @return Number $rtn true if sent; false if could not be sent
	 */
	static function sendEmailToSubscribers($article, $email_template_name)
	{
		if(!$article->published) return;

		$blog = $article->Blog("WHERE subscription_type='instant'");
		if(!$blog) return;

		$subscribers = $blog->Subscribers();
		
		if(!count($subscribers)) return;
		
		$emailTemplate = EmailTemplate::getEmailTemplate($email_template_name);

		if(!$emailTemplate) return;
		
		// Get the article id now before sending to email manager which won't be able to get it
		// from the subscriber obj.
		$emailTemplate->message = str_replace("[article_id]", $article->article_id, $emailTemplate->message);
		
		foreach($subscribers as $subscriber)
		{
			$mgr = new EmailManager($subscriber, $emailTemplate);
			$rtn = $mgr->sendEmail();
		}
	}
	
	static function registerCommentAdapter()
	{
		CommentManager::registerAdapter("article", Article, ArticleCommentXref);
	}
	
	static function registerSearchables($searchables)
	{
		trace("ArticleManager registerSearchables", 3);
		$searchables["article"] = array(Article);
		return $searchables;
	}
	
	static function registerTaxonomyClasses($classes)
	{
		$classes[] = Article;
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
		
		trace("Component article is deleting objects dependent on user_id {$user_id}", 3);
	
		$article = new Article();
		$article->delete("WHERE author_id={$user_id}");
		
		return $user;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ArticleUpgradeManager();
		$mgr->upgrade($version);
	}	
}