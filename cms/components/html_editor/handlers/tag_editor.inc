<?php
Fakoli::using("html_editor");
?>
<table class='form'>
 <tr>
  <td class='label'><label>Current HTML Tag</label>
  <td class='field'><span id='tag_editor_tagName' style='display: inline-block; width: 100px;border: solid 1px #bbb; color: #333; padding: 2px; border-radius: 4px;margin-right:5px;'></span><a id='tag_editor_upButton' class='button' style='margin-right: 4px' href='#' onclick='new TagEditor().up();return false'>&uarr;</a><a id='tag_editor_downButton' class='button' style='font-weight: bold' href='#' onclick='new TagEditor().down(); return false'>&darr;</a></td>
 </tr>
 <tr>
  <td class='label'><label>ID</label></td>
  <td class='field'><input id='tag_editor_id' value='' type='text'/></td>
 </tr>
 <tr>
  <td class='label'><label>Class</label></td>
  <td class='field'><input id='tag_editor_class' value='' type='text' size='60'/></td>
 </tr>
 <tr>
  <td class='label'><label>Style</label></td>
  <td class='field'><input id='tag_editor_style' value='' type='text' size='60'/></td>
 </tr>
</table>
<br/>
<a class='button' href='#' onclick="ModalDialog.getActiveDialog().hide();">Done</a>
<script type='text/javascript'>
window.addEvent('domready', function()
{
	new TagEditor().initializeDialog();
});
</script>