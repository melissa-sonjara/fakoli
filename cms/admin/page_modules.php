<?php

require_once "../../include/config.inc";

require_once "../datamodel/page.inc";
require_once "../datamodel/module.inc";
require_once "../datamodel/menus.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";
require_once "../../framework/tree.inc";
require_once "include/page_tabs.inc";

$menu_item = "Pages";

$page_id = checkNumeric($_GET["page_id"]);

if (!$page_id) redirect("index.php");

$page = new Page($page_id);

$tabs = pageTabs($page_id);

$title = "Position Modules for {$page->page_title}";

$modulePositions = reindexList($page->PageModuleXrefs(), "module_id");
$modules = query(Module, "ORDER BY title");
$positions = $page->getPositions();

if ($method == "POST")
{
	$xref = new PageModuleXref();
	$xref->delete("WHERE page_id=$page_id");
	
	foreach($_POST["module"] as $module_id => $position)
	{
		if ($position)
		{
			$xref = new PageModuleXref();
			$xref->page_id = $page_id;
			$xref->module_id = $module_id;
			$xref->position = $position;
			$xref->sort_order = $_POST["sort_order"][$module_id];
			$xref->save();
		}
	}
	
	redirect("page_modules.php?page_id=$page_id");
}

require_once admin_begin_page;

$tabs->writeHTML();
?>
<div id="form" style="clear:left;border:solid 1px #000; padding: 4px;">
<form method="POST" action="?page_id=<?echo $page_id?>">
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

 /*if($module->title == "Login") { 
 	echo '<option selected="Login">Login</option>';
 	$position->sort_order=1;
 }*/
 
if ($module->title == "Search") { 
	
	echo "<option selected='Right'>Right</option>";
	$position->sort_order=1;
}
else {
    echo "<option selected=''></option>";
}
 
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
<input type="submit" class="button" name="submit" value="Update Module Positions"/><NOBR>&nbsp;&nbsp;&nbsp;<input type="button" class="button" value="Preview Page" name="preview" onclick="window.open('/page.php?page_id=<?echo $page_id?>', 'popupwindow1', 'width=900, height=600, resizable=no, scrollbars=yes'); return false;"/>
<?

?>
</form>
</div>
<?
require_once admin_end_page;

?>
