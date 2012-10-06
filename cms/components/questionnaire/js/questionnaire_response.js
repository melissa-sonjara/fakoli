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
	
	initialize: function(itemPk, item_id, responsePk, component_name)
	{
		this.itemPk = itemPk;
		this.item_id = item_id;
		this.responsePk = responsePk;
		this.component_name = component_name;
	},

	closeDialog: function()
	{
		this.dialog.hide();
	}
});