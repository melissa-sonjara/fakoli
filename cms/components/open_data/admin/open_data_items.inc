<?php
/**************************************************************

Copyright (c) 2015 Sonjara, Inc

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

Fakoli::using("OpenData");
Fakoli::usingFeature("data_view");

$menu_item = "Open Data";

$openData = Query::create(OpenData, "ORDER BY class")->execute();

$table = new DataListView($openData, "open_data");

$table->column("Class", "<a href='open_data_form?open_data_id={open_data_id}'>{class}</a>", true)
	   ->column("Field", "{field}", true)
	  ->column("Open Data Level", "{open_data_level}", true)
	  ->column("Source", "{source}", true)
	  ->column("Data Standard", "{data_standard}", true, "text-align: right");
	  
$table->pageSize = 20;
$table->sortable = true;
$table->filter = true;
$table->emptyMessage = "No open data meta tags have been created yet.";

$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a class='button' href='open_data_form'>Add a New Open Data Meta Tag</a>