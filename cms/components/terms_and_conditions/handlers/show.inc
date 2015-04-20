<?php
Fakoli::using("terms_and_conditions", "page", "settings");

$identifier = Settings::getValue("terms_and_conditions", "terms_page_identifier");
$title = Settings::getValue("terms_and_conditions", "title");
$button = Settings::getValue("terms_and_conditions", "button_label");

if (!$button) $button = "I Agree to the {$title} as Shown Above";

$page = Page::findByIdentifier($identifier);

?>
<div style="width: 790px; height: 440px; overflow-y: scroll;margin-bottom: 4px"><?echo $page->description?></div>
<button class="button" onclick="TermsAndConditions.approve()"><?echo $button?></button>