<?php
$page_role = "admin";

require_once "../../include/config.inc";

require_once "../../cms/datamodel/menus.inc";
require_once "../../cms/datamodel/site.inc";
require_once "../../include/permissions.inc";

$sites = query(Site, "ORDER BY site_name");

$menusBySite = groupedQuery(Menu, "ORDER BY name", "site_id");

$title = "Menus";

require_once admin_begin_page;

foreach($sites as $site)
{
?>
<h3><?echo $site->site_name?> (<a href="<?echo $site->domain?>" target="_blank"><?echo $site->domain?>)</a></h3>
<?
	if (array_key_exists($site->site_id, $menusBySite))
	{
		$menus = $menusBySite[$site->site_id];
?>
<dl>
<?
		foreach($menus as $menu)
		{
			
?>
	<dt><a href="menu_form.php?menu_id=<?echo $menu->menu_id?>"><?echo $menu->name?> (<?echo $menu->identifier?>)</a></dt>
	<dd><?echo $menu->description?></dd>
<?		
		}

?>
</dl>
<?
	}
	else
	{
?>
<p><em>No menus have been defined for this site.</em></p>
<?
	} 
}
?>
<br/>
<a class="button" href="menu_form.php">Add a Menu</a>
<?
require_once admin_end_page;
?>
