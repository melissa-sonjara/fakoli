<?php
Fakoli::using("comment");

$commentView = CommentView::fromGET();

echo $view->writeScript();
$view->drawView();