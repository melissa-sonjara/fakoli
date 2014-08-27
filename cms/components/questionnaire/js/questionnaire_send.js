/**
 * QuestionnaireSend contains the methods for managing
 * surveys that request responses via email.
 * 
 */
var QuestionnaireSendManager =  new Class 
({
	dialog: null, 
	
	itemPk: null,		///< the id of the questionnaire or survey table (e.g., questionnaire_id)
	item_id: null, 		///< the id value of the questionnaire class being created/updated
	
	component_name: null,	///< the name of the component (e.g., questionnaire)
	///< some component may have more than one questionnaire so item class is needed

	item_label: null, 		///< the item pretty class name or label for delete confirmation

	send_test_email_dialog: null, 	///< handler that lets users specify test email recipients
	
	send_test_email_handler: null, 	///< handler that sends one test to user
	
	send_additional_handler: null,	///< handler to send additional emails
	
	open_handler: null,				///< handler that opens survey to respondents
	
	close_handler: null,			///< handler that closes survey to respondents
	
	reminder_dialog: null,			///< the handler that sends reminders to nonresponders
	
	send_page_identifier: null,		///< the page to preview/send survey requests
	
	recipients_dialog: null,		
	
	message_select_dialog: null,		
	
	info_msg: null, 				///< whether email messages were sent.
		
	dashboard_action: 0,		///< whether we are calling from the dashboard
		
	initialize: function(itemPk, item_id,
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
		
		this.itemPk = itemPk;
		this.item_id = item_id;
		this.component_name = component_name;
		this.send_test_email_dialog = send_test_email_dialog;
		this.send_test_email_handler = send_test_email_handler;
		this.send_additional_handler = send_additional_handler;
		this.open_handler = open_handler;
		this.close_handler = close_handler;
		this.reminder_dialog = reminder_dialog;
		this.send_page_identifier = send_page_identifier;
		this.recipients_dialog = recipients_dialog;
		this.message_select_dialog = message_select_dialog;
			
		if(info_msg != '')
		{
			this.setInfoMsg(info_msg);
		}
	},
	
	/**
	 * validation msg on the survey preview page at the top of the
	 * page
	 */
	setValidationMsg: function(msg)
	{
		if(!msg) return;
		
		var div = document.id('questionnaire_heading');
		if(!div) return;
		var elt = new Element('div', {'id': 'warning'}); 
		
		elt.set("html", msg);
		elt.inject(div, "bottom");
	},
	
	/**
	 * msg on top of preview/manage page
	 */
	setInfoMsg: function(msg)
	{
		if(!msg) return;
		
		var elt = document.id('action_result');
		if(!elt) return;
		
		var success = "OK";
		if(msg.match(/ not /))
		{
			success = "FAIL";
		}
		
		this.showActionResult(msg, success);
	},	
	
	openToRespondents: function()
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.open_handler + '?' + this.itemPk + '=' + this.item_id, 
			'method': 	'get',
			'onSuccess': function(response) 
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
			}.bind(this),
								
			'onFailure':	function() { alert("Failed to communicate with server");}
		});
		
		request.send();	
	},
	
	closeToRespondents: function()
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.close_handler + '?' + this.itemPk + '=' + this.item_id, 
			'method': 	'get',
			'onSuccess': function(response) 
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
			}.bind(this),
										
			'onFailure':	function() { alert("Failed to communicate with server");}
		});
		
		request.send();	
	},
	
	/**
	 * When user closes survey from the dashboard, update the status in the
	 * data list view and change the action drop down from reopen to close
	 * or close to reopen.
	 * 
	 * @param status: "Closed" or "Open"
	 */
	updateStatus: function(status)
	{
		var elt = document.id('status_' + this.item_id);
		if(!elt) return;
		
		elt.set("html", status);
		
		elt = document.id('action_' + this.item_id);
		if(!elt) return;
			
		elt.getChildren("option").each(function(option)
		{
			var html = option.get("html");
			if(option.get("value") == "reopen")
			{
				option.set("value", "close");
				option.set("html", html.replace(/ReOpen/, "Close"));
			}
			else if(option.get("value") == "close")
			{
				option.set("value", "reopen");
				option.set("html", html.replace(/Close/, "ReOpen"));
			}
		});	
	},
			
	/**
	 * Called from preview/manage page or survey response intro tester
	 * block for logged in users.
	 * 
	 * Result message is set in elt with id 'action_result'.
	 */
	sendTestEmail: function()
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.send_test_email_handler + '?' + this.itemPk + '=' + this.item_id, 
			'method': 	'get',
			'onSuccess': function(response) 
			{ 
			if (response.indexOf("OK") == 0 || response.indexOf("FAIL") == 0)
			{			
				var responseFields = response.split("|");	
				
				var elt = document.id('action_result');
				if(!elt) return;
			    var msg = responseFields[1];
				this.showActionResult(msg, responseFields[0]);
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
	 * Can be called from survey_dashboard or from survey_preview/
	 * manage tab
	 */
	showTestEmailDialog: function()
	{
		this.dialog = modalPopup('Send Test Email', '/action/' + this.component_name + '/' + this.send_test_email_dialog + '?' + this.itemPk + '=' + this.item_id + '&dashboard=' + this.dashboard_action, '530px', 'auto', true);		
	},

	testEmailResult: function(response)
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
			document.id('SendTestEmail_form__error').set('html', response);
		}
	},
	
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
	showActionResult: function(msg, success)
	{
		var elt = "";
		if(!this.dashboard_action)
		{
			elt = document.id('action_result');
		}
		else
		{
			elt = document.id('action_result_' + this.item_id);	
		}
		
		if(!elt) return;
		elt.set("html", msg);
		elt.setStyle("display", "");
		elt.setStyle("color", "");
		
		if(success == "FAIL")
		{
			elt.setStyle("color", "red");
		}
	},
	
	showRecipientsDialog: function()
	{
		this.dialog = modalPopup('Survey Recipients', '/action/' + this.component_name + '/' + this.recipients_dialog + '?' + this.itemPk + '=' + this.item_id, '530px', 'auto', true);			
	},

	showReminderDialog: function()
	{
		this.dialog = modalPopup('Send Reminders', '/action/' + this.component_name + '/' + this.reminder_dialog + '?' + this.itemPk + '=' + this.item_id + '&dashboard=' + this.dashboard_action, '550px', 'auto', true);
	},

	/**
	 * Reminders may be send from either the dashboard of the
	 * preview/manage page.
	 * 
	 * @param response
	 */
	reminderDialogResult: function(response)
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
			document.id('Reminder_form__error').set('html', response);
		}
	},
	
	
	showAdvancedFeaturesDialog: function()
	{
		this.dialog = modalPopup('Advanced Message Features', '/action/questionnaire/advanced_message_features_dialog', '450px', 'auto', true);
	},	
	
	showMessageSelectDialog: function()
	{
		this.dialog = modalPopup('Messages', '/action/' + this.component_name + '/' + this.message_select_dialog, '450px', 'auto', true);		
	},
	
	messageSelectResult: function(response)
	{
		this.closeDialog();
		
		if(response)	
		{
			var elt = $$(document.getElementsByName("message"))[0];
			if(elt)
			{
				elt.set("value", response);
			}
		}
	},	
	
	/**
	 * From survey_email form, user can automatically add
	 * either themselves or the sender email.
	 */
	addEmailToCCRecipients: function(email)
	{
		var elt;
		
		if(email == 'sender')
		{
			var elt = $$(document.getElementsByName('sender_email'))[0];
			if(!elt) return;
			email = elt.get("value");
		}
	
		var elt = $$(document.getElementsByName('cc_recipients'))[0];
		
		if(!elt) return;
		var new_value = elt.get("value");
		
		if(new_value)
		{
			new_value += ",";
		}
		new_value += email;
		
		elt.set("value", new_value);
	},
		
	showAdditionalRecipientsDialog: function()
	{
		this.dialog = modalPopup('Additional Recipients', '/action/' + this.component_name + '/' + this.recipients_dialog + '?' + this.itemPk + '=' + this.item_id + '&type=1', '530px', 'auto', true);				
	},
	
	/**
	 * Called from preview/manage page manage buttons/links
	 */
	sendAdditionalEmails: function()
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.send_additional_handler + '?' + this.itemPk + '=' + this.item_id, 
			'method': 	'get',
			'onSuccess': function(response) 
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
			}.bind(this),
												
			'onFailure':	function() { alert("Failed to communicate with server");}
		});
				
		request.send();	
	},
	
	/**
	 * Called from manage links on preview page and from 
	 * dashboard js manager
	 * 
	 * @param action
	 */
	handleSendAction: function(action)
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
	},
	
	closeDialog: function()
	{
		this.dialog.hide();
	}
});