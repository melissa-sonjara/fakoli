<?php
Fakoli::using("scroll");

$scroll = Fakoli::get("content_object");
$page->page_title = $scroll->title;

$view = new ScrollView($scroll);
$view->drawView();