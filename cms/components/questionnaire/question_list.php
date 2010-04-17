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
 * 
 * Title: question_list.php
 * 
 * Description: Numbered list view of questions in a questionnaire.
 * User can select a question to edit.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 11/9/09
 * 
 */

require_once realpath(dirname(__FILE__)."/../../include/config.inc");
require_once realpath(dirname(__FILE__)."/../../cms/datamodel/questionnaire.inc");
require_once realpath(dirname(__FILE__)."/../../framework/auto_form.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/question_set.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/questionnaire_tabs.inc");

$questionnaire_key = checkNumeric($_GET["questionnaire_key"]);

if (!$questionnaire_key) 
	redirect("questionnaire_list.php");

$questionnaire = new Questionnaire($questionnaire_key);

$listView = new QuestionSet($questionnaire_key);

$tabs = questionnaireTabs($questionnaire_key);

if ($method == "POST")
{
	if ($_POST["done"])
	{
		redirect("questionnaire_preview.php?questionnaire_key=$questionnaire_key");
	}
	else
	{
		$listView->reorderQuestions();
		// refresh
		redirect("question_list.php?questionnaire_key=$questionnaire_key");
	}
}

$script .= $listView->writeScript();

require_once realpath(dirname(__FILE__)."/../../templates/begin_page.inc");

$tabs->writeHTML();
?>

<div style="clear:left;border:solid 1px #000; padding: 4px;">
<h3><?echo $questionnaire->title ?></h3>
<?
$listView->drawList();
?>
<br/>
<form method="POST" action="">
<input class='button' type="submit" name="done" value="Done Adding Questions"/>
</form>
</div>
<?
require_once realpath(dirname(__FILE__)."/../../templates/end_page.inc");
?>