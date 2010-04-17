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
 * Title: questionnaire_form.php
 * 
 * Description: Form to add/update/delete a questionnaire
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 1/8/10 
 */

require_once realpath(dirname(__FILE__)."/../../include/config.inc");
require_once realpath(dirname(__FILE__)."/../../cms/datamodel/questionnaire.inc");
require_once realpath(dirname(__FILE__)."/../../framework/auto_form.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/questionnaire_tabs.inc");

$questionnaire_key = checkNumeric($_GET["questionnaire_key"]);
$class_key = checkNumeric($_GET["class_key"]);

$questionnaire = new Questionnaire();

if(!$questionnaire_key AND $class_key)
	redirect("index.php");

if ($questionnaire_key)
{
	$questionnaire->load($questionnaire_key);
}
else // new questionnaire
{
	$questionnaire->class_key = $class_key; 
}

$form = new AutoForm($questionnaire);

$form->hide("class_key");
$form->required("title");
$form->button("Cancel", "questionnaire_list.php?class_key=$class_key");

if($questionnaire->isEditable())
{
	$form->allowDelete = true;	
	$form->onDelete = onDeleteCascade;
}

$tabs = questionnaireTabs($questionnaire_key);

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->queryString = "questionnaire_key={$form->data->questionnaire_key}";
		$tabs->next();
	}
}

$script .= $form->writeScript();

require_once realpath(dirname(__FILE__)."/../../templates/begin_page.inc");

$tabs->writeHTML();

?>
<div style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$form->drawForm();
?>
</div><?
require_once realpath(dirname(__FILE__)."/../../templates/end_page.inc");

function onDeleteCascade(&$obj)
{
	global $class_key;
	
	$qstn = $obj;
	
	if(!$class_key)
		$class_key = $qstn->class_key;
	
	$questions = $qstn->Questions();
	
	if(count($questions) > 0)
	{
		foreach($questions as $question)
		{
			$question->delete();
		}
	}
	
	$qstn->delete();
	
	redirect("questionnaire_list.php?class_key={$class_key}");
	
}
?>