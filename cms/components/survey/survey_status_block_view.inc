<?php
Fakoli::usingFeature("status_tree");

class SurveyStatusBlockView extends StatusBlockView
{
	var $activityName;
	var $deleteUrl = "";
	var $editUrl;
	var $reviewUrl;
	var $publicUrl;
	var $statusUrl;
	var $iconView = "/fakoli/images/icon_view.gif";
	var $iconPublic = "/fakoli/images/icon_view_public.gif";
	var $iconEdit = "/fakoli/images/icon_edit.gif";
	var $iconOpen = "/fakoli/images/icon_reopen.gif";
	var $iconClose = "/fakoli/images/icon_close.gif";
	var $iconDelete = "/fakoli/images/icon_delete.gif";
	var $iconResults = "/fakoli/images/icon_survey_results.gif";
	
	function SurveyStatusBlockView($item)
	{
		$this->activityName = "Survey";
		$pk = $item->getPrimaryKey();
		$this->deleteUrl = "survey_delete?survey_id={$item->$pk}";
		$this->editUrl = "survey_form?survey_id={$item->$pk}";
		$this->statusUrl = "/action/survey/survey_set_status?survey_id={$item->$pk}&status=";
		$this->resultsUrl = "survey_data?survey_id={$item->$pk}";
		$this->publicUrl = "survey_intro?survey_id={$item->$pk}";
		$this->reviewUrl = "survey_data?survey_id={$item->$pk}";
		$this->item = $item;
	}
	
	
	function getViewHTML()
	{
		global $user;
		
		$html = "<div class=\"status_block\">";

		$html .= "<div id=\"col2\" style=\"float: right\"><br/>\n";	
		$html .= $this->getLinksHTML();
		$html .= "</div>\n<br/>\n";
		$html .= $this->getSummaryHTML();
		$html .= "</div>\n";
		
		return $html;		
	}
	
	// AJG - use buttons instead of links for consistency on this site
	function getButtonHTML($url, $icon, $text, $newWindow = false, $class = "")
	{
		if($newWindow)
			$fn = "window.open";
		else
			$fn = "go";
			
		$class = (!$class) ? "button" : $class;
		$img = ($icon) ? "<img src='$icon' style='border: none;display: inline-block;vertical-align: middle;padding-right: 4px'>" : ""; 
		
		$html = "<p><button $target class=\"$class\" onclick='$fn(\"$url\"); return false;'>
		$img&nbsp;$text&nbsp;</button></p>";
		
		return $html;
	}
	
	function getDeleteButton($class = "")
	{
		$class = (!$class) ? "button" : $class;
		$icon = ($this->iconDelete) ? "<img src=\"$this->iconDelete\" style='border: none;display: inline-block;vertical-align: middle;padding-right: 4px'/>\n" : "";

		$message = "Are you sure you want to delete this {$this->activityName}?";

		$html = "<p><button href= class=\"$class\"   
		onclick=\"if (confirm('$message')) go('$this->deleteUrl');\">
		$icon&nbsp;Delete this $this->activityName</button></p>";
		
		return $html;
		
	}
	
	function getSummaryHTML()
	{
		$view = new SurveyView($this->item);
		$html = $view->getSurveyDetails();		
		return $html;
		
	}
	
	
	function getLinksHTML()
	{
		global $user;
		
		if($this->item->status == survey_not_sent) // not sent
		{
			$html .= $this->getEditButton();
		}
		elseif($this->item->status == survey_open) // open
		{
			$html .= $this->getCloseButton();
		}
		elseif($this->item->status == survey_closed) // closed
		{
			$html .= $this->getReopenButton();
		}
		
		if($this->item->status > survey_not_sent)
		{
			$html .= $this->getResultsButton();
			$html .= $this->getViewPublicPageButton();	
		}

		$html .= $this->getViewButton();
		
		if($this->item->status != survey_open)
			$html .= $this->getDeleteButton();
		
		return $html;
	}
			

	function getResultsButton()
	{
		$html = $this->getButtonHTML($this->resultsUrl, $this->iconResults, "{$this->activityName} Results");
		
		return $html;
	}

	
	function getCloseButton()
	{
		$html = $this->getButtonHTML($this->statusUrl . survey_closed, $this->iconClose, "Close this {$this->activityName}");
		
		return $html;		
	}
	
	function getReopenButton()
	{
		$html = $this->getButtonHTML($this->statusUrl . survey_open, $this->iconOpen, "Reopen this {$this->activityName}");
		
		return $html;		
	}

	function getViewPublicPageButton()
	{
		$html = $this->getButtonHTML($this->publicUrl, $this->iconPublic, "Public View of {$this->activityName}");
		
		return $html;		
		
	}
}

?>