<?php
Fakoli::using("comment");

$view = CommentView::fromGET();

$script .=  $view->writeScript();

$view->drawView();

if ($user || Settings::getValue("comment", "allow_anonymous_commenting"))
{
	$view->drawForm();
}
?>