<?php
Fakoli::using("comment");

$view = CommentView::fromGET();

$script .=  $view->writeScript();

$view->drawView();

$view->drawForm();

?>