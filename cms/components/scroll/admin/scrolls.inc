<?php 
/**************************************************************

 Copyright (c) 2016 Sonjara, Inc

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

Fakoli::using("scroll", "site");
Fakoli::usingFeature("data_view", "data_tabs");

Fakoli::assertRole("admin,editor");

$title = "Manage Scrolls";
$menu_item = "Scrolls";

$site_id = checkNumeric($_GET["site_id"]);

$sites = query(Site, "ORDER BY site_name");

if (!$site_id) $site_id = $sites[0]->site_id;

$tabs = new DataItemTabBar("tabs", $sites);
$scrollsBySite = Query::create(Scroll, "WHERE site_id=:site ORDER BY identifier")
					->bind(":site", $site_id)
					->execute();


$scrollTable = new DataListView($scrollsBySite, "scrolls");
$scrollTable->column("Identifier", "<a href='/admin/scroll_form?scroll_id={scroll_id}'>{identifier}</a>", true, "width: 20%")
			->column("Title", "{title}", true, "width: 55%")
			->column("# Parts", "{countParts()}", true, "width: 20%; text-align: center")
			->column("", "<a href='#' onclick='ScrollPartManager.copyScrollDialog({scroll_id}); return true'><i class='far fa-fw fa-copy'></i></a>", true, "width: 5%");

		  
$scrollTable->sortable = true;
$scrollTable->filter = true;
$scrollTable->cssStyle = "width: 100%";
$scrollTable->emptyMessage = "No Scrolls have been created for this site";

$script = $scrollTable->writeScript();

$tabs->writeHTML();
?>
<div class="tab_border">
<?php

$scrollTable->drawView();
?>
</div>
<br/>
<a class='button' href='/admin/scroll_form'>Add a New Scroll</a>
