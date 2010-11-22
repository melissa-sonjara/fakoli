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
 * 
 * Title: survey_template_preview.inc
 * 
 * Description: Preview of a survey template. Admin users
 * can decide if it is ready to be published and used. 
 * 
 * @author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 10/19/10
 * 
 */

Fakoli::using("survey");
Fakoli::usingFeature("auto_form");

require_once realpath(dirname(__FILE__)."/../../questionnaire/questionnaire_form.inc");
require_once realpath(dirname(__FILE__)."/../survey_template_view.inc");

$survey_template_id = checkNumeric($_GET["survey_template_id"]);

if (!$survey_template_id) 
	redirect("/survey_dashboard");

$surveyTemplate = new SurveyTemplate($survey_template_id);

$questionCount = queryValue(SurveyQuestionXref, "COUNT(1)", "WHERE survey_template_id=$survey_template_id");
$view = new SurveyTemplatePreview($surveyTemplate);

$surveyTemplate->filter = new InclusionFilter("status");
 
if($questionCount > 0)
{
	$form = new AutoForm($surveyTemplate);
	$form->submitLabel = ($surveyTemplate->isPublished()) ? "Remove from Publication" : "Ready for Use";
	$form->onSaveComplete = updateStatus;	
	$form->hide("status");
}

$tabs = surveyTemplateTabs($survey_template_id);
$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";

if ($method == "POST")
{
	if ($form AND $form->save())
	{
		//redirect("/survey_templates");
	}
}


if($form)
	$script .= $form->writeScript();

$script .= $view->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<h3><?php echo $surveyTemplate->title ?></h3>
<?
$view->drawView();
if($form)
{
	$form->drawForm();
}
?>
</div>
<?php 
function updateStatus($form)
{
	global $_POST;
	$surveyTemplate = $form->data;
	
	$submit = $_POST["submit"];
	if($submit == "Ready for Use")
		$surveyTemplate->setPublished();
	else
		$surveyTemplate->setInProgress();
	
	redirect("/survey_templates");
}
?>