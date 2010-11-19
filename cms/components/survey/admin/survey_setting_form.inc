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
 * For to update survey global settings.
 */


Fakoli::using("email", "survey");
Fakoli::usingFeature("auto_form", "data_view");

$surveySettings = SurveySetting::loadSettings();
if(!$surveySettings)
	$surveySettings = new SurveySetting();

$form = new AutoForm($surveySettings);

$redirect = "/admin/index";
$form->button("Cancel", $redirect);
$form->required("sender_email", "email_subject", "message_footer");
$form->annotate("sender_email", "the email address that will appear as the sender of the survey email");
$form->annotate("email_subject", "the subject line in the email of survey requests");
$form->annotate("message_footer", "the text to be included below the main 
message in all surveys, providing users with a link to response. 
The text within brackets represent codes that will be filled in with data 
from the survey record when the survey is sent. 
<button onclick=\"mergeCodePopup({$page->admin_page_id}, this); return false\">View Merge Codes</button>");
$form->getRenderer("message_footer")->rows = 10;

$script .= $form->writeScript();
$script .= SurveyMergeCodeDialog::writeScript();

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$form->drawForm();
?>