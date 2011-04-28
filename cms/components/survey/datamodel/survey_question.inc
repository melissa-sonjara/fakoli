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

define("survey_question_answered", 1);
define("survey_question_template", 2);

class SurveyQuestion extends DataItem
{
	var $fields = array (	"survey_question_id"	=>	Number,
							"question_type_id"		=>	Number,
							"question"				=>	HTML,
							"question_name"			=>	String,
							"options"				=>	Text,
							"required"				=>	Number,
							"char_limit"			=>	Number,
							"num_rows"				=>	Number,
							"locked"				=>	Boolean
						);
						
	var $relations = array (	
					"Answer"				=>	Answer,
					"Survey"				=>	Survey,
					"SurveyQuestionXref"	=>	SurveyQuestionXref
	);
		
	// Answer to one object's questionnaire's question
	function Answer($response_id)
	{	
		$answer = querySingle(Answer, "WHERE response_id = $response_id AND survey_question_id = {$this->survey_question_id}");
		return $answer;
	}
	
	function Survey()
	{
		return $this->crossReference(Survey, SurveyQuestionXref);
	}
	
	function SurveyQuestionXref($survey_id)
	{
		if($this->survey_question_id)
			return querySingle(SurveyQuestionXref, "WHERE survey_question_id = {$this->survey_question_id} AND survey_id = $survey_id");
	}
		
	function isEditable()
	{
		if(!$this->survey_question_id)
			return true;
		
		$count = queryValue(SurveyAnswer, "COUNT(1)", "WHERE survey_question_id={$this->survey_question_id}");

		return ($count > 0) ? false : true;
	}
	
	/*
	 * If the question is in one or more survey templates,
	 * then the form is drawn read only.
	 */
	function isTemplateQuestion()
	{
		if(!$this->survey_question_id)
			return false;
			
		$count = queryValue(SurveyQuestionXref, "COUNT(1)", "WHERE survey_question_id={$this->survey_question_id} AND survey_template_id is not null");

		return ($count > 0) ? true : false;
			
	}
	
	/*
	 * If the question is editable and is linked only to this survey or
	 * survey template, then allow delete. This function is called
	 * from survey_question_form to determine if a user can delete his/her
	 * own question. 
	 */
	// 
	// 
	function allowDelete()
	{
		if(!$this->survey_question_id)
			return true;
			
		if(!$this->isEditable()) 
			return false;
			
		$count = queryValue(SurveyQuestionXref, "COUNT(1)", "WHERE survey_question_id={$this->survey_question_id}");
		
		return ($count > 1) ? false : true;
	}
	
	/*
	 * Called from CMS?
	 */
	function allowLogicalDelete()
	{
		
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
	
	
	function getInfoText($index)
	{
		static $text = array(
				1	=>	"<i>Note:&nbsp;</i>This question has been answered. You may edit the question but be careful not to change its meaning.",
				2	=>	"This question belongs to a survey template and cannot be edited."
		);
		
		$html = "<p>$text[$index]</p>\n";
		
		return $html;
	}
	
	
	function SurveyQuestion()
	{
		$this->table = "survey_question";
		$this->primary_key = "survey_question_id";
		
		$this->DataItem(func_get_args());
	}
}

