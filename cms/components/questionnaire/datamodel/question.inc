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
 * Defines the relations and functions required of a Question class
 * that uses Questionnaire form and results classes.
 * 
 * Question classes should contain a sort order field called either
 * sort_order or
 * question_number
 * 
 * @author janice
 *
 */
abstract class AbstractQuestion extends DataItem
{		
	var $editable = true; // pseudo field
		
	var $fieldAliases = array(
		"char_limit"	=>	"Character Limit for Answer",
		"num_rows"		=>	"Number of Rows for Answer",
		"options"		=>	"Options for Answer",
		"required"		=>	"Answer	Required",
	);
	
	/**
	 * Returns true if the question can be edited and false
	 *  if it cannot.
	 */
	abstract function isEditable();
	
	abstract function QuestionType();
	
	/**
	 * Sets the default values for a question object:
	 * 
	 */
	function setDefaultValues()
	{
		$this->num_rows = 3;
		$this->required = 0; 
			
		$qTypes = Query::create('QuestionType', "ORDER BY sort_order")
			->execute();
			
		if(count($qTypes) > 0)
		{
			$this->question_type_id = $qTypes[0]->question_type_id;
		}
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
	
	function isMultiChoice()
	{
		return ($this->question_type_id == 1) ? true : false;
	}
	
	function isDropDown()
	{
		return ($this->question_type_id == 6) ? true : false;
	}
	
	function isHeading()
	{
		return ($this->question_type_id == 7) ? true : false;
	}
	
	function formatEditableMessage()
	{
		if(!$this->editable)
		{
			return "<p><b><i>Note:&nbsp;</i>This question has been answered. You may edit the question but be careful not to change its meaning.</b></p>\n";
		}
	}
	
	function __construct()
	{
	}

}

class Question extends AbstractQuestion
{
	var $fields = array (	"question_id"			=>	'Number',
							"questionnaire_id"		=>	'Number',
							"question_number"		=>	'Number',
							"question_type_id"		=>	'Number',
							"question"				=>	'HTML',
							"question_name"			=>	'String',
							"options"				=>	'Text',
							"required"				=>	'Number',
							"char_limit"			=>	'Number',
							"num_rows"				=>	'Number',
							"context_help"			=>	'Text'
						);
						
	var $relations = array (	
					"Questionnaire"		=>	'Questionnaire',
					"QuestionType"		=>	'QuestionType',
					"Answers"			=>	'QuestionnaireAnswer',
	);
	
		
	function Questionnaire()
	{
		return $this->getRelated('Questionnaire');
	}
	
	function QuestionType()
	{
		return $this->getRelated('QuestionType');
	}
	
	function Answers($constraint = "")
	{
		return $this->getRelatedList('QuestionnaireAnswer', "question_id", $constraint);
	}	
		
	function isEditable()
	{
		if(!$this->question_id)
		{
			return true;
		}
		
		$count = Query::create('QuestionnaireAnswer', "WHERE question_id=:question_id")
			->bind(":question_id", $this->question_id)
			->executeValue("COUNT(1)");

		return ($count > 0) ? false : true;
	}
	
	/**
	 * Do not allow delete if there are answers linked to the question
	 */
	function allowDelete()
	{
		if(!$this->question_id)
		{
			return false;
		}
		
		$count = count($this->Answers());
		return ($count > 0) ? false : true;
	}
					
	function __construct()
	{
		$this->table = "question";
		$this->primary_key = "question_id";
		
		parent::__construct(func_get_args());
	}
}


/**@}*/?>
