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
 * questionnaire_list.php
 * 
 * Description: Displays a list of questionnaire of one type.
 * User can choose one to edit, create new, or
 * copy an existing questionnaire.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 12/21/09
 */

require_once realpath(dirname(__FILE__)."/../../include/config.inc");
require_once realpath(dirname(__FILE__)."/../../cms/datamodel/questionnaire.inc");
require_once realpath(dirname(__FILE__)."/../../framework/auto_form.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/questionnaire_view.inc");

$class_key = checkNumeric($_GET["class_key"]);

if(!$class_key)
	redirect("index.php");

$diClass = new DataItemClass($class_key);
$classObj = $diClass->getClass();

$subtitle = $classObj->prettifyClassName() . " Questionnaires";

$view = new QuestionnaireListView($class_key);

require_once realpath(dirname(__FILE__)."/../../templates/begin_page.inc");
echo "<h3>$subtitle</h3>";

$view->drawView();

echo "<br/><a class='button' href='questionnaire_form.php?class_key=$class_key'>Add a Questionnaire</a><br/>";

require_once realpath(dirname(__FILE__)."/../../templates/end_page.inc");
?>