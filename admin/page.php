<?php
require_once "../include/config.inc";
require_once "../cms/core.inc";

Fakoli::using("component");

Fakoli::assertRole("admin");

$identifier = $_GET["identifier"];

if (!$identifier) $identifier = "index";

$pages = Query::create(AdminPage, "WHERE identifier=:id")
			  ->bind(":id", $identifier)
			  ->execute();
			  
if (count($pages) != 1)
{
	throw new FakoliException("Missing or ambiguous page identifier '$identifier'");
}

$page = $pages[0];

$admin_menu = Fakoli::getAdminMenu();

include $page->server_path;

?>