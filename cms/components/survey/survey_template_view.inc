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

Fakoli::using("survey", "questionnaire");
Fakoli::usingFeature("tab_bar");


function surveyTemplateTabs($key = "", $token = "")
{
	$tabs = getSurveyTemplateTabs();
	$qs = ($key) ? "survey_template_id=$key" : "";
	$tabBar = new TabBar("tabs", $tabs, $qs);

	return $tabBar;
}

function getSurveyTemplateTabs()
{
		$tabs = array(	
			"Survey Template Form"		=>	"survey_template_form",
			"Survey Template Questions"	=>	"survey_template_questions",
			"Survey Template Preview &amp; Publish" => "survey_template_preview"
		);
	return $tabs;
}


class SurveyTemplateView
{
	
	function SurveyTemplateView($surveyTemplate)
	{
	}
	
	static function drawView($surveyTemplate)
	{
		$mgr = new SurveyTemplateManager($surveyTemplate);
		$qForm = new QuestionnaireForm($mgr);
		
		$qForm->writeHTML(true);	
	}
}


class SurveyTemplatePreview
{
	var $qForm;
	
	function SurveyTemplatePreview($surveyTemplate)
	{
		$mgr = new SurveyTemplateManager($surveyTemplate);
		$this->qForm = new QuestionnaireForm($mgr, true);
	}
	
	function writeScript()
	{
		return $this->qForm->writeScript();
	}
	
	function drawView()
	{
		$this->qForm->writeHTML(true);	
	}
}


class SurveyQuestionSelectForm
{
	var $tree;
	var $item;
	var $buttons = array();
	var $noneText = "There are no questions.";
	var $labelCSS = "";					///< CSS Class to use for label cells
	var $buttonCSS = "";				///< CSS Class to use for buttons
	var $componentPath ="/fakoli";
	
	function SurveyQuestionSelectForm($item)
	{
		$this->item = $item;
		
		global $auto_form_defaults;
		foreach($auto_form_defaults as $field => $value)
		{
			$this->$field = $value;
		}
		
		$surveyQuestionSets = $this->getQuestionSets();
		$surveyTemplates = indexedQuery(SurveyTemplate, "WHERE status = ". survey_template_published, "survey_template_id");
		$surveyQuestions = indexedQuery(SurveyQuestion, "", "survey_question_id"); 
		
		$tree = new TreeControl("question_tree", "", "tree", true, 600);
		$tree->height = "350";
		$tree->indent = "0px";
		$tree->selectMode = 'multi';
	
		if(count($surveyQuestionSets) > 0)
		{
			foreach($surveyQuestionSets as $survey_template_id => $questionSet)
			{
				$surveyTemplate = $surveyTemplates[$survey_template_id];
				$questionSetNode = new TreeNode("survey_template_{$survey_template_id}", $surveyTemplate->title, null, false, "bare_node_closed", "bare_node_open"); 
		
				foreach($questionSet as $question)
				{		
					$survey_question_id = $question->survey_question_id;
					$surveyQuestion = $surveyQuestions[$survey_question_id];
	
					$questionNode = new TreeNode("question[$survey_question_id]", $surveyQuestion->question, false, false, "bare_node_closed", 
						"bare_node_open");
					$questionNode->leafStyle = "plain_node_leaf";
					$questionSetNode->add($questionNode);	
				}
				$tree->add($questionSetNode);
			} // end foreach surveyQuestionSets as questionSet
		}
		$this->tree = $tree;
	}
	
	/*
	 * Get the questions that are not already in the question set
	 * that the user is revising.
	 */
	function getQuestionSets()
	{
		$pk = $this->item->getPrimaryKey();
		$constraint = "WHERE survey_template_id > 0";
		$indexedQuestions = indexedQuery(SurveyQuestionXref, "WHERE $pk = {$this->item->$pk}", "survey_question_id");
		if(count($indexedQuestions) > 0)
		{
			$list = implode(",", array_keys($indexedQuestions));
			$constraint .= " AND survey_question_id NOT IN ($list)";
		}

		$surveyQuestionSets = groupedQuery(SurveyQuestionXref, $constraint, "survey_template_id");
		
		return $surveyQuestionSets;
	}
	
	function writeScript()
	{
		return "";
	}
		
	function drawForm()
	{
?>
<form method="POST" action="" id="survey_question_select_form">
<?php

		if(count($this->tree->children) == 0)
			echo $this->noneText;
		$this->tree->writeHTML();
		
		
?>
<br/>
<input type="submit" name="submit" class="button" value="Select Questions"/>
<?php
		$this->drawButtons();
?>
<br/></form>
<?php
	}
	
	function save()
	{
		global $_POST;

		$pk = $this->item->getPrimaryKey();
		$questionIds = $_POST["question"];
	
		if(isset($questionIds) AND count($questionIds) > 0)
		{
			$list = implode(",", array_keys($questionIds));
			$questions = query(SurveyQuestion, "WHERE survey_question_id IN ($list)");
			if(count($questions) > 0)
			{
				$sort_order = queryValue(SurveyQuestionXref, "MAX(sort_order)", "WHERE $pk = {$this->item->$pk}") + 1;
			
				foreach($questions as $question)
				{
					$xref = new SurveyQuestionXref();
					$xref->$pk = $this->item->$pk;
					$xref->survey_question_id = $question->survey_question_id;
					$xref->sort_order = $sort_order;
					$xref->save();
					$sort_order++;			
				}
			}
		}
		return true;
	}
	
	/**
	 * Adds a custom button to the form.
	 *
	 * @param string $text the button label text
	 * @param string $url the URL to handle the button press
	 * @param string $confirm optional confirmation message
	 */
	function button($text, $url, $confirm = null)
	{
		$this->buttons[] = array('text' => $text, 'url' => $url, 'confirm' => $confirm);
	}
	
	/**
	 * Draws any additional buttons specified in the calling script.
	 *
	 */
	function drawButtons()
	{
		foreach($this->buttons as $button)
		{
			echo "&nbsp;&nbsp;&nbsp;&nbsp;";
			$this->drawOneButton($button['url'], $button['text'], $button['confirm']);
		}

	}
	
	function drawOneButton($url, $text, $confirm = "", $dialog = false)
	{
		if ($confirm)
		{
			$link = "if (confirm('".jsSafe($confirm)."')) go('{$url}'); return false;";
		}
		else
		{
			$link = "go('$url'); return false;";
		}
	
		echo "<input type='button' class='{$this->buttonCSS}' onclick=\"$link\" value=\"$text\"/>";
		
	}
	
}