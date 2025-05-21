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

$ids = checkNumeric($_GET["section_content_id"]);

$qs = "section_content_id[]=".implode("&section_content_id[]=", $_GET["section_content_id"]);

if (!$ids) redirect("/index");

// Default dialog to properties for first selected item
$content = new SectionContent($ids[0]);
$section = $content->Section();

$title = "Position Modules for {$section->section_title}";

$modulePositions = reindexList($content->SectionContentModuleXrefs(), "module_id");
$modules = query('Module', "ORDER BY title");
$positions = $content->getPositions();

if ($method == "POST")
{
	$section->clearModuleCache();
		
	foreach($ids as $section_content_id)
	{
		$xref = new SectionContentModuleXref();
		$xref->delete("WHERE section_content_id=$section_content_id");
		
		foreach($_POST["module"] as $module_id => $position)
		{
			if ($position)
			{
				$xref = new SectionContentModuleXref();
				$xref->section_content_id = $section_content_id;
				$xref->module_id = $module_id;
				$xref->position = $position;
				$xref->sort_order = $_POST["sort_order"][$module_id];
				$xref->save();
			}
		}
	}
	
	Fakoli::end("OK");
}

$script .= <<<ENDSCRIPT
<script type='text/javascript'>

window.addEvent('domready', function() 
{
	document.id('SectionContentModules_form').iFrameFormRequest(
	{
		'onComplete': function(response) { sectionContentManager.modulesChanged(response); }, 
		'onFailure': function() {document.id('SectonContentModules_form_error').set('text','Failed to communicate with server'); }
	});
});

</script>
ENDSCRIPT;
?>
<div style="clear:left;border:solid 1px #000; padding: 4px;">
<p class='error' style='display: none' id='SectionContentModules_form__error'></p>
<form id="SectionContentModules_form" method="POST" action="/action/section/content_modules?<?echo $qs?>">
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
