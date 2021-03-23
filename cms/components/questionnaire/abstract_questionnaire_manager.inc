<?php
/**\addtogroup questionnaire
 * @{
 */


Fakoli::using("questionnaire");


/**
 * Questionnaire/Survey Implementation instructions
 *
 * Components that interface with Questionnaire must meet the following
 * requirements
 *
 * 1) DataItem classes
 * Item class - the class that can retrieve Questions and Answers.
 * Should extend AbstractQuestionnaire
 *
 * Question class - the class that contains question data and links
 * to the item class either directly through a foreign key or through
 * an xref table. Should extend AbstractQuestion. Question data items must
 * contains:
 * question
 * question_name
 * question_type_id
 * options
 * required
 * char_limit
 * num_rows
 * context_help
 *
 * Response class - the class that links to both the Questionnaire item and the
 * Answers. Extend AbstractQuestionnaireResponse class. This class should
 * contain a default format for how a respondent should appear in the results
 * display (by token or name)
 *
 * Answer class - the class that links a question to its answer and to its
 * respondent. Extend AbstractQuestionnaireAnswer
 *
 * 2) Manager classes:
 * Implementing components should also have the following classes modeled
 * on Questionnaire manager classes but extending the abstract class.
 *
 * [MyClassName]DashboardManager extends AbstractQuestionnaireDashboardManager (optional)
 * [MyClassName]CreateManager extends AbstractQuestionnaireCreateManager
 * [MyClassName]ResponseManager extends AbstractQuestionnaireResponseManager
 * [MyClassName]ResultsManager extends AbstractQuestionnaireResultsManager
 *
 * 3) Handler files:
 * Implementing components may need the following handler files in their
 * component:
 *
 * clone_[my_item_class]_dialog - create a copy of a questionnaire/survey - requires dashboard manager
 * question_delete - if no xref
 * question_remove - if xref table
 * question_names_dialog - if results are displayed in spreadsheet
 * [my_item_class]_delete - for delete from list or dashboard page - requires dashboard manager
 * reorder_questions - for draggable reorder from question list page
 * response_view - to display one respondent's answers in a modal dialog
 */



/**
 * Contains methods used by all 3 classes
 * 
 * @author janice
 *
 */
class AbstractQuestionnaireManager
{
	function AbstractQuestionnaireManager()
	{
	}
	
	/**
	 * Retrieves questions through the questionnaire or survey
	 * DataItem object.
	 * 
	 * Since this method is needed for all 3 managers, classes that
	 * extend should call shared static method
	 * 
	 */
	function getQuestions()
	{
		$sort_field = $this->getSortField();
		$questions = $this->item->Questions("ORDER BY $sort_field");
		return QuestionnaireHelper::setQuestionNumbers($this->item, $questions);
	}
	
	/**
	 * Retrieves the table that has the question sort order field
	 * either Question or a QuestionXref
	 * 
	 * If the item should have a relation to retrieve the
	 * questions or xrefs that is the pluralized version of the class
	 * name.
	 * e.g., SurveyQuestionXrefs or Questions
	 * 
	 * If not, override this function.
	 */
	function getSortableItems()
	{
		if($this->getXrefClass())
		{
			$relation =  pluralize($this->getSortableClass());
		}
		else
		{
			$relation = "Questions";
		}
		$sort_field = $this->getSortField();

		return $this->item->$relation("ORDER BY $sort_field");
	}
	
	function getSortableClass()
	{
		return ($this->getXrefClass()) ? $this->getXrefClass() : $this->getQuestionClass();
	}
	
	function getSortableObj()
	{
		$class = $this->getSortableClass();
		return new $class();
	}
	
	function getSortField()
	{
		$obj = $this->getSortableObj();
		return ($obj->hasField("sort_order")) ? "sort_order" : "question_number";
	}
	
	/**
	 * Returns the primar key name of the question obj
	 */
	function getQuestionKey()
	{
		$qClass = $this->getQuestionClass();
		$question = new $qClass();
		return $question->getPrimaryKey();
	}
			
	/**
	 * Called by QuestionRenderers from QuestionnaireForm 
	 * Given an instance of a question, return its number.
	 * Some renderers need to skip the numbering (headings).
	 */
	function getQuestionNumber($question)
	{
		return QuestionnaireHelper::getQuestionNumber($this->item, $this->getQuestions(), $question);
	}	

	function getAnswerKey()
	{
		$class = $this->getAnswerClass();
		$obj = new $class();
		return $obj->getPrimaryKey();
	}
	
	function getPrimaryKey()
	{
		return $this->item->getPrimaryKey();
	}
		
} // end AbstractQuestionnaireManager



/**
 * Provides static function support for Questionnaire Manager
 * methods that are shared among types
 * QuestionnaireCreateManager extends AbstractQuestionnaireCreateManager
 * QuestionnaireResponseManager extends AbstractQuestionnaireResponseManager
 * QuestionnaireResultsManager extends AbstractQuestionnaireResultsManager
 * 
 * For example, all three need the function getQuestionNumber and getQuestions, 
 * so they should refer their methods to this static method to avoid duplicate 
 * code.
 *
 */
class QuestionnaireHelper
{
	function QuestionnaireHelper()
	{
		
	}
	
	static function getQuestionNumber($item, $questions, $question)
	{
		if(count($questions) > 0)
		{
			$idx = 1;
			foreach($questions as $q)
			{
				$renderer = QuestionField::create($item, $q);
				
				if (!$renderer->skipNumbering)
				{
					$idx++;
				}
				
				if ($question->question_id == $q->question_id) return $idx;
			}
		}
		return "";		
	}
	
	/**
	 * Given an array of question objects, set the question_number
	 * field or pseudo field so that Question Renderer won't need to
	 * call getQuestionNumber for each question.
	 * 
	 * @param Array $questions - array of objects of a Question DataItem class
	 */
	static function setQuestionNumbers($item, $questions)
	{
		if(count($questions) == 0) return $question;
		
		$idx = 1;
		foreach($questions as $question)
		{
			$renderer = QuestionField::create($item, $question);
			
			if (!$renderer->skipNumbering)
			{
				$question->question_number = $idx;
				$idx++;
			}
		}
		return $questions;
	}	


	/**
	 * On the question names dialog, format the input field for editing
	 * the question names.
	 * 
	 * @param obj $question
	 */
	static function formatQuestionNameInputField($question)
	{
		if(!$question->question_name)
			$question->question_name = substr($question->question, 0, 79);

		$pk = $question->getPrimaryKey();
		return $question->format("<input type='text' name=\"question_{$question->$pk}\"
		value=\"{question_name}\" size=\"40\"/>");	
	}	
		
} // end QuestionnaireHelper

/**@}*/?>