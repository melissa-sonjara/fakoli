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

Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("data_view");

require_once ('jpgraph/jpgraph.php');
require_once ('jpgraph/jpgraph_pie.php');
require_once ('jpgraph/jpgraph_pie3d.php');

$question_id = checkNumeric($_GET["question_id"]);
$survey_id = checkNumeric($_GET["survey_id"]);
$survey_question_id = checkNumeric($_GET["survey_question_id"]);

if($survey_question_id)
	$question = new SurveyQuestion($survey_question_id);
$survey = new EmailSurvey($survey_id);
$answers = $survey->Answers("WHERE survey_question_id=$survey_question_id");

$options = explode("\n", $question->options);
$idx = 1;
$optionAnswers = array();
$legengs = array();

if(count($options) > 0)
{
	$answerValues = regroupList($answers, "value");
				
	foreach($options as $option)
	{	
		$legends[] = trim($option);
		$count = count($answerValues[$idx]);
		$optionAnswers[] = $count;
		$idx++;	
	}
}		

// Create the Pie Graph.
$graph = new PieGraph(850,400);
$graph->SetFrame(false);
$graph->SetAntiAliasing();

// Set A title for the plot
//$graph->title->Set(stripHTML($question->question));
//$graph->title->SetFont(FF_VERDANA,FS_BOLD,12); 
//$graph->title->SetColor("darkblue");
//$graph->legend->Pos(0.1,0.2);

// Create pie plot
$p1 = new PiePlot($optionAnswers);
//$p1->SetTheme("sand");
$p1->SetCenter(0.25);
//$p1->SetAngle(30);
$p1->value->SetFont(FF_ARIAL,FS_NORMAL,12);
$p1->SetLegends($legends);
$p1->SetShadow();

$graph->Add($p1);
$graph->Stroke();	
?>
