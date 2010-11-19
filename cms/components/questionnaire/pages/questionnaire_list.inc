<?php
/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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
 * Description: Displays a list of questionnaire of one type.
 * User can choose one to edit, create new, or
 * copy an existing questionnaire.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 12/21/09
 */

Fakoli::using("questionnaire");
Fakoli::usingFeature("data_view");

$questionnaires = query(Questionnaire, "ORDER BY questionnaire_id DESC");

$table = new DataListView($questionnaires, "questionnaires");
$table->column("Title", "<a href=\"/questionnaire_form?questionnaire_id={questionnaire_id}\">{title}</a>", false)
	->column(" ", "<a class = \"button\" href=\"/action/questionnaire/questionnaire_copy?questionnaire_id={questionnaire_id}\">Create Copy</a>", false, "width: 20%");

$table->emptyMessage = "There are no questionnaires defined.";
$table->sortable = false;
$table->pageSize = 20;
$table->filter = false;

$script .= $table->writeScript();

$table->drawView();

echo "<p><a class='button' href='/questionnaire_form'>Add a Questionnaire</a></p>\n";

?>