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
 * survey_response_view.ince
 *
 * Description: Displays a response to a questionnaire
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/13/10
 */


Fakoli::using("questionnaire", "survey", "email");

$response_id = checkNumeric($_GET["response_id"]);

if(!$response_id)
	Fakoli::end("unknown survey response");

$response = new SurveyResponse($response_id);

$mgr = new SurveyResponseManager($response);
$view = new QuestionnaireResponseView($mgr);

echo $view->writeScript();

echo "<p><label>Survey: </label> ". $response->Survey()->getTitle(). "</p>\n";
echo "<p><label>Respondent: </label>". $mgr->getRespondent() . "</p>\n";
echo "<p><label>Date Submitted: </label>". $response->format("{last_modified:F d, Y}") . "</p>\n";

$view->drawView();
?>