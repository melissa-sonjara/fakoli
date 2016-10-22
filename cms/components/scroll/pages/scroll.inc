<?php
Fakoli::using("scroll");

$scroll = Fakoli::get("content_object");

$view = new ScrollView($scroll);
$view->drawView();