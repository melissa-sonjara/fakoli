<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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

Fakoli::using("questionnaire", "svg_charts");

/**
 * 
 * Draws a graph of the aggregated answers for a given question.
 * Uses either the default SurveyCreateManager class or a custom
 * manager class set in the calling code using the syntax
 * 
 * 	QuestionnaireResultsGraph::$surveyManager = MyCustomSurveyManager;		
 * 
 * The custom survey manager must contain the following functions in
 * order to use this graphing function:
 * 
 * static function getSurvey($survey_id)
 * static function getQuestion($question_id)
 * 
 * @param String $surveyField - name of the field that has relation
 * "Answers" so that it can retrieve an array of answer DataItem 
 * objects with field "value" using the constraint $qField=$question_id
 * 
 * @param String $qField - the name of the question_id field (e.g.,
 * 	question_id or survey_question_id
 * 
 * @param Number $survey_id - the key value of the survey or other
 * object that can instantiate the DataItem object that has relation "Answers"
 * 
 * @param Number $question_id - the key value of the questionnaire
 * or survey question that can instantiate the question DataItem object.
 */
class QuestionnaireResultsGraph
{
	static $surveyManager;
	
	/**
	 * Draws an interactive graphical view of answers to one question.
	 * 
	 * @param obj $question - object of class Question, SurveyQuestion or other question DataItem class
	 * 
	 * @param obj $resultsManager - object of the Survey/Questionnaire Results manager class;
	 * this class is currently not being used but may be used in the future to specify 
	 * customization options for graphing
	 * 
	 * @param Array $answers - all survey or questionnaire answers to this question
	 */
	static function drawSelectGraph($question, $resultsManager, $answers)
	{
		$pk = $question->getPrimaryKey();
		$question_id = $question->$pk;
		$options = explode("\n", $question->options);
		$idx = 1;
		$optionAnswers = array();
		$legends = array();
		
		$total = 0;
		
		if(count($options) > 0)
		{
			$answerValues = regroupList($answers, "value");
						
			foreach($options as $option)
			{	
				$legends[] = trim($option);
				$count = count($answerValues[$idx]);
				$optionAnswers[] = $count;
				$idx++;	
				$total += $count;
			}
		}		
		
		if ($total > 0)
		{
			$idx = 1;
			foreach($legends as $legend)
			{
				$count = count($answerValues[$idx]);
				$percent = number_format($count / $total, 1);
				$legend .= " $percent%";
			}
		}

		$pie = new PieChart("results_$question_id", 650, 400, $optionAnswers, $legends, "standard");
		$pie->setRadius(150);
		$pie->setCenter(180,180);
		$pie->setLabelSize(12);
		$pie->setStrokeWidth(2);
		$pie->showLegend(true, 375, 20);
		$pie->shadow = false;
		//$pie->emboss = true;
		$pie->draw();
	}
	
	/**
	 * Draws an interactive graphical view of answers to one question.
	 * 
	 * @param obj $question - object of class Question, SurveyQuestion or other question DataItem class
	 * 
	 * @param obj $resultsManager - object of the Survey/Questionnaire Results manager class;
 	 * this class is currently not being used but may be used in the future to specify 
	 * customization options for graphing
	 * 
	 * @param Array $answers - all survey or questionnaire answers to this question
	 */
	static function drawChecklistGraph($question, $resultsManager, $answers)
	{
		$pk = $question->getPrimaryKey();
		$question_id = $question->$pk;
		$counts = QuestionnaireDataView::getCheckListAnswerCounts($answers);
		
		$datay = array();
		
		$optionAnswers = array();
		$count = array();
		$optionText = QuestionnaireResultsGraph::getOptionText($question->options);
		$steps = count($optionText);
		
		for($idx = 1; $idx <= $steps; ++$idx)
		{
			//echo "idx is $idx<br/>";
			// store for calculating mean and median
			$totalValue += $idx * $counts[$idx];
			$totalCount += $counts[$idx];
			$datay[] = $counts[$idx] ? $counts[$idx] : 0;
			$datax[] = $optionText[$idx];
		}
		
		// Setup the graph.
		$graph = new Histogram(650,300);
		$graph = new Histogram("histogram_$question_id", count($datax) * 80 + 100, 400, 50, 10, count($datax) * 80, 300, "standard");
		$graph->setLabels($datax);
		$graph->ticks = 10;
		$graph->columnMargin = 0.4;
		
		$series = new HistogramSeries("block", "Answers", $datay);
		$graph->addSeries($series);
		$graph->draw();
	}
	
	/**
	 * Draws an interactive graphical view of answers to one question.
	 * 
	 * @param obj $question - object of class Question, SurveyQuestion or other question DataItem class
	 * 
	 * @param obj $resultsManager - object of the Survey/Questionnaire Results manager class;
 	 * this class is currently not being used but may be used in the future to specify 
	 * customization options for graphing
	 * 
	 * @param Array $answers - all survey or questionnaire answers to this question
	 */
	static function drawRatingsGraph($question, $resultsManager, $answers)
	{
		$pk = $question->getPrimaryKey();
		$question_id = $question->$pk;
		$answerValues = regroupList($answers, "value");
		
		$datay = array();
		
		$optionAnswers = array();
		$count = array();
		$optionText = QuestionnaireResultsGraph::getRatingsOptionText($question->options);
		$steps = count($optionText);
		
		for($idx = 1; $idx <= $steps; ++$idx)
		{
			//echo "idx is $idx<br/>";
			// store for calculating mean and median
			$counts[$idx] = count($answerValues[$idx]);
			$totalValue += $idx * $counts[$idx];
			$totalCount += $counts[$idx];
			$datay[] = $counts[$idx];
			$datax[] = $optionText[$idx];
		}
		
		$mean = "Average: ".number_format($totalValue / $totalCount, 1);
		
		// Setup the graph.
		$graph = new Histogram("histogram_$question_id", count($datax) * 80 + 100, 400, 50, 10, count($datax) * 80, 300, "standard");
		$graph->setLabels($datax);
		$graph->ticks = 10;
		$graph->columnMargin = 0.4;
		
		//$graph->setTitle($mean);
		
		$series = new HistogramSeries("block", "Ratings", $datay);
		$graph->addSeries($series);
		$graph->draw();	
	}
	
	/**
	 * Deprecated - call drawGraph from question renderer
	 * 
	 * @param String $surveyField
	 * @param String $qField
	 * @param Number $survey_id
	 * @param Number $question_id
	 */
	static function drawSelectResults($surveyField, $qField, $survey_id, $question_id)
	{
		if (!QuestionnaireResultsGraph::$surveyManager)
		{
			echo "<p><em>Survey Graphs are not available</em></p>";
			return;
		}
		
		$surveyManager = new QuestionnaireResultsGraph::$surveyManager();
		
		$question = $surveyManager->getQuestion($question_id);
		$survey = $surveyManager->getSurvey($survey_id);
		
		$answers = $survey->Answers("WHERE $qField=$question_id");

		QuestionnaireResultsGraph::drawSelectGraph($question, $surveyManager, $answers);
	}
	
	/**
	 * Deprecated - call drawGraph from question renderer
	 * 
	 * @param String $surveyField
	 * @param String $qField
	 * @param Number $survey_id
	 * @param Number $question_id
	 */
	static function drawChecklistResults($surveyField, $qField, $survey_id, $question_id)
	{
		if (!QuestionnaireResultsGraph::$surveyManager)
		{
			echo "<p><em>Survey Graphs are not available</em></p>";
			return;
		}
		
		$surveyManager = new QuestionnaireResultsGraph::$surveyManager();
		
		$question = $surveyManager->getQuestion($question_id);
		$survey = $surveyManager->getSurvey($survey_id);
				
		$answers = $survey->Answers("WHERE $qField=$question_id");
		
		QuestionnaireResultsGraph::drawChecklistGraph($question, $surveyManager, $answers);
	}
	
	
	/**
	 * Deprecated - call drawGraph from question renderer
	 * 
	 * @param String $surveyField
	 * @param String $qField
	 * @param Number $survey_id
	 * @param Number $question_id
	 */
	static function drawRatingResults($surveyField, $qField, $survey_id, $question_id)
	{
		if (!QuestionnaireResultsGraph::$surveyManager)
		{
			echo "<p><em>Survey Graphs are not available</em></p>";
			return;
		}
		
		$surveyManager = new QuestionnaireResultsGraph::$surveyManager();
		
		$question = $surveyManager->getQuestion($question_id);
		$survey = $surveyManager->getSurvey($survey_id);
				
		$answers = $survey->Answers("WHERE $qField=$question_id");
		
		QuestionnaireResultsGraph::drawRatingsGraph($question, $surveyManager, $answers);
		
	}
	
	private static function getRatingsOptionText($options)
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
				$optionText[$idx] = ($idx == 1) ? jsSafe(trim($from)) : jsSafe(trim($to));
				
		}
		return $optionText;
	}
	
	private static function getOptionText($options)
	{
		$optionText = array();
		
		$options = explode("\n", $options);
		
		$idx = 1;
		foreach($options as $option)
		{
			$optionText[$idx++] = jsSafe(wordwrap(trim(stripHTML($option)), 12));
		}
	
		return $optionText;
	}	
}
?>