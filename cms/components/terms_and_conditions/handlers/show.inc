<?php
Fakoli::using("terms_and_conditions", "page", "settings");

$identifier = Settings::getValue("terms_and_conditions", "terms_page_identifier");
$title = Settings::getValue("terms_and_conditions", "title");

$page = Page::findByIdentifier($identifier);

?>
<div style="width: 790px; height: 440px; overflow-y: scroll"><?echo $page->description?></div>
<button class="button" onclick="TermsAndConditions.approve()">I Agree to the <?echo $title?> as Shown Above</button>