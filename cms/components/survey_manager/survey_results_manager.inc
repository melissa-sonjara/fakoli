<?php
/**************************************************************

Copyright (c) 2007,2008 Sonjara, Inc

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and or sell
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

Fakoli::using("survey", "questionnaire");
Fakoli::usingFeature("grouped_data_view");

class SurveyResultsView
{
	var $obj;
	var $survey;
	var $questions;
	var $answers;
	var $emptyMessage = "There are no results.";
	var $optionDataView;
	var $textDataView;
	var $optionAnswers;
	var $textAnswers;
	var $responseCount;

	function SurveyResultsView($mgr)
	{
		$this->mgr = $mgr;
		$this->survey = $this->mgr->item;
		$qPk = $this->mgr->getQuestionKey();

		$this->questionTypes = QuestionType::getQuestionTypeList();

		$this->questions = $this->mgr->getQuestions();

		if(count($this->questions) > 0)
			$answers = $this->mgr->getAnswers();
			
		if(count($answers) > 0)
		{
			$this->responseCount = $this->mgr->getResponseCount();
			$this->answers = regroupList($answers, $qPk);

			foreach($this->questions as $question)
			{
				$answers = $this->answers[$question->$qPk];
				$this->getAggregateData($question, $answers);
			}
		}

		$this->buildOptionDataView();
		$this->buildTextDataView();
	}

	function getAggregateData($question, $answers)
	{
		$qPk = $this->mgr->getQuestionKey();

		$questionType = new QuestionType($question->question_type_id);

		switch($questionType->class_name)
		{
			case MultipleChoiceView:
					
				$this->optionAnswers[$question->$qPk] = $this->getSingleSelectQuestionResultData($question, $answers);
				break;
					
			case RatingView:

				$this->optionAnswers[$question->$qPk] = $this->getRatingsQuestionResultData($question, $answers);
				break;
					
			case ShortTextView:

				$this->textAnswers[$question->$qPk] = $answers;
				break;
					
			case FreeTextView:

				$this->textAnswers[$question->$qPk] = $answers;
				break;
					
			case CheckListView:

				$this->optionAnswers[$question->$qPk] = $this->getCheckListQuestionResultData($question, $answers);
				break;
					
			case SelectFieldView:

				$this->optionAnswers[$question->$qPk] = $this->getSingleSelectQuestionResultData($question, $answers);
				break;
					
			default:
					
				trace("getAggregateData: Unknown question type", 2);
				die("Unknown question type");
		}

	}

	/*
	 * Use this function for multiple choice, and select field, get
	 * the count of answers for each option in the question option list.
	 */
	function getSingleSelectQuestionResultData($question, $answers)
	{
		$qPk = $this->mgr->getQuestionKey();
		$options = explode("\n", $question->options);
		$idx = 1;

		if(count($options) > 0)
		{
			if(count($answers) > 0)
			$answerValues = regroupList($answers, "value");
			else
			$answerValues = array();
			$optionAnswers = array();

			foreach($options as $option)
			{
				$count = count($answerValues[$idx]);
				$optionAnswer = new OptionAnswer($question->$qPk, $idx, $option, $count);
				array_push($optionAnswers, $optionAnswer);
				$idx++;
					
			}
			return $optionAnswers;
		}
	}

	/*
	 * Use this for ratings - each radio button is not labeled in
	 * the question option field so we need to create a label
	 * for each in the results data display.
	 */
	function getRatingsQuestionResultData($question, $answers)
	{
		$qPk = $this->mgr->getQuestionKey();
		if(count($answers) > 0)
			$answerValues = regroupList($answers, "value");
			
		$optionAnswers = array();
		$count = array();
		$optionText = $this->getOptionText($question->options);
		$steps = count($optionText);

		for($idx = 1; $idx <= $steps; ++$idx)
		{
			//echo "idx is $idx<br/>";
			// store for calculating mean and median
			$counts[$idx] = count($answerValues[$idx]);
			$optionAnswer = new OptionAnswer($question->$qPk, $idx, $optionText[$idx], $counts[$idx]);
			array_push($optionAnswers, $optionAnswer);
		}

		$mean = new OptionAnswer($question->$qPk, ++$idx, "Mean", $this->getMean($counts));
		//$median = new OptionAnswer($question->$qPk, ++$idx, "Median", $this->getMedian($optionAnswers));
		array_push($optionAnswers, $mean);
		//array_push($optionAnswers, $median);

		return $optionAnswers;
	}

	function getOptionText($options)
	{
		$optionText = array();

		list($from, $to, $steps) = explode("\n", $options);
		if (!$from) $from = "Lowest";
		if (!$to) $to = "Highest";
		if (!$steps) $steps = 5;

		for($idx = 1; $idx <= $steps; ++$idx)
		{
			if($idx > 1 AND $idx < $steps)
				$optionText[$idx] = $idx;
			else
				$optionText[$idx] = ($idx == 1) ? $from : $to;
		}
		return $optionText;
	}

	function getMean($counts)
	{
		$total = 0;
		$num = 0;

		foreach($counts as $value => $count)
		{
			$total += $value * $count;
			$num += $count;
		}

		$mean = ($num > 0) ? round($total / $num, 2) : 0;
		return $mean;
	}

	/*
	 * The median will either be the middle value in the array if the count is odd
	 * or the average of the center two numbers if the count is even.
	 */
	function getMedian($optionAnswers)
	{
		$median = 0.0;
		$index_1 = 0;
		$index_2 = 0;

		// determine if odd or even
		$no_elements = count($optionAnswers);
		//echo "no elements is $no_elements<br/>";
		$odd = $no_elements % 2;

		//odd take the middle number
		if ($odd == 1)
		{
			//determine the middle
			$the_index_1 = $no_elements / 2;

			//cast to integer
			settype($the_index_1, "integer");

			//calculate the median
			$median = $optionAnswers[$the_index_1]->option;
		}
		else
		{
			//determine the two middle numbers
			$the_index_1 = $no_elements / 2;
			$the_index_2 = $the_index_1 - 1;

			//calculate the median
			$median = ($optionAnswers[$the_index_1] + $optionAnswers[$the_index_2]) / 2;
		}

		return $median;
	}

	/*
	 * For checklists, we need to get the array of answers
	 * from the value field b/c users can choose more than one.
	 * We store the answer comma delimited so explode and loop
	 * through to get the count.
	 */
	function getCheckListQuestionResultData($question, $answers)
	{
		$qPk = $this->mgr->getQuestionKey();
		$options = explode("\n", $question->options);
		$idx = 1;

		// Explode and record the count of each value answer
		$answerCounts = $this->getCheckListAnswerCounts($answers);
		if(count($options) > 0)
		{
			$optionAnswers = array();
				
			foreach($options as $option)
			{
				$count = $answerValues[$idx];
				$optionAnswer = new OptionAnswer($question->$qPk, $idx, $option, $answerCounts[$idx]);
				array_push($optionAnswers, $optionAnswer);
				$idx++;
			}
		}
		return $optionAnswers;
	}

	static function getCheckListAnswerCounts($answers)
	{
		if(count($answers) == 0)
			return;

		$count = array();
			
		foreach($answers as $answer)
		{
			$values = $answer->value;

			// include 0 value, first checkbox
			if(strlen($values)==1 AND is_numeric($values))
				$values = array($values);
			elseif(strlen($values)==0)
				$values = "";
			else
				$values = explode(",", $values);
				
			foreach($values as $value => $idx)
			{
				$count[$idx] += 1;
			}
		}
		return $count;
	}

	function writeScript()
	{
		$script = $this->optionDataView->writeScript();
		$script .= $this->textDataView->writeScript();
		return $script;
	}


	function buildOptionDataView()
	{
		$dataView = new GroupedDataListView($this->optionAnswers, "OptionDataList");
		$dataView->column("Option", "{option}", false, "width: 90%")
		->column("Count", "{count}", false, "width: 10%");
		$dataView->groupBy($this->questions, array($this, getGroupTitle));
		$dataView->mode = 'tree';
		$dataView->emptyMessage = "There are no aggregated data results.";
		$dataView->excelFile = "option_surveyData.xls";
		$this->optionDataView = $dataView;
	}


	function getGroupTitle($question)
	{
		$questionTypes = QuestionType::getQuestionTypeList();
		return  "{$question->question_number}. {$question->question} " . $questionTypes[$question->question_type];
	}


	function buildTextDataView()
	{
		$table = new GroupedDataListView($this->textAnswers, "AnswerTextList");
		$table->column("Answer Text", array(SurveyResultsView, formatAnswer));
		$table->groupBy($this->questions, array(SurveyResultsView, formatQuestion));
		$table->mode = 'tree';
		$table->emptyMessage = "There are no text results.";
		$table->excelFile = "text_surveyData.xls";

		$this->textDataView = $table;
	}

	// question number is a psuedo field for some question dataitem objects
	static function formatQuestion($question)
	{
		return $question->question_number . ". " . $question->question;
	}

	static function formatAnswer($answer)
	{
		return ($answer->value) ? $answer->value : "Not Answered";
	}


	function drawView()
	{
		echo $this->getHeading();
		$this->optionDataView->drawView();
		echo "<h4>Free Text Responses</h4>";
		$this->textDataView->drawView();
	}

	/*
	 * Applications that don't want to see the the number of
	 * unsubmitted surveys, should return 0 recipient count from
	 * the results manager to omit that portion of the heading.
	 */
	function getHeading()
	{
		$recipientCount = $this->mgr->getRecipientCount();

		$countText = ($this->responseCount) ? $this->responseCount : "0";
		$text = "<h3>Aggregated Survey Answers based on $countText ";
		$text .= ($recipientCount) ? "out of $recipientCount " : "";
		$text .= "Respondents</h3>\n";

		return $text;
	}
}


/*
 * Show the results for one survey in the format:
 *
 * Company Name    Question 1   Question 2...
 * Acme             Answer 1    Answer 2
 *
 * Steps:
 *
 * 1) Get all response objects for this survey. A response
 * object is a record that links one person's set of answers
 * to one survey's set of questions.
 *
 * 2) For each response object, create a dummy data model
 * object of class SpreadsheetResponse. This object will hold
 * the response object's name (e.g., participant or company name)
 * and the answers to each question in the survey. Since the
 * number of questions in a surey varies, we create the field list
 * for SurveySpreadsheet dynamically and assign each field its
 * formatted answer value, retrieved from the QuestionnaireForm class.
 *
 * 3) Give all the spreadsheet objects to DataListView to render
 * the table.
 *
 * @param mgr - instance of SurveyResultsManager
 * which can access array of response objects and
 * get the question set for the survey
 */
class SurveySpreadsheetView extends SurveyResultsView
{
	var $mgr;
	var $table;
	var $spreadsheetResponses; // save this var in case needed for a custom output

	function SurveySpreadsheetView($mgr)
	{
		$this->mgr = $mgr;
		$this->spreadsheetResponses = $this->getSpreadsheetResponses($mgr->Responses(), $mgr->getAnswers(), $mgr->getQuestionKey());
		$this->table = $this->buildTable($this->spreadsheetResponses, $mgr->getQuestions());
	}

	/*
	 * $responses - array of response objects that link a survey to a set of answers.
	 *
	 * $surveyAnswers - array of all answers to this survey.
	 *
	 * $qPk - the survey question table primary key name (e.g., survey_id)
	 */
	function getSpreadsheetResponses($responses, $surveyAnswers, $qPk)
	{
		if(count($responses) == 0)
		return;
			
		$pk = $responses[0]->primary_key;
		$answers = reindexList($surveyAnswers, $pk);
		$responseManager = $this->mgr->getResponseManager($responses[0]);

		$view = new QuestionnaireForm($responseManager);

		/*
	 	* Reuse the same view obj for each response, replacing only the response
	 	* obj and the set of answers, not re-retrieving the question set.
	 	*/
		foreach($responses as $response)
		{
			// Don't reinstantiate the responseManager,
			// just give it the new obj.
			$responseManager->item = $response;
			$responseAnswers = $answers[$response->$pk];
			if(!is_array($responseAnswers))
			$responseAnswers = array($responseAnswers);
			$view->answers = reindexList($responseAnswers, $qPk);
				
			$idx = 1;
			$spreadsheetResponse = new SpreadsheetResponse();
	
			$fields = $spreadsheetResponse->getFields();
			$values = array();
				
			$values["name"] = $responseManager->getRespondent();
			$fields[$pk] = Number;
			$values[$pk] = $response->$pk;
				
			foreach($view->questions as $question)
			{
				$fields["answer_{$idx}"] = String;
				$values["answer_{$idx}"] = $question->getAnswer();
				$idx++;
			}
			$spreadsheetResponse->fields = $fields;
				
			// Assign each field its value.
			foreach($spreadsheetResponse->fields as $name => $type)
			$spreadsheetResponse->$name = $values[$name];

			$spreadsheetResponses[] = $spreadsheetResponse;
		}
		return $spreadsheetResponses;
	}

	function buildTable($spreadsheetResponses, $questions)
	{
		$table = new DataListView($spreadsheetResponses, "surveySpreadsheet");
		$table->column($this->mgr->getRespondentColumnHeading(), "{name}", true, "width: 40px");

		if(count($questions) > 0)
		{
			$idx = 1;
			foreach($questions as $question)
			{
				$title = ($question->question_name) ? $question->question_name :  ellipsis(stripHTML($question->question), 80);
				$table->column("{$title}", "{answer_$idx}", false, "width: 120px");
				$idx++;
			}
		}

		$width = (120 * $idx )+ 40;

		$table->emptyMessage = "There are no responses to this survey.";
		$table->sortable = true;
		$table->paginate = false;
		$table->filter = false;
		$table->cssStyle = "width: {$width}px; font-size: 10px; white-space: normal;";
		$table->excelFile = "surveySpreadsheet.xls";

		return $table;
	}

	function writeScript()
	{
		return $this->table->writeScript();
	}

	function drawView()
	{
		$this->table->drawView();
	}

}


class SpreadsheetResponse extends DataItem
{
	var $fields = array(
			"name"		=>	String,
			//question/answer fields built at run time
	);

	function SpreadsheetResponse()
	{
		$this->table = "spreadsheet_response";
		$this->primary_key = "survey_id";

		$this->DataItem(func_get_args());
	}
}


/*
 * Dummy dataitem class to enable use of dataview on
 * option results
 */
class OptionAnswer extends DataItem
{
	var $fields = array(
					"question_id"		=>	Number,
					"option_number"		=>	Number, // position in list
					"option"			=>	String,
					"count"				=>	Number
	);

	function OptionAnswer($question_id, $option_number, $option, $count)
	{
		$this->table = "option_answer";
		$this->primary_key = "question_id";
		$this->question_id = $question_id;
		$this->option_number = $option_number;
		$this->option = $option;
		$this->count = $count;
	}
}


/*
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

	function drawView()
	{
		?>
<h4>Responses Received</h4>
<p><?echo SurveyManager::getSurveyProgressBar($this->survey)?></p>
		<?
		if (count($this->mgr->getResponseCount()) > 0)
		{
			foreach($this->questions as $question)
			{
				$renderer = QuestionField::create($this->survey, $question);
				?>
<h4><?echo $question->question_number?>. <?echo $question->question?></h4>
				<?
				$renderer->writeReportHTML();
			}
		}
		else
		{
			echo "<p><em>{$this->emptyMessage}</em></p>";
		}
	}

	/*
	 * Respond to event fired by questionnaire component graph handlers
	 */
	static function getSurvey($survey_id)
	{
		trace("Returning Survey {$survey_id[0]}", 3);
		return new Survey($survey_id[0]);
	}

	/*
	 * Respond to event fired by questionnaire component graph handlers
	 */
	static function getQuestion($question_id)
	{
		trace("Returning Question {$question_id[0]}", 3);
		return new SurveyQuestion($question_id[0]);
	}

}


