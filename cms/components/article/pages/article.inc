<?php

Fakoli::using("article", "comment");
Fakoli::usingFeature("auto_form");

$article_id = checkNumeric($_GET["article_id"]);

$article = new Article($article_id);

$page->page_title = $article->title;
?>
<div id="article"><? 
if ($article->image_id)
{
	?><img align="left" style="margin-right: 20px;"
	src="/action/image/thumbnail?image_id=<?echo $image_id?>&size=320" alt="<?echo $image->ALT_tag?>" /> <?
}
?> <? echo $article->message ?></div>
<br />
<br />
<br />
<br />
<br />
<br />

<?
if($article->allow_comment) {

	?>
<h3>Comment on this article</h3>

<div id="comment"><?php 


$cv = new CommentsView();

$cv->CommentForm($article_id, $user->site_user_id);

if ($method == POST && $_POST['title']) {
	$cv->saveComments();
}

$cv->displayComments();

?>
</div>
<?
}
?>



