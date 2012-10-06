/**
 * QuestionnaireDashboard contains the methods for managing
 * the questionnaires from the list or dashboard page.
 * 
 * 
 */
var QuestionnaireDashboardManager =  new Class 
({
	dialog: null, 
	
	itemPk: null,	//< the id of the questionnaire or survey table (e.g., questionnaire_id)
	
	component_name: null,	//< the name of the component (e.g., questionnaire)
	//< some component may have more than one questionnaire so item class is needed

	item_label: null, 	//< the item pretty class name or label for delete confirmation

	clone_dialog: null, 	//< the name of the clone dialog handler
	
	questionnaire_form_identifier: null, //< the identifier of the questionnaire create/update form
							//< where user is redirected after clone action. 
						
	item_delete_handler: null, //< the handler that deletes a questionnaire
	
	initialize: function(
			itemPk, 
			component_name, 
			item_label, 
			clone_dialog, 
			questionnaire_form_identifier, 
			item_delete_handler
			)
	{
		this.itemPk = itemPk;
		this.component_name = component_name;
		this.item_label = item_label;
		this.clone_dialog = clone_dialog;
		this.questionnaire_form_identifier = questionnaire_form_identifier;
		this.item_delete_handler = item_delete_handler;
	},
	
	/*
	 * Handle Actions drop down in questionnaire_dashboard table
	 * 
	 * Components may wish to implement additional actions.
	 * 
	 * @param elt - the Action drop down element
	 * 
	 * @item_id - the id value of the questionnaire table
	 */
	handleQuestionnaireAction: function(elt, item_id)
	{
		var action = elt.value;
		
		if(action == 'edit')
		{
			go(this.questionnaire_form_identifier + '?' + this.itemPk + '=' + item_id);
		}
		else if(action == 'delete')
		{
			this.deleteQuestionnaire(item_id);
		}
		else if(action == 'clone')
		{
			this.showCloneQuestionnaireDialog(item_id);
		}
					
		elt.set('value', '');
	},


	deleteQuestionnaire: function(item_id)
	{
		var msg = "Are you sure you want to delete this " + this.item_label + '?';
		
		if (!confirm(msg)) return;
					
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.item_delete_handler + '?' + this.itemPk + '=' + item_id, 
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
		}.bind(this),
			
		'onFailure':	function() { alert("Failed to communicate with server");}
		});
		
		request.send();		
	},
	
	showCloneQuestionnaireDialog: function(item_id)
	{
		this.dialog = modalPopup('Save As...', '/action/' + this.component_name + '/' + this.clone_dialog + '?' + this.itemPk + '=' + item_id, '700px', 'auto', true);		
	},
	
	/**
	* The clone dialog must specify the form id is "Questionnaire_form" in the
	* call to new AutoForm
	* 
	* The form id is the id set in the AbstractQuestionnaireCreateManager function buildCloneForm
	*/
	cloneQuestionnaireResult: function(response)
	{
		if (response.indexOf("OK") == 0)
		{
			var responseFields = response.split("|");		
			go(this.questionnaire_form_identifier + '?' + this.itemPk + '=' + responseFields[1]);
		}

		$('CloneQuestionnaire_form__error').set('html', response);
	},
	
	closeDialog: function()
	{
		this.dialog.hide();
	}
});