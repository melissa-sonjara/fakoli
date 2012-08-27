<?php

class SurveyTableHelper
{
	function SurveyTableHelper()
	{
	}

	static function formatSurveyLink($survey)
	{
		$path = ($survey->status != "not sent") ? "survey_results" : "survey_form";
		return "<a href=\"/$path?survey_id={$survey->survey_id}\">{$survey->title}</a>";
	}

	static function formatSurveyStatus($survey)
	{
		return Survey::$statusTypes[$survey->status];
	}

	static function formatStartDate($survey)
	{
		$date = ($survey->start_date > 0) ? $survey->format("{start_date}") : '-- / -- / ----';
		return $date;
	}


	static function formatResponseStatus($response)
	{
		$statusTitle = $response->statusTypes[$response->status];
		return $statusTitle;
	}


	static function formatResponseSubmitDate($response)
	{
		return ($response->last_modified > 0) ? $response->format("{last_modified}") : "N/A";
	}
	
	/*
	 * Action drop down on survey dashboard (list) page.
	 * 
	 * Actions:
	 *  View
	 *  Edit (if not sent)
	 *  Send (if not sent)
	 *  Close (if open)
	 *  Reopen (if closed)
	 * 
	 * Parameters to Ledger function handleLedgerActions:
	 * this, response_id
	 */
	static function formatActions($survey)
	{
		$responseCount = $survey->getResponseCount();
		$html = $survey->format("<select style='width: 80px' onchange=\"new Survey().handleSurveyAction(this, {survey_id}, $responseCount); return false;\">");
		$status = $survey->status;
		
		$actions = array(
			""				=>	"",
			"view"			=>	"View",
			"public_view"	=>	"Respondent's View",
			"clone"			=>	"Clone Survey",
		);

		if($status != "not sent" && $responseCount > 0)
		{
			$actions["view_results"] =  "View Results";
		}
		
		if($status == "not sent")
		{
			$actions["edit"] = "Edit";
			$actions["send"] = "Preview &amp; Send";
		}
		if($status == "open")
		{
			$actions["close"] = "Close Survey";
			if(count($survey->getNonResponders()) > 0)
			{
				$actions["send_reminders"] = "Send Reminders";
			}	
		}			
		else if($status == "closed")
		{
			$actions["open"] = "Open Survey";
			$actions["reopen"] = "Reopen Survey";
		}

		if($survey->allowDelete())
			$actions["delete"] = "Delete";

		foreach($actions as $identifier => $label)
		{
			$value = htmlSafe($identifier);
			$html .= "<option value='$value'>$label</option>";	
		}
		$html .= "</select>\n";
		
		return $html;	
	}	
	
	static function formatResponseLink($response)
	{
		if($response->isSubmitted())
			return $response->format("<a href='#' onclick=\"new SurveyResponse().showSurveyResponse({response_id}); return false\">{token}</a>");	
		else
			return $response->format("{token}");
	}
}

