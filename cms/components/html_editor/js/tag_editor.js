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
		hrefControl: null,
		targetControl: null,
		hrefRow: null,
		targetRow: null,
	
		srcRow: null,
		srcControl: null,
		altRow: null,
		altControl: null,
	
		initialize: function()
		{
		},
		
		showTagEditor: function(editor)
		{			
			this.editor = editor;
			this.dialog = modalPopup("Edit HTML Tag", "/action/html_editor/tag_editor?editor=" + this.editor.name, 600, "auto", true, true); 
		},
		
		initializeDialog: function()
		{
			this.tagNameControl = document.id('tag_editor_tagName');
			this.idControl = document.id("tag_editor_id");
			this.classControl = document.id("tag_editor_class");
			this.styleControl = document.id("tag_editor_style");
		
			this.hrefRow = document.id("tag_editor_href_row");
			this.hrefControl = document.id("tag_editor_href");
			
			this.targetRow = document.id("tag_editor_target_row");
			this.targetControl = document.id("tag_editor_target");
			
			this.altRow = document.id("tag_editor_alt_row");
			this.altControl = document.id("tag_editor_alt");
			this.srcRow = document.id("tag_editor_src_row");
			this.srcControl = document.id("tag_editor_src");
			
			this.idControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			this.classControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			this.styleControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			
			this.hrefControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			this.targetControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			
			this.altControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			this.srcControl.addEvent('keyup', function(e) { this.updateElement(); }.bind(this));
			
			var elt = this.editor.selection.getNode();
			this.selectElement(elt);
			
			this.bindDialog();
		},
		
		selectElement: function(elt)
		{
			if (!elt) return;
			
			//elt = document.id(elt);
			this.elements = [];
			while(elt.tagName != 'BODY')
			{
				this.elements.push(elt);
				elt = elt.parentElement;
			}
			
			this.level = 0;
			
		},
		
		bindDialog: function()
		{
			var element = this.elements[this.level];
			
			var attribs = this.editor.dom.getAttribs(element);
			
			this.tagNameControl.set('html', element.nodeName);
			this.idControl.set('value', element.id);
			this.classControl.set('value', this.editor.dom.getAttrib(element, 'class'));
			this.styleControl.set('value', this.editor.dom.getAttrib(element, 'style'));
			
			if (element.tagName == "A")
			{
				this.hrefRow.setStyle('display', 'table-row');
				this.targetRow.setStyle('display', 'table-row');
				this.hrefControl.set('value', this.editor.dom.getAttrib(element, 'href'));
				this.targetControl.set('value', this.editor.dom.getAttrib(element, 'target'));
			}
			else
			{
				this.hrefRow.setStyle('display', 'none');
				this.targetRow.setStyle('display', 'none');				
			}
			
			if (element.tagName == "IMG")
			{
				this.srcRow.setStyle('display', 'table-row');
				this.srcControl.set('value', this.editor.dom.getAttrib(element, 'src'));
				this.altRow.setStyle('display', 'table-row');
				this.altControl.set('value', this.editor.dom.getAttrib(element, 'alt'));
			}
			else
			{
				this.srcRow.setStyle('display', 'none');
				this.altRow.setStyle('display', 'none');
			}
		},
		
		updateElement: function()
		{
			var element = this.elements[this.level];

			this.tagNameControl.set('html', element.nodeName);
			this.editor.dom.setAttrib(element, 'id', this.idControl.value);
			this.editor.dom.setAttrib(element, 'class', this.classControl.value);
			this.editor.dom.setAttrib(element, 'style', this.styleControl.value);
			
			if (element.tagName == "A")
			{
				this.editor.dom.setAttrib(element, 'href', this.hrefControl.value);
				this.editor.dom.setAttrib(element, 'target', this.targetControl.value);
			}
			
			if (element.tagName == "IMG")
			{
				this.editor.dom.setAttrib(element, 'src', this.srcControl.value);
				this.editor.dom.setAttrib(element, 'alt', this.altControl.value);				
			}
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
		},
		
		prev: function()
		{
			var element = this.elements[this.level];
			var elt = element.getPrev();
			if (elt)
			{
				this.selectElement(elt);
				this.bindDialog();
			}
		},
		
		next: function()
		{
			var element = this.elements[this.level];
			var elt = element.getNext();
			if (elt)
			{
				this.selectElement(elt);
				this.bindDialog();
			}
		}
	});

	var instance;
	return function()
	{
		return instance ? instance : instance = new TagEditorSingleton();
	};
})();