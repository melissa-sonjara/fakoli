var LibraryManager =  (function()
{
	var LibraryManagerSingleton = new Class(
	{
		dialog: null,
		listBox: null,
			
		initialize: function()
		{
		},
		
		editDocumentDetailsDialog: function(document_id)
		{
			this.dialog = modalPopup('Edit Document Details', '/action/fileshare/edit?document_id=' + document_id, '550px', 'auto', true);
		},
		
		editDocumentDetailsResult: function(response)
		{
			if(response == "OK")
				window.location.reload();
			else
			{
				var err = $('DocumentDetails_form__error');
				err.set('html', response);
				err.setStyle('display', 'table-cell');
			}		
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
		
		uploadFileshareFile: function(document_library_id)
		{
			this.dialog = modalPopup('Upload File', '/action/fileshare/upload?document_library_id=' + document_library_id, '550px', 'auto', true);
		},
		
		uploadFile: function(document_library_id)
		{
			this.dialog = modalPopup('Upload File', '/action/document/upload?document_library_id=' + document_library_id, '550px', 'auto', true);
		},
		
		openFileShareDialog: function(document_library_id)
		{
			var title;
			if(document_library_id)
				title = "Modify FileShare Library";
			else
				title = "Create a FileShare Library";
					
			this.dialog = modalPopup(title, '/action/fileshare/fileshare_form?document_library_id=' + document_library_id, '550px', 'auto', true);
		},
		
		saveFileShare: function(response)
		{
			if(response == "OK")
				window.location.reload();
			else
			{
				var err = $('Fileshare_form__error');
				err.set('html', response);
				err.setStyle('display', 'table-cell');
			}
		},
		
		closeFileShareDialog: function()
		{
			this.dialog.hide();
		},
			

		
		/*
		 * When the user clicks the x in the user name div,
		 * remove the user from the member scrollbox
		 */
		removeMember: function(document_library_id, user_id)
		{
			var request = new Request({
				url: "/action/fileshare/remove_member?document_library_id=" + document_library_id + "&user_id=" + user_id,
				method: 'get',
				
				onSuccess: function(response) 
				{ 
					var box = $('member_scrollbox');

					var children = box.getChildren();
								
					children.each(function(member)
					{
						if(member.id == "user_id_" + user_id)
						{
							member.fade('out');
							(function(){ member.destroy(); }).delay(500);
						}
					});
				
				}.bind(this)
			});
			request.send();	
		},

		
		/*
		 * Hide the progressive search list box after an item
		 * is selected. Remove the search text from the input box. 
		 */
		closeProgressiveSearch: function()
		{
			var list = $('LibraryGroupMembers_form_name_progressive_search');
			list.setStyles({'display': 'none'});	
			var input = $('LibraryGroupMembers_form_name');
			if(input)
			{
				input.set('value', '');
			}
		},
				
		
		memberProgressiveSearch: function(document_library_id)
		{
			var field = 'LibraryGroupMembers_form_name';			
			this.listBox = new ProgressiveSearch(field, {'search': '/action/fileshare/user_search_handler?document_library_id=' + document_library_id + '&field=' + field,  width: '450px', minimumLength: 3, 'cssClass': 'scrollbox'});
		},
		
		keywordProgressiveSearch: function(document_library_id)
		{
			var field = 'LibrarySearch_form_keyword';
			this.listBox = new ProgressiveSearch(field, {'search': '/action/fileshare/keyword_search_handler?document_library_id=' + document_library_id + '&field=' + field, 'cssClass': 'scrollbox'});		
		},

		addMemberFromProgressiveSearch: function(document_library_id, user_id)
		{
			var request = new Request({
				url: "/action/fileshare/add_member?document_library_id=" + document_library_id + "&user_id=" + user_id,
				method: 'get',
				
				onSuccess: function(response) 
				{ 
					var box = $('member_scrollbox');
					
					var divId = "user_id_" + user_id;
					var div = new Element('div', {'id': divId, 'class': ''});
					div.set('html', response);
					div.inject(box);
					this.closeProgressiveSearch();

				}.bind(this)
			});
	
			request.send();		
		},
		
		openPermissionsDialog: function(document_library_id)
		{
			this.dialog = modalPopup('Fileshare Access', '/action/fileshare/global_permissions_form?document_library_id=' + document_library_id, '550px', 'auto', true);		
		},
		
		permissionsEdited: function(response)
		{
			if(response == "OK")
				this.closeDialog();
			else
			{
				var err = $('RolePermission_form__error');
				err.set('html', response);
				err.setStyle('display', 'table-cell');
			}	
		},
		
		closeDialog: function()
		{
			this.dialog.hide();
		}
		
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new LibraryManagerSingleton();
	};
	
})();