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

		showSurveyReminderDialog: function(survey_id)
		{
			this.dialog = modalPopup('Send Survey Reminders', '/action/survey/survey_reminder_form?survey_id=' + survey_id, '550px', 'auto', true);
		},
	
		surveyReminderFormResult: function(response)
		{
			var elt = $("survey_reminder_result");
			if(response == "OK" || response == "FAIL")
			{
				this.closeDialog();
				elt.set("display", "block");
				if(response == "OK")
				{
					elt.set("class", "");
					elt.set("text", "Message has been sent.");
				}
				else
				{
					elt.set("text", "Message could not be sent.");
				}
			}
			else
			{
				$('SurveyReminder_form__error').set('html', response);
			}
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