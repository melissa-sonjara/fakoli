<?php
require_once "include/config.inc";
require_once "cms/datamodel/page.inc";
require_once "cms/components/views/page_view.inc";

$page_id = checkNumeric($_GET["page_id"]);
$identifier = mysql_escape_string($_GET["identifier"]);

if (!$page_id && !$identifier) $identifier = "index";

if ($page_id)
{
	$page = new Page($page_id);
}
else
{
	$page = querySingle(Page, "WHERE identifier='$identifier'");
}

if (!$page) die("Unrecognized page '$identifier'");
if (!$page->published && !checkRole("admin")) die("Unrecognized page '$identifier'");

$page_role = $page->role;

if (!checkRole($page->role))
{
	redirect("/login");
}

require_once "include/permissions.inc";

$pageView = new PageView($page, "{$page->template}.tpl");

echo $pageView->drawView();
?>