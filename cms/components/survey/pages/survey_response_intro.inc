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
 * Description: Landing page for public survey access. Displays a link
 * to the Survey Response Form that pops up a token input validation
 * form before redirecting to the survey response form.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/18/10
 */

Fakoli::using("survey", "questionnaire");

$survey_id = checkNumeric($_GET["survey_id"]);
$token = mysql_escape_string($_GET["token"]);
$msg = checkNumeric($_GET["msg"]);

if(!$survey_id)
	redirect("index");

// check if test user was redirected here from survey_response_form or survey_response_preview
if($survey_id && $token)
{
	$response = SurveyResponse::findResponseByToken($survey_id, $token);
}

$survey = new Survey($survey_id);
$mgr = new SurveySendManager($survey);
$responseMgr = SurveyResponseManager::create($survey_id);

$page->page_title = "{$survey->title}";

$script .= $mgr->writeScript();
$script .= $responseMgr->writeScript();

if($survey->isClosed())
{
	echo $survey->format("<p>{title} closed on {end_date:F d, Y}</p>");
}
elseif($response && $response->isSubmitted())
{
	echo "<p>You have already submitted your survey.</p>\n";
}
else
{
	echo "<p>" . formatAsHTML($survey->introduction) . "</p>\n";
	echo "<p><a href=\"#\" onclick=\"questionnaireMgr.showResponseTokenDialog(); return false;\">{$survey->title}</a></p>\n";
}

if(($user && $survey->isAuthor()) || ($response && $response->isTester()))
{
	echo $mgr->getTesterIntro($response);
}
?>