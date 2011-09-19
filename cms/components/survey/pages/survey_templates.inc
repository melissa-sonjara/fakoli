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
 * Description: Lets admin define sets of questions for use
 * in a survey. A question may be part of more than one set.
 */

Fakoli::using("survey");
Fakoli::usingFeature("data_view");

$surveyTemplates = query(SurveyTemplate, "ORDER BY title");

$table = new DataListView($surveyTemplates, "surveyTemplates");
$table->column("Title", "<a href=\"/survey_template_form?survey_template_id={survey_template_id}\">{title}", true, "width: 25%")
	->column("Introduction", "{introduction:80}", false, "width: 20%")
	->column("Message", "{message:80}", false, "width: 20%")
	->column("Status", getSurveyTemplateStatus, false, "width: 10%")
	->column("View", "<a class='button' href='#' onclick=\"new Survey().showSurveyTemplateView({survey_template_id}); return false;\">View</a>", false, "width: 5%");

$table->emptyMessage = "There are no survey templates defined.";
$table->sortable = false;
$table->pageSize = 10;
$table->filter = false;
$table->cssStyle = "width: 90%";
$script .= $table->writeScript();

?>
<div id='survey_table'><br>
<br>
<?php
$table->drawView();
?> <br />
<a class="button" href="/survey_template_form">Create a Survey Template</a>
</div>
<?php

function getTemplateView($surveyTemplate)
{
	return "<a class=\"button\" href=\"javascript:modalPopup('Survey Template', '/survey_template_preview_popup?survey_template_id={$surveyTemplate->survey_template_id}', '700px', 'auto');\">View</a>\n";
}

function getSurveyTemplateStatus($surveyTemplate)
{
	return $surveyTemplate->statusTypes[$surveyTemplate->status];
}
?>