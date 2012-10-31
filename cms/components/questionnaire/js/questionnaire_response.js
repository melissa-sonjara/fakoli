/**
 * QuestionnaireResponse contains methods for managing
 * the response handling of a questionnaire.
 * 
 * 
 */
var QuestionnaireResponseManager =  new Class 
({
	dialog: null, 

	itemPk: null,	//< the id of the questionnaire or survey table (e.g., questionnaire_id)
	item_id: null, //< the id value of the questionnaire class being created/updated

	responsePk: null,	//< the id of the response table (e.g., response_id)
		
	component_name: null,	//< the name of the component (e.g., questionnaire)
	
	exclude_handler: null, 	///< handler that sets a response to be excluded from results

	response_intro_identifier: null, 		///< the survey response intro page 
	
	response_form_identifier: null,			///< the response form page identifier
	
	initialize: function(itemPk, item_id, responsePk, 
			component_name,
			response_intro_identifier,
			response_form_identifier
			)
	{
		this.itemPk = itemPk;
		this.item_id = item_id;
		this.responsePk = responsePk;
		this.component_name = component_name;
		this.exclude_handler = exclude_handler;
		this.response_intro_identifier = response_intro_identifier;
		this.response_form_identifier = response_form_identifier;
	},

	resetToken: function(response_id)
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + reset_token_handler + '?' + this.responsePk + '=' + response_id, 
			'method': 	'get',
			'onSuccess': function(response) 
			{ 
				if (response == "OK")
				{
					go(this.response_intro_identifier + '?' + this.itemPk + '=' + this.item_id);
				}
				else
				{
					alert(response);
				}
			},
						
			'onFailure':	function() { alert("Failed to communicate with server");}
		});
						
		request.send();	
	},
	
	/*
	 * Present input box for users to enter their survey
	 * response token. 
	 * 
	 */
	showResponseTokenDialog: function()
	{
		var valid = false;
		
		while(!valid)
		{
			var token = prompt("Enter your access token", "");
			if (token == null) return;
			
			valid = token.match(/^\w{5}$/);
		}

		go(this.response_form_identifier + '?' + this.itemPk + '=' + this.item_id + '&token=' + token);
	},
	
	excludeResponse: function(response_id)
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.exclude_handler + '?' + this.responsePk + '=' + response_id, 
			'method': 	'get',
			'onSuccess': function(response) 
			{ 
				if (response == "OK")
				{
					window.location.reload();
				}
				else
				{
					alert(response);
				}
			},	'onFailure':	function() { alert("Failed to communicate with server");}
			});
								
		request.send();	
	},
	

	closeDialog: function()
	{
		this.dialog.hide();
	}
});