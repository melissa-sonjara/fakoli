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

Fakoli::using("section");
Fakoli::usingFeature("data_view", "data_tabs");

$menu_item = "Sections";

$site_id = checkNumeric($_GET["site_id"]);

$sites = query('Site', "ORDER BY site_name");


if (!$site_id) $site_id = $sites[0]->site_id;

$tabs = new DataItemTabBar("tabs", $sites);

$sections = Query::create('Section', "WHERE site_id=:s ORDER BY section")
				 ->bind(":s", $site_id)
				 ->execute();

$table = new DataListView($sections, "sections");
$table->column("Section", "<a href='/admin/section_form?section_id={section_id}'>{section}</a>", true)
	  ->column("Title", "{section_title}", true)
	  ->column("Default Role", "{default_role}", true)
	  ->column("Default Page", "<a href='/{section}/'>{default_page}</a>", true);

$table->filter = true;
$table->sortable = true;
$table->emptyMessage = "No sections have been created.";

$script .= $table->writeScript();

$tabs->writeHTML();
?>
<div class='tab_border'>
<?

$table->drawView();
?>
<br/>
<a class='button' href='/admin/section_form?site_id=<?echo $site_id?>'>Add a New Section</a>&nbsp;&nbsp;<a class='button' href='/action/section/export'>Export Section Layout</a>&nbsp;&nbsp;<a class='button' href='section_import'>Import Section Layout</a>
</div>
