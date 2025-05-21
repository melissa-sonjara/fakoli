<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::using("section", "menu", "module");
Fakoli::usingFile("framework/auto_form.inc");

require_once "include/section_tabs.inc";

$menu_item = "Sections";

$section_id = checkNumeric($_GET["section_id"]);

if (!$section_id) redirect("/index");

$section = new Section($section_id);

$tabs = sectionTabs($section_id);
$tabs->section = "section_modules";

$title = "Position Modules for {$section->section_title}";

$modulePositions = reindexList($section->SectionModuleXrefs(), "module_id");
$modules = query('Module', "ORDER BY title");
$positions = $section->getPositions();

if ($method == "POST")
{
	$xref = new SectionModuleXref();
	$xref->delete("WHERE section_id=$section_id");
	$section->clearModuleCache();
	
	foreach($_POST["module"] as $module_id => $position)
	{
		if ($position)
		{
			$xref = new SectionModuleXref();
			$xref->section_id = $section_id;
			$xref->module_id = $module_id;
			$xref->position = $position;
			$xref->sort_order = $_POST["sort_order"][$module_id];
			$xref->save();
		}
	}
	
	redirect("/admin/section_modules?section_id=$section_id");
}

$tabs->writeHTML();
?>
<div class="tab_border">
<form method="POST" action="?section_id=<?echo $section_id?>">
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
    echo "<option value=''></option>";
 
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
<input type="submit" class="button" name="submit" value="Update Module Positions"/>
<?

?>
</form>
</div>
