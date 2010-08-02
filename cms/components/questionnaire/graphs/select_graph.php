<?php

require_once "../../../../include/config.inc";

require_once  $config['homedir']."/cms/datamodel/questionnaire.inc";
require_once  $config['homedir']."/cms/datamodel/survey.inc";
require_once  $config['homedir']."/framework/data_view.inc";
require_once  $config['homedir']."/modules/include/survey.inc";
require_once  $config['homedir']."/cms/components/questionnaire/results_view.inc";
require_once  $config['homedir']."/cms/components/questionnaire/results_report.inc";
require_once ('jpgraph/jpgraph.php');
require_once ('jpgraph/jpgraph_pie.php');
require_once ('jpgraph/jpgraph_pie3d.php');

$question_id = checkNumeric($_GET["question_id"]);
$survey_id = checkNumeric($_GET["survey_id"]);

$question = new Question($question_id);
$survey = new EmailSurvey($survey_id);
$answers = $survey->Answers("WHERE question_id=$question_id");

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
$graph = new PieGraph(500,300);
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
$p1->SetCenter(0.4);
//$p1->SetAngle(30);
$p1->value->SetFont(FF_ARIAL,FS_NORMAL,12);
$p1->SetLegends($legends);
$p1->SetShadow();

$graph->Add($p1);
$graph->Stroke();	
?>