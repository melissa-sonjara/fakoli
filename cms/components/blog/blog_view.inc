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

Fakoli::using("user", "blog", "article", "comment", "component");
Fakoli::usingFeature("paged_list");

/**
 * IBlogViewHandler defines the interface that BlogViewHandlers must provide.
 * @author andy
 *
 */
interface IBlogViewHandler
{
	/**
	 * Draw the list of articles for the specified blog.
	 * @param Blog $blog the Blog object
	 */
	function drawArticleList($blog);
	
	/**
	 * Draw the specific article in the blog.
	 * @param Blog $blog the Blog object
	 * @param integer $article_id the ID of the Article
	 */
	function drawArticle($blog, $article_id);
}



/**
 * DefaultBlogViewHandler provides a standard, no-frills display for blog entries.
 * @author andy
 *
 */
class DefaultBlogViewHandler implements IBlogViewHandler
{
	function DefaultBlogViewHandler()
	{
	}
	
	static function register($view)
	{
		$view->registerHandler("Standard", new DefaultBlogViewHandler());
		return $view;
	}
	
	function drawArticleList($blog, $year = false, $month = false, $term_id = null)
	{
		global $page;
		global $script;

		$this->blog = $blog;
		
		$page->page_title = $blog->title;
		
		if ($year)
		{
			$constraint = " AND YEAR(publish_date)=$year";

			if ($month)
			{
				$constraint .= " AND MONTH(publish_date) = $month";
			}			

		}
		
		if ($term_id)
		{
			$constraint .= " AND article_id IN (SELECT id FROM taxonomy_term_association where class='Article' AND term_id=$term_id)";
		}
		
		$articles = $blog->Articles("WHERE published=1 AND publish_date <= curdate()" . $constraint);	
		
		echo "<div id='blog_description' class='Standard'>{$blog->description}</div>";
		
		BlogManager::inlineEditingControls($blog);
		
		$list = new ServerPagedList($articles, "article_list", array($this, formatArticle));
		$list->CSSclass = "Standard";
		$list->styles = "clear: both";
		
		if ($blog->articles_per_page)
			{
				$list->pageSize = $blog->articles_per_page;
			}
		else
			{
				$list->pageSize = 5;
			}
		
		$script .= $list->writeScript();
		$list->drawList();
	}
		
	function formatArticle($article)
	{
		$out = "<div class='article_summary'>";
		
		if ($article->image_id)
		{
			$image = $article->Image();
			$out .= "<img src='/action/image/iconize?image_id={$article->image_id}&size=120' alt='{$image->ALT_tag}' align='left'/>";
		}
		
		$out .= "<h4><a href='{$this->blog->identifier}?article_id={$article->article_id}'>";
		$out .= "{$article->title}</a></h4>";
		
		if ($article->teaser) $out .= $article->teaser;
		$out .= "<div class='blog_timestamp'>";
		$out .= "By " . $article->author . ", posted on " . $article->formatPostDate();
		$out .= "</div>";
		$out .= "</div>";
		return $out;
	}
	
	function drawArticle($blog, $article_id)
	{	
		global $page;
		$article = new Article($article_id);
		$page->page_title = $article->title;
		
		echo $article->message;
		echo "<div class='blog_timestamp'>";
		echo "By " . $article->author . ", posted on " .$article->formatPostDate();
		echo "</div>";
		if ($article->tags)
		{
			echo "<div class='blog_tags'>" . $article->tags . "</div>";
		}		
	}
}




/**
 * NewsBlogViewHandler provides a simple blog view tailored displaying news articles.
 * @author andy
 *
 */
class NewsBlogViewHandler implements IBlogViewHandler
{
	function NewsBlogViewHandler()
	{
	}
	
	static function register($view)
	{
		$view->registerHandler("News", new NewsBlogViewHandler());
		return $view;
	}
	
	function drawArticleList($blog, $year = false, $month = false, $term = null)
	{
		global $page;
		global $script;
		
		$this->blog = $blog;
		
		$page->page_title = $blog->title;
		
		if ($year)
		{
			$constraint = " AND YEAR(publish_date) = $year";
			
			if ($month)
			{
				$constraint .= " AND MONTH(publish_date) = $month";
			}			

		}
		
		if ($term_id)
		{
			$constraint .= " AND article_id IN (SELECT id FROM taxonomy_term_association where class='Article' AND term_id=$term_id)";
		}
		
		$articles = $blog->Articles("WHERE published=1 AND publish_date <= curdate()" . $constraint);	
		
		echo "<div id='blog_description' class='Standard'>{$blog->description}</div>";
	
		BlogManager::inlineEditingControls($blog);
				
		$list = new ServerPagedList($articles, "article_list", array($this, formatArticle));
		$list->CSSclass = "Standard";
		$list->styles = "clear: both";
		$list->pageSize = $blog->articles_per_page;
		
		$script .= $list->writeScript();
		$list->drawList();
	}
		
	function formatArticle($article)
	{
		
		$out = "<div class='article_summary'>";
		
		if ($article->image_id)
		{
			$image = $article->Image();
			$out .= "<img src='/action/image/iconize?image_id={$article->image_id}&size=120' alt='{$image->ALT_tag}' align='left'/>";
		}
		
		$out .= "<h4><a href='{$this->blog->identifier}?article_id={$article->article_id}'>";
		$out .= "{$article->title}</a></h4>";
		$out .= $article->format("<strong>{formatPostDate()}</strong>");
		if ($article->teaser) $out .= " - ".$article->teaser;
		$out .= "<br/><a href='{$this->blog->identifier}?article_id={$article->article_id}'>Read More &raquo;</a></div>";
		return $out;
	}
	
	function drawArticle($blog, $article_id)
	{	
		global $page;
		$article = new Article($article_id);
		$page->page_title = $article->title;
		
		echo $article->message;
		
?><br/><a class="button" href="/<?echo $blog->identifier?>">&laquo; Back to <?echo $blog->title?></a><?
	}
}


class BookBlogViewHandler implements IBlogViewHandler
{
	function DefaultBlogViewHandler()
	{
	}
	
	static function register($view)
	{
		$view->registerHandler("Book", new BookBlogViewHandler());
		return $view;
	}
	
	function drawArticleList($blog, $year = false, $month = false, $term_id = null)
	{
		global $page;
		global $script;
		
		$this->blog = $blog;
		
		$page->page_title = $blog->title;

		if ($year)
		{
			$constraint = " AND YEAR(publish_date) = $year";

			if ($month)
			{
				$constraint .= " AND MONTH(publish_date) = $month";
			}			
		}
		
		if ($term_id)
		{
			$constraint .= " AND article_id IN (SELECT id FROM taxonomy_term_association where class='Article' AND term_id=$term_id)";
		}
		
		
		$articles = $blog->Articles("WHERE published=1 AND publish_date <= curdate()" . $constraint);	
		
		echo "<div id='blog_description' class='Book'>{$blog->description}</div>";

		BlogManager::inlineEditingControls($blog);
		
		$list = new PagedList($articles, "article_list", array($this, formatArticle));
		$list->CSSclass = "Standard";
		$list->styles = "clear: both; width: 100%";
		$list->pageSize = $blog->articles_per_page;
		
		$script .= $list->writeScript();
		$list->drawList();
	}
		
	function formatArticle($article)
	{
		$out = "<div class='article_summary'>";
		
		if ($article->image_id)
		{
			$image = $article->Image();
			$out .= "<img src='/action/image/iconize?image_id={$article->image_id}&size=120' alt='{$image->ALT_tag}' align='left'/>";
		}
		
		$out .= "<h4><a href='{$this->blog->identifier}?article_id={$article->article_id}'>";
		$out .= "{$article->title}</a></h4>";
		
		if ($article->teaser) $out .= $article->teaser;
		$out .= "</div>";
		return $out;
	}
	
	function drawArticle($blog, $article_id)
	{	
		global $page;
		$article = new Article($article_id);
		$page->page_title = $article->title;
		
		echo "<div class='chapter'>\n";
		
		echo $article->message;
				
		$articles = $blog->Articles("WHERE published=1");	
		
		$idx = findItemIndex($articles, "article_id", $article->article_id);
		
		echo "<br/>";
		
		if ($idx < count($articles) - 1)
		{
?>
<a href="<?echo $blog->identifier?>?article_id=<?echo $articles[$idx+1]->article_id?>" class="button" style="float: right"><?echo ellipsis($articles[$idx+1]->title, 60, true)?> &raquo;</a>
<?
		}
		
		if ($idx > 0)
		{
?>
<a href="<?echo $blog->identifier?>?article_id=<?echo $articles[$idx-1]->article_id?>" class="button" style="float: left">&laquo; <?echo ellipsis($articles[$idx-1]->title, 60, true)?></a>
<?
		}
		else
		{
?>
<a href="<?echo $blog->identifier?>" class="button" style="float: left">&laquo; Back to Contents Page</a>
<?
		}
		
		echo "<div style='clear:both'>&nbsp;</div></div>";
	}
}

class BlogView
{
	var $blog;
	var $handlers;
	
	function BlogView($blog)
	{
		$this->blog = $blog;
		ComponentManager::fireEvent("RegisterBlogViewHandlers", $this);
	}
	
	function registerHandler($type, $handler)
	{
		$this->handlers[$type] = $handler;
	}
	
	function drawSubscribeButton()
	{
		if (!Settings::getValue("blog", "allow_subscription_options")) return;
		
		if ($this->blog->allow_subscriptions)
		{
			$buttonFormat = Settings::getValue("blog", "subscribe_button_format");
			
			echo $this->blog->format("<a id='blog_subscription_button' class='button' href='#' onclick='new BlogManager().showSubscriptionDialog({blog_id},\"{title:jsSafe}\"); return false;'>$buttonFormat</a>");
		}
	}
	
	function drawArticleList($year = false, $month = false, $term_id = null)
	{
		$type = $this->blog->blog_type;
		if (!array_key_exists($type, $this->handlers)) $type = "Standard";
	
		trace ("Month is " . $month, 3);
		
		$this->drawSubscribeButton();

		$this->handlers[$type]->drawArticleList($this->blog, $year, $month, $term_id);
	}
	
	function drawArticle($article_id)
	{		
		global $script;
		global $method;
		global $user;
			
		$type = $this->blog->blog_type;
		if (!array_key_exists($type, $this->handlers)) $type = "Standard";
		
		$this->handlers[$type]->drawArticle($this->blog, $article_id);

		if (Settings::getValue("settings", "enable_inline_editing") && Settings::checkPermission("settings", "editor_roles"))
		{
			$article = new Article($article_id);
			$versioningControls = ComponentManager::fireEvent("RenderVersioningControls", $article);
			echo "<p id='inline_editing'><a href='#' onclick='new ArticleManager().editArticle({$article_id}, {$this->blog->blog_id}); return false;'>Edit</a>$versioningControls</p>";
		}

		$this->drawSubscribeButton();
		
		if ($this->blog->allow_comments && $article_id)
		{
			CommentView::addCommentPanel();
		}
	}
	
	static function registerStandardHandlers($view)
	{
		$view->registerHandler("Standard", new DefaultBlogViewHandler());
		$view->registerHandler("News", new NewsBlogViewHandler());
		$view->registerHandler("Book", new BookBlogViewHandler());
		
		return $view;
	}	
}

class BlogListView
{
	var $table;
	
	function BlogListView()
	{
		$blogs = Query::create(Blog, "ORDER BY title")->execute();
		
		$this->table = $this->buildTable($blogs);
	}
	
	function buildTable($blogs)
	{
		$table = new DataListView($blogs, "blogs");
		$table->column("Title", array($this, formatTitleLink), true, "width: 40%")
	  		->column("Identifier", "{identifier}", true, "width: 30%")
	  		->column("Owner", "{getOwnerName()}", true, "width: 20%")
	  		->column("# Articles", "{countArticles()}", true, "width: 10%; text-align: center")
	  		->column("Published", array($this, showPublished), true, "width: 10%; text-align: center");
	  
		$table->sortable = true;
		$table->filter = true;
		$table->pageSize = 15;
		$table->emptyMessage = "No blogs have been created yet.";
		
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
	
	function formatTitleLink($blog)
	{
		return $blog->format("<a href='blog_form?blog_id={blog_id}'>{title}</a>");
	}

	function showPublished($blog)
	{
		$img = $blog->published ? "on.png" : "off.png";
		$alt = $blog->published ? "Published" : "Unpublished";
		
		
		return "<img src='/fakoli/images/$img' alt='$alt' onclick='toggleBlogPublished(this, {$blog->blog_id})' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer'/>";
	}	
}

class AdminBlogListView extends BlogListView
{
	function AdminBlogListView()
	{
		parent::BlogListView();
	}
	
	function formatTitleLink($blog)
	{
		return $blog->format("<a href='/admin/blog_form?blog_id={blog_id}'>{title}</a>");
	}
}

class BlogArticlesView
{
	var $blog;
	var $table;
	
	function BlogArticlesView($blog)	
	{
		$this->blog = $blog;
		
		$articles = $blog->Articles();

		$this->table = $this->buildTable($articles);	
	}
	
	function buildTable($articles)
	{
		$table = new DataListView($articles, "articles");
		$table->column("Title", array($this, formatTitle), true)
	  		->column("Author", "{getAuthorName()}", true)
	  		->column("Date Published", "{publish_date}", true)
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
		global $blog;
		return "<a title=\"{$item->title}\" href=\"blog_article_form?blog_id={$blog->blog_id}&article_id={$item->article_id}\">". ellipsis($item->title, 80) ."</a>";
	}
	
	function drawAddButton()
	{
		echo "<br/>\n<a class='button' href=\"blog_article_form?blog_id={$this->blog->blog_id}\">Add an Article</a>\n<br/>\n";
	}
}

class AdminBlogArticlesView extends BlogArticlesView
{
	function AdminBlogArticlesView($blog)
	{
		parent::BlogArticlesView($blog);
	}

	function buildTable($articles)
	{
		$table = parent::buildTable($articles);
		if ($this->blog->default_article_order == "SORT")
		{
			$table->enableDragReorder("/action/blog/reorder_articles?blog_id={$this->blog->blog_id}");
			$table->dragText = "<span style='font-size: 10px'>Click and drag to change the order of the articles</span>";
		}
		
		return $table;
	}
	
	function formatTitle($item)
	{
		global $blog;
		return "<a title=\"{$item->title}\" href=\"/admin/blog_article_form?blog_id={$blog->blog_id}&article_id={$item->article_id}\">". ellipsis($item->title, 80) ."</a>";
	}	
	
	function drawAddButton()
	{
		echo "<p>\n<a class='button' href=\"/admin/blog_article_form?blog_id={$this->blog->blog_id}\">Add an Article</a>\n</p>\n";
	}
}

class BlogSubscriberListView
{
	var $blog;
	var $table;
	
	function BlogSubscriberListView ($blog_id)
	{
		$blog = new Blog ($blog_id);
		
		$constraint = " WHERE blog_id = {$blog_id}";
		
		$blog_subscribers = query(BlogSubscriber, $constraint);
		
		$this->table = new DataListView($blog_subscribers, "blogSubscribers");
		$this->table->column("Subscriber Email", "{subscriber_email}", true);
		
		if (Settings::getValue("blog", "enable_extended_subscription_fields"))
		{
			$this->table->column("First Name", "{first_name}", true);
			$this->table->column("Last Name", "{last_name}", true);
			$this->table->column("Organization", "{organization}", true);
		}
		
		$this->table->column("Remove", array(BlogSubscriberListView, formatUnsubscribeLink), true, "width: 40%; text-align: center");
			
		$this->table->excelFile = "blog_subscribers.xls";
		$this->table->emptyMessage = "Currently there are no subscribers to this blog.";
		$this->table->zebra = 1;
	}

	function formatUnsubscribeLink($blog_subscriber)
	{
		$blog_id = $blog_subscriber->blog_id;
		$blog_subscriber_id = $blog_subscriber->blog_subscriber_id;
		
		$unsubscribeLink = "<a href='/action/blog/unsubscribe?blog_id={$blog_id}&blog_subscriber_id={$blog_subscriber_id}'>Remove</a>";
		return $unsubscribeLink ;
	}
	
	function drawView()
	{
		global $script;
		$script .= $this->table->writeScript();
		
		$this->table->drawView();
	}
}