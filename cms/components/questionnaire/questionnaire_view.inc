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

 /* Title: questionnaire_view.inc
 * 
 * Description: Class for reviewing the questionnaire in the
 * creation/editing interface. Class for listing all saved
 * questionnaires of a certain type (e.g., list of survey
 * questionnaires, list of appliciton questionnaires) so
 * the user can select one to edit.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 11/4/09
*/

require_once realpath(dirname(__FILE__)."/../../components/questionnaire/questionnaire_form.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/question_renderer.inc");

/*
 * For questionnaire creator to view the layout of the questionnaire
 * from the questionnaire data entry interface.
 */
class QuestionnairePreview extends QuestionnaireForm
{
	function QuestionnairePreview($obj)
	{
		$this->obj = $obj;
		$this->readonly = true;
		
		$questions = $this->obj->Questions("ORDER BY question_number");
		
		foreach($questions as $question)
		{
			$this->questions[] = QuestionField::create($this, $question);
		}
	}
}


/*
 * Displays a table list view of questionnaires of one
 * type.
 */
class QuestionnaireListView
{
	var $className;  // e.g., QuarterlyReport
	var $classObj; // e.g., QuarterlyReport class object
	var $class_key;
	var $questionnaires;
	var $noneText = "There are not yet any questionnaires.";
	
	function QuestionnaireListView($class_key)
	{
		$this->questionnaires = query(Questionnaire, "WHERE class_key=$class_key ORDER BY questionnaire_key DESC");
		$this->class_key = $class_key;
		$diClass = new DataItemClass($class_key);
		$this->className = $diClass->class_name;
		$this->classObj = $diClass->getClass();
		$this->noneText = "There are not yet any {$this->classObj->prettifyClassName()} questionnaires.";
	}
	
	function drawView()
	{

		if(count($this->questionnaires) > 0)
		{
		echo "<table class=\"list\">";
		echo "<tr>";
		echo "<th>Title</th>";
		echo "<th>{$this->classObj->prettifyClassName(true)}</th>";
		echo "<th></th>"; // Edit buton
		echo "<th></th>"; // Create Copy button
		echo "</tr>";
	
		foreach($this->questionnaires as $questionnaire)
		{
			$count = queryValue($this->className, "COUNT(1)", "WHERE questionnaire_key={$questionnaire->questionnaire_key}");
			$editUrl = "questionnaire_form.php?questionnaire_key={$questionnaire->questionnaire_key}&class_key={$this->class_key}";
			
			echo "<tr>";
			echo "<td><a href='{$editUrl}'>{$questionnaire->title}</a></td>";
			echo "<td>$count</td>";
			echo "<td><a class='button' href='{$editUrl}'>&nbsp;Edit&nbsp;</a></td>";
			echo "<td style='width: 90px'><a class='button' href='questionnaire_copy.php?questionnaire_key={$questionnaire->questionnaire_key}&class_key=$class_key'>&nbsp;Create Copy&nbsp;</a></td>";
			echo "</tr>";
		}
		
		echo "</table>";
		}
		else
		{
			echo $this->noneText."<br/>";
		}
	}
}
?>
