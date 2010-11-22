<?php

Fakoli::using("survey");

$identifier = $_REQUEST["identifier"];

echo SurveyMenuView::getView($identifier);
