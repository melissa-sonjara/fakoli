/**
 * QuestionnaireSend contains the methods for managing
 * surveys that request responses via email.
 * 
 */
var QuestionnaireSendManager =  new Class 
({
	dialog: null, 
	
	itemPk: null,		///< the id of the questionnaire or survey table (e.g., questionnaire_id)
	
	component_name: null,	///< the name of the component (e.g., questionnaire)
	///< some component may have more than one questionnaire so item class is needed

	item_label: null, 		///< the item pretty class name or label for delete confirmation

	send_test_email_dialog: null, 	///< handler that lets users specify test email recipients
	
	send_test_email_handler: null, 	///< handler that sends one test to user
	
	open_handler: null,				///< handler that opens survey to respondents
	
	close_handler: null,			///< handler that closes survey to respondents
	
	reminder_dialog: null,			///< the handler that sends reminders to nonresponders
	
	send_page_identifier: null,		///< the page to preview/send survey requests
	
	recipients_dialog: null,		
	
	initialize: function(itemPk, component_name,
			item_label,
			send_test_email_dialog,
			send_test_email_handler,
			open_handler,
			close_handler,
			reminder_dialog,
			send_page_identifier,
			recipients_dialog
			)
	{
		
		this.itemPk = itemPk;
		this.component_name = component_name;
		this.send_test_email_dialog = send_test_email_dialog;
		this.send_test_email_handler = send_test_email_handler;
		this.open_handler = open_handler;
		this.close_handler = close_handler;
		this.reminder_dialog = reminder_dialog;
		this.send_page_identifier = send_page_identifier;
		this.recipients_dialog = recipients_dialog;
		
	},
	
	openToRespondents: function(item_id)
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.open_handler + '?' + this.itemPk + '=' + item_id, 
			'method': 	'get',
			'onSuccess': function(response) 
			{ 
				if (response == "OK") 
				{
					window.location.reload();
				}
			},
								
			'onFailure':	function() { alert("Failed to communicate with server");}
		});
	},
	
	closeToRespondents: function(item_id)
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.close_handler + '?' + this.itemPk + '=' + item_id, 
			'method': 	'get',
			'onSuccess': function(response) 
			{ 
				if (response == "OK") 
				{
					window.location.reload();
				}
			},
										
			'onFailure':	function() { alert("Failed to communicate with server");}
		});
	},
			
	sendTestEmail: function(item_id)
	{
		var request = new Request(
		{
			'url': 		'/action/' + this.component_name + '/' + this.send_test_email_handler + '?' + this.itemPk + '=' + item_id, 
			'method': 	'get',
			'onSuccess': function(response) 
			{ 
			  if(response == "OK" || response == "FAIL")
			    {
			     this.closeDialog();
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
	showTestEmailDialog: function(item_id)
	{
		this.dialog = modalPopup('Send Test Email', '/action/' + this.component_name + '/' + this.send_test_email_dialog + '?' + this.itemPk + '=' + item_id, '530px', 'auto', true);		
	},

	testEmailResult: function(response)
	{
		if(response == "OK" || response == "FAIL")
		{
			this.closeDialog();
		}
		else
		{
			$('SendTestEmail_form__error').set('html', response);
		}
	},
	
	showRecipientsDialog: function(item_id)
	{
		this.dialog = modalPopup('Survey Recipients', '/action/' + this.component_name + '/' + this.recipients_dialog + '?' + this.itemPk + '=' + item_id, '530px', 'auto', true);			
	},

	showReminderDialog: function(item_id)
	{
		this.dialog = modalPopup('Send Reminders', '/action/' + this.component_name + '/' + this.reminder_dialog + '?' + this.itemPk + '=' + item_id, '550px', 'auto', true);
	},

	reminderDialogResult: function(response)
	{			
		if(response == "OK" || response == "FAIL")
		{
			this.closeDialog();
		}
		else
		{
			$('Reminder_form__error').set('html', response);
		}
	},
	
	
	showAdvancedFeaturesDialog: function()
	{
		this.dialog = modalPopup('Advanced Message Features', '/action/questionnaire/advanced_message_features_dialog', '450px', 'auto', true);
	},	
	
	
	closeDialog: function()
	{
		this.dialog.hide();
	}
});