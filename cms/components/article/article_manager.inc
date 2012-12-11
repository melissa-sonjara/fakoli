<?php
Fakoli::using("component", "comment");

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
		$articleTypeSelect = new SelectFieldRenderer($form, "article_type", "Article Type");
		$articleTypeSelect->allowAddEntry();

		$form->getRenderer("message")->height = "480px";
		$form->getRenderer("teaser")->height = "120px";
		
		$sites = query(Site, "ORDER BY site_name");
	
		$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Sites", $sites, "site_name", ArticleSiteXref);

		$form->getRenderer("archive_date")->validator->required = false;
		
		
		return $form;
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
	
	static function upgradeComponent($version)
	{
		$mgr = new ArticleUpgradeManager();
		$mgr->upgrade($version);
	}	
}