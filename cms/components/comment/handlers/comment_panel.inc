<?php
Fakoli::using("comment");

$view = CommentView::fromGET();

echo $view->writeScript();
$view->drawView();