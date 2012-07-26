<?php
Fakoli::using("survey");

$identifier = $_REQUEST["identifier"];

$menu = new SurveyMenuView($identifier, "top");
$menu->drawView();