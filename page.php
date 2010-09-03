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
}
else
{
	try
	{
		$page = Query::create(Page, "WHERE identifier=:i")
					->bind(":i", $identifier)
					->executeSingle();
		
		if (!$page->published && !checkRole("admin")) throw new FakoliException("Unrecognized page '$identifier'");
		$pageView = new PageView($page, "{$page->template}.tpl");
	}
	catch(DataNotFoundException $e)
	{
		try
		{
			$page = Query::create(ComponentPage, "WHERE identifier=:i AND enabled=1")
					->bind(":i", $identifier)
					->executeSingle();
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		}
		catch(DataNotFoundException $e)
		{
			throw new FakoliException("Unrecognized page '$identifier'");
		}
	}
	
}

$page_role = $page->role;

if (!checkRole($page->role))
{
	redirect("/login");
}

echo $pageView->drawView();

?>