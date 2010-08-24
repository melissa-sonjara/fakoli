<?php
Fakoli::assertRole("admin");

$component_page_id = checkNumeric($_GET["component_page_id"]);

if (!$component_page_id) die("");

$page = new ComponentPage($component_page_id);

$filter = new InclusionFilter("component_page_id");

if (isset($_GET["enabled"]))
{
	$filter->add("enabled");
	$page->enabled = checkNumeric($_GET["enabled"]);
}

if (isset($_GET["role"]))
{
	$filter->add("role");
	$page->role = mysql_escape_string($_GET["role"]);
}

if (isset($_GET["template"]))
{
	$filter->add("template");
	$page->template = mysql_escape_string($_GET["template"]);
}

$page->filter = $filter;
$page->save();
echo "1";
?>