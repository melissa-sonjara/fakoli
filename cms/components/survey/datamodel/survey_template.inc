<?php


define("survey_in_progress", 0);
define("survey_published", 1);

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
				"Questions"		=>	SurveyQuestion
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
	
	function isPublished()
	{
		return ($this->status == survey_published) ? true : false;
	}
	
	function setPublished()
	{
		$this->filter = new InclusionFilter(status);
		$this->status = survey_published;
		$this->save();	
	}
	
	function setInProgress()
	{
		$this->filter = new InclusionFilter(status);
		$this->status = survey_in_progress;
		$this->save();	
	}
		
	function SurveyTemplate()
	{
		$this->table = "survey_template";
		$this->primary_key = "survey_template_id";

		$this->DataItem(func_get_args());	
		
	}
}