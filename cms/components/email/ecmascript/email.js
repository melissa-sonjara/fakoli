var EmailManager = (function()
{
	class EmailManagerSingleton
	{
		constructor()
		{
			this.dialog = null;
		}

		sendingClassListDialog(response)
		{

		}

		closeClassListDialog()
		{
			this.dialog.hide();
		}

		openClassListDialog()
		{
			this.dialog = modalPopup('Select Sending Class', '/action/email/auto_create_class_list', '450px', 'auto', true);
		}

		createMergeCode(relation, field)
		{
			var map = document.getElementById('MergeCode_form_map');
			var name = document.getElementById('MergeCode_form_name');
			var description = document.getElementById('MergeCode_form_description');

			if (map)
				map.value = relation + '.' + field;

			if (name)
				name.value = relation.toLowerCase() + '_' + field;

			if (description)
				description.textContent = '';

			this.closeMergeCodeDialog();
		}

		closeMergeCodeDialog()
		{
			this.dialog.hide();
		}

		openMergeCodeDialog(class_name)
		{
			this.dialog = modalPopup('View Related Values and Create Merge Code', '/action/email/auto_create_merge_code?class_name=' + class_name, '400px', 'auto', true);
		}

		showAdvancedFeatures()
		{
			var class_name = document.getElementById('EmailTemplate_form_class_name').value;
			this.dialog = floatingPopup('advanced_feature_popup', 'Advanced Features', '/action/email/advanced_email_features?class_name=' + class_name, '500px', 'auto', true, true);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * *
		 *
		 *           Mail To form
		 *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * */

		sendMailTo(response)
		{
			if (response == "OK")
			{
				this.closeMailToDialog();
				notification("Your message has been sent.");
			}
			else
			{
				var err = document.getElementById('MailTo_form__error');
				err.innerHTML = response;
				err.style['display'] = 'table-cell';
			}
		}

		closeMailToDialog()
		{
			this.dialog.hide();
		}

		contactUsResult(response)
		{
			if (response == "OK")
			{
				this.closeDialog();
				notification("Your message has been sent");
			}
			else
			{
				var err = document.getElementById('ContactUs_form__error');
				err.innerHTML = response;
				err.style['display'] = 'table-cell';
			}
		}

		showContactUsDialog()
		{
			this.dialog = modalPopup('Contact Us', '/action/email/contact_us_dialog', '600px', 'auto', true);
		}

		mailto(to, subject, message)
		{
			if (!to)
				to = "";
			if (!subject)
				subject = "";
			if (!message)
				message = "";
			this.dialog = modalPopup('Send an Email', '/action/email/mail_to?to=' + to + "&subject=" + subject + "&message=" + message, '520px', 'auto', true);
		}

		showMessageLog(email_log_id)
		{
			modalPopup("Message Details", "/action/email/email_log_message?email_log_id=" + email_log_id, '800px', 'auto', false);
		}

		closeDialog()
		{
			if (this.popup)
				this.popup.hide();
			else if (this.dialog)
				this.dialog.hide();
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new EmailManagerSingleton();
	};

})();
