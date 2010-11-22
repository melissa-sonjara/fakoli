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


Fakoli::using("survey");
Fakoli::usingFeature("tree");

$survey_template_id = checkNumeric($_GET["survey_template_id"]);

if(!$survey_template_id)
	redirect("/survey_templates");

$surveyTemplate = new SurveyTemplate($survey_template_id);
$redirect = "/survey_template_questions?survey_template_id=$survey_template_id";
$form = new SurveyQuestionSelectForm($surveyTemplate);
$form->button("Cancel", $redirect);

$tabs = surveyTemplateTabs($survey_template_id);
$tabs->page = "survey_template_questions";
$tabs->active = "Survey Template Questions";
$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";
$styles .= "<link href=\"/templates/css/tree.css\" rel=\"stylesheet\"/>";

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Select Survey Template Questions</h3>
<?
$form->drawForm();
?></div>
