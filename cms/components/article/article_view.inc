<?php
/**\addtogroup article
 * @{
 */



class ArticleListView
{
	var $table;
	
	function __construct()
	{

		$articles = query('Article', "ORDER BY last_modified DESC");
	
		$this->table = $this->buildTable($articles);
	}
	
	function buildTable($articles)
	{
		$table = new DataListView($articles, "articles");
		$table->column("Title", array($this, formatTitle), true)
	  		->column("Article Type", "{article_type}", true)
	  		->column("Author", "{getAuthorName()}", true)
	  		->column("Date Created", "{created_date}", true)
	  		->column("Published", array($this, showPublished), false, "text-align: center");
	  
		$table->sortable = true;
		$table->filter = true;
		$table->excelFile = "articles.xls";
		$table->pageSize = 20;
		$table->emptyMessage = "No articles have been created yet.";
		
		return $table;
	}
	
	function writeScript()
	{
		return $this->table->writeScript();
	}
	
	function drawView()
	{
		$this->table->drawView();
	}
	

	function showPublished($article)
	{
		$img = $article->published ? "on.png" : "off.png";
		$alt = $article->published ? "Published" : "Unpublished";

		return "<img src='/fakoli/images/$img' alt='$alt' onclick='toggleArticlePublished(this, {$article->article_id})' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer'/>";
	}

	function formatTitle($item)
	{
		return "<a title=\"{$item->title}\" href=\"/article_form?article_id={$item->article_id}\">". ellipsis($item->title, 80) ."</a>";
	}	
}

class AdminArticleListView extends ArticleListView
{
	function __construct()
	{
		parent::__construct();
	}	
	
	function formatTitle($item)
	{
		return "<a title=\"{$item->title}\" href=\"/admin/article_form?article_id={$item->article_id}\">". ellipsis($item->title, 80) ."</a>";
	}		
}
/**@}*/?>
