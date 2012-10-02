var SurveyResults =  (function()
{
	var SurveyResultsSingleton = new Class(
	{
		dialog: null,
		question_count: null,
			
		initialize: function()
		{
		},
			
		showQuestionNamesDialog: function(survey_id)
		{
			this.dialog = modalPopup('Survey Spreadsheet Column Headings', '/action/survey_manager/question_names_form?survey_id=' + survey_id, '700px', 'auto', true);		
		},
		
		questionNamesFormResult: function(response)
		{
			if (response == "OK")
			{
				window.location.reload();
			}

			$('QuestionNames_form__error').set('html', response);
		},
	
			
		closeDialog: function()
		{
			this.dialog.hide();
		}
		  
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new SurveyResultsSingleton();
	};
	
})();