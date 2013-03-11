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

/**
 * Temporary short-term solution to need to delete test
 * survey responses.
 * 
 */
Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("data_view");

$responses = Query::create(SurveyResponse, "WHERE status='submitted' AND survey_id NOT IN (SELECT survey_id FROM survey WHERE deleted=true)")->execute();

$table = new DataListView($responses, "surveyResponses");
$table->column("Token", array(SurveyTableHelper, formatResponseLink), true)
	->column("Survey", "{Survey.title}", true)
	->column("Status", "{status:prettify}", true)
	->column("Date Submitted", "{last_modified:short}", true)
	->column("", "<a class='button' href='/action/survey/survey_response_delete?response_id={response_id}'>Delete</a>")
	;

$table->emptyMessage = "There are no survey responses.";
$table->sortable = true;
$table->pageSize = 20;
$table->filter = false;
$table->cssStyle = "width: 100%";
$table->excelFile = "surveyResponses.xls";

$script .= $table->writeScript();

$table->drawView();
?>