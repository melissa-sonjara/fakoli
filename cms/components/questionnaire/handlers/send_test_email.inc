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

/**
 * Sample code for sending test email. Questionnaire component does not
 * use this handler.
 */
Fakoli::using("questionnaire", "email", "settings");

$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

if(!$questionnaire_id)
{
	Fakoli::end("cannot send test email - unknown questionnaire");
}

if(!$user)
{
	Fakoli::end("cannot send test email - user not logged in");
}

$questionnaire = new Questionnaire($questionnaire_id);
$mgr = new QuestionnaireSendManager($questionnaire);

$rtn = $mgr->sendTests($user->email);

$sent = ($rtn) ? "OK" : "FAIL";
$send_result = ($rtn) ? questionnaire_test_sent : questionnaire_test_failed;
Fakoli::end("{$sent}|" .$mgr->getInfoMsg($send_result));
?>