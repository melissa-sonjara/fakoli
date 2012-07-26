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



class Questionnaire extends DataItem
{
	var $fields = array("questionnaire_id"			=>	Number,
						"title"						=>	String,
						"composite_class"			=>	String
	);
	
	var $relations = array("QuestionnaireType"		=>	QuestionnaireType,
							"Questions"				=>	Question
	);
		
	function Questions($constraint = "")
	{
		return $this->getRelatedList(Question, "questionnaire_id", $constraint);
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


?>