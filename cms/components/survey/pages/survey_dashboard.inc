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


/* survey_dashboard.inc
 *
 * Description: Survey management dashboard; Survey tab
 * of logged in user interface
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 6/3/2010
 */

Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("data_view");

if(!$user)
	redirect("/login");

$constraint = "WHERE deleted=0 ";
if(!checkRole("admin"))
{
	$pk = $user->getPrimaryKey();
	$constraint .= "AND user_id={$user->$pk} ";
}

$surveys = Query::create(Survey, "$constraint  ORDER BY status, start_date DESC")
	->execute();

$table = new DataListView($surveys, "surveys");
$table->column("Survey Title", array(SurveyTableHelper, formatSurveyLink), true, "width: 15%")
	->column("Author", "{User.first_name} {User.last_name}", true, "width: 10%")
	->column("Status", array(SurveyTableHelper, formatSurveyStatus), true, "width: 10%")
	->column("Date Sent", array(SurveyTableHelper, formatStartDate), true, "width: 12%")
	->column("Percentage of Surveys Received", array(SurveyTableHelper, getSurveyProgressBar), true, "width: 20%")
 	->column("Actions", array(SurveyTableHelper, formatActions), true, "width: 20%");
	;
 	
$table->emptyMessage = "There are no surveys.";
$table->sortable = true;
$table->pageSize = 10;
$table->filter = true;
$table->cssStyle = "width: 98%";
$script .= $table->writeScript();

$table->drawView();
?>
<br>
<a class="button" href="survey_form">Create a Survey</a>
