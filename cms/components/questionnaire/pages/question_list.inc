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

Fakoli::using("questionnaire");
Fakoli::usingFeature("auto_form");

require_once realpath(dirname(__FILE__)."/../questionnaire_tabs.inc");
require_once realpath(dirname(__FILE__)."/../question_set.inc");

$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

if (!$questionnaire_id) 
	redirect("/questionnaire_list");

$mgr = new QuestionnaireManager(new Questionnaire($questionnaire_id));
$listView = new QuestionSet($mgr);

$tabs = questionnaireTabs($questionnaire_id);

if ($method == "POST")
{
	if ($_POST["done"])
	{
		redirect("/questionnaire_preview?questionnaire_id=$questionnaire_id");
	}
	else
	{
		$listView->reorderQuestions();
		// refresh
		redirect("/question_list?questionnaire_id=$questionnaire_id");
	}
}

$script .= $listView->writeScript();
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