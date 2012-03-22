<?php
require_once "include/config.inc";
require_once "cms/core.inc";

Fakoli::using("page", "component", "section", "settings");


$page_id = checkNumeric($_GET["page_id"]);
$identifier = checkIdentifier($_GET["identifier"]);
$sectionIdentifier = checkIdentifier($_GET["section"]);

if (!$sectionIdentifier) $sectionIdentifier = "/";

global $section;

$section = Section::createFromIdentifier($sectionIdentifier);

if (!$page_id && !$identifier)
{
	redirect("/{$section->section}/$section->default_page");
}

trace("\$identifier = $identifier", 2);

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
	if (!$section)
	{
		throw new FakoliException("The section you were attempting to access could not be found.");
	}

	$content = $section->getContent($identifier);
	if (!$content)
	{
		throw new FakoliException("The page you were attempting to access could not be found.");
	}
	
	if (!checkRole($section->getRole($content)) ||
		!Settings::checkPermissions($section->getPermissions($content)))
	{
		redirect("/login");		
	}
	
	trace("\$_SERVER['HTTPS'] = {$_SERVER['HTTPS']}", 3);
	trace("\$_SERVER['REQUEST_URI'] = {$_SERVER['REQUEST_URI']}", 3);
	
	$https = $_SERVER['HTTPS'];
	if ($https == "off") $https = false; // For IIS
	
	$ssl = $section->use_SSL || $content->use_SSL;
	
	if ($ssl && !$https )
	{
		redirect("https://".$config['http_host'].$_SERVER['REQUEST_URI']);
	}
	else if (!$ssl && $https && $identifier != "login" && !isset($_REQUEST["login"]))
	{
		redirect("http://".$config['http_host'].$_SERVER['REQUEST_URI']);
	}

	ComponentManager::fireEvent("ResolveIdentifier", $identifier);
}

?> 