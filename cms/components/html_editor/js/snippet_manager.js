/**
 * 
 */

var SnippetManager =  (function()
{
	var SnippetManagerSingleton = new Class(
	{
		editor: "",
		dialog: Class.Empty,
		snippetDialog: null,
		
		initialize: function()
		{
		},
		
		showPicker: function(editor)
		{			
			this.editor = editor;
			this.dialog = modalPopup("Insert Snippet", "/action/html_editor/snippet_picker?Editor=" + this.editor.name, 600, "auto", true, false); 
		},
		
		hidePicker: function()
		{
			if (this.dialog)
			{
				this.dialog.hide();
				this.dialog = null;
			}
		},
		
		hide: function()
		{
			this.hideSnippetDialog();
			this.hidePicker();
		},
		
		insertSnippet: function(snippet_id)
		{
			var snippet = httpRequest('/action/html_editor/snippet?snippet_id=' + snippet_id);
			
			snippet = this.substituteParameters(snippet);
			
			var selection = this.editor.selection;
			
			if (selection.isCollapsed())
			{
				this.editor.insertContent(snippet);
			}
			else
			{
				content = selection.getContent();
				selection.setContent(snippet);
			}

			this.hide();
		},
		
		substituteParameters: function(snippet)
		{
			var parameters = document.id('snippet_parameters');
			if (parameters)
			{
				var inputs = parameters.getElements('input');
				inputs.each(function(input)
				{
					snippet = snippet.split("{" + input.name + "}").join(input.value);
				});
			}
			
			return snippet;
		},
		
		selectSnippet: function(snippet_id)
		{
			this.insertSnippet(snippet_id);
			this.hideSnippetDialog();
			this.hidePicker();
		},
		
		showSnippetDialog: function(snippet_id)
		{
			if (typeof snippet_id == 'undefined') snippet_id = "";
			var title = snippet_id ? "Edit Snippet" : "Create Snippet";
			this.snippetDialog = modalPopup(title, "/action/html_editor/snippet_form?snippet_id=" + snippet_id, 600, "auto", true, true);
		},
		
		hideSnippetDialog: function()
		{
			if (this.snippetDialog)
			{
				this.snippetDialog.hide();
				this.snippetDialog = null;
			}
		},
		
		updateSnippetPanel: function(snippet_id)
		{
			this.dialog.show(null, '/action/html_editor/snippet_picker?snippet_id=' + snippet_id);
		},
		
		snippetSaved: function(response)
		{
			if (!response.match(/^\d+$/))
			{
				document.id("Snippet_form__error").set('html', response).setStyle('display', 'block');
				return;
			}
			
			this.updateSnippetPanel(response);
			this.hideSnippetDialog();
		}
	});
	
	var instance;
	return function()
	{
		return instance ? instance : instance = new SnippetManagerSingleton();
	};
	
})();
