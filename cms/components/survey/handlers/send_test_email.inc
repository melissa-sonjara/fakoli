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

Fakoli::using("survey", "questionnaire", "email", "settings");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
{
	ajaxReturn("cannot send test email - unknown survey");
}

$survey = new Survey($survey_id);

if(!$survey->isAuthor())
{
	ajaxReturn("insufficient privileges to send test survey email");
}

$mgr = new SurveySendManager($survey);
$rtn = $mgr->sendTests($user->email);

$sent = ($rtn) ? "OK" : "FAIL";
$send_result = ($rtn) ? questionnaire_test_sent : questionnaire_test_failed;
Fakoli::end("{$sent}|" . $mgr->getInfoMsg($send_result));
?>