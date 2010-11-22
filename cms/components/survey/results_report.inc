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
 * Title: results_view.inc
 * 
 * Description: Class for displaying aggregate data for 
 * questionnaire responses.
 * 
 * Parameters:
 * $obj - class that links to all the responses (e.g., Survey)
 * 
 * Steps:
 * 
 * 1) Get the questionnaire linked to obj.
 * 
 * 2) Get each of the questions. For each question, get the aggregation
 * renderer defined for that question type.
 * 
 * 3) Get all the answers through the obj datamodel - these will be only
 * the answers of submitted responses (not in progress or not started)
 * 
 * 4) Get the aggregate data. Save the aggregate data from multi choice, 
 * ratings, checklist, and select field in optionAnswers; save the free text
 * and short text in textAnswers.  
 * 
 * 5) Build a data view, grouped by question, for each display type and 
 * draw the views.
 */

Fakoli::using("questionnaire");

//require_once realpath(dirname(__FILE__)."/../../questionnaire/questionnaire_form.inc");
//require_once realpath(dirname(__FILE__)."/../../questionnaire/question_renderer.inc");

class SurveyReport
{
	var $mgr;
	var $survey;
	var $questions;
	var $answers;
	var $emptyMessage = "There are no results to display.";
	var $responseCount;
	
	function SurveyReport($mgr)
	{
		$this->mgr = $mgr;
		$this->survey = $this->mgr->item;
		$this->questions = $this->mgr->getQuestions();
	}
	
	function writeScript()
	{
		return "";
	}

	function getSurveyProgressBar($survey)
	{
		$percentage = 0;
		$total = queryValue(SurveyResponse, "COUNT(1)", "WHERE survey_id={$survey->survey_id}");
		if($total > 0)
		{
			$this->responseCount = $survey->getResponseCount();
			$percentage = round($this->responseCount / $total * 100, 0);
		}
		
		$html = "<div class='col2'>\n";
		$html .= $percentage . "%";
		if ($this->responseCount) $html .= " <span style='font-size: 8pt'>($this->responseCount&nbsp;of&nbsp;$total)</span>";
		$html .= "</div>\n";
		$html .= "<div class='progress_bar'>\n";
		$html .= "<div class='progress' style='width: $percentage%'>\n";
		$html .= "</div></div>\n";
		
		return $html;
	}
	
	function drawView()
	{
		?>
		<h4>Responses Received</h4>
		<p>
		<?echo $this->getSurveyProgressBar($this->survey)?>
		</p>
		<?
		if ($this->responseCount)
		{
			foreach($this->questions as $question)
			{
				$renderer = QuestionField::create($this->survey, $question);
?><h4><?echo $question->question_number?>. <?echo $question->question?></h4><?
				$renderer->writeReportHTML();		
			}
		}
		else
		{
			echo "<p><em>{$this->emptyMessage}</em></p>";
		}
	}
}
		
		
?>