/**
 *
 */

var SnippetManager = (function()
{
	class SnippetManagerSingleton
	{
		constructor()
		{
			this.editor = "";
			this.dialog = null;
			this.snippetDialog = null;
		}

		showPicker(editor)
		{
			this.editor = editor;
			this.dialog = modalPopup("Insert Snippet", "/action/html_editor/snippet_picker?Editor=" + this.editor.name, 600, "auto", true, false);
		}

		hidePicker()
		{
			if (this.dialog)
			{
				this.dialog.hide();
				this.dialog = null;
			}
		}

		hide()
		{
			this.hideSnippetDialog();
			this.hidePicker();
		}

		insertSnippet(snippet_id)
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
		}

		substituteParameters(snippet)
		{
			var parameters = document.getElementById('snippet_parameters');
			if (parameters)
			{
				var inputs = parameters.querySelectorAll('input');
				inputs.forEach(function(input)
				{
					snippet = snippet.split("{" + input.name + "}").join(input.value);
				});
			}

			return snippet;
		}

		selectSnippet(snippet_id)
		{
			this.insertSnippet(snippet_id);
			this.hideSnippetDialog();
			this.hidePicker();
		}

		showSnippetDialog(snippet_id)
		{
			if (typeof snippet_id == 'undefined') snippet_id = "";
			var title = snippet_id ? "Edit Snippet" : "Create Snippet";
			this.snippetDialog = modalPopup(title, "/action/html_editor/snippet_form?snippet_id=" + snippet_id, 600, "auto", true, true);
		}

		hideSnippetDialog()
		{
			if (this.snippetDialog)
			{
				this.snippetDialog.hide();
				this.snippetDialog = null;
			}
		}

		updateSnippetPanel(snippet_id)
		{
			this.dialog.show(null, '/action/html_editor/snippet_picker?snippet_id=' + snippet_id);
		}

		snippetSaved(response)
		{
			if (!response.match(/^\d+$/))
			{
				var errorEl = document.getElementById("Snippet_form__error");
				errorEl.innerHTML = response;
				errorEl.style['display'] = 'block';
				return;
			}

			this.updateSnippetPanel(response);
			this.hideSnippetDialog();
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new SnippetManagerSingleton();
	};

})();
