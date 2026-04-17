/**
 * QuestionnaireResponse contains methods for managing
 * the response handling of a questionnaire.
 *
 *
 */
class QuestionnaireResponseManager
{
	constructor(itemPk, item_id, responsePk,
			component_name,
			exclude_handler,
			response_intro_identifier,
			response_form_identifier,
			reset_token_handler)
	{
		this.dialog = null;

		this.itemPk = itemPk;	//< the id of the questionnaire or survey table (e.g., questionnaire_id)
		this.item_id = item_id; //< the id value of the questionnaire class being created/updated

		this.responsePk = responsePk;	//< the id of the response table (e.g., response_id)

		this.component_name = component_name;	//< the name of the component (e.g., questionnaire)

		this.exclude_handler = exclude_handler; 	///< handler that sets a response to be excluded from results

		this.response_intro_identifier = response_intro_identifier; 		///< the survey response intro page

		this.response_form_identifier = response_form_identifier;			///< the response form page identifier

		this.reset_token_handler = reset_token_handler; 				///< reset tester token to not started
	}

	resetToken(response_id)
	{
		fetch('/action/' + this.component_name + '/' + this.reset_token_handler + '?' + this.responsePk + '=' + response_id)
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
			})
			.catch(function() { alert("Failed to communicate with server"); });
	}

	/*
	 * Present input box for users to enter their survey
	 * response token.
	 *
	 */
	showResponseTokenDialog()
	{
		var valid = false;

		while(!valid)
		{
			var token = prompt("Enter your access token", "");
			if (token == null) return;

			valid = token.match(/^\w{5}$/);
		}

		go(this.response_form_identifier + '?' + this.itemPk + '=' + this.item_id + '&token=' + token);
	}

	excludeResponse(response_id)
	{
		fetch('/action/' + this.component_name + '/' + this.exclude_handler + '?' + this.responsePk + '=' + response_id)
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
			})
			.catch(function() { alert("Failed to communicate with server"); });
	}


	closeDialog()
	{
		this.dialog.hide();
	}
}
