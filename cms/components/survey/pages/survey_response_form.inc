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
 * survey_response.inc
 *
 * Description: Displays a survey questionnaire for a user with token
 * access to answer.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/13/10
 */

Fakoli::using("survey", "email", "survey_manager");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);
$token = $_GET["token"];

if(!$survey_id)
	redirect("/index");

if(!$token)
	redirect("survey_response_intro?survey_id={$survey_id}");

$response = querySingle(SurveyResponse, "WHERE survey_id={$survey_id} AND token='{$token}'");

if(!$response)
	redirect("survey_response_intro?survey_id={$survey_id}");

$survey = $response->Survey();

if(!$survey->isOpen() OR !$response->isEditable())
	redirect("survey_response_intro?survey_id=$survey_id&msg=1");

$form = new QuestionnaireForm(new SurveyResponseManager($response));
$form->onSaveComplete = saveResponse;

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("survey_response_preview?survey_id={$survey_id}&token=$token");
	}
	else // refresh
		redirect("survey_response_form?survey_id={$survey_id}&token=$token");
}

$script .= $form->writeScript();

echo "<h3>". $survey->title . "</h3>\n";
if ($survey->instructions)
{
	echo "<p>" . formatAsHTML($survey->instructions) . "</p>\n";
}
$form->drawForm();
?>
<br>
<?

function saveResponse($obj)
{
	$response = $obj->mgr->item;
	$response->setStatus("in_progress");
}

?>