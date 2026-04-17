var TagEditor = (function()
{
	class TagEditorSingleton
	{
		constructor()
		{
			this.editor = "";
			this.dialog = null;

			this.elements = [];
			this.level = 0;

			this.tagNameControl = null;
			this.idControl = null;
			this.classControl = null;
			this.styleControl = null;
			this.hrefControl = null;
			this.targetControl = null;
			this.hrefRow = null;
			this.targetRow = null;

			this.srcRow = null;
			this.srcControl = null;
			this.altRow = null;
			this.altControl = null;
		}

		showTagEditor(editor)
		{
			this.editor = editor;
			this.dialog = modalPopup("Edit HTML Tag", "/action/html_editor/tag_editor?editor=" + this.editor.name, 600, "auto", true, true);
		}

		initializeDialog()
		{
			this.tagNameControl = document.getElementById('tag_editor_tagName');
			this.idControl = document.getElementById("tag_editor_id");
			this.classControl = document.getElementById("tag_editor_class");
			this.styleControl = document.getElementById("tag_editor_style");

			this.hrefRow = document.getElementById("tag_editor_href_row");
			this.hrefControl = document.getElementById("tag_editor_href");

			this.targetRow = document.getElementById("tag_editor_target_row");
			this.targetControl = document.getElementById("tag_editor_target");

			this.altRow = document.getElementById("tag_editor_alt_row");
			this.altControl = document.getElementById("tag_editor_alt");
			this.srcRow = document.getElementById("tag_editor_src_row");
			this.srcControl = document.getElementById("tag_editor_src");

			this.idControl.addEventListener('keyup', function(e) { this.updateElement(); }.bind(this));
			this.classControl.addEventListener('keyup', function(e) { this.updateElement(); }.bind(this));
			this.styleControl.addEventListener('keyup', function(e) { this.updateElement(); }.bind(this));

			this.hrefControl.addEventListener('keyup', function(e) { this.updateElement(); }.bind(this));
			this.targetControl.addEventListener('keyup', function(e) { this.updateElement(); }.bind(this));

			this.altControl.addEventListener('keyup', function(e) { this.updateElement(); }.bind(this));
			this.srcControl.addEventListener('keyup', function(e) { this.updateElement(); }.bind(this));

			var elt = this.editor.selection.getNode();
			this.selectElement(elt);

			this.bindDialog();
		}

		selectElement(elt)
		{
			if (!elt) return;

			//elt = document.getElementById(elt.id);
			this.elements = [];
			while (elt.tagName != 'BODY')
			{
				this.elements.push(elt);
				elt = elt.parentElement;
			}

			this.level = 0;
		}

		bindDialog()
		{
			var element = this.elements[this.level];

			var attribs = this.editor.dom.getAttribs(element);

			this.tagNameControl.innerHTML = element.nodeName;
			this.idControl.value = element.id;
			this.classControl.value = this.editor.dom.getAttrib(element, 'class');
			this.styleControl.value = this.editor.dom.getAttrib(element, 'style');

			if (element.tagName == "A")
			{
				this.hrefRow.style['display'] = 'table-row';
				this.targetRow.style['display'] = 'table-row';
				this.hrefControl.value = this.editor.dom.getAttrib(element, 'href');
				this.targetControl.value = this.editor.dom.getAttrib(element, 'target');
			}
			else
			{
				this.hrefRow.style['display'] = 'none';
				this.targetRow.style['display'] = 'none';
			}

			if (element.tagName == "IMG")
			{
				this.srcRow.style['display'] = 'table-row';
				this.srcControl.value = this.editor.dom.getAttrib(element, 'src');
				this.altRow.style['display'] = 'table-row';
				this.altControl.value = this.editor.dom.getAttrib(element, 'alt');
			}
			else
			{
				this.srcRow.style['display'] = 'none';
				this.altRow.style['display'] = 'none';
			}
		}

		updateElement()
		{
			var element = this.elements[this.level];

			this.tagNameControl.innerHTML = element.nodeName;
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
		}

		up()
		{
			this.level++;
			if (this.level >= this.elements.length)
			{
				this.level = this.elements.length - 1;
			}
			this.bindDialog();
		}

		down()
		{
			this.level--;
			if (this.level < 0)
			{
				this.level = 0;
				var elt = this.elements[this.level];
				if (elt.childElementCount > 0)
				{
					this.selectElement(elt.children[0]);
				}
			}
			this.bindDialog();
		}

		prev()
		{
			var element = this.elements[this.level];
			var elt = element.previousElementSibling;
			if (elt)
			{
				this.selectElement(elt);
				this.bindDialog();
			}
		}

		next()
		{
			var element = this.elements[this.level];
			var elt = element.nextElementSibling;
			if (elt)
			{
				this.selectElement(elt);
				this.bindDialog();
			}
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new TagEditorSingleton();
	};
})();
