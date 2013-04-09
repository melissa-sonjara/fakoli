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

Fakoli::using("document", "fileshare");
Fakoli::usingFeature("data_view", "facet_manager", "tab_bar");

$document_library_id = checkNumeric($_GET["document_library_id"]);
$keyword = mysql_escape_string($_GET["keyword"]);

if (!$document_library_id)
{
	redirect("fileshare_libraries");
}

$page->page_title = "Fileshare Library Files";

$library = new DocumentLibrary($document_library_id);

if(!$library->allowAccess())
	redirect("/fileshare_libraries");

$mgr = new FileshareManager($library, $keyword);

$table = new DataListView($mgr->documents, "library_documents");
$table->column("Document", array($mgr, formatDocumentTitle), true, "width: 45%")
	->column("Author", array($mgr, getDocumentAuthor), true, "width: 20%") 
	->column("Publication Date", "{publication_date}", true, "width: 15%")
	->column("Last Modified", "{last_modified:short}", true, "width: 20%")
	;
		  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "There are no documents in this library.";
$table->cssStyles = "width: 640px";

if($library->isOwner() || checkRole("fileshare,admin"))
	$tabs = FileshareManager::libraryTabs($document_library_id);

$facetManager = new FacetManager('facet_manager', $table);
TaxonomyManager::addFacets($facetManager, Document, true, "160px", "200px", "200px");

$script .= $facetManager->writeScript();
$script .= $mgr->writeScript();
$script .= $table->writeScript();
$script .= <<<ENDSCRIPT
<script type='text/javascript'>
window.addEvent('domready', function()
{
	var keywordSearch = new LibraryManager().keywordProgressiveSearch($document_library_id);
});
</script>
ENDSCRIPT;
?>
<div id="fileshare_content">
<?
if($tabs)
{
	$tabs->writeHTML();
?>
<div id="tab_border">
<?php 
}
?>
<table id="library_search" class="layout" style="width: auto"><tr><td>
<table class="layout" style="width: 100%">
<tr>
<td style="width: 40%">
<h3>Files in <?echo $library->name?></h3>
</td>
<td style="text-align: right; padding-right: 10px;">
<form id="keyword_search_form" method="GET" action="">
<label for='keyword'>Keyword</label>
<input id='LibrarySearch_form_keyword' type='text' name='keyword' autocomplete='off' value='' size='20'/>
&nbsp;
<input type='submit' class='button' name='submit' value='Search'/>
<input type="hidden" name="document_library_id" value="<?php echo $document_library_id ?>"/>
</form>
</td>
</tr>
</table>
<?
$facetManager->drawForm();

$table->drawView();

if($library->fileshareAllowUpload())
{
?><br>
<div id="submit_block">
<a class="button" href="#" onclick="new LibraryManager().uploadFileshareFile(<?php echo $document_library_id ?>); return false">Upload a File</a>
</div>

<?php
}
?>
</td></tr></table>
<?php 

if($tabs)
{
?>
</div>
<?php 
}
?></div>