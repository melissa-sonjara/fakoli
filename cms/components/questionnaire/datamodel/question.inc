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


class Question extends DataItem
{
	var $fields = array (	"question_id"			=>	Number,
							"questionnaire_id"		=>	Number,
							"question_number"		=>	Number,
							"question_type_id"		=>	Number,
							"question"				=>	HTML,
							"question_name"			=>	String,
							"options"				=>	Text,
							"required"				=>	Number,
							"char_limit"			=>	Number,
							"num_rows"				=>	Number,
							"context_help"			=>	Text
						);
						
	var $relations = array (	
								"Answer"			=>	Answer,
								"Questionnaire"		=>	Questionnaire,
								"QuestionType"		=>	QuestionType
	);
	
	
	var $fieldAliases = array(
		"char_limit"	=>	"Character Limit for Answer",
		"num_rows"		=>	"Number of Rows for Answer",
		"options"		=>	"Options for Answer",
		"required"		=>	"Answer	Required",
	);
	
	// Answer to one object's questionnaire's question
	function Answer()
	{	
		$answer = querySingle(Answer, "WHERE question_id = {$this->question_id}");
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
	
	function QuestionType()
	{
		return $this->getRelated(QuestionType);
	}
	
	function isEditable()
	{
		if(!$this->question_id)
			return true;
		
		$count = queryValue(Answer, "COUNT(1)", "WHERE question_id={$this->question_id}");

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
	
	function isFreeText()
	{
		return ($this->question_type_id == 4) ? true : false;
	}

	function isShortText()
	{
		return ($this->question_type_id == 3) ? true : false;
	}

	function isCheckList()
	{
		return ($this->question_type_id == 5) ? true : false;
	}
		
	function isRatings()
	{
		return ($this->question_type_id == 2) ? true : false;	
	}
		
	function Question()
	{
		$this->table = "question";
		$this->primary_key = "question_id";
		
		$this->DataItem(func_get_args());
	}
}


?>