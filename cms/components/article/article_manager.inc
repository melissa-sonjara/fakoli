<?php

class ArticleManager
{
	function ArticleManager()
	{
		
	}
	
	static function buildArticleForm($article)
	{
		$form = new AutoForm($article);
		$form->required("title");
		$form->allowDelete = true;
		$imageSelect = new ImageSelectFieldRenderer($form, "image_id");
		$form->alias("image_id", "Associated Image");
		$form->readonly("created_date");
		$form->hide("author_id"); //TODO - allow author selection/re-assignment?
		$articleTypeSelect = new SelectFieldRenderer($form, "article_type", "Article Type");
		$articleTypeSelect->allowAddEntry();

		$sites = query(Site, "ORDER BY site_name");
	
		$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Sites", $sites, "site_name", ArticleSiteXref);

		$form->getRenderer("archive_date")->validator->required = false;
		
		return $form;
	}
	
	static function upgradeComponent()
	{
		$mgr = new ArticleUpgradeManager();
		$mgr->upgrade();
	}	
}