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


require_once realpath(dirname(__FILE__)."/../../framework/data_item.inc");


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
		$this->table = "question";
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
		$this->table = "answer";
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
					Questionnaires 	=>	Questionnaire
	);
	
	function Questionnaires($constraint = "")
	{
		return $this->getRelatedList(Questionnaire, "class_key", $constraint);
	}
	
	function getClass()
	{
		$class_name = $this->class_name;
		$classObj = new $class_name();

		return $classObj;
	}
	
	function DataItemClass()
	{
		$this->table = "dataitem_class";
		$this->primary_key = "class_key";
		
		$this->DataItem(func_get_args());
		
	}
}
?>