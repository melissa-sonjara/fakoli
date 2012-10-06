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
	
	
	initialize: function(itemPk, item_id, component_name, question_names_dialog,
			response_view_dialog, response_view_title)
	{
		this.itemPk = itemPk;
		this.item_id = item_id;
		this.component_name = component_name;
		this.question_names_dialog = question_names_dialog;
		this.response_view_dialog = response_view_dialog;
		this.response_view_title = response_view_title;
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

		$('QuestionNames_form__error').set('html', response);
	},

	closeDialog: function()
	{
		this.dialog.hide();
	}
});