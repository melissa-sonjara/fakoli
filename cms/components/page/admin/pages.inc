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

Fakoli::using("page");
Fakoli::using("site");

Fakoli::assertRole("admin,editor");

Fakoli::usingFeature("data_view", "data_tabs");

$title = "Manage Pages";
$menu_item = "Pages";

$site_id = checkNumeric($_GET["site_id"]);

$sites = query(Site, "ORDER BY site_name");

if (!$site_id) $site_id = $sites[0]->site_id;

$tabs = new DataItemTabBar("tabs", $sites);
$pagesBySite = Query::create(Page, "WHERE site_id=:site ORDER BY identifier")
					->bind(":site", $site_id)
					->execute();

$pageTable = new DataListView($pagesBySite, "pages");
$pageTable->column("Page Identifier", "<a href='/admin/page_form?page_id={page_id}'>{identifier}</a>", true, "width: 10%")
		  ->column("Title", "{page_title}", true, "width: 40%")
		  ->column("Code File", "{php_code_file}", true, "width: 30%")
		  ->column("Created Date", "{created_date:m/d/Y}", true, "width: 10%;text-align: center")
		  ->column("Last Edited", "{edited_date:m/d/Y}", true, "width:10%;text-align: center");
		  
$pageTable->sortable = true;
$pageTable->filter = true;
$pageTable->cssStyle = "width: 100%";
$pageTable->excelFile = "pages.xls";
$pageTable->emptyMessage = "There are no pages for this site.";

$script = $pageTable->writeScript();

$tabs->writeHTML();
?>
<div class="tab_border">
<?

$pageTable->drawView();
?>
<br/><br/>
<input type="button" class="button" value=" Add a New Page" onclick="go('/admin/page_form');">&nbsp;&nbsp;<a class='button' href='/action/page/export'>Export Pages</a>
</div>
<?

?>