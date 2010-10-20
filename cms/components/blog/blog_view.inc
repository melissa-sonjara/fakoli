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

Fakoli::using("user", "blog", "component");
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
	
	function drawArticleList($blog)
	{
		global $page;
		global $script;
		
		$this->blog = $blog;
		
		$page->page_title = $blog->title;
		
		$articles = $blog->Articles("WHERE published=1 ORDER BY created_date {$this->blog->default_article_order}");	

		echo "<div id='blog_description' class='Standard'>{$blog->description}</div>";
		
		$list = new PagedList($articles, "article_list", array($this, formatArticle));
		$list->CSSclass = "Standard";
		$list->styles = "clear: both";
		$list->pageSize = 5;
		
		$script .= $list->writeScript();
		$list->drawList();
	}
		
	function formatArticle($article)
	{
		$out = "<div class='article_summary'>";
		
		if ($article->image_id)
		{
			$image = $article->Image();
			$out .= "<img src='/action/image/thumbnail?image_id={$article->image_id}&size=120' alt='{$image->ALT_tag}' align='left'/>";
		}
		
		$out .= "<h4><a href='/{$this->blog->identifier}?article_id={$article->article_id}'>";
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
		
		echo $article->message;
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
	
	function drawArticleList($blog)
	{
		global $page;
		global $script;
		
		$this->blog = $blog;
		
		$page->page_title = $blog->title;
		
		$articles = $blog->Articles("WHERE published=1 ORDER BY created_date {$this->blog->default_article_order}");	

		echo "<div id='blog_description' class='Standard'>{$blog->description}</div>";
		
		$list = new PagedList($articles, "article_list", array($this, formatArticle));
		$list->CSSclass = "Standard";
		$list->styles = "clear: both";
		$list->pageSize = 10;
		
		$script .= $list->writeScript();
		$list->drawList();
	}
		
	function formatArticle($article)
	{
		$out = "<div class='article_summary'>";
		
		if ($article->image_id)
		{
			$image = $article->Image();
			$out .= "<img src='/action/image/thumbnail?image_id={$article->image_id}&size=120' alt='{$image->ALT_tag}' align='left'/>";
		}
		
		$out .= "<h4><a href='/{$this->blog->identifier}?article_id={$article->article_id}'>";
		$out .= "{$article->title}</a></h4>";
		$out .= "<strong>".formatDateLong($article->created_date)."</strong>";
		if ($article->teaser) $out .= " - ".$article->teaser;
		$out .= "<br/><a href='/{$this->blog->identifier}?article_id={$article->article_id}'>Read More &raquo;</a></div>";
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
	
	function drawArticleList($blog)
	{
		global $page;
		global $script;
		
		$this->blog = $blog;
		
		$page->page_title = $blog->title;
		
		$articles = $blog->Articles("WHERE published=1 ORDER BY created_date {$this->blog->default_article_order}");	

		echo "<div id='blog_description' class='Book'>{$blog->description}</div>";
		
		$list = new PagedList($articles, "article_list", array($this, formatArticle));
		$list->CSSclass = "Standard";
		$list->styles = "clear: both";
		$list->pageSize = 5;
		
		$script .= $list->writeScript();
		$list->drawList();
	}
		
	function formatArticle($article)
	{
		$out = "<div class='article_summary'>";
		
		if ($article->image_id)
		{
			$image = $article->Image();
			$out .= "<img src='/action/image/thumbnail?image_id={$article->image_id}&size=120' alt='{$image->ALT_tag}' align='left'/>";
		}
		
		$out .= "<h4><a href='/{$this->blog->identifier}?article_id={$article->article_id}'>";
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
		
		echo $article->message;
				
		$articles = $blog->Articles("WHERE published=1 ORDER BY created_date {$this->blog->default_article_order}");	
		
		$idx = findItemIndex($articles, "article_id", $article->article_id);
		
		echo "<br/>";
		if ($idx < count($articles) - 1)
		{
?>
<a href="/<?echo $blog->identifier?>?article_id=<?echo $articles[$idx+1]->article_id?>" class="button" style="float: right"><?echo ellipsis($articles[$idx+1]->title, 60, true)?> &raquo;</a>
<?
		}
		
		if ($idx > 0)
		{
?>
<a href="/<?echo $blog->identifier?>?article_id=<?echo $articles[$idx-1]->article_id?>" class="button" style="float: left">&laquo; <?echo ellipsis($articles[$idx-1]->title, 60, true)?></a>
<?
		}
		else
		{
?>
<a href="/<?echo $blog->identifier?>" class="button" style="float: left">&laquo; Back to Contents Page</a>
<?
		}
		
		echo "<div style='clear:both'>&nbsp;</div>";
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
	
	function drawArticleList()
	{
		$type = $this->blog->blog_type;
		if (!array_key_exists($type, $this->handlers)) $type = "Standard";
		
		$this->handlers[$type]->drawArticleList($this->blog);
	}
	
	function drawArticle($article_id)
	{			
		$type = $this->blog->blog_type;
		if (!array_key_exists($type, $this->handlers)) $type = "Standard";
		
		$this->handlers[$type]->drawArticle($this->blog, $article_id);
	}
	
	static function registerStandardHandlers($view)
	{
		$view->registerHandler("Standard", new DefaultBlogViewHandler());
		$view->registerHandler("News", new NewsBlogViewHandler());
		$view->registerHandler("Book", new BookBlogViewHandler());
		
		return $view;
	}	
}