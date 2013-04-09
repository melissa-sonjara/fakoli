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
 * survey_email.inc
 *
 * Description: Link an email template to a specific questionnairesurvey.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 6/23/10
 */


Fakoli::using("questionnaire", "survey", "settings", "email");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);
$error = checkNumeric($_GET["error"]);

if(!$survey_id)
	redirect("survey_dashboard");

$survey = new Survey($survey_id);
$page->page_title .= " for {$survey->title}";

$survey->filter = new InclusionFilter("sender_email", "recipients",  "cc_recipients", "additional_recipients", "message");

$sendMgr = new SurveySendManager($survey);

if(!$survey->sender_email)
	$survey->sender_email = $sendMgr->getSenderEmail();

$form = new AutoForm($survey);

$form->annotate("recipients", "(separate email addresses with a comma ',')");
$form->required("sender_email");

if(!$survey->allow_anonymous_responses)
{
	$form->required("recipients", "message");
}
else
{
	$form->validator->add(new RequiredQuestionnaireMessageValidator($form, "message"));
}

$form->getRenderer("message")->rows = 10;
$form->submitLabel = "Save Survey Email";
$form->regexp("sender_email", "\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b", "Please supply a valid email address containing a domain name.");
$form->getRenderer("message")->columns = 80;

$sendMgr->setOneRecipientField($form, "recipients");
$sendMgr->setOneRecipientField($form, "cc_recipients", "CC Recipients");
$sendMgr->setOneRecipientField($form, "additional_recipients");
$form->annotate("cc_recipients", $sendMgr->getCCRecipientsAnnotation());
$form->annotate("additional_recipients", "Send additional survey request emails (separate email addresses with a comma ',').");

if($survey->isAuthor() && !$survey->isClosed())
{
	$form->annotate("message", $sendMgr->getMessageAnnotation());
	if($survey->isSent())
	{
		$survey->recipients = implode(", ", explode(",", $survey->recipients));
		$form->readOnly("recipients");
	}
}
else
{
	$form->readOnlyForm = true;
	$form->valueCSS = 'ro_value';
}

if(!$survey->isAuthor() || !$survey->isSent())
{
	$form->hide("additional_recipients");
}

$tabs = SurveyManager::surveyTabs($survey_id);

$script .= $form->writeScript();
$script .= $sendMgr->writeScript();

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}


$tabs->writeHTML();
?>
<div id="survey_tab_border">
<button id="next_page_button" class="button"
	onclick="go('<?echo $tabs->getNextPage() ?>')">Next Page &raquo;</button>
<h3>Survey Email</h3>
<div id="inner_border">
<?php
echo $sendMgr->getEmailPageHeading();

$form->drawForm();
?></div>
</div>