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
				this.cloneSurvey(survey_id);
			}
			else if(action == "view_results")
			{
				go("survey_data?survey_id=" + survey_id);
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
	
		/*
		 * Remove survey question from survey_questions table list view
		 * and hide the removed row.
		 */
		removeSurveyQuestion: function(survey_question_xref_id)
		{
			if (confirm("Are you sure you want to remove this question?"))
			{		
				var request = new Request(
				{
					'url': 		'/action/survey/survey_question_remove?survey_question_xref_id=' + survey_question_xref_id, 
					  'method': 	'get',
					 'onSuccess': function(response) 
					 { 
						if (response == "OK") 
						{
							var tr = $("survey_question_xref_id_" + survey_question_xref_id);
							tr.setStyle("display", "none");
						}
					 },
				
					 'onFailure':	function() { alert("Failed to communicate with server");}
				 });
				
				request.send();				
			}		
		},
		
		/*
		 * Remove button from survey question form.
		 */
		removeSurveyQuestionFromForm: function(survey_id, survey_question_id)
		{
			var request = new Request(
			{
				'url': 		'/action/survey/survey_question_remove?survey_id=' + survey_id + '&survey_question_id=' + survey_question_id, 
				  'method': 	'get',
				 'onSuccess': function(response) 
				 { 
					if (response == "OK") 
					{
						go("survey_questions?survey_id=" + survey_id);
					}
				 },
				
			 'onFailure':	function() { alert("Failed to communicate with server");}
			 });
				
			request.send();						
		},
	
		
		showQuestionNamesDialog: function(survey_id)
		{
			this.dialog = modalPopup('Survey Spreadsheet Column Headings', '/action/survey/question_names_form?survey_id=' + survey_id, '700px', 'auto', true);		
		},
		
		questionNamesFormResult: function(response)
		{
			if (response == "OK")
			{
				window.location.reload();
			}

			$('QuestionNames_form__error').set('html', response);
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
	
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * 
		 *           Question Form
		 * 
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		questionFormSetup: function(question_type_id)
		{
			var form = $('SurveyQuestion_form');
			this.setQuestionTypeFields(question_type_id, form);
		},
		
		setQuestionTypeOptions: function()
		{
			var typeElt = $('question_type_id');
			this.onChangeQuestionType(typeElt);
		},
		
		setQuestionTypeFields: function(type, form)
		{
			var question_type_id = parseInt(type);
			
			switch(question_type_id)
			{
				case 1: // multiple choice
					this.showOptionsField(form);
					this.hide_tr('SurveyQuestion_form_num_rows');
					this.hideRatingsFields();
					this.hide_tr('SurveyQuestion_form_char_limit');
					this.renderRequiredCheckbox(form);
					break;

				case 2: // rating
					this.hideOptionsField(form);
					this.hide_tr('SurveyQuestion_form_num_rows');
					this.showRatingsFields();
					this.hide_tr('SurveyQuestion_form_char_limit');
					this.renderRequiredCheckbox(form);
					break;
			
				case 3: // short text
					this.hideOptionsField(form);
					this.hide_tr('SurveyQuestion_form_num_rows');
					this.hideRatingsFields();
					this.show_tr('SurveyQuestion_form_char_limit');
					this.renderRequiredCheckbox(form);
					break;
					
				case 4: // free text
					this.hideOptionsField(form);
					this.show_tr('SurveyQuestion_form_num_rows');
					this.hideRatingsFields();
					this.show_tr('SurveyQuestion_form_char_limit');
					this.renderRequiredCheckbox(form);
					break;
				
				case 5: // checklist
					this.showOptionsField(form);
					this.hide_tr('SurveyQuestion_form_num_rows');
					this.hideRatingsFields();
					this.hide_tr('SurveyQuestion_form_char_limit');
					this.renderRequiredNumber(form);
					break;
					
				case 6: // drop down list
					this.showOptionsField(form);
					this.hide_tr('SurveyQuestion_form_num_rows');
					this.hideRatingsFields();
					this.hide_tr('SurveyQuestion_form_char_limit');
					this.renderRequiredCheckbox(form);
					break;
					
				default:
					break;
			}
		},
		
		onChangeQuestionType: function(typeElt)
		{
			var type = typeElt.value;
			var form = $('SurveyQuestion_form');
		
			var question_type_id = parseInt(type);
			this.setQuestionTypeFields(question_type_id, form);
		},
		
		hideOptionsField: function(form)
		{
			var option_label_tr = this.getOptionsLabel(form);
	
			this.hide_tr('SurveyQuestion_form_options');
			if(option_label_tr)
				option_label_tr.setStyle("display", "none");	
		},
		
		getOptionsLabel: function(form)
		{
			var option_label_tr;			
			var option_label = form.getElements("label[for='options']").each(function(label) 
			{ 
				option_label_tr = findAncestor(label, "tr");
			});
			
			return option_label_tr;
		},
		
		showOptionsField: function(form)
		{
			var option_label_tr;
			option_label_tr = this.getOptionsLabel(form);

			this.show_tr('SurveyQuestion_form_options');
			
			if(option_label_tr)
				option_label_tr.setStyle("display", "");
		},
		
		showRatingsFields: function()
		{
			this.show_tr('SurveyQuestion_form_label_for_lowest');
			this.show_tr('SurveyQuestion_form_label_for_highest');
			this.show_tr('SurveyQuestion_form_number_of_steps');
		},
		
		hideRatingsFields: function()
		{
			this.hide_tr('SurveyQuestion_form_label_for_lowest');
			this.hide_tr('SurveyQuestion_form_label_for_highest');
			this.hide_tr('SurveyQuestion_form_number_of_steps');
		},
		
		// Render the required field as a boolean checkbox
		renderRequiredCheckbox: function(form)
		{
			var requiredElt = $('SurveyQuestion_form_required');		
			var required_label;
			
			var required_label = form.getElements("label[for='required']").each(function(label) 
			{ 
				label.set("html", "Answer Required");
			});

			var value;
			value = this.getRequiredValue(requiredElt);
						
			requiredElt.set("type", "checkbox");
			requiredElt.set("size", "");
			requiredElt.set("value", 1);
			if(value > 0)
				requiredElt.checked = true;
			requiredElt.set("onkeypress", "");			
		},
		
		getRequiredValue: function(requiredElt)
		{
			var value;
			
			if(requiredElt.get("type") == "checkbox")
			{
				 if(requiredElt.checked == true)
					 value = 1;
				 else
					 value = 0;
			}
			else
				value = requiredElt.value;
		
			return value;
		},
		
		// render the Required field as a number
		renderRequiredNumber: function(form)
		{
			var requiredElt = $('SurveyQuestion_form_required');
			
			var required_label;		
			var required_label = form.getElements("label[for='required']").each(function(label) 
			{ 
				label.set("html", "Number Required");
			});

			var value;
			value = this.getRequiredValue(requiredElt);
	
			requiredElt.set("type", "text");
			requiredElt.set("size", 5);
			requiredElt.set("value", value);
			requiredElt.set("onkeypress", "return maskInput(event, 0);");
		},

		
		hide_tr: function(id)
		{
			var elt = $(id);
			if(elt)
				var tr = findAncestor(elt, "tr");
			if(tr)
				tr.setStyle("display", "none");		
		},
	
		show_tr: function(id)
		{
			var elt = $(id);
			if(elt)
				var tr = findAncestor(elt, "tr");
			if(tr)
				tr.setStyle("display", "");		
		},
		
		showQuestionDetails: function(survey_id, survey_question_id)
		{
			this.dialog = modalPopup('Survey Question', '/action/survey/survey_question_details?survey_id=' + survey_id + '&survey_question_id=' + survey_question_id, '500px', 'auto', true);		
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