/**
 *
 */
var CustomCSSManager = (function()
{
	class CustomCSSManagerSingleton
	{
		constructor()
		{
			this.editor = "";
			this.dialog = null;
		}

		showCSSEditor(editor)
		{
			this.editor = editor;
			this.dialog = modalPopup("Edit Custom CSS", "/action/html_editor/custom_css_edit?editor=" + this.editor.name, 600, "auto", true, false);
		}

		hideCSSEditor()
		{
			if (this.dialog)
			{
				this.dialog.hide();
				this.dialog = null;
			}
		}

		cssEditorResponse(response)
		{
			if (response == "OK")
			{
				this.hideCSSEditor();

				var doc = this.editor.getDoc();
				var sheets = doc.querySelectorAll("link[rel=stylesheet]");
				for (var i = 0; i < sheets.length; ++i)
				{
					var link = sheets[i];
					if (link.href.includes("/action/html_editor/custom_css"))
					{
						link.href = link.href.replace(/\?.*|$/, "?ts=" + new Date().getTime());
					}
				}
			}
			else
			{
				notification(response);
			}
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new CustomCSSManagerSingleton();
	};
})();
