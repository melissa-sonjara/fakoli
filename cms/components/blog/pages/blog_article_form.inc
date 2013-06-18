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
 
Fakoli::using("article", "site", "image", "field_renderers", "blog");
Fakoli::usingFile("framework/auto_form.inc");

$article = new Article();
$article_id = checkNumeric($_GET["article_id"]);
$blog_id = checkNumeric($_GET["blog_id"]);

if (!$blog_id) redirect("/index");

$blog = new Blog($blog_id);

if ($article_id)
{
	$article->load($article_id);
}
else
{
	$article->created_date = now();
}


$article->article_type = $blog->title;

$form = BlogManager::buildBlogArticleForm($article, $blog);
if(!$article->article_id || !$article->published)
{
	$form->onSaveComplete = onPublishArticle;
}

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/blog_articles?blog_id={$blog->blog_id}");
	}
}

if ($form->data->article_id)
{
	$page->page_title = "Edit Article";
}
else
{
	$page->page_title =  "Add Article";

	if (!$article_id) $form->hide("created_date");
}

$tabs = BlogManager::blogTabs($blog_id);
$tabs->page = "blog_articles";

$styles .= BlogManager::getStyles();

$script = $form->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<?
$form->drawForm();

function onPublishArticle($form)
{
	ArticleManager::onPublishArticle($form->data);
}
?>
</div>
<br/>