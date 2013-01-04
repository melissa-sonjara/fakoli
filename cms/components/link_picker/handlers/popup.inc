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

Fakoli::usingFeature("tree");

$editor = $_GET["Editor"];

$tabs = linkPickerTabs("Editor=$editor");
$tabs->page = "/action/link_picker/link_picker";
$tabs->dynamicLoad("link_picker_panel");


$tree = new TreeControl("items");
$tree->width = "100%";
$tree->height = "300";
$tree->selectMode = "single";

$script .= $tree->writeScript();
$script .= <<<ENDSCRIPT
<script type="text/javascript">

function insertLink(url)
{
	var target = document.id('target').value;
	theEditors['$editor'].formatSelection("<a href=\"" + url + "\" target=\"" + target + "\">");
	closeLinkPicker();
}
</script>
ENDSCRIPT;

$script .= $tabs->writeScript();

echo $script;
?>
<label>Open Link in </label>
<select id='target'>
 <option value="_self">Same Window</option>
 <option value="_blank">New Window</option>
</select><br/>
<?
$tabs->writeHTML();
?><div style="clear: left" id="link_picker_panel"><?echo $tree->writeHTML()?></div>