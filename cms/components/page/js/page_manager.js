var PageManager = (function()
{
	var PageManagerSingleton = new Class(
	{
		dialog: null,
		document_library_id: null,
		versioned: false,
		
		initialize: function()
		{
		},
		
		editPage: function(page_id, versioned)
		{
			this.versioned = versioned;
			this.dialog = modalPopup('Edit Page Details', '/action/page/edit?page_id=' + page_id, '900px', 'auto', true);
		},
		
		editResult: function(result)
		{
			if (result == "OK")
			{
				this.closeDialog();
				if (this.versioned)
				{
					var url = new URI();
					url.setData('version', 'draft');
					window.location.href = url.toString();
				}
				else
				{
					window.location.reload();
				}
			}
			else if (result == "DELETED")
			{
				window.location.href = "/index";
			}
			else
			{
				document.id('Page_form__error').set({'text': result, 'display': 'table-cell'});
			}
		},
		
		closeDialog: function()
		{
			this.dialog.hide();		
		},
		
	});
	
	var instance;
	return function()
	{
		return instance ? instance : instance = new PageManagerSingleton();
	};
	
})();
