var TagEditor =  (function()
{
	var TagEditorSingleton = new Class(
	{
		editor: "",
		dialog: Class.Empty,
		
		initialize: function()
		{
		},
		
		showTagEditor: function(editor)
		{			
			this.editor = editor;
			this.dialog = modalPopup("Edit HTML Tag", "/action/html_editor/tag_editor?editor=" + this.editor.name, 600, "auto", true, false); 
		},
	});

	var instance;
	return function()
	{
		return instance ? instance : instance = new TagEditorSingleton();
	};
})();