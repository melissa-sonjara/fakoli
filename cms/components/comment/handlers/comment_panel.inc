<?php
Fakoli::using("comment");

$view = CommentView::fromGET();

echo $view->writeScript();
$view->drawView();

if ($user || Settings::getValue("comment", "allow_anonymous_commenting"))
{
	$view->drawForm();
}