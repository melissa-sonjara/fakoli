<?php
/**\addtogroup questionnaire
 * @{
 */



/*
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

class QuestionnaireDataView
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

	function __construct($mgr)
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
			case 'MultipleChoiceView':
					
				$this->optionAnswers[$question->$qPk] = $this->getSingleSelectQuestionResultData($question, $answers);
				break;
					
			case 'RatingView':

				$this->optionAnswers[$question->$qPk] = $this->getRatingsQuestionResultData($question, $answers);
				break;
					
			case 'ShortTextView':

				$this->textAnswers[$question->$qPk] = $answers;
				break;
					
			case 'FreeTextView':

				$this->textAnswers[$question->$qPk] = $answers;
				break;
					
			case 'CheckListView':

				$this->optionAnswers[$question->$qPk] = $this->getCheckListQuestionResultData($question, $answers);
				break;
					
			case 'SelectFieldView':

				$this->optionAnswers[$question->$qPk] = $this->getSingleSelectQuestionResultData($question, $answers);
				break;

			case 'HeadingView':
				break;
				
			default:
					
				trace("getAggregateData: Unknown question type {$questionType->class_name}", 2);
				die("QuestionniareDataView getAggregateData: Unknown question type {$questionType->class_name}");
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
				$count = $answerCounts[$idx];
				if (!isset($count)) $count = 0;
				$optionAnswer = new OptionAnswer($question->$qPk, $idx, $option, $count);
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
		$dataView->groupBy($this->questions, array($this, 'getGroupTitle'));
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
		$table->column("Answer Text", array('QuestionnaireDataView', 'formatAnswer'));
		$table->groupBy($this->questions, array('QuestionnaireDataView', 'formatQuestion'));
		$table->mode = 'tree';
		$table->emptyMessage = "There are no text results.";
		$table->excelFile = "text_surveyData.xls";

		$this->textDataView = $table;
	}

	// question number is a pseudo field for some question dataitem objects
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
	 * 
	 * This is the case for surveys where there is not a set
	 * of recipients who receive a request.
	 */
	function getHeading()
	{
		$fn = array($this->mgr, formatResultsHeading);
		if (is_callable($fn))
		{
			return call_user_func($fn);
		}			

		$recipientCount = $this->mgr->getRecipientCount();

		$countText = ($this->responseCount) ? $this->responseCount : "0";
		$text = "<h3>Aggregated Survey Answers based on $countText ";
		$text .= ($recipientCount) ? "out of $recipientCount " : "";
		$text .= "Respondents</h3>\n";

		return $text;
	}
}


/**@}*/?>
