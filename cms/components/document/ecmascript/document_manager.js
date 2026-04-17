/**
 * Handles document library interface.
 */
class DocumentManager
{
	constructor(document_library_id)
	{
		this.dialog = null;
		this.document_library_id = document_library_id;
	}

	editDocumentDetailsDialog(document_id)
	{
		this.dialog = modalPopup('Edit Document Details', '/action/document/edit?document_id=' + document_id, '900px', 'auto', true);
	}

	editResult(result)
	{
		// Deal with browser extension that inject embed tabs, such as Nok Nok MFAC
		result = result.replace(/<embed[^>]*>.*?<\/embed>/gi, '').replace(/<embed[^>]*\/>/gi, '');
		if (result == "1")
		{
			this.closeDialog();
			window.location.reload();
		}
		else
		{
			document.getElementById('Document_form__error').textContent = result;
		}
	}

	showDocumentDetailsDialog(document_id)
	{
		this.dialog = modalPopup('Document Details', '/action/document/document_details_dialog?document_id=' + document_id, '900px', 'auto', true);
	}

	closeEditDocumentDetails()
	{
		this.dialog.hide();
	}

	deleteFile(document_id)
	{
		if (confirm("Are you sure you want to delete this file?"))
		{
			fetch('/action/document/delete?document_id=' + document_id)
				.then(r => r.text())
				.then(function(response)
				{
					// Deal with browser extension that inject embed tabs, such as Nok Nok MFAC
					response = response.replace(/<embed[^>]*>.*?<\/embed>/gi, '').replace(/<embed[^>]*\/>/gi, '');
					if (response == "1")
					{
						window.location.reload();
					}
				})
				.catch(function() { alert("Failed to communicate with server"); });
		}
	}

	uploadFile()
	{
		this.dialog = modalPopup('Upload File', '/action/document/upload?document_library_id=' + this.document_library_id + '&folder_select=0', '900px', 'auto', true);
	}

	uploadResult(responseText)
	{
		// Deal with browser extension that inject embed tabs, such as Nok Nok MFAC
		responseText = responseText.replace(/<embed[^>]*>.*?<\/embed>/gi, '').replace(/<embed[^>]*\/>/gi, '');
		if (responseText == "1")
		{
			this.closeDialog();
			window.location.reload();
		}
		else
		{
			alert(responseText);
		}
	}

	rescanLibrary()
	{
		fetch('/action/document/rescan?document_library_id=' + this.document_library_id)
			.then(r => r.text())
			.then(function(response)
			{
				// Deal with browser extension that inject embed tabs, such as Nok Nok MFAC
				response = response.replace(/<embed[^>]*>.*?<\/embed>/gi, '').replace(/<embed[^>]*\/>/gi, '');
				if (response == "OK")
				{
					window.location.reload();
				}
				else
				{
					alert(response);
				}
			})
			.catch(function() { alert("Failed to communicate with server"); });
	}

	closeDialog()
	{
		this.dialog.hide();
	}
}
