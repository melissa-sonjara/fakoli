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

Fakoli::using("calendar", "taxonomy");
Fakoli::usingFeature("data_view", "filter_form", "facet_manager");

$menu_item = "Calendar Events";

function formatEvent($event)
{
	return "<a href='/admin/event_form?event_id={$event->event_id}'>{$event->title}</a><em> {$event->event_type}</em><p>".ellipsis($event->description, 120)."</p>";
}

$title = "Events";

$filter = CalendarHelper::eventFilterSetup();
$constraint .= $filter->getConstraint();

$events = query('Event', "$constraint ORDER BY start_date");
$lastUpdated = "";

$table = new DataListView($events, "events");
$table->column("Event", 'formatEvent', true, "width: 30%")
	  ->column("Calendar", "{Calendar.name}", true, "width: 15%")
	  ->column("Location", "{location}", true, "width: 25%")
	  ->column("Start Date", "{start_date}", true, "width: 15%")
	  ->column("End Date", "{end_date}", true, "width: 15%");
	  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 20;
$table->emptyMessage = "No Events defined";

$facetManager = new FacetManager('facet_manager', $table);
TaxonomyManager::addFacets($facetManager, 'Event', true, "160px", "200px", "200px");

$script .= $facetManager->writeScript();
$script .= $filter->writeScript();
$script .= $table->writeScript();

$facetManager->drawForm();
$filter->drawForm();
$table->drawView();
?>
<br/>
<input type="button" class="button" value="  Add a new Event  " onclick="go('/admin/event_form');">
