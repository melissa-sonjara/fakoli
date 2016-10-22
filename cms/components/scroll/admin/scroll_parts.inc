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

Fakoli::using("scroll");

Fakoli::assertRole("admin,editor");

$menu_item = "Scrolls";

$scroll_id = checkNumeric($_GET["scroll_id"]);

if (!$scroll_id) redirect("scrolls");

$scroll = new Scroll($scroll_id);

$parts = $scroll->Parts();

$table = new DataListView($parts, "parts");
$table->column("Title", "<a href='scroll_part_form?scroll_id={scroll_id}&scroll_part_id={scroll_part_id}'>{title}</a>", true, "width:40%")
	  ->column("Content Type", "{content_type:prettify}", true, "width: 10%; text-align: center")
	  ->column("Code File", "{php_code_file}", true, "width: 50%");

$table->filter = false;
$table->sortable = false;
$table->paginate = false;
$table->enableDragReorder("/action/scroll/reorder_scroll_parts");
$table->emptyMessage = "No parts have been added to this scroll";
$table->dragText = "<span style='font-size: 10px'>Click and drag to reorder parts</span>";

$tabs = ScrollManager::scrollTabs($scroll_id);

$script .= $table->writeScript();

$tabs->writeHTML();
?>
<div class='tab_border'>
<?php 
$table->drawView();
?>
<br/>
<a class='button' href='scroll_part_form?scroll_id=<?php echo $scroll_id?>&content_type=HTML'>Add an HTML Part</a>&nbsp;&nbsp;
<a class='button' href='scroll_part_form?scroll_id=<?php echo $scroll_id?>&content_type=Code'>Add a Code Part</a>
</div>
