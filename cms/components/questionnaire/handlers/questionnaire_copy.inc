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

 /* 
 * Description: Nondisplay script to copy a questionnaire. User
 * may wish to copy a questionnaire when the type of editing
 * desired is beyond what is allowed for questionnaires that
 * have been answered.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 1/10/10
 */

Fakoli::using("questionnaire");

$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

if ($questionnaire_id)
{
	$questionnaire = new Questionnaire($questionnaire_id);
	$copyQstn = new Questionnaire();
	
	$oldTitle = $questionnaire->title;
	do
	{
		$newTitle = "Copy of " . $oldTitle;
		$oldTitle = $newTitle;	
		$matches = $copyQstn->queryValue("COUNT(1)", "WHERE title = '{$newTitle}'");
	} while ($matches > 0);
	
	$copyQstn->title = $newTitle;
	$copyQstn->composite_class = $questionnaire->composite_class;
	$copyQstn->save();
	
	$questions = $questionnaire->Questions("ORDER BY question_number");
	if(count($questions) > 0)
	{
		foreach($questions as $question)
		{
			 $copyQuestion = $question;
			 $copyQuestion->question_id = 0;
			 $copyQuestion->questionnaire_id = $copyQstn->questionnaire_id;
			 $copyQuestion->save();
		}
	}
	
}

redirect("/questionnaire_list");

?>