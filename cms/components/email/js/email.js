var EmailManager =  (function()
{
	var EmailManagerSingleton = new Class(
	{
		dialog: null,
			
		initialize: function()
		{
		},
		
		sendingClassListDialog: function(response)
		{
			
		},
		
		closeClassListDialog: function()
		{
			this.dialog.hide();
		},
		
		openClassListDialog: function()
		{
			this.dialog = modalPopup('Select Sending Class', '/action/email/auto_create_class_list', '450px', 'auto', true);
		},
			
		createMergeCode: function(relation, field)
		{
			var map = document.id('MergeCode_form_map');
			var name = document.id('MergeCode_form_name');
			var description = document.id('MergeCode_form_description');
			
			if(map)
				map.value = relation + '.' + field;
			
			if(name)
				name.value = relation.toLowerCase() + '_' + field; 
				
			if(description)
				description.set('text', '');
			
			this.closeMergeCodeDialog();
		},
		
		closeMergeCodeDialog: function()
		{
			this.dialog.hide();
		},
		
		
		openMergeCodeDialog: function(class_name)
		{
			this.dialog = modalPopup('View Related Values and Create Merge Code', '/action/email/auto_create_merge_code?class_name=' + class_name, '400px', 'auto', true);
		},
		
		showAdvancedFeatures: function()
		{
			var class_name = document.id('EmailTemplate_form_class_name').value;
			this.dialog = floatingPopup('advanced_feature_popup', 'Advanced Features', '/action/email/advanced_email_features?class_name=' + class_name, '500px', 'auto', true, true);
		},
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * 
		 *           Mail To form
		 *           
		 * * * * * * * * * * * * * * * * * * * * * * * * * * */         
		
		sendMailTo: function(response)
		{
			if (response == "OK")
			{
				this.closeMailToDialog();
				notification("Your message has been sent.");
			}
			else
			{
				var err = document.id('MailTo_form__error');
				err.set('html', response);
				err.setStyle('display', 'table-cell');
			}
		},
		
		closeMailToDialog: function()
		{
			this.dialog.hide();
		},
		
		contactUsResult: function(response)
		{
			if (response == "OK")
			{
				this.closeDialog();
				notification("Your message has been sent");
			}
			else
			{
				var err = document.id('ContactOrganizer_form__error');
				err.set('html', response);
				err.setStyle('display', 'table-cell');
			}
		},	
		
		showContactUsDialog: function()
		{
			this.dialog = modalPopup('Contact Us', '/action/email/contact_us_dialog', '600px', 'auto', true);		
		},
		
	
		contactUsResult: function(response)
		{
			if (response == "OK")
			{
				this.closeDialog();
				notification("Your message has been sent");
			}
			else
			{
				var err = document.id('ContactUs_form__error');
				err.set('html', response);
				err.setStyle('display', 'table-cell');
			}
		},	
	
		mailto: function(to, subject, message)
		{
			if(!to)
				to = "";
			if(!subject)
				subject = "";
			if(!message)
				message = "";
			this.dialog = modalPopup('Send an Email', '/action/email/mail_to?to=' + to + "&subject=" + subject + "&message=" + message, '520px', 'auto', true);
		},
		
		showMessageLog: function(email_log_id)
		{
			modalPopup("Message Details", "/action/email/email_log_message?email_log_id=" + email_log_id, '800px', 'auto', false);
		},
		
		closeDialog: function()
		{
			if(this.popup)
				this.popup.hide();
			else if(this.dialog)
				this.dialog.hide();
		}
	
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new EmailManagerSingleton();
	};
	
})();