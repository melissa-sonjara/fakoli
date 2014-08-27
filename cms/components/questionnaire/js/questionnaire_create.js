/**
 * QuestionnaireCreate contains the following methods for managing
 * the create/update/delete of a questionnaire.
 * 
 * 
 */
var QuestionnaireCreateManager =  new Class 
({
	dialog: null, 
	
	qPk: null,		//< the id of the question table (e.g., question_id)
	itemPk: null,	//< the id of the questionnaire or survey table (e.g., questionnaire_id)
	item_id: null, //< the id value of the questionnaire class being created/updated
	xrefPk: null, 	//< the pk to the xref class; can be empty
	
	component_name: null,	//< the name of the component (e.g., questionnaire)
	//< some component may have more than one questionnaire so item class is needed

	question_delete_handler: null,	//< the name of the delete handler file - defaults to "question_delete"
	
	question_remove_handler: null, //< when questions are linked through xref
	
	question_list_identifier: null, //< the identifier of the page that lists the questions (e.g., questionnaire_questions)
	
	initialize: function(qPk, itemPk, item_id, xrefPk, component_name,
			question_delete_handler, question_remove_handler,
			question_list_identifier
			)
	{
		this.qPk = qPk;
		this.itemPk = itemPk;
		this.item_id = item_id;
		this.xrefPk = xrefPk; //< can be empty	
		this.component_name = component_name;
		this.question_delete_handler = question_delete_handler;
		this.question_remove_handler = question_remove_handler;
		this.question_list_identifier = question_list_identifier;
	},
	
	
	/*
	* Remove question from questionnaire_questions table list view
	* and hide the removed row.
	* 
	* Requires that the Questionnaire Question Data List View
	* set $table->rowId = true;
	* so that the deleted row can be hidden using 
	* this.qPk to get the question pk.
	* 
	* @param question_id - the key value to the question table
	*/
	deleteQuestion: function(question_id)
	{
		if (!confirm("Are you sure you want to delete this question?"))
		{
			return;
		}
			
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.question_delete_handler + '?' + this.qPk + '=' + question_id + '&' + this.itemPk + '=' + this.item_id, 
			 'method': 	'get',
			 'onSuccess': function(response) 
			{ 
				if (response == "OK") 
				{
					var tr = document.id(this.qPk + '_' + question_id);
					tr.setStyle("display", "none");
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
		
	
	/*
	 * Remove survey question from survey_questions table list view
	 * and hide the removed row.
	 */
	removeQuestion: function(xref_id)
	{
		if (confirm("Are you sure you want to remove this question?"))
		{		
			var request = new Request(
			{
				'url': 		'/action/' + this.component_name + '/' + this.question_remove_handler + '?' + this.xrefPk + '=' + xref_id + '&' + this.itemPk + '=' + this.item_id,  
				  'method': 	'get',
				 'onSuccess': function(response) 
				 { 
					if (response == "OK") 
					{
						var tr = document.id(this.xrefPk + '_' + xref_id);
						tr.setStyle("display", "none");
					}
					else
					{
						alert(response);
					}	
				 }.bind(this),
			
				 'onFailure':	function() { alert("Failed to communicate with server");}
			 });
			
			request.send();				
		}		
	},
	
	/*
	 * Remove button from question form.
	 * 
	 * When xref table is used, we don't use the standard delete button in
	 * the AutoForm because that would delete the question - we want only
	 * to delete the xref to it.
	 */
	removeQuestionFromForm: function(question_id)
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.question_remove_handler + '?' + this.qPk + '=' + question_id + '&' + this.itemPk + '=' + this.item_id,  
		    'method': 	'get',
			 'onSuccess': function(response) 
			 { 
				if (response == "OK") 
				{
					go(this.question_list_identifier + '?' + this.itemPk + '=' + this.item_id);
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
	 * Can be called from any questionnaire/survey component.
	 */
	showSampleQuestionnaire: function()
	{
		this.dialog = modalPopup('Sample Questionnaire', '/action/questionnaire/sample_questionnaire', '600px', 'auto', true);		
	},	
	
	
	closeDialog: function()
	{
		this.dialog.hide();
	}
});