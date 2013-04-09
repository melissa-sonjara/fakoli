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
 * survey_response_preview.inc
 *
 * Description: Displays a preview of survey response to the person
 * completing the survey. Validates that all required fields are complete
 * and lets the user submit.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/19/10
 */

Fakoli::using("survey", "questionnaire");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);
$token = $_GET["token"];

if(!$survey_id)
{
	redirect("/index");
}

if(!$token)
{
	redirect("survey_response_intro?survey_id={$survey_id}");
}

$response = SurveyResponse::findResponseByToken($survey_id, $token);

if(!$response)
{
	redirect("survey_response_intro?survey_id={$survey_id}");
}

$survey = $response->Survey();
if(!$survey->isOpen() OR !$response->isEditable())
{
	redirect("survey_response_intro?survey_id=$survey_id");
}

$view = new QuestionnaireForm(new SurveyResponseManager($response), true);

$response->filter = new ExclusionFilter("last_modified", "include_in_results");
$form = new AutoForm($response);
$form->hide("survey_id", "token", "email", "last_modified", "status");
$form->submitLabel = "Submit";
$form->button("Revise Survey", "survey_response_form?survey_id=$survey_id&token=$token");
$form->onSaveComplete = updateStatus;

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("survey_response_confirmation?survey_id={$survey_id}");
	}
}

$script .= $form->writeScript();
?>
<p>A preview of your responses is shown below. To submit your survey,
scroll to the bottom of the page and click the "Submit" button.</p>
<?
echo "<h3>". $response->Survey()->title . "</h3>\n";
$view->drawView();
$form->drawForm();

function updateStatus($obj)
{
	$response = $obj->data;
	$response->setStatus("submitted");
}
?>