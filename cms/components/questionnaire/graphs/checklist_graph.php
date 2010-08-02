<?php

require_once "../../../../include/config.inc";

require_once  $config['homedir']."/cms/datamodel/questionnaire.inc";
require_once  $config['homedir']."/cms/datamodel/survey.inc";
require_once  $config['homedir']."/framework/data_view.inc";
require_once  $config['homedir']."/modules/include/survey.inc";
require_once  $config['homedir']."/cms/components/questionnaire/results_view.inc";
require_once  $config['homedir']."/cms/components/questionnaire/results_report.inc";
require_once ('jpgraph/jpgraph.php');
require_once ('jpgraph/jpgraph_bar.php');

function getOptionText($options)
{
	$optionText = array();
	
	$options = explode("\n", $this->question->options);
	
	$idx = 1;
	foreach($options as $option)
	{
		$optionText[$idx++] = trim(stripHTML($option));
	}

	return $optionText;
}
	
$question_id = checkNumeric($_GET["question_id"]);
$survey_id = checkNumeric($_GET["survey_id"]);

$question = new Question($question_id);
$survey = new EmailSurvey($survey_id);
$answers = $survey->Answers("WHERE question_id=$question_id");
		
$answerValues = regroupList($answers, "value");

$datay = array();

$optionAnswers = array();
$count = array();
$optionText = getOptionText($question->options);
$steps = count($optionText);

for($idx = 1; $idx <= $steps; ++$idx)
{
	//echo "idx is $idx<br/>";
	// store for calculating mean and median
	$counts[$idx] = count($answerValues[$idx]);
	$totalValue += $idx * $counts[$idx];
	$totalCount += $counts[$idx];
	$datay[] = $counts[$idx];
	$datax[] = $optionText[$idx];
}


// Setup the graph.
$graph = new Graph(500,300);
$graph->img->SetMargin(60,20,35,75);
$graph->SetScale("textint");
$graph->SetFrame(false); // No border around the graph

// Setup font for axis
$graph->xaxis->SetFont(FF_VERDANA,FS_NORMAL,10);
$graph->yaxis->SetFont(FF_VERDANA,FS_NORMAL,10);

// Show 0 label on Y-axis (default is not to show)
$graph->yscale->ticks->SupressZeroLabel(false);

// Setup X-axis labels
$graph->xaxis->SetTickLabels($datax);
$graph->xaxis->SetLabelAngle(50);

// Create the bar pot
$bplot = new BarPlot($datay);
$bplot->SetWidth(0.6);


// Set color for the frame of each bar
$bplot->SetColor("black");
$graph->Add($bplot);

// Finally send the graph to the browser
$graph->Stroke();
?>
?>