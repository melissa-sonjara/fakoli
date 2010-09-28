<?php
Fakoli::using("component", "menu", "module");
Fakoli::usingFile("framework/auto_form.inc");

require_once "include/component_tabs.inc";

$menu_item = "Components";

$page_id = checkNumeric($_GET["component_page_id"]);

if (!$page_id) redirect("/index");

$page = new ComponentPage($page_id);

$tabs = componentPageTabs($page_id);
$tabs->page = "component_page_modules";

$title = "Position Modules for {$page->page_title}";

$modulePositions = reindexList($page->ComponentPageModuleXrefs(), "module_id");
$modules = query(Module, "ORDER BY title");
$positions = $page->getPositions();

if ($method == "POST")
{
	$xref = new ComponentPageModuleXref();
	$xref->delete("WHERE component_page_id=$page_id");
	
	foreach($_POST["module"] as $module_id => $position)
	{
		if ($position)
		{
			$xref = new ComponentPageModuleXref();
			$xref->component_page_id = $page_id;
			$xref->module_id = $module_id;
			$xref->position = $position;
			$xref->sort_order = $_POST["sort_order"][$module_id];
			$xref->save();
		}
	}
	
	redirect("/admin/component_page_modules?component_page_id=$page_id");
}

$tabs->page = "/admin/component_page_modules";

$tabs->writeHTML();
?>
<div id="form" style="clear:left;border:solid 1px #000; padding: 4px;">
<form method="POST" action="?component_page_id=<?echo $page_id?>">
<table class="list">
 <tr>
  <th>Module</th>
  <th>Position</th>
  <th>Sort Order</th>
 </tr>
<?
foreach($modules as $module)
{
	
	 $position = $modulePositions[$module->module_id]; 

?>
<tr>
 <td><strong><a href="<?echo $module->getAdminForm()?>?module_id=<?echo $module->module_id?>" ><?echo $module->title?></a></strong></td>
 <td><select name="module[<?echo $module->module_id?>]">
 <?
 	option("", "", $position->position);
 	
	foreach($positions as $p)
	{
 		option($p, ucwords($p), $position->position);
 	}
 ?>
 </select></td>
 <td>
 <input type="text" name="sort_order[<?echo $module->module_id?>]" maxlength="4" style="width: 50px" value="<?echo $position->sort_order?>"/></td>
</tr>	
<?	
}
?>
</table>
<br/>
<input type="submit" class="button" name="submit" value="Update Module Positions"/>&nbsp;&nbsp;&nbsp;<input type="button" class="button" value="Preview Component Page" name="preview" onclick="window.open('/<?echo $page->identifier?>', 'popupwindow1', 'width=900, height=600, resizable=no, scrollbars=yes'); return false;"/>
<?

?>
</form>
</div>
<?


?>
