var SurveyResponse =  (function()
{
	var SurveyResponseSingleton = new Class(
	{
		dialog: null,
		question_count: null,
			
		initialize: function()
		{
		},
	
		showSurveyResponse: function(response_id)
		{
			this.dialog = modalPopup('Survey Response', '/action/survey/survey_response_view?response_id=' + response_id, '700px', 'auto', true);		
		},	

		showTokenDialog: function(survey_id)
		{
			var valid = false;
			
			while(!valid)
			{
				var token = prompt("Enter your survey access token");
				if (token == null) return;
				
				valid = token.match(/^\w{5}$/);
			}

			go('survey_response_form?survey_id=' + survey_id + '&token=' + token);
		},

		closeDialog: function()
		{
			this.dialog.hide();
		}
		  
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new SurveyResponseSingleton();
	};
	
})();