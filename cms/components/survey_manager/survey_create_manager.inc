<?php



class SurveyQuestionForm
{
	function SurveyQuestionForm()
	{

	}

	static function questionTypeSetup(&$form, $question, $editable)
	{
		$pk = $question->getPrimaryKey();
		
		if($question->isRatings() || $editable)
		{
			$form->add(new RatingQuestionFieldRenderer($form), "label_for_lowest");
			$form->add(new StringFieldRenderer($form), "label_for_highest");
			$form->add(new NumberFieldRenderer($form), "number_of_steps");	
		}
		
		if($editable || $question->isFreeText() || $question->isShortText())
		{
			$form->getRenderer("num_rows")->size = 5;
			$form->getRenderer("char_limit")->size = 5;
		}
		else
			$form->hide("num_rows", "char_limit");

		if($question->$pk && $question->isRatings())
			$form = SurveyQuestionForm::setOptionsToRatings($form);
		elseif($editable)
			$form = SurveyQuestionForm::setDefaultRatings($form);

		$form->labelCSS = "survey_question_form_label";
				
		return $form;
	}

	static function setRatingsToOptions()
	{
		global $_POST;

		$options = $_POST["label_for_lowest"] . "\r\n";
		$options .= $_POST["label_for_highest"] . "\r\n";
		$options .= $_POST["number_of_steps"] . "\r\n";

		$_POST["options"] = $options;
	}

	static function setOptionsToRatings($form)
	{
		list($from, $to, $steps) = explode("\n", $form->data->get("options"));

		$form->data->set("label_for_lowest", $from);
		$form->data->set("label_for_highest", $to);
		$form->data->set("number_of_steps", $steps);

		return $form;
	}

	static function setDefaultRatings($form)
	{
		$form->data->set("label_for_lowest", "Lowest");
		$form->data->set("label_for_highest", "Highest");
		$form->data->set("number_of_steps", 5);

		return $form;
	}
}
