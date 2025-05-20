<?php
/**\addtogroup video
 * @{
 */


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
 
Fakoli::using("blog");

class VideoBlogHandler implements IBlogViewHandler
{
	var $blog;
	
	function __construct()
	{
	}
	
	static function register($view)
	{
		$view->registerHandler("Video", new VideoBlogHandler());
		return $view;
	}
	
	function drawArticleList($blog)
	{
		global $page;
		global $script;

		$script .= <<<ENDSCRIPT
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		$$("#article_list a").each(function(elt) 
		{		
			elt.addEvents(
			{
				'mouseover': function()
				{
					this.morph({'background-color': '#A9CDED', 'border-color': '#666666'});
				}.bind(elt),
				'mouseout': function()
				{
					this.morph({'background-color': '#DEFAFF', 'border-color': '#cccccc'});
				}.bind(elt)
			});
		});
	});
	</script>
ENDSCRIPT;
	
		$this->blog = $blog;
		
		$page->page_title = $blog->title;
		
		$articles = $blog->Articles("WHERE published=1 ORDER BY created_date {$this->blog->default_article_order}");	

		echo "<div id='blog_description' class='Video'>{$blog->description}</div>";
		
		$list = new PagedList($articles, "article_list", array($this, formatArticle));
		$list->CSSclass = "Video";
		$list->styles = "clear: both";
		$list->pageSize = 5;
		
		$script .= $list->writeScript();
		$list->drawList();
	}
		
	function formatArticle($article)
	{
		$out = "<a href='/{$this->blog->identifier}?article_id={$article->article_id}' style='display: block'>";
		if ($article->image_id)
		{
			$image = $article->Image();
			$out .= "<img src='/action/image/thumbnail?image_id={$article->image_id}&size=120' alt='{$image->ALT_tag}' align='left'/>";
		}
		
		$out .= "<div class='article_summary'><h4>{$article->title}</h4>";
		if ($article->teaser) $out .= $article->teaser;
		$out .= "</div><div style='clear: both'></div></a>";
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
/**@}*/?>
