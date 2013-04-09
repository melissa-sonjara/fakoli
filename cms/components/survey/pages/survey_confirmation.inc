<?php
Fakoli::using("questionnaire", "survey", "settings", "email", "settings");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
{
	redirect("survey_dashboard");
}

$survey = new Survey($survey_id);
$page->page_title .= " for " . $survey->title;

$survey->filter = new InclusionFilter("confirmation_message");

$form = new AutoForm($survey);
$form->submitLabel = "Save Confirmation Message";
$mgr = new SurveySendManager($survey);

$tabs = SurveyManager::surveyTabs($survey_id);

$script .= $form->writeScript();

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}


$tabs->writeHTML();
?>
<div id="survey_tab_border">
<button id="next_page_button" class="button"
	onclick="go('<?echo $tabs->getNextPage() ?>')">Next Page &raquo;</button>
<h3>Confirmation Message for Respondents</h3>
<p>If left blank, respondents will see the following message:</p>
<?php
echo "<div class='default_confirmation'>";
echo $mgr->formatDefaultConfirmationMessage();
echo "</div>";

$form->drawForm();

?>
</div>