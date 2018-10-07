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

function showEventBubble(id, link)
{
	document.id(id).show();
	document.id(id).fade('in');
	var coord = document.id(link).getCoordinates();
	document.id(id).setStyles({'top': coord.top - document.id(id).getCoordinates().height + 4, 'left': coord.left - 60});
}

function hideEventBubble(id)
{
	document.id(id).fade('out');
}

var eventDialog;

function showEventDetail(id, handler_class)
{
	eventDialog = modalPopup("Event Details", '/action/calendar/event_details?event_id=' + id + '&handler_class=' + handler_class, '800px', 'auto', true, true);
}

function editEvent(id)
{
	eventDialog = modalPopup("Edit Event", '/action/calendar/event_edit?event_id=' + id, '800px', 'auto', true, true);
}

function newEvent(calendar_id)
{
	eventDialog = modalPopup("Add Event", '/action/calendar/event_edit?calendar_id=' + calendar_id, '800px', 'auto', true, true);
}

function editEventResult(result)
{
	window.location.reload();
}

function showEventList(event_ids, handler_class)
{
	eventDialog = modalPopup("List of Events", '/action/calendar/event_list?event_ids=' +event_ids + '&handler_class=' + handler_class, '800px', 'auto', true, true);
}

