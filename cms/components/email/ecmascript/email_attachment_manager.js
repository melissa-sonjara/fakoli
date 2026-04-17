/**
 * Handles users adding uploaded files to an email form.
 */
class EmailAttachmentManager
{
	constructor(list, control, cssClass, deleteIcon)
	{
		this.dialog = null;
		this.uploadDialog = null;
		this.form = null;
		this.list = null;
		this.cssClass = "";
		this.deleteIcon = "";
		this.idTagName = null;
		this.classVarName = null;

		this.form = document.getElementById('attachmentForm');
		this.list = document.getElementById(list);
		this.control = document.getElementById(control);
		this.cssClass = cssClass;
		this.deleteIcon = deleteIcon;

		this.idTagName = "attachment";
		this.classVarName = "emailAttachmentMgr";

		//this.formSetup();
	}

	formSetup()
	{
		var attachmentDialog = document.getElementById("attachmentDialog");
		if (!attachmentDialog)
		{
			this.loadDialog(this.form);
		}
		else
		{
			this.configureForm(this.form);
			this.uploadDialog = new ModalDialog(document.getElementById("attachmentDialog"), {draggable: false, closeLink: 'closeAttachmentDialog'});
		}
	}

	loadDialog(form)
	{
		this.uploadDialog = new ModalDialog("attachmentDialog", {title: "Add Attachment", draggable: false, closeLink: 'closeAttachmentDialog'});
		this.uploadDialog.show(function() { this.configureForm(this.uploadDialog.options.body.querySelector('form')); }.bind(this), '/action/attachment/dialog');
	}

	configureForm(form)
	{
		if (typeof form === 'string')
			form = document.getElementById(form);

		form.iFrameFormRequest(
		{
			'onRequest': function() { this.postStart(); return true; }.bind(this),
			'onComplete': function(response) { this.postComplete(response); }.bind(this)
		});

		this.form = form;
	}

	showAddAttachmentDialog()
	{
		this.loadDialog();
	}

	postStart()
	{
		document.getElementById('attachmentDialogMessage').innerHTML = "Uploading file...";
	}

	postComplete(response)
	{
		var match = response.match(/^(.*?):(.*?):(.*?):(.*)$/);

		if (match)
		{
			var id = match[1];
			var name = match[2];
			var icon = match[3];
			var size = match[4];

			this.list.innerHTML += this.formatFilename(id, icon, name, name, size);

			this.setControlValue(id);

			this.uploadDialog.hide();
		}
		else
		{
			document.getElementById('attachmentDialogMessage').innerHTML = response;
		}
	}

	formatFilename(id, icon, name, display_name, size)
	{
		var html = "<li id='" + this.idTagName + '_' + id + "' class='" + this.cssClass + "'><span><img src='" + icon + "' alt='Icon' style='display: inline-block;vertical-align: middle'/>&nbsp;" +
		display_name + "&nbsp;(" + size + ")&nbsp;" +
		"<a href='#' onclick='" + this.classVarName + ".removeAttachment(\"" + name + "\", " + id + "); return false' title='Remove this Attachment'>" +
		"<img src='" + this.deleteIcon + "' style='display:inline-block; vertical-align: middle' alt='Remove this Attachment'/></a></span></li>";

		return html;
	}

	setControlValue(id)
	{
		if (this.control.value) this.control.value += ",";
		this.control.value += id;
	}

	/**
	 * Since email attachments are not saved, we only need to remove
	 * the element from the document and update the field that is
	 * read in $_POST.
	 *
	 * @param filename
	 * @param id
	 */
	removeAttachment(filename, id)
	{
		if (!confirm("Are you sure you want to remove " + filename + "?")) return;

		this.disposeElt(id);

		if (this.control.value)
		{
			var regexp = new RegExp("\\b" + id + "\\b");
			// remove the id from the field value
			this.control.value = this.control.value.replace(regexp, "");
			// clean up the commas
			this.control.value = this.control.value.replace(/,,/, ",");
			this.control.value = this.control.value.replace(/,$/, "");
			this.control.value = this.control.value.replace(/^,/, "");
		}
	}

	disposeElt(id)
	{
		var elt = document.getElementById(this.idTagName + '_' + id);
		if (elt)
		{
			elt.remove();
		}
	}

	closeDialog()
	{
		this.dialog.hide();
	}
} // end EmailAttachmentManager


/**
 * Handles users adding attachments from the document library or
 * to an email form.
 */
class EmailDocumentAttachmentManager extends EmailAttachmentManager
{
	constructor(list, control, cssClass, deleteIcon)
	{
		super(list, control, cssClass, deleteIcon);
		this.idTagName = "attachment";
		this.classVarName = "emailDocAttachmentMgr";
	}

	showDocumentSelectDialog()
	{
		this.dialog = modalPopup("Select Document from a Library", "/action/email/document_select_dialog", '600px', 'auto', true);
	}

	documentAttachmentResult(response)
	{
		if (response.indexOf("OK") == 0)
		{
			var responseFields = response.split("|");
			var id = responseFields[1];
			var library_name = responseFields[2];
			var name = responseFields[3];
			var file_name = responseFields[4];
			var icon = responseFields[5];
			var size = responseFields[6];
			var display_name = library_name + "&nbsp;" + name + "<br>" + file_name;

			this.list.innerHTML += this.formatFilename(id, icon, name, display_name, size);
			this.setControlValue(id);

			this.closeDialog();
		}
		else
		{
			document.getElementById("DocumentSelect_form__error").innerHTML = response;
		}
	}
}
