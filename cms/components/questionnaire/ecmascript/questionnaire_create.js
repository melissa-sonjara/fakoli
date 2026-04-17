/**
 * QuestionnaireCreate contains the following methods for managing
 * the create/update/delete of a questionnaire.
 *
 *
 */
class QuestionnaireCreateManager
{
	constructor(qPk, itemPk, item_id, xrefPk, component_name,
			question_delete_handler, question_remove_handler,
			question_list_identifier
			)
	{
		this.dialog = null;

		this.qPk = qPk;		//< the id of the question table (e.g., question_id)
		this.itemPk = itemPk;	//< the id of the questionnaire or survey table (e.g., questionnaire_id)
		this.item_id = item_id; //< the id value of the questionnaire class being created/updated
		this.xrefPk = xrefPk; 	//< the pk to the xref class; can be empty

		this.component_name = component_name;	//< the name of the component (e.g., questionnaire)
		//< some component may have more than one questionnaire so item class is needed

		this.question_delete_handler = question_delete_handler;	//< the name of the delete handler file - defaults to "question_delete"

		this.question_remove_handler = question_remove_handler; //< when questions are linked through xref

		this.question_list_identifier = question_list_identifier; //< the identifier of the page that lists the questions (e.g., questionnaire_questions)

		this.xrefPk = xrefPk; //< can be empty
		this.component_name = component_name;
		this.question_delete_handler = question_delete_handler;
		this.question_remove_handler = question_remove_handler;
		this.question_list_identifier = question_list_identifier;
	}


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
	deleteQuestion(question_id)
	{
		if (!confirm("Are you sure you want to delete this question?"))
		{
			return;
		}

		fetch('/action/' + this.component_name + '/' + this.question_delete_handler + '?' + this.qPk + '=' + question_id + '&' + this.itemPk + '=' + this.item_id)
			.then(r => r.text())
			.then(function(response)
			{
				if (response == "OK")
				{
					var tr = document.getElementById(this.qPk + '_' + question_id);
					tr.style['display'] = "none";
				}
				else
				{
					alert(response);
				}
			}.bind(this))
			.catch(function() { alert("Failed to communicate with server"); });
	}


	/*
	 * Remove survey question from survey_questions table list view
	 * and hide the removed row.
	 */
	removeQuestion(xref_id)
	{
		if (confirm("Are you sure you want to remove this question?"))
		{
			fetch('/action/' + this.component_name + '/' + this.question_remove_handler + '?' + this.xrefPk + '=' + xref_id + '&' + this.itemPk + '=' + this.item_id)
				.then(r => r.text())
				.then(function(response)
				{
					if (response == "OK")
					{
						var tr = document.getElementById(this.xrefPk + '_' + xref_id);
						tr.style['display'] = "none";
					}
					else
					{
						alert(response);
					}
				}.bind(this))
				.catch(function() { alert("Failed to communicate with server"); });
		}
	}

	/*
	 * Remove button from question form.
	 *
	 * When xref table is used, we don't use the standard delete button in
	 * the AutoForm because that would delete the question - we want only
	 * to delete the xref to it.
	 */
	removeQuestionFromForm(question_id)
	{
		fetch('/action/' + this.component_name + '/' + this.question_remove_handler + '?' + this.qPk + '=' + question_id + '&' + this.itemPk + '=' + this.item_id)
			.then(r => r.text())
			.then(function(response)
			{
				if (response == "OK")
				{
					go(this.question_list_identifier + '?' + this.itemPk + '=' + this.item_id);
				}
				else
				{
					alert(response);
				}
			}.bind(this))
			.catch(function() { alert("Failed to communicate with server"); });
	}

	/**
	 * Can be called from any questionnaire/survey component.
	 */
	showSampleQuestionnaire()
	{
		this.dialog = modalPopup('Sample Questionnaire', '/action/questionnaire/sample_questionnaire', '600px', 'auto', true);
	}

	closeDialog()
	{
		this.dialog.hide();
	}
}
