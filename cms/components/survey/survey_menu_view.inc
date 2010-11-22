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
		
		// Map the page identifier to the navigation menu item
		// that should appear selected.
		$navMap = array(
				"survey_dashboard"	=>	"survey_dashboard",
				"survey_data_table"	=>	"survey_dashboard",
				"suvey_data"		=>	"survey_dashboard",
				"survey_email"		=>	"survey_template_select",
				"survey_form"		=>	"survey_template_select",
				"survey_list"		=>	"survey_list",
				"survey_manage"		=>	"survey_dashboard",
				"survey_preview"	=>	"survey_template_select",
				"survey_question_form"	=>	"survey_template_select",
				"survey_questions"	=>	"survey_template_select",
				"survey_results"	=>	"survey_template_select",
				"survey_template_form"	=>	"survey_templates",
				"survey_template_preview"	=>	"survey_templates",
				"survey_template_question_form"	=>	"survey_templates",
				"survey_template_question_select"	=>	"survey_templates",
				"survey_template_questions"	=>		"survey_templates",
				"survey_template_select"	=>"survey_templates",
				"survey_templates"		=>	"survey_templates"
		);
		
		if(array_key_exists($current, $navMap))
			$navId = $navMap[$current];
		
		foreach($menuItems as $item)
		{
			if ($navId == $item->identifier)
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
