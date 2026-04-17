var PageManager = (function()
{
	class PageManagerSingleton
	{
		constructor()
		{
			this.dialog = null;
			this.document_library_id = null;
			this.versioned = false;
		}

		editPage(page_id, versioned)
		{
			this.versioned = versioned;
			this.dialog = modalPopup('Edit Page Details', '/action/page/edit?page_id=' + page_id, '900px', 'auto', true);
		}

		editResult(result)
		{
			if (result == "OK")
			{
				this.closeDialog();
				if (this.versioned)
				{
					var url = new URL(window.location.href);
					url.searchParams.set('version', 'draft');
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
				var errorEl = document.getElementById('Page_form__error');
				errorEl.textContent = result;
				errorEl.style['display'] = 'table-cell';
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
		return instance ? instance : instance = new PageManagerSingleton();
	};

})();
