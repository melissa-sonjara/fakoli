/**
 * QuestionnaireDashboard contains the methods for managing
 * the questionnaires from the list or dashboard page.
 *
 *
 */
class QuestionnaireDashboardManager
{
	constructor(
			itemPk,
			component_name,
			item_label,
			clone_dialog,
			questionnaire_form_identifier,
			item_delete_handler,
			item_view_dialog,
			response_form_identifier,
			results_page_identifier
			)
	{
		this.dialog = null;

		this.itemPk = itemPk;	///< the id of the questionnaire or survey table (e.g., questionnaire_id)

		this.component_name = component_name;	///< the name of the component (e.g., questionnaire)
		///< some component may have more than one questionnaire so item class is needed

		this.item_label = item_label; 	//< the item pretty class name or label for delete confirmation

		this.clone_dialog = clone_dialog; 	///< the name of the clone dialog handler

		this.questionnaire_form_identifier = questionnaire_form_identifier; //< the identifier of the questionnaire create/update form
								///< where user is redirected after clone action.

		this.item_delete_handler = item_delete_handler; ///< the handler that deletes a questionnaire

		this.item_view_dialog = item_view_dialog; 	///< handler to display view of the survey/questionnaire

		this.response_form_identifier = response_form_identifier;		///< identifier of response form

		this.results_page_identifier = results_page_identifier;		///< identifier of first page of results tab set

		this.questionnaireSendMgr = null;		///< the var for the send manager js file, if used
	}

	/*
	 * Handle Actions drop down in questionnaire_dashboard table
	 *
	 * Components may wish to implement additional actions.
	 *
	 * @param elt - the Action drop down element
	 *
	 * @item_id - the id value of the questionnaire table
	 */
	handleQuestionnaireAction(elt, item_id)
	{
		var action = elt.value;

		/*
		 * Actions applicable if QuestionnaireSendManager is implemented
		 */
		if(this.questionnaireSendMgr && (action == "send" || action == "send_test" ||
				action == "close" || action == "reopen" || action == "send_reminders" ||
				action == "send_additional"))
		{
			this.questionnaireSendMgr.item_id = item_id;
			this.questionnaireSendMgr.dashboard_action = 1;
			this.questionnaireSendMgr.handleSendAction(action);
		}
		else
		{
			this.handleDashboardAction(action, item_id);
		}

		elt.value = '';
	}


	handleDashboardAction(action, item_id)
	{
		switch(action)
		{
			case 'view':
			this.showQuestionnaireViewDialog(item_id);
			break;

			case 'edit':
			go(this.questionnaire_form_identifier + '?' + this.itemPk + '=' + item_id);
			break;

			case 'public_view':
			go(this.response_form_identifier + '?' + this.itemPk + '=' + item_id);
			break;

			case 'clone':
			this.showCloneQuestionnaireDialog(item_id);
			break;

			case 'view_results':
			go(this.results_page_identifier + '?' + this.itemPk + '=' + item_id);
			break;

			case 'delete':
			this.deleteQuestionnaire(item_id);
			break;
		}
	}

	showQuestionnaireViewDialog(item_id)
	{
		this.dialog = modalPopup(this.item_label + ' View', '/action/' + this.component_name + '/' + this.item_view_dialog + '?' + this.itemPk + '=' + item_id, '700px', 'auto', true);
	}

	deleteQuestionnaire(item_id)
	{
		var msg = "Are you sure you want to delete this " + this.item_label + '?';

		if (!confirm(msg)) return;

		fetch('/action/' + this.component_name + '/' + this.item_delete_handler + '?' + this.itemPk + '=' + item_id)
			.then(r => r.text())
			.then(function(response)
			{
				if (response == "OK")
				{
					window.location.reload();
				}
				else
				{
					alert(response);
				}
			}.bind(this))
			.catch(function() { alert("Failed to communicate with server"); });
	}

	showCloneQuestionnaireDialog(item_id)
	{
		this.dialog = modalPopup('Save As...', '/action/' + this.component_name + '/' + this.clone_dialog + '?' + this.itemPk + '=' + item_id, '700px', 'auto', true);
	}

	/**
	* The clone dialog must specify the form id is "Questionnaire_form" in the
	* call to new AutoForm
	*
	* The form id is the id set in the AbstractQuestionnaireCreateManager function buildCloneForm
	*/
	cloneQuestionnaireResult(response)
	{
		if (response.indexOf("OK") == 0)
		{
			var responseFields = response.split("|");
			go(this.questionnaire_form_identifier + '?' + this.itemPk + '=' + responseFields[1]);
		}

		document.getElementById('CloneQuestionnaire_form__error').innerHTML = response;
	}

	closeDialog()
	{
		this.dialog.hide();
	}
}
