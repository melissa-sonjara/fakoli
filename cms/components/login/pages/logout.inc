<?php


session_destroy();

$section = Section::findSection("/");
$url = $section->getSectionURL();
if (!$url) $url = "/index";

redirect($url);
?>