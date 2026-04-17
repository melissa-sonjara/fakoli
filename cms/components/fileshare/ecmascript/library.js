var LibraryManager = (function()
{
	class LibraryManagerSingleton
	{
		constructor()
		{
			this.dialog = null;
			this.listBox = null;
		}

		editDocumentDetailsDialog(document_id)
		{
			this.dialog = modalPopup('Edit Document Details', '/action/fileshare/edit?document_id=' + document_id, '550px', 'auto', true);
		}

		editDocumentDetailsResult(response)
		{
			if (response == "OK")
				window.location.reload();
			else
			{
				var err = document.getElementById('DocumentDetails_form__error');
				err.innerHTML = response;
				err.style['display'] = 'table-cell';
			}
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
						if (response == "1")
						{
							window.location.reload();
						}
					})
					.catch(function() { alert("Failed to communicate with server"); });
			}
		}

		uploadFileshareFile(document_library_id)
		{
			this.dialog = modalPopup('Upload File', '/action/fileshare/upload?document_library_id=' + document_library_id, '550px', 'auto', true);
		}

		uploadFile(document_library_id)
		{
			this.dialog = modalPopup('Upload File', '/action/document/upload?document_library_id=' + document_library_id, '550px', 'auto', true);
		}

		openFileShareDialog(document_library_id)
		{
			var title;
			if (document_library_id)
				title = "Modify FileShare Library";
			else
				title = "Create a FileShare Library";

			this.dialog = modalPopup(title, '/action/fileshare/fileshare_form?document_library_id=' + document_library_id, '550px', 'auto', true);
		}

		saveFileShare(response)
		{
			if (response == "OK")
				window.location.reload();
			else
			{
				var err = document.getElementById('Fileshare_form__error');
				err.innerHTML = response;
				err.style['display'] = 'table-cell';
			}
		}

		closeFileShareDialog()
		{
			this.dialog.hide();
		}


		/*
		 * When the user clicks the x in the user name div,
		 * remove the user from the member scrollbox
		 */
		removeMember(document_library_id, user_id)
		{
			fetch("/action/fileshare/remove_member?document_library_id=" + document_library_id + "&user_id=" + user_id)
				.then(r => r.text())
				.then(function(response)
				{
					var box = document.getElementById('member_scrollbox');

					Array.from(box.children).forEach(function(member)
					{
						if (member.id == "user_id_" + user_id)
						{
							member.style['opacity'] = 0;
							setTimeout(function() { member.remove(); }, 500);
						}
					});
				}.bind(this));
		}


		/*
		 * Hide the progressive search list box after an item
		 * is selected. Remove the search text from the input box.
		 */
		closeProgressiveSearch()
		{
			var list = document.getElementById('LibraryGroupMembers_form_name_progressive_search');
			Object.assign(list.style, {'display': 'none'});
			var input = document.getElementById('LibraryGroupMembers_form_name');
			if (input)
			{
				input.value = '';
			}
		}


		memberProgressiveSearch(document_library_id)
		{
			var field = 'LibraryGroupMembers_form_name';
			this.listBox = new ProgressiveSearch(field, {'search': '/action/fileshare/user_search_handler?document_library_id=' + document_library_id + '&field=' + field, width: '450px', minimumLength: 3, 'cssClass': 'scrollbox'});
		}

		keywordProgressiveSearch(document_library_id)
		{
			var field = 'LibrarySearch_form_keyword';
			this.listBox = new ProgressiveSearch(field, {'search': '/action/fileshare/keyword_search_handler?document_library_id=' + document_library_id + '&field=' + field, 'cssClass': 'scrollbox'});
		}

		addMemberFromProgressiveSearch(document_library_id, user_id)
		{
			fetch("/action/fileshare/add_member?document_library_id=" + document_library_id + "&user_id=" + user_id)
				.then(r => r.text())
				.then(function(response)
				{
					var box = document.getElementById('member_scrollbox');

					var divId = "user_id_" + user_id;
					var div = document.createElement('div');
					div.id = divId;
					div.className = '';
					div.innerHTML = response;
					box.appendChild(div);
					this.closeProgressiveSearch();
				}.bind(this));
		}

		openPermissionsDialog(document_library_id)
		{
			this.dialog = modalPopup('Fileshare Access', '/action/fileshare/global_permissions_form?document_library_id=' + document_library_id, '550px', 'auto', true);
		}

		permissionsEdited(response)
		{
			if (response == "OK")
				this.closeDialog();
			else
			{
				var err = document.getElementById('RolePermission_form__error');
				err.innerHTML = response;
				err.style['display'] = 'table-cell';
			}
		}

		closeDialog()
		{
			this.dialog.hide();
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new LibraryManagerSingleton();
	};

})();
