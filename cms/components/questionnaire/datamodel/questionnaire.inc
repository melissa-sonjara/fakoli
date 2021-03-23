<?php
/**\addtogroup questionnaire
 * @{
 */


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

/**
 * Defines the relations and functions required of a Questionnaire
 * class that uses Questionnaire form and results classes.
 * 
 * @author janice
 *
 */
abstract class AbstractQuestionnaire extends DataItem
{
	
	function AbstractQuestionnaire()
	{
		
	}
	
	abstract function Questions();
	
	abstract function Answers();
	
	abstract function isEditable();
	
	abstract function allowDelete();
	
	/**
	 * Set the initial default values in the item
	 * e.g., user_id = user->user_id
	 * status = xx
	 * 
	 * Used by questionnaire clone and can be used
	 * on the first form page.
	 */
	abstract function setDefaults();
	
	abstract function isAuthor();
}

class Questionnaire extends AbstractQuestionnaire
{
	var $fields = array("questionnaire_id"			=>	Number,
						"title"						=>	String,
						"composite_class"			=>	String
	);
	
	var $relations = array(
			"Questions"				=>	Question,
			"Answers"				=>	QuestionnaireAnswer,
	);
	
		
	function Questions($constraint = "")
	{
		return $this->getRelatedList(Question, "questionnaire_id", $constraint);
	}
	
	/**
	 * Retrieve all answers to the questionnaire.
	 * 
	 * for results aggregation
	 */
	function Answers($constraint = "")
	{
		$query = "WHERE response_id IN (SELECT response_id FROM questionnaire_response WHERE status = 'submitted' AND questionnaire_id=:questionnaire_id)";
		if($constraint)
		{
			$query .= preg_replace("/WHERE/i", " AND", $constraint);
		}
		
		return Query::create(QuestionnaireAnswer, $query)
			->bind(":questionnaire_id", $this->questionnaire_id)
			->execute();
	}
	
	/**
	 * @param $constraint
	 */
	function Responses($constraint = "")
	{
		return $this->getRelatedList(QuestionnaireResponse, "", $constraint);
	}
			
	function isEditable()
	{
		if(!$this->questionnaire_id)
			return true;

		if(!$this->composite_class)
			return true;
			
		$count = queryValue($this->composite_class, "COUNT(1)", "WHERE questionnaire_id={$this->questionnaire_id}"); 	

		return ($count > 0) ? false : true;
	}
	
	function isAuthor()
	{
		global $user;
		
		if(!$user) return false;
		
		return true;
	}
	
	function allowDelete()
	{
		if(!$this->questionnaire_id)
		{
			return false;
		}
		
		return true;
	}
	
	function setDefaults()
	{
		// nothing to do.
	}
		
	function getNotEditableHTML()
	{
		$className = $this->composite_class;
		$class = new $className();
		$text = "<i>Note:&nbsp;</i>This questionnaire is linked to one or more
			{$class->prettifyClassName(true)}. You may edit the questions but be careful not to change their meaning.";
		return $text;
	}	
	
	
	function Questionnaire()
	{
		$this->table = "questionnaire";
		$this->primary_key = "questionnaire_id";
		
		$this->DataItem(func_get_args());
		
	}
}


/**@}*/?>