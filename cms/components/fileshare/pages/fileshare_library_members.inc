<?
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

Fakoli::using("document", "role", "fileshare");
Fakoli::usingFile("framework/tab_bar.inc");

$document_library_id = checkNumeric($_GET["document_library_id"]);

if (!$document_library_id)
{
	redirect("fileshare_libraries");
}

$page->page_title = "Fileshare Library User Community";
$library = new DocumentLibrary($document_library_id);
if(!$library->isOwner() && !checkRole("fileshare,admin"))
	redirect("fileshare_libraries");

$tabs = FileshareManager::libraryTabs($document_library_id);
//$styles .= FileshareManager::getStyles();


$script .= <<<ENDSCRIPT
<script type='text/javascript'>

window.addEvent('domready', function()
{
	var memberSearch = new LibraryManager().memberProgressiveSearch($document_library_id);
});
</script>
ENDSCRIPT;

$mgr = new UserManager();
$class_name = $mgr->getUserClass();
$xrefClass = new FileshareUserXref(); 
$members = Query::create($class_name, "WHERE user_id in (select user_id from {$xrefClass->table} where document_library_id=:d) ORDER BY last_name, first_name")
	->bind(":d", $document_library_id)
	->execute();
?>
<div id="fileshare_content">
<?
$tabs->writeHTML();
?>
<div id="tab_border" style="height: auto;">
<h3>Fileshare Member Community for <?echo $library->name?></h3>
<table class="layout">
<tr><td>
<?
FileshareManager::drawMemberSearchForm($library);
?> 
<div id="member_select">
<div id="member_scrollbox" style="height: 200px;">
<?php
FileshareManager::constructListMembers($members, $library->document_library_id);

$param = "?document_library_id=$document_library_id";

?>
</div>
<div id="submit_block">
<div style="float: left; padding: 2px 0">
<a class="button" href="fileshare_library_form<?php echo $param ?>">BACK</a>
</div>
<div style="float: right; padding: 2px 0">
<a class="button" href="fileshare_library_files<?php echo $param ?>">CONTINUE</a>
</div>
<div style="clear: both"></div>
</div>
</div>
</td></tr></table></div></div>