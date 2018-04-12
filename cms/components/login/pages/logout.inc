<?php


session_destroy();
ComponentManager::fireEvent("OnLogout");

$section = Section::findSection("/");
$url = $section->getSectionURL();
if (!$url) $url = "/index";

redirect($url);
?>