<?php


define("survey_template_in_progress", 0);
define("survey_template_published", 1);

class SurveyTemplate extends DataItem
{
	var $fields = array(
				"survey_template_id"	=>	Number,
				"title"					=>	String,
				"introduction"			=>	Text,
				"message"				=>	Text,
				"status"				=>	Number
	);
	
	var $relations = array(
				"Questions"		=>	SurveyQuestion,
				"SurveyQuestionXrefs"	=>	SurveyQuestionXref
	);
	
	var $statusTypes = array(
				"0"		=>	"In Progress",
				"1"		=>	"Published"
	);
	
	function Questions()
	{
		if($this->survey_template_id)
			return $this->crossReference(SurveyQuestion, SurveyQuestionXref, "ORDER BY sort_order");
	}
	
	function SurveyQuestionXrefs($constraint = "ORDER BY sort_order")
	{
		return $this->getRelatedList(SurveyQuestionXref, "survey_template_id", $constraint);
	}
	
	function isPublished()
	{
		return ($this->status == survey_template_published) ? true : false;
	}
	
	
	function setPublished()
	{
		$this->filter = new InclusionFilter(status);
		$this->status = survey_template_published;
		$this->save();	
	}
	
	function setInProgress()
	{
		$this->filter = new InclusionFilter(status);
		$this->status = survey_template_in_progress;
		$this->save();	
	}
		
	function SurveyTemplate()
	{
		$this->table = "survey_template";
		$this->primary_key = "survey_template_id";

		$this->DataItem(func_get_args());	
		
	}
}