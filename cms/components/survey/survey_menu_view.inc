<?php

class SurveyMenuView
{	
	function SurveyMenuView()
	{
	}
		
	static function getMenuItems($current)
	{
		$path = "/fakoli/images/";

		$menuItems[0] = new SurveyMenuItem("survey_dashboard", 
			"Survey Dashboard", 
			$path . "button_survey_dashboard.png",
			$path . "button_survey_dashboard_hover.png"
		);
		
		$menuItems[1] = new SurveyMenuItem("survey_template_select", 
			"Create a Survey", 
			$path . "button_create_survey2.jpg",
			$path . "button_create_survey_hover.jpg"
		);
		$menuItems[2] = new SurveyMenuItem("survey_list", 
			"Manage Surveys",
			$path . "button_manage_surveys.jpg", 
			$path . "button_manage_survey_hover.jpg"
			);

		
		if(checkRole("admin"))
		{			
			$menuItems[4] = new SurveyMenuItem(
				"survey_templates", 
				"Survey Templates", 
				$path . "button_create_survey_template.png", 
				$path . "button_create_survey_template_hover.jpg"
			);
		}
		
		return $menuItems;
	}
	
	static function getView($current = "")
	{
		$menuItems = SurveyMenuView::getMenuItems($current);
		
		foreach($menuItems as $item)
		{
			if ($current == $item->identifier)
			{
				$html .= "<p><a href=\"{$item->identifier}\"><img style=\"border: none\" alt=\"{$item->altTag}\" src=\"{$item->active}\"></a></p>\n";
			}
			else
			{
				$html .= "<p><a href=\"{$item->identifier}\"><img style=\"border: none\" alt=\"{$item->altTag}\" src=\"{$item->image}\" onmouseover=\"this.src='{$item->active}';\" onmouseout=\"this.src='{$item->image}';\"></a></p>\n";		
			}
		}
		
		return $html;
	}
}

class SurveyMenuItem
{
	var $identifier;
	var $altTag;
	var $image;
	var $active;
	
	function SurveyMenuItem($identifier, $altTag, $image, $active)
	{
		$this->identifier = $identifier;
		$this->altTag = $altTag;
		$this->image = $image;
		$this->active = $active;	
	}
}
