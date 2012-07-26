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
 * Lets admin users select from standard questions to create a
 * set of questions.
 *
 * @author Janice Gallant for Sonjara, Inc.
 *
 * 10/17/2010
 */

Fakoli::using("survey", "questionnaire");
Fakoli::usingFeature("auto_form");

$survey_template_id = checkNumeric($_GET["survey_template_id"]);

$surveyTemplate = new SurveyTemplate();

if($survey_template_id)
	$surveyTemplate->load($survey_template_id);

$surveyTemplate->filter = new ExclusionFilter("status");
$form = new AutoForm($surveyTemplate);
$form->required("name", "message");
$form->annotate("message", "Create a boilerplate message for the email survey requests.
<a class='button' href='#' onclick=\"new Survey().showAdvancedFeaturesDialog(); return false\">Advanced Features</a>"
);

$tabs = SurveyTemplateManager::surveyTemplateTabs($survey_template_id, false);

if ($method == "POST")
{
	if($form->save())
	{
		$tabs->queryString = "survey_template_id={$form->data->survey_template_id}";
		$tabs->next();
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border"><?php
$form->drawForm();
?></div>
