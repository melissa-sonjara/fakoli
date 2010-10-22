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

/*
 * Title: email_template_list.php
 * 
 * Description: table list view of email template text.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 6/7/10
 */


Fakoli::using("email");
Fakoli::usingFeature("data_view");

$emailTemplates = query(EmailTemplate);

$table = new DataListView($emailTemplates, "templates");
$table->column("Name", "<a href=\"/admin/email_template_form?email_template_id={email_template_id}\">{name}</a>", true)
		->column("Recipients", "{recipients}", true)
		->column("Subject", "{subject}", true)
	  	->column("Message", "{message}", true);
	  	
$table->emptyMessage = "There are no email templates defined.";
$table->sortable = true;
$table->pageSize = 4;
$table->filter = true;
$table->cssStyle = "width: 100%";

$script .= $table->writeScript();

$table->drawView();

echo "<p><a class='button' href='/admin/email_template_form'>Add a Template</a></p>\n";
?>