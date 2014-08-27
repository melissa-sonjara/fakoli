/**
 * QuestionnaireResponse contains the following methods for managing
 * the results display of a questionnaire.
 */
var QuestionnaireResultsManager =  new Class 
({
	dialog: null, 

	itemPk: null,	//< the id of the questionnaire or survey table (e.g., questionnaire_id)
	item_id: null, //< the id value of the questionnaire class being created/updated
	
	component_name: null,	//< the name of the component (e.g., questionnaire)
	
	question_names_dialog: null, //< the name of the action handler that displays the question names'
								 //< in a spreadsheet form
	
	
	// The response handler is in results rather than response b/c it displays the 
	// response in the results tab to the questionnaire/survey owner/author.
	response_view_dialog: null, //< the handler that displays the response in a modal dialog
	response_view_title: null, //< The title for the response view modal dialog
	
	exclude_handler: null, 	///< handler that sets a response to be excluded from results
	responsePk: null,		//< the id of the response table (e.g., response_id)
	
	initialize: function(itemPk, item_id, component_name, question_names_dialog,
			response_view_dialog, response_view_title, exclude_handler, responsePk
			)
	{
		this.itemPk = itemPk;
		this.item_id = item_id;
		this.component_name = component_name;
		this.question_names_dialog = question_names_dialog;
		this.response_view_dialog = response_view_dialog;
		this.response_view_title = response_view_title;
		this.exclude_handler = exclude_handler;
		this.responsePk = responsePk;
	},
	
	
	showResponse: function(responsePk, response_id)
	{
		this.dialog = modalPopup(this.response_view_title, '/action/' + this.component_name + '/' + this.response_view_dialog + '?' + responsePk + '=' + response_id, '700px', 'auto', true);		
	},		
	
	showQuestionNamesDialog: function()
	{
		this.dialog = modalPopup('Spreadsheet Column Headings', '/action/' + this.component_name + '/' + this.question_names_dialog + '?' + this.itemPk + '=' + this.item_id, '700px', 'auto', true);		
	},
	
	questionNamesFormResult: function(response)
	{
		if (response == "OK")
		{
			window.location.reload();
		}

		document.id('QuestionNames_form__error').set('html', response);
	},

	/**
	 * Include/Exclude a response from the responses table list view
	 * 
	 * @param Number response_id
	 * @param Boolean include_in_results
	 */
	toggleIncludeResponse: function(response_id, include_in_results)
	{
		var request = new Request(
		{	
			'url': 		'/action/' + this.component_name + '/' + this.exclude_handler + '?' + this.responsePk + '=' + response_id + '&include_in_results=' + include_in_results, 
			'method': 	'get',
			'onSuccess': function(response) 
			{ 
				if (response == "OK")
				{
					var elt = document.id('response_id_' + response_id);
					var tr = findAncestor(elt, "tr");
					var td = tr.getChildren("td")[3];
					var a = td.getChildren("a")[0];
					var img = a.getChildren("img");
					var str = a.get("onclick");
					var icon = String(img.get("src"));
					var alt = String(img.get("alt"));
							
					if(include_in_results)
					{
						img.set("src", icon.replace(/off.png/, "on.png"));
						img.set("alt", alt.replace(/exclude/, "include"));
						a.set("onclick", str.replace(/include/, "exclude"));
					}
					else
					{
						img.set("src", icon.replace(/on.png/, "off.png"));
						img.set("alt", alt.replace(/include/, "exclude"));
						a.set("onclick",  str.replace(/exclude/, "include"));
					}
				}
				else
				{
					alert(response);
				}
			}.bind(this),	
			
			'onFailure':	function() { alert("Failed to communicate with server");}
					
		});
										
		request.send();			
	},
	
	/**
	 * From table list view of survey responses, allow survey owner to exclude/include individual reponses.
	 * 
	 * @param response_id
	 */
	excludeResponse: function(response_id)
	{
		this.toggleIncludeResponse(response_id, 0);
	},
	
	/**
	 * From table list view of survey responses, allow survey owner to exclude/include individual reponses.
	 * 
	 * @param response_id
	 */
	includeResponse: function(response_id)
	{
		this.toggleIncludeResponse(response_id, 1);
	},
		
	closeDialog: function()
	{
		this.dialog.hide();
	}
});