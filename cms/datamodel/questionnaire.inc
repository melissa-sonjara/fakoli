<?php
/* =======================================================================
 * USAID/DG Epp Funding Application
 * Developed by Sonjara, Inc.
 * For further information please contact Siobhan Green at sio@sonjara.com
 * =======================================================================
 * 
 * Title: datamodel/questionnaire.inc
 * 
 * 
 */

require_once realpath(dirname(__FILE__)."/../framework/data_item.inc");
require_once realpath(dirname(__FILE__)."/../datamodel/dataitem_class.inc");
require_once realpath(dirname(__FILE__)."/../components/questionnaire/questionnaire_form.inc");


class Questionnaire extends DataItem
{
	var $fields = array("questionnaire_key"			=>	Number,
						"title"						=>	String,
						"class_key"					=>	Number
	);
	
	var $relations = array("QuestionnaireType"	=>		QuestionnaireType,
							"Questions"		=>	Question,
							"DataItemClass"		=>	DataItemClass
	);
		
	function Questions($constraint = "")
	{
		return $this->getRelatedList(Question, "questionnaire_key", $constraint);
	}
	
	function Question($constraint)
	{
		$questions = $this->Questions("WHERE LOWER(question) LIKE LOWER('%$constraint%')");
		if(count($questions) > 0)
			$question = $questions[0];
			
		return $question;
		
	}
	
	function DataItemClass()
	{
		return $this->getRelated(DataItemClass);
	}
	
	
	function isEditable()
	{
		if(!$this->questionnaire_key)
			return true;
			
		$classObj = $this->DataItemClass();
		$count = queryValue($classObj->class_name, 
		"COUNT(1)", "WHERE questionnaire_key={$this->questionnaire_key}"); 	
		return ($count > 0) ? false : true;
	}
	
	function getNotEditableHTML()
	{
		$classObj = $this->DataItemClass();
		$class = new $classObj->class_name();
		$text = "<i>Note:&nbsp;</i>This questionnaire is linked to one or more
			{$class->prettifyClassName(true)}. You may edit the questions but be careful not to change their meaning.";
		return $text;
	}	
	
	function Questionnaire()
	{
		$this->table = "questionnaire_epp";
		$this->primary_key = "questionnaire_key";
		
		$this->DataItem(func_get_args());
		
	}
}



class Question extends DataItem
{
	var $fields = array (	"question_key"			=>	Number,
							"questionnaire_key"		=>	Number,
							"question_number"		=>	Number,
							"question_type"			=>	Number,
							"question"				=>	HTML,
							"options"				=>	Text,
							"required"				=>	Number,
							"char_limit"			=>	Number,
							"num_rows"				=>	Number
						);
						
	var $relations = array (	
								"Answer"			=>	Answer,
								"Questionnaire"		=>	Questionnaire
	);
	
	// Answer to one object's questionnaire's question
	function Answer($container_key, $class_key)
	{	
		$answer = querySingle(Answer, "WHERE container_key = $container_key AND class_key=$class_key AND question_key = {$this->question_key}");
		return $answer;
	}
	
	function Questionnaire()
	{
		return $this->getRelated(Questionnaire);
	}
	
	function QuestionnaireType()
	{
		$questionnaire = $this->Questionnaire();
		return $questionnaire->QuestionnaireType();
	}
	
	function isEditable()
	{
		if(!$this->question_key)
			return true;
		
		$count = queryValue(Answer, "COUNT(1)", "WHERE question_key={$this->question_key}");

		return ($count > 0) ? false : true;
	}
	
	/* get the list of values the user checked in a checkbox */
	function getValueList($values)
	{
		$optionArr = explode("\n", $this->options);

		$options = QuestionnaireForm::incrementArray($optionArr);

		// include 0 value, first checkbox
		if(strlen($values)==1 AND is_numeric($values))
			$values = array($values);
		elseif(strlen($values)==0)
			$values = array();
		else
			$values = explode(",", $values);

		$listArr = array();		
		if(count($options) > 0)
		{
			foreach($options as $value => $text)
			{
				if(array_search($value, $values) !== FALSE)
					array_push($listArr, $text);				
			}
		}
			
		if(count($listArr) > 0)
			$list = implode($listArr, ",");
		
		return $list;
	}
	
	function getNotEditableHTML()
	{
		$text = "<i>Note:&nbsp;</i>This question has been answered. You may edit the question but be careful not to change its meaning.";
		return $text;	
	}
	
	
	function Question()
	{
		$this->table = "question_epp";
		$this->primary_key = "question_key";
		
		$this->DataItem(func_get_args());
	}
						
}




class Answer extends DataItem
{
	var $fields = array (	"answer_key"				=>	Number,
							"container_key"				=>	Number,  // key to the container data item object, e.g., Application application_key
							"class_key"					=>	Number, // to get to container class name 
							"question_key"				=>	Number,
							"value"						=>	String
						);
						
	var $relations = array ("Question"	=>	Question
							);

	function Question()
	{
		return $this->getRelated(Question, "question_key");
	}
	
	function Answer()
	{
		$this->table = "answer_epp";
		$this->primary_key = "answer_key";
		
		$this->DataItem(func_get_args());
	}
						
	
}


class DataItemClass extends DataItem
{
	var $fields = array(
					"class_key"		=>	Number,
					"class_name"	=>	String
	);
	
	var $relations = array(
					Questionnaires 	=>	Questionnaire,
					Attachments		=>	Attachment,
					MessageMerges	=>	MessageMerge
	);
	
	function Questionnaires($constraint = "")
	{
		return $this->getRelatedList(Questionnaire, "class_key", $constraint);
	}
	
	function Attachments($constraint = "")
	{
		return $this->getRelatedList(Attachment, "class_key", $constraint);
	}
	
	function MessageMerges()
	{
		$merges = query(MessageMerge, "WHERE merge_key IN (SELECT merge_key FROM merge_class_jn_epp WHERE class_key={$this->class_key})");
		return $merges;
	}
	
	function getClass()
	{
		$class_name = $this->class_name;
		$classObj = new $class_name();

		return $classObj;
	}
	
	function DataItemClass()
	{
		$this->table = "dataitem_class_epp";
		$this->primary_key = "class_key";
		
		$this->DataItem(func_get_args());
		
	}
}
?>