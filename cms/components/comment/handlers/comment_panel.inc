<?php
Fakoli::using("comment");

$view = CommentView::fromGET();

echo $view->writeScript();

?>
<div id="comment_panel">
<?
$view->drawView();

if ($user || Settings::getValue("comment", "allow_anonymous_commenting"))
{
	$view->drawForm();
}
?>
</div>