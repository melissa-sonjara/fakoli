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

Fakoli::using("component");
Fakoli::usingFile("framework/data_view.inc");

Fakoli::assertRole("admin");

$tabs = ComponentManager::componentListTabs();

$menu_item = "Components";
$title = "Manage Components";


$components = query('Component', "ORDER BY name");

$table = new DataListView($components, "components");
$table->column("Component Name", "<a href='/admin/component_form?component_id={component_id}'>{name}</a><span style='font-size: 10px'>{description}</span>", true, "width: 70%")
	  ->column("Version", "{version}", true, "width: 10%; text-align: center")
	  ->column("Author",  "{author}", true, "width: 20%; text-align: center");

$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->summary = "List of installed components";

$script .= $table->writeScript();


$tabs->writeHTML();
?>
<div class="tab_border">
<?
$table->drawView();
?>
<br/>
<a class="button" href="/action/component/scan"> Scan for Component Updates </a>
<br/>
<br/>
</div>
