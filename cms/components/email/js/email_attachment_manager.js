/**
 * Handles users adding attachments from the document library to an
 * email form.
 */
var EmailAttachmentManager =  new Class
({
	dialog: null,	

	
	initialize: function(list, control, cssClass, deleteIcon)
	{
		this.list = document.id(list);
		this.control = document.id(control);
		this.cssClass = cssClass;
		this.deleteIcon = deleteIcon;
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
			
			this.list.innerHTML += 
				"<li id='document_" +id + "' class='" + this.cssClass + "'><span><img src='" + icon + "' alt='Icon' style='display: inline-block;vertical-align: middle'/>&nbsp;" +
				library_name + "&nbsp;" + name + "<br>" + file_name + "&nbsp;(" + size + ")&nbsp;" +
				"<a href='#' onclick='emailAttachmentMgr.removeAttachment(\"" + name + "\", " + id + "); return false' title='Remove this Attachment'>" +
				"<img src='" + this.deleteIcon + "' style='display:inline-block; vertical-align: middle' alt='Remove this Attachment'/></a></span></li>";
			
			if (this.control.value) this.control.value += ",";
			this.control.value += id;
			
			this.closeDialog();
		}
		else
		{
			$("DocumentSelect_form__error").set('html', response);
		}		
	},
	
	/**
	 * Since email attachments are not saved, we only need to remove
	 * the element from the document.
	 * 
	 * @param filename
	 * @param id
	 */
	removeAttachment: function(filename, id)
	{
		if (!confirm("Are you sure you want to remove " + filename + "?")) return;
		
		var elt = $('document_' + id);
		if(elt)
		{
			elt.dispose();
		}
	},	
	
	closeDialog: function()
	{
		this.dialog.hide();
	},
});