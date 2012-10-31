<?php
Fakoli::using("section", "field_renderers");
Fakoli::usingFeature("auto_form");

Fakoli::assertRole("admin");
$ids = checkNumeric($_GET["section_content_id"]);

// Reset each of the supplied section content records to default.

$sectionContent = new SectionContent();
$sectionContent->filter = new InclusionFilter("section_content_id", "template", "role", "use_SSL");

foreach($ids as $id)
{
	$sectionContent->section_content_id = $id;
	$sectionContent->template = "";
	$sectionContent->role = "";
	$sectionContent->use_SSL = false;
	$sectionContent->save();
}
	
Fakoli::end("OK");
?>