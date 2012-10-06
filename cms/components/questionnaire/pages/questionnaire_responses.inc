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

Fakoli::using("questionnaire");
Fakoli::usingFeature("data_view");

$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

if(!$questionnaire_id)
	redirect("index");

$questionnaire = new Questionnaire($questionnaire_id);
$page->page_title = "Questionnaire Responses for {$questionnaire->title}";

$mgr = new QuestionnaireResultsManager($questionnaire);

$responses = $questionnaire->Responses("ORDER BY date_submitted");

$table = new DataListView($responses, "questionnaireResponses");
$table->column("Respondent", array($mgr, formatResponseLink), true)
	->column("Status", "{status:prettify}", true)
	->column("Date Submitted", "{date_submitted|N/A}", true)
;

$table->emptyMessage = "There are no responses to this questionnaire.";
$table->sortable = true;
$table->pageSize = 20;
$table->filter = false;
$table->cssStyle = "width: 100%";
$table->excelFile = "questionnaireResults.xls";

$tabs = QuestionnaireCreateManager::getTabs($questionnaire_id);

$script .= $table->writeScript();
$script .= $mgr->writeScript();


$tabs->writeHTML();
?>
<div id="questionnaire_tab_border">
<?

$table->drawView();

// This button is provided because this component is a sandbox test component. Normally
// the responses would be email token based or on a form in another part of the application.
?><p><a class='button' href='questionnaire_response_form?questionnaire_id=<?php echo $questionnaire_id ?>'>Respond to Questionnaire</a></p>
</div>
