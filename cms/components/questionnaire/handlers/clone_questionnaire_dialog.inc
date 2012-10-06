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
 * Display a "Save As" dialog to let user save an existing questionnaire as a new
 * questionnaire with a different title. Validate that the title is unique.
 * 
 */
Fakoli::using("questionnaire");
Fakoli::usingFeature("auto_form");

$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

if(!$questionnaire_id)
{
	ajaxReturn("unknown questionnaire");
}

$questionnaire = new Questionnaire($questionnaire_id);

$mgr = new QuestionnaireDashboardManager($questionnaire);
$form = $mgr->buildCloneForm();

$form->submitLabel = "Save Questionnaire Copy";

if ($method == "POST")
{
	$obj =& $form->data;
	$obj->fromPOST();
	$form->msg = $form->validator->validate();
	if ($form->msg == "")
	{
		$dst = $mgr->cloneQuestionnaire();
		ajaxReturn("OK|{$dst->questionnaire_id}");		
	}
	else
	{
		ajaxReturn($form->msg);
	}
}

echo $form->writeScript();

$form->drawForm();

?>