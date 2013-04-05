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


Fakoli::using("calendar");
Fakoli::usingFeature("data_view");

$menu_item = "Time Zones";

$zones = Query::create(TimeZone, "ORDER BY time_zone_name")->execute();

$table = new DataListView($zones, "time_zones");
$table->column("Time Zone Name", "<a href='/admin/time_zone_form?time_zone_id={time_zone_id}'>{time_zone_name}</a>", true)
	->column("Standard Offset", "{standard_offset}", true, "text-align: right")
	->column("Daylight Offset", "{daylight_offset}", true, "text-align: right")
	;
 
$table->sortable = true;
$table->filter = false;
$table->pageSize = 10;
$table->emptyMessage = "No time zones have been defined";

$script .= $table->writeScript();
$table->drawView()
?>
<p><a class='button' href='/admin/time_zone_form'>Add a Time Zone</a></p>