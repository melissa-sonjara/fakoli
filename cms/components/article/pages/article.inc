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

Fakoli::using("article", "comment");
Fakoli::usingFeature("auto_form");

$article_id = checkNumeric($_GET["article_id"]);

if (!$article_id)
{
	throw new FakoliException("No article specified");
}

$article = new Article($article_id);

$page->page_title = $article->title;

if($article->allow_comment)
{
	CommentView::addCommentPanel();
}


if ($view && $method == "POST")
{
	if($view->save())
	{
		redirect("article?article_id=$article_id");			
	}
}


?>
<div id="article" style="padding-bottom: 20px;"><? 
if ($article->image_id)
{
	$image = $article->Image();
	?><img align="left" style="margin-right: 20px;"
	src="/action/image/thumbnail?image_id=<?echo $image->image_id?>&size=320" alt="<?echo $image->ALT_tag?>" /> <?
}
?> <? echo $article->message ?></div>

<?

if (Settings::getValue("settings", "enable_inline_editing") && Settings::checkPermission("settings", "editor_roles"))
{
	echo "<p id='inline_editing' class='inline_editor_toolbar'><a href='#' class='edit' onclick='new ArticleManager().editArticle({$article->article_id}); return false;'>Edit</a></p>";
}

if($view) 
{
	echo "<div id='comment_panel'>";
	
	$view->drawView();
	if($user || Settings::getValue("comment", "allow_anonymous_commenting"))
	{
		$view->drawForm();
	}
	echo "</div>\n"; // end div comment_panel
}
?></div>



