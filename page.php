<?php
require_once "include/config.inc";
require_once "cms/core.inc";

Fakoli::using("page", "component");


$page_id = checkNumeric($_GET["page_id"]);
$identifier = mysql_escape_string($_GET["identifier"]);

if (!$page_id && !$identifier) $identifier = "index";

if ($page_id)
{
	$page = new Page($page_id);

	$page_role = $page->role;
	
	if (!checkRole($page->role))
	{
		redirect("/login");
	}
	
	echo $pageView->drawView();
	
}
else
{
	ComponentManager::fireEvent("ResolveIdentifier", $identifier);
}

?>