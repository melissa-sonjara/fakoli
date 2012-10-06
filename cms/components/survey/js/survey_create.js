var SurveyCreate =  (function()
{
	var SurveyCreateSingleton = new Class(
	{
		dialog: null,
		question_count: null,
			
		initialize: function()
		{
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
					'url': 		'/action/survey/survey_delete?survey_id=' + survey_id, 
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
		
		cloneSurvey: function(survey_id)
		{
			var request = new Request(
			{
				'url': 		'/action/survey/clone_survey?survey_id=' + survey_id, 
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
				 }.bind(this),
				
			 'onFailure':	function() { alert("Failed to communicate with server");}
			 });
				
			request.send();						
		},
	
			
		closeDialog: function()
		{
			this.dialog.hide();
		}
		  
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new SurveyCreateSingleton();
	};
	
})();