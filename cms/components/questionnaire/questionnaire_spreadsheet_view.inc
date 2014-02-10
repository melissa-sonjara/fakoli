<?php

/*
 * Show the results for one questionnaire in the format:
 *
 * Company Name    Question 1   Question 2...
 * Acme             Answer 1    Answer 2
 *
 * Steps:
 *
 * 1) Get all response objects for this questionnaire. A response
 * object is a record that links one person's set of answers
 * to one questionnaire's set of questions.
 *
 * 2) For each response object, create a dummy data model
 * object of class SpreadsheetResponse. This object will hold
 * the response object's name (e.g., participant or company name)
 * and the answers to each question in the questionnaire. Since the
 * number of questions in a surey varies, we create the field list
 * for SurveySpreadsheet dynamically and assign each field its
 * formatted answer value, retrieved from the QuestionnaireForm class.
 *
 * 3) Give all the spreadsheet objects to DataListView to render
 * the table.
 *
 * @param mgr - instance of SurveyResultsManager
 * which can access array of response objects and
 * get the question set for the questionnaire
 */
class QuestionnaireSpreadsheetView
{
	var $mgr;
	var $table;
	var $spreadsheetResponses; // save this var in case needed for a custom output

	function QuestionnaireSpreadsheetView($mgr)
	{
		$this->mgr = $mgr;
		$this->spreadsheetResponses = $this->getSpreadsheetResponses($mgr->Responses(), $mgr->getAnswers(), $mgr->getQuestionKey());
		$this->table = $this->buildTable($this->spreadsheetResponses, $mgr->getQuestions());
	}

	/*
	 * $responses - array of response objects that link a questionnaire to a set of answers.
	 *
	 * $questionnaireAnswers - array of all answers to this questionnaire.
	 *
	 * $qPk - the questionnaire question table primary key name (e.g., questionnaire_id)
	 */
	function getSpreadsheetResponses($responses, $questionnaireAnswers, $qPk)
	{
		if(count($responses) == 0)
		{
			return;
		}
		
		$pk = $responses[0]->primary_key;
		$answers = reindexList($questionnaireAnswers, $pk);
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
				$values["answer_{$idx}"] = $question->getSpreadSheetAnswer();
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
		$table = new DataListView($spreadsheetResponses, "questionnaireSpreadsheet");
		$table->column($this->mgr->getRespondentColumnHeading(), $this->mgr->getRespondentColumnFormat(), true, "width: 40px");

		$this->mgr->addRespondentAdditionalColumns($table);
		
		if(count($questions) > 0)
		{
			$idx = 1;
			foreach($questions as $question)
			{
				$title = ($question->question_name) ? ellipsis(stripHTML($question->question_name), 80) :  ellipsis(stripHTML($question->question), 80);
				$table->column("{$title}", "{answer_$idx}", false, "width: 120px");
				$idx++;
			}
		}

		$width = (120 * $idx )+ 40;

		$table->emptyMessage = "There are no responses to this questionnaire.";
		$table->sortable = true;
		$table->paginate = false;
		$table->dragColumnReorder = true;
		$table->filter = false;
		$table->cssStyle = "width: {$width}px; font-size: 10px; white-space: normal;";
		$table->excelFile = "questionnaireSpreadsheet.xls";

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



?>