/**
 * Handles document library interface.
 */
var DocumentManager =  new Class
({
	dialog: null,
	document_library_id: null,
	
	initialize: function(document_library_id)
	{
		this.document_library_id = document_library_id;
	},
	
	editDocumentDetailsDialog: function(document_id)
	{
		this.dialog = modalPopup('Edit Document Details', '/action/document/edit?document_id=' + document_id, '900px', 'auto', true);
	},
	
	editResult: function(result)
	{
		if (result == "1")
		{
			this.closeDialog();
			window.location.reload();
		}
		else
		{
			document.id('Document_form__error').set({'text': result, 'display': 'table-cell'});
		}
	},
	
	showDocumentDetailsDialog: function(document_id)
	{
		this.dialog = modalPopup('Document Details', '/action/document/document_details_dialog?document_id=' + document_id, '900px', 'auto', true);		
	},
	
	closeEditDocumentDetails: function()
	{
		this.dialog.hide();		
	},
	
	deleteFile: function(document_id)
	{
		if (confirm("Are you sure you want to delete this file?"))
		{
			
			var request = new Request(
					{'url': 		'/action/document/delete?document_id=' + document_id, 
					  'method': 	'get',
					 'onSuccess': function(response) 
				{ 
				if (response == "1") 
				{
					window.location.reload();
				}
			 },
			
			 'onFailure':	function() { alert("Failed to communicate with server");}});
			
			request.send();				
		}
		
	},
	
	uploadFile: function()
	{
		this.dialog = modalPopup('Upload File', '/action/document/upload?document_library_id=' + this.document_library_id + '&folder_select=0', '900px', 'auto', true);
	},
	
	uploadResult: function(responseText)
	{
		if (responseText == "1") 
		{
			this.closeDialog();
			window.location.reload();
		}
		else
		{
			alert(responseText);
		}	
	},

	rescanLibrary: function()
	{
		var request = new Request(
		{
			'url': 		'/action/document/rescan?document_library_id=' + this.document_library_id, 
			'method': 	'get',
			'onSuccess': function(response) 
			{ 
			if (response == "OK") 
			{
				window.location.reload();
			}
			else
			{
				alert(result);
			}	
		 },
		
		 'onFailure':	function() { alert("Failed to communicate with server");}});
		
		request.send();
	},
	
	closeDialog: function()
	{
		this.dialog.hide();
	}

});