<?php
Fakoli::using("html_editor");
?>
<table class='form'>
 <tr>
  <td class='label'><label>Current HTML Tag</label>
  <td class='field'><div id='tag_editor_tagName' style='width: 100px'></div><a id='tag_editor_upButton' class='button' href='#' onclick='new TagEditor().upTag();return false'>Up</a><a id='tag_editor_downButton' class='button' href='#' onclick='new TagEditor().downTag(); return false'>Dn</a></td>
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
<a class='button' href='#'>Done</a>
<script type='text/javascript'>
window.addEvent('domready', function()
{
	new TagEditor().initializeDialog();
});
</script>