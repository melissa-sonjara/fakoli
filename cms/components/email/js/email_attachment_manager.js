/**
 * Handles users adding uploaded files to an email form.
 */
var EmailAttachmentManager =  new Class
({
	dialog: null,	
	uploadDialog: Class.Empty,
	form: Class.Empty,
	list: Class.Empty,
	cssClass: "",
	deleteIcon: "",
	idTagName: null,
	classVarName: null,
	
	initialize: function(list, control, cssClass, deleteIcon)
	{
		this.form = document.id('attachmentForm');
		this.list = document.id(list);
		this.control = document.id(control);
		this.cssClass = cssClass;
		this.deleteIcon = deleteIcon;
		
		this.idTagName = "attachment";
		this.classVarName = "emailAttachmentMgr";
	
		// empty for document library
		if(this.form)
		{
			this.formSetup();
		}
	},
	
	formSetup: function()
	{
		this.form.iFrameFormRequest(
		{
			'onRequest': function() { this.postStart(); return true; }.bind(this),
			'onComplete': function(response) { this.postComplete(response);}.bind(this)
		});	
		
		this.uploadDialog = new ModalDialog(document.id("attachmentDialog"), {draggable: false, closeLink: 'closeAttachmentDialog'});	
	},
	
	showAddAttachmentDialog: function()
	{
		this.form.reset();
		document.id('attachmentDialogMessage').innerHTML = "";
		this.uploadDialog.show();
	},
	
	postStart: function()
	{
		document.id('attachmentDialogMessage').innerHTML = "Uploading file...";
	},
	
	postComplete: function(response)
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
			document.id('attachmentDialogMessage').innerHTML = response;
		}
	},
	
	formatFilename: function(id, icon, name, display_name, size)
	{
		var html = "<li id='" + this.idTagName + '_' + id + "' class='" + this.cssClass + "'><span><img src='" + icon + "' alt='Icon' style='display: inline-block;vertical-align: middle'/>&nbsp;" +
		display_name + "&nbsp;(" + size + ")&nbsp;" +
		"<a href='#' onclick='" + this.classVarName + ".removeAttachment(\"" + name + "\", " + id + "); return false' title='Remove this Attachment'>" +
		"<img src='" + this.deleteIcon + "' style='display:inline-block; vertical-align: middle' alt='Remove this Attachment'/></a></span></li>";
	
		return html;
	},
	
	setControlValue: function(id)
	{
		if (this.control.value) this.control.value += ",";
		this.control.value += id;	
	},
	
	/**
	 * Since email attachments are not saved, we only need to remove
	 * the element from the document and update the field that is 
	 * read in $_POST.
	 * 
	 * @param filename
	 * @param id
	 */
	removeAttachment: function(filename, id)
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
	},	
	
	disposeElt: function(id)
	{
		var elt = $(this.idTagName + '_' + id);
		if(elt)
		{
			elt.dispose();
		}	
	},
	
	closeDialog: function()
	{
		this.dialog.hide();
	}
}); // end EmailAttachmentManager


/**
 * Handles users adding attachments from the document library or
 * to an email form.
 */
var EmailDocumentAttachmentManager =  new Class
({
	Extends: EmailAttachmentManager,
	
	initialize: function(list, control, cssClass, deleteIcon)
	{
		this.parent(list, control, cssClass, deleteIcon);
		this.idTagName = "attachment";	
		this.classVarName = "emailDocAttachmentMgr";
	},
	
	showDocumentSelectDialog: function()
	{
		this.dialog = modalPopup("Select Document from a Library", "/action/email/document_select_dialog", '600px', 'auto', true);
	},

	documentAttachmentResult: function(response)
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
			$("DocumentSelect_form__error").set('html', response);
		}		
	},
});