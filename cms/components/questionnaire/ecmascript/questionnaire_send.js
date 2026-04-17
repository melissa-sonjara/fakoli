/**
 * QuestionnaireSend contains the methods for managing
 * surveys that request responses via email.
 *
 */
class QuestionnaireSendManager
{
	constructor(itemPk, item_id,
			component_name,
			item_label,
			send_test_email_dialog,
			send_test_email_handler,
			send_additional_handler,
			open_handler,
			close_handler,
			reminder_dialog,
			send_page_identifier,
			recipients_dialog,
			message_select_dialog,
			info_msg
			)
	{
		this.dialog = null;

		this.itemPk = itemPk;		///< the id of the questionnaire or survey table (e.g., questionnaire_id)
		this.item_id = item_id; 	///< the id value of the questionnaire class being created/updated

		this.component_name = component_name;	///< the name of the component (e.g., questionnaire)
		///< some component may have more than one questionnaire so item class is needed

		this.item_label = item_label; 		///< the item pretty class name or label for delete confirmation

		this.send_test_email_dialog = send_test_email_dialog; 	///< handler that lets users specify test email recipients

		this.send_test_email_handler = send_test_email_handler; 	///< handler that sends one test to user

		this.send_additional_handler = send_additional_handler;	///< handler to send additional emails

		this.open_handler = open_handler;				///< handler that opens survey to respondents

		this.close_handler = close_handler;			///< handler that closes survey to respondents

		this.reminder_dialog = reminder_dialog;			///< the handler that sends reminders to nonresponders

		this.send_page_identifier = send_page_identifier;		///< the page to preview/send survey requests

		this.recipients_dialog = recipients_dialog;

		this.message_select_dialog = message_select_dialog;

		this.info_msg = null; 				///< whether email messages were sent.

		this.dashboard_action = 0;		///< whether we are calling from the dashboard

		if(info_msg != '')
		{
			this.setInfoMsg(info_msg);
		}
	}

	/**
	 * validation msg on the survey preview page at the top of the
	 * page
	 */
	setValidationMsg(msg)
	{
		if(!msg) return;

		var div = document.getElementById('questionnaire_heading');
		if(!div) return;
		var elt = document.createElement('div');
		elt.id = 'warning';

		elt.innerHTML = msg;
		div.appendChild(elt);
	}

	/**
	 * msg on top of preview/manage page
	 */
	setInfoMsg(msg)
	{
		if(!msg) return;

		var elt = document.getElementById('action_result');
		if(!elt) return;

		var success = "OK";
		if(msg.match(/ not /))
		{
			success = "FAIL";
		}

		this.showActionResult(msg, success);
	}

	openToRespondents()
	{
		fetch('/action/' + this.component_name + '/' + this.open_handler + '?' + this.itemPk + '=' + this.item_id)
			.then(r => r.text())
			.then(function(response)
			{
				if (response == "OK")
				{
					if(!this.dashboard_action)
					{
						window.location.reload();
					}
					else
					{
						this.updateStatus("Open");
					}
				}
			}.bind(this))
			.catch(function() { alert("Failed to communicate with server"); });
	}

	closeToRespondents()
	{
		fetch('/action/' + this.component_name + '/' + this.close_handler + '?' + this.itemPk + '=' + this.item_id)
			.then(r => r.text())
			.then(function(response)
			{
				if (response == "OK")
				{
					if(!this.dashboard_action)
					{
						window.location.reload();
					}
					else
					{
						this.updateStatus("Closed");
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
	 * When user closes survey from the dashboard, update the status in the
	 * data list view and change the action drop down from reopen to close
	 * or close to reopen.
	 *
	 * @param status: "Closed" or "Open"
	 */
	updateStatus(status)
	{
		var elt = document.getElementById('status_' + this.item_id);
		if(!elt) return;

		elt.innerHTML = status;

		elt = document.getElementById('action_' + this.item_id);
		if(!elt) return;

		elt.querySelectorAll("option").forEach(function(option)
		{
			var html = option.innerHTML;
			if(option.value == "reopen")
			{
				option.value = "close";
				option.innerHTML = html.replace(/ReOpen/, "Close");
			}
			else if(option.value == "close")
			{
				option.value = "reopen";
				option.innerHTML = html.replace(/Close/, "ReOpen");
			}
		});
	}

	/**
	 * Called from preview/manage page or survey response intro tester
	 * block for logged in users.
	 *
	 * Result message is set in elt with id 'action_result'.
	 */
	sendTestEmail()
	{
		fetch('/action/' + this.component_name + '/' + this.send_test_email_handler + '?' + this.itemPk + '=' + this.item_id)
			.then(r => r.text())
			.then(function(response)
			{
				if (response.indexOf("OK") == 0 || response.indexOf("FAIL") == 0)
				{
					var responseFields = response.split("|");

					var elt = document.getElementById('action_result');
					if(!elt) return;
					var msg = responseFields[1];
					this.showActionResult(msg, responseFields[0]);
				}
				else
				{
					alert(response);
				}
			}.bind(this))
			.catch(function() { alert("Failed to communicate with server"); });
	}


	/**
	 * Can be called from survey_dashboard or from survey_preview/
	 * manage tab
	 */
	showTestEmailDialog()
	{
		this.dialog = modalPopup('Send Test Email', '/action/' + this.component_name + '/' + this.send_test_email_dialog + '?' + this.itemPk + '=' + this.item_id + '&dashboard=' + this.dashboard_action, '530px', 'auto', true);
	}

	testEmailResult(response)
	{
		if (response.indexOf("OK") == 0 || response.indexOf("FAIL") == 0)
		{
			var responseFields = response.split("|");
			var msg = responseFields[1];
			this.showActionResult(msg, responseFields[0]);
			this.closeDialog();
		}
		else
		{
			document.getElementById('SendTestEmail_form__error').innerHTML = response;
		}
	}

	/**
	 * Display the result of email send attempt in a grey background
	 * box at the top of the page.
	 *
	 * The calling page must have the div id on the page:
	 * <p id='action_result' class='questionnaire_action_result' style='display: none'></p>
	 *
	 * @param String msg
	 * @param String success : "OK" or "FAIL"
	 */
	showActionResult(msg, success)
	{
		var elt = "";
		if(!this.dashboard_action)
		{
			elt = document.getElementById('action_result');
		}
		else
		{
			elt = document.getElementById('action_result_' + this.item_id);
		}

		if(!elt) return;
		elt.innerHTML = msg;
		elt.style['display'] = "";
		elt.style['color'] = "";

		if(success == "FAIL")
		{
			elt.style['color'] = "red";
		}
	}

	showRecipientsDialog()
	{
		this.dialog = modalPopup('Survey Recipients', '/action/' + this.component_name + '/' + this.recipients_dialog + '?' + this.itemPk + '=' + this.item_id, '530px', 'auto', true);
	}

	showReminderDialog()
	{
		this.dialog = modalPopup('Send Reminders', '/action/' + this.component_name + '/' + this.reminder_dialog + '?' + this.itemPk + '=' + this.item_id + '&dashboard=' + this.dashboard_action, '550px', 'auto', true);
	}

	/**
	 * Reminders may be send from either the dashboard of the
	 * preview/manage page.
	 *
	 * @param response
	 */
	reminderDialogResult(response)
	{
		if (response.indexOf("OK") == 0 || response.indexOf("FAIL") == 0)
		{
			var responseFields = response.split("|");

			var msg = responseFields[1];
			this.showActionResult(msg, responseFields[0]);
			this.closeDialog();
		}
		else
		{
			document.getElementById('Reminder_form__error').innerHTML = response;
		}
	}


	showAdvancedFeaturesDialog()
	{
		this.dialog = modalPopup('Advanced Message Features', '/action/questionnaire/advanced_message_features_dialog', '450px', 'auto', true);
	}

	showMessageSelectDialog()
	{
		this.dialog = modalPopup('Messages', '/action/' + this.component_name + '/' + this.message_select_dialog, '450px', 'auto', true);
	}

	messageSelectResult(response)
	{
		this.closeDialog();

		if(response)
		{
			var elt = document.getElementsByName("message")[0];
			if(elt)
			{
				elt.value = response;
			}
		}
	}

	/**
	 * From survey_email form, user can automatically add
	 * either themselves or the sender email.
	 */
	addEmailToCCRecipients(email)
	{
		var elt;

		if(email == 'sender')
		{
			elt = document.getElementsByName('sender_email')[0];
			if(!elt) return;
			email = elt.value;
		}

		elt = document.getElementsByName('cc_recipients')[0];

		if(!elt) return;
		var new_value = elt.value;

		if(new_value)
		{
			new_value += ",";
		}
		new_value += email;

		elt.value = new_value;
	}

	showAdditionalRecipientsDialog()
	{
		this.dialog = modalPopup('Additional Recipients', '/action/' + this.component_name + '/' + this.recipients_dialog + '?' + this.itemPk + '=' + this.item_id + '&type=1', '530px', 'auto', true);
	}

	/**
	 * Called from preview/manage page manage buttons/links
	 */
	sendAdditionalEmails()
	{
		fetch('/action/' + this.component_name + '/' + this.send_additional_handler + '?' + this.itemPk + '=' + this.item_id)
			.then(r => r.text())
			.then(function(response)
			{
				if (response.indexOf("OK") == 0 || response.indexOf("FAIL") == 0)
				{
					var responseFields = response.split("|");
					var sent = responseFields[1];
					go(this.send_page_identifier + '?' + this.itemPk + '=' + this.item_id + '&sent=' + sent);
				}
				else
				{
					alert(response);
				}
			}.bind(this))
			.catch(function() { alert("Failed to communicate with server"); });
	}

	/**
	 * Called from manage links on preview page and from
	 * dashboard js manager
	 *
	 * @param action
	 */
	handleSendAction(action)
	{
		switch(action)
		{
			case 'send_test':
			this.showTestEmailDialog();
			break;

			case 'close':
			this.closeToRespondents();
			break;

			case 'reopen':
			this.openToRespondents();
			break;

			case 'send_reminders':
			this.showReminderDialog();
			break;

			case 'send':
			go(this.send_page_identifier + '?' + this.itemPk + '=' + this.item_id);
			break;

			case 'send_additional':
			this.sendAdditionalEmails();
			break;

			default:
			alert("Invalid action: " + action);
			break;
		}
	}

	closeDialog()
	{
		this.dialog.hide();
	}
}
