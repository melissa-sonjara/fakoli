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
 
Fakoli::using("article", "site", "image", "field_renderers");
Fakoli::usingFile("framework/auto_form.inc");

$article = new Article();
$article_id = checkNumeric($_GET["article_id"]);
	
if ($article_id)
{
	$article->load($article_id);
}
else
{
	$article->created_date = now();
}


$form = ArticleManager::buildArticleForm($article);
$redirect = "/articles";
$form->button("Cancel", $redirect);
$form->button("Preview", "javascript:modalPopup(\\'Preview\\', \\'/blog?article_id=$article_id\\', 800, \\'auto\\')");

if(!$article_id || !$article->published)
{
	$form->onSaveComplete = onPublishArticle;
}

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/articles");
	}	
}


if ($form->data->article_id)
{
	$title = "Edit Article";
}
else
{
	$title =  "Add Article";

	if (!$article_id) $form->hide("created_date");
}

$script = $form->writeScript();

$form->drawForm();

function onPublishArticle($form)
{
	ArticleManager::onPublishArticle($form->data);
}
?>
