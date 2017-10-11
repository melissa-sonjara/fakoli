/**
 * 
 */
var CustomCSSManager =  (function()
{
	var CustomCSSManagerSingleton = new Class(
	{
		editor: "",
		dialog: Class.Empty,
		
		initialize: function()
		{
		},
		
		showCSSEditor: function(editor)
		{			
			this.editor = editor;
			this.dialog = modalPopup("Edit Custom CSS", "/action/html_editor/custom_css_edit?editor=" + this.editor.name, 600, "auto", true, false); 
		},
		
		hideCSSEditor: function()
		{
			if (this.dialog)
			{
				this.dialog.hide();
				this.dialog = null;
			}
		},
		
		cssEditorResponse: function(response)
		{
			if (response == "OK")
			{
				this.hideCSSEditor();
				
				var doc = this.editor.getDoc();
				for (var link of doc.querySelectorAll("link[rel=stylesheet]")) 
				{
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
	});

	var instance;
	return function()
	{
		return instance ? instance : instance = new CustomCSSManagerSingleton();
	};
})();