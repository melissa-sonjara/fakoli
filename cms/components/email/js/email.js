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
			var map = $('MergeCode_form_map');
			var name = $('MergeCode_form_name');
			var description = $('MergeCode_form_description');
			
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
			var class_name = $('EmailTemplate_form_class_name').value;
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
				var err = $('MailTo_form__error');
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
				var err = $('ContactOrganizer_form__error');
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
				var err = $('ContactUs_form__error');
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