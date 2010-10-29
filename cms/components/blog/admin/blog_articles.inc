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

Fakoli::using("article", "blog");
Fakoli::usingFile("framework/data_view.inc");

require_once "include/blog_tabs.inc";

$blog_id = checkNumeric($_GET["blog_id"]);

if (!$blog_id) redirect("/admin/index");

global $blog;
$blog = new Blog($blog_id);

$title = "Articles in {$blog->title}";
$menu_item = "Articles";

$script .= <<<ENDSCRIPT
<script type="text/javascript">
function togglePublished(img, id)
{
	var enable;
	if (img.alt == "Published")
	{
		publish = 0;
	}
	else
	{
		publish = 1;
	}
	
	var request = new Request({
									url: "/action/article/update?article_id=" + id +"&published=" + publish,
									method: 'get',
									onSuccess: function (response) 
									{ 
										if (response == 1) 
										{
											img.src = publish ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
											img.alt = publish ? "Published" : "Unpublished";
										}
									},
									onFailure: function () {},
									async: true
								});
	request.send();
}
</script>
ENDSCRIPT;

function showPublished($article)
{
	$img = $article->published ? "on.png" : "off.png";
	$alt = $article->published ? "Published" : "Unpublished";

	return "<img src='/fakoli/images/$img' alt='$alt' onclick='togglePublished(this, {$article->article_id})' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer'/>";
}
$tabs = blogTabs($blog_id);

$articles = $blog->Articles("ORDER BY last_modified DESC");

$table = new DataListView($articles, "articles");
$table->column("Title", formatTitle, true)
	  ->column("Author", "{getAuthorName()}", true)
	  ->column("Date Created", "{created_date}", true)
	  ->column("Published", showPublished, false, "text-align: center");
	  
	  
$table->sortable = true;
$table->filter = true;
$table->excelFile = "articles.xls";
$table->pageSize = 20;
$table->emptyMessage = "No articles have been created yet.";

function formatTitle($item)
{
	global $blog;
	return "<a title=\"{$item->title}\" href=\"/admin/blog_article_form?blog_id={$blog->blog_id}&article_id={$item->article_id}\">". ellipsis($item->title, 80) ."</a>";
}

$script .= $table->writeScript();



$tabs->writeHTML();
?>
<div id="form"  style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$table->drawView();
?>
<br/>
<a class="button" href="/admin/blog_article_form?blog_id=<?echo $blog_id?>">Add an Article</a>
<br/>
</div>
<br/>