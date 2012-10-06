var Survey =  (function()
{
	var SurveySingleton = new Class(
	{
		dialog: null,
		question_count: null,
			
		initialize: function()
		{
		},
		
		showIntroductionDialog: function()
		{
			this.dialog = modalPopup('Survey Introductions', '/action/survey/introduction_select_dialog', '450px', 'auto', true);		
		},
		
		introductionSelectResult: function(response)
		{
			this.closeDialog();
			
			if(response)	
			{
				var elt = $("Survey_form_introduction");
				if(elt)
					elt.set("value", response);
			}
	
		},
		
		showMessageSelectDialog: function()
		{
			this.dialog = modalPopup('Survey Messages', '/action/survey/message_select_dialog', '450px', 'auto', true);		
		},
		
		messageSelectResult: function(response)
		{
			this.closeDialog();
			
			if(response)	
			{
				var elt = $("Survey_form_message");
				if(elt)
					elt.set("value", response);
			}
	
		},	
	

		showAdvancedFeaturesDialog: function()
		{
			this.dialog = modalPopup('Advanced Message Features', '/action/survey/advanced_message_features_dialog', '450px', 'auto', true);
		},
				
		// survey preview & send
		showSurveyPreview: function(survey_id)
		{
			this.dialog = modalPopup('Survey Preview', '/action/survey/survey_view?survey_id=' + survey_id, '700px', 'auto', true);
		},	
		

		openSurvey: function(survey_id)
		{
			var request = new Request(
			{
				'url': 		'/action/survey/open_survey?survey_id=' + survey_id, 
				  'method': 	'get',
				 'onSuccess': function(response) 
				 { 
					if (response == "OK") 
					{
						window.location.reload();
					}
				},
					
				 'onFailure':	function() { alert("Failed to communicate with server");}
			});
						
			request.send();
		},
		
		closeSurvey: function(survey_id)
		{
			var request = new Request(
			{
				'url': 		'/action/survey/close_survey?survey_id=' + survey_id, 
				  'method': 	'get',
				 'onSuccess': function(response) 
				 { 
					if (response == "OK") 
					{
						window.location.reload();
					}
				},
					
				 'onFailure':	function() { alert("Failed to communicate with server");}
			});
				
			request.send();			
		},
		
		reopenSurvey: function(survey_id)
		{
			var request = new Request(
			{
				'url': 		'/action/survey/reopen_survey?survey_id=' + survey_id, 
				  'method': 	'get',
				 'onSuccess': function(response) 
				 { 
					if (response == "OK") 
					{
						window.location.reload();
					}
				},
					
				 'onFailure':	function() { alert("Failed to communicate with server");}
			});
						
			request.send();
		},
	
		deleteSurvey: function(survey_id, responseCount)
		{
			var msg = "Are you sure you want to delete this survey?";
			if(responseCount > 0)
				msg = "Are you sure you want to delete this survey and its " + responseCount + " responses?";
		
			if (confirm(msg))
			{		
				var request = new Request(
				{
					'url': 		'/action/survey/delete_survey?survey_id=' + survey_id, 
					  'method': 	'get',
					 'onSuccess': function(response) 
				{ 
					if (response == "OK") 
					{
						window.location.reload();
					}
				 },
				
				 'onFailure':	function() { alert("Failed to communicate with server");}
				 });
				
				request.send();				
			}		
		},
	
				
		/*
		 * Handle Actions drop down in survey_dashboard table
		 */
		handleSurveyAction: function(elt, survey_id, response_count)
		{
			var action = elt.value;
			
			if(action == 'edit')
			{
				go("survey_form?survey_id=" + survey_id);
			}
			else if(action == 'view')
			{
				this.showSurveyPreview(survey_id);
			}
			else if(action == 'public_view')
			{
				go("survey_response_intro?survey_id=" + survey_id);
			}
			else if(action == 'delete')
			{
				this.deleteSurvey(survey_id, response_count);
			}
			else if(action == 'clone')
			{
				new SurveyCreate().cloneSurvey(survey_id);
			}
			else if(action == "view_results")
			{
				go("survey_data?survey_id=" + survey_id);
			}
			else if(action == "open")
			{
				this.openSurvey(survey_id);
			}			
			else if(action == "close")
			{
				this.closeSurvey(survey_id);
			}
			else if(action == "reopen")
			{
				this.reopenSurvey(survey_id);
			}
			else if(action == "send_reminders")
			{
				this.showSurveyReminderDialog(survey_id);
			}
			else if(action == "send")
			{
				go("survey_preview?survey_id=" + survey_id);
			}
					
			elt.set('value', '');
		},
	
				
			
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * 
		 *           Survey Response
		 * 
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
		showSurveyResponse: function(response_id)
		{
			this.dialog = modalPopup('Survey Response', '/action/survey/survey_response_view?response_id=' + response_id, '700px', 'auto', true);		
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
		return instance ? instance : instance = new SurveySingleton();
	};
	
})();