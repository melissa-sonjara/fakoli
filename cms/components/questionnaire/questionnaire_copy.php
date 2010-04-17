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
 * Title: questionnaire_copy.php
 * 
 * Description: Nondisplay script to copy a questionnaire. User
 * may wish to copy a questionnaire when the type of editing
 * desired is beyond what is allowed for questionnaires that
 * have been answered.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 1/10/10
 */

require_once realpath(dirname(__FILE__)."/../../include/config.inc");
require_once realpath(dirname(__FILE__)."/../../cms/datamodel/questionnaire.inc");

$questionnaire_key = checkNumeric($_GET["questionnaire_key"]);
$class_key = checkNumeric($_GET["class_key"]);

if ($questionnaire_key)
{
	$questionnaire = new Questionnaire($questionnaire_key);
	$copyQstn = new Questionnaire();
	$copyQstn->title = "Copy of {$questionnaire->title}";
	$copyQstn->class_key = $questionnaire->class_key;
	$copyQstn->save();
	
	$questions = $questionnaire->Questions("ORDER BY question_number");
	if(count($questions) > 0)
	{
		foreach($questions as $question)
		{
			 $copyQuestion = $question;
			 $copyQuestion->question_key = 0;
			 $copyQuestion->questionnaire_key = $copyQstn->questionnaire_key;
			 $copyQuestion->save();
		}
	}
	
}

redirect("questionnaire_list.php?class_key=$class_key");

?>