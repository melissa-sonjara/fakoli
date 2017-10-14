var TagEditor =  (function()
{
	var TagEditorSingleton = new Class(
	{
		editor: "",
		dialog: Class.Empty,
		
		elements: [],
		level: 0,
		
		tagNameControl: null,
		idControl: null,
		classControl: null,
		styleControl: null,
		
		initialize: function()
		{
		},
		
		showTagEditor: function(editor)
		{			
			this.editor = editor;
			this.dialog = modalPopup("Edit HTML Tag", "/action/html_editor/tag_editor?editor=" + this.editor.name, 600, "auto", true, false); 
		},
		
		initializeDialog: function()
		{
			this.tagNameControl = document.id('tag_editor_tagName');
			this.idControl = document.id("tag_editor_id");
			this.classControl = document.id("tag_editor_class");
			this.styleControl = document.id("tag_editor_style");
		
			this.idControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			this.classControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			this.styleControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			
			var elt = this.editor.selection.getNode();
			if (!elt) return;
			
			//elt = document.id(elt);
			this.elements = [];
			while(elt.tagName != 'BODY')
			{
				this.elements.push(elt);
				elt = elt.parentElement;
			}
			
			this.level = 0;
			
			this.bindDialog();
		},
		
		bindDialog: function()
		{
			var element = this.elements[this.level];
			
			var attribs = this.editor.dom.getAttribs(element);
			
			this.tagNameControl.set('html', element.nodeName);
			this.idControl.set('value', element.id);
			this.classControl.set('value', this.editor.dom.getAttrib(element, 'class'));
			this.styleControl.set('value', this.editor.dom.getAttrib(element, 'style'));
		},
		
		updateElement: function()
		{
			var element = this.elements[this.level];

			this.tagNameControl.set('html', element.nodeName);
			this.editor.dom.setAttrib(element, 'id', this.idControl.value);
			this.editor.dom.setAttrib(element, 'class', this.classControl.value);
			this.editor.dom.setAttrib(element, 'style', this.styleControl.value);
		},
		
		up: function()
		{
			this.level++;
			if (this.level > this.elements.length)
			{
				this.level = this.elements.length;
			}
			this.bindDialog();
		},
		
		down: function()
		{
			this.level--;
			if (this.level < 0) this.level = 0;
			this.bindDialog();
		}
	});

	var instance;
	return function()
	{
		return instance ? instance : instance = new TagEditorSingleton();
	};
})();