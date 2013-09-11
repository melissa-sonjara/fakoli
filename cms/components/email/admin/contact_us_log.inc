<?php
/**************************************************************

 Copyright (c) 2013 Sonjara, Inc

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

Fakoli::using("email");
Fakoli::usingFeature("data_view");

$menu_item = "Contact Us Log";

$messages = Query::create(ContactUs, "ORDER BY date_sent DESC")->execute();

$table = new DataListView($messages, "contact_us_messages");
$table->column("Subject", "<a href='contact_us_message?contact_us_id={contact_us_id}'>{subject|<em>No Subject</em>}", true, 'width: 25%')
	  ->column("From", "{full_name} &lt;{email}&gt;", true, "width: 25%")
	  ->column("Contact Topic", "{ContactTopic.topic}", true, "width: 25%")
	  ->column("Date", "{date_sent}", true, "width: 20%");

$table->pageSize = 25;
$table->filter = true;
$table->sortable = true;
$table->emptyMessage = "No Contact Us messages have been received";
$table->excelFile = "contact_us_log.xls";

$script .= $table->writeScript();
$table->drawView();
?>