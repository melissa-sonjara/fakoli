/**
 * QuestionnaireResponse contains the following methods for managing
 * the results display of a questionnaire.
 */
class QuestionnaireResultsManager
{
	constructor(itemPk, item_id, component_name, question_names_dialog,
			response_view_dialog, response_view_title, exclude_handler, responsePk
			)
	{
		this.dialog = null;

		this.itemPk = itemPk;	//< the id of the questionnaire or survey table (e.g., questionnaire_id)
		this.item_id = item_id; //< the id value of the questionnaire class being created/updated

		this.component_name = component_name;	//< the name of the component (e.g., questionnaire)

		this.question_names_dialog = question_names_dialog; //< the name of the action handler that displays the question names'
								 //< in a spreadsheet form


		// The response handler is in results rather than response b/c it displays the
		// response in the results tab to the questionnaire/survey owner/author.
		this.response_view_dialog = response_view_dialog; //< the handler that displays the response in a modal dialog
		this.response_view_title = response_view_title; //< The title for the response view modal dialog

		this.exclude_handler = exclude_handler; 	///< handler that sets a response to be excluded from results
		this.responsePk = responsePk;		//< the id of the response table (e.g., response_id)
	}


	showResponse(responsePk, response_id)
	{
		this.dialog = modalPopup(this.response_view_title, '/action/' + this.component_name + '/' + this.response_view_dialog + '?' + responsePk + '=' + response_id, '700px', 'auto', true);
	}

	showQuestionNamesDialog()
	{
		this.dialog = modalPopup('Spreadsheet Column Headings', '/action/' + this.component_name + '/' + this.question_names_dialog + '?' + this.itemPk + '=' + this.item_id, '700px', 'auto', true);
	}

	questionNamesFormResult(response)
	{
		if (response == "OK")
		{
			window.location.reload();
		}

		document.getElementById('QuestionNames_form__error').innerHTML = response;
	}

	/**
	 * Include/Exclude a response from the responses table list view
	 *
	 * @param Number response_id
	 * @param Boolean include_in_results
	 */
	toggleIncludeResponse(response_id, include_in_results)
	{
		fetch('/action/' + this.component_name + '/' + this.exclude_handler + '?' + this.responsePk + '=' + response_id + '&include_in_results=' + include_in_results)
			.then(r => r.text())
			.then(function(response)
			{
				if (response == "OK")
				{
					var elt = document.getElementById('response_id_' + response_id);
					var tr = findAncestor(elt, "tr");
					var td = tr.querySelectorAll("td")[3];
					var a = td.querySelectorAll("a")[0];
					var img = a.querySelector("img");
					var str = a.getAttribute("onclick");
					var icon = String(img.getAttribute("src"));
					var alt = String(img.getAttribute("alt"));

					if(include_in_results)
					{
						img.setAttribute("src", icon.replace(/off.png/, "on.png"));
						img.setAttribute("alt", alt.replace(/exclude/, "include"));
						a.setAttribute("onclick", str.replace(/include/, "exclude"));
					}
					else
					{
						img.setAttribute("src", icon.replace(/on.png/, "off.png"));
						img.setAttribute("alt", alt.replace(/include/, "exclude"));
						a.setAttribute("onclick", str.replace(/exclude/, "include"));
					}
				}
				else
				{
					alert(response);
				}
			}.bind(this))
			.catch(function() { alert("Failed to communicate with server"); });
	}

	/**
	 * From table list view of survey responses, allow survey owner to exclude/include individual reponses.
	 *
	 * @param response_id
	 */
	excludeResponse(response_id)
	{
		this.toggleIncludeResponse(response_id, 0);
	}

	/**
	 * From table list view of survey responses, allow survey owner to exclude/include individual reponses.
	 *
	 * @param response_id
	 */
	includeResponse(response_id)
	{
		this.toggleIncludeResponse(response_id, 1);
	}

	closeDialog()
	{
		this.dialog.hide();
	}
}
