/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above
 copyright holders shall not be used in advertising or otherwise
 to promote the sale, use or other dealings in this Software
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

var LinkPicker = (function()
{
	class LinkPickerSingleton
	{
		constructor()
		{
			this.editor = "";
			this.dialog = null;
			this.document_id = null;
			this.tr = null;
		}

		show(editor, chooser)
		{
			if (!chooser) chooser = "link_picker";

			this.editor = editor;
			this.dialog = modalPopup("Link Picker", "/action/link_picker/popup?Editor=" + this.editor.name + "&chooser=" + chooser, 800, "auto", true, false);
		}

		hide()
		{
			this.dialog.hide();
		}

		insertLink(url)
		{
			var target = document.getElementById('target').value;
			var link = "<a href=\"" + url + "\" target=\"" + target + "\">";
			var selection = this.editor.selection;

			if (selection.isCollapsed())
			{
				this.editor.insertContent(link + url + "</a>");
			}
			else
			{
				var content = selection.getContent();
				selection.setContent(link + content + "</a>");
			}

			this.hide();
		}

		showLibrary(id)
		{
			document.querySelectorAll('#popup_tabs .current a').forEach((a) => { a.loadTab("/action/link_picker/document_picker?Editor=" + this.editor.name + "&document_library_id=" + id); });
		}

		documentSelected()
		{
			document.getElementById('linkButton').disabled = false;
		}

		toggleDocumentSelected(elt, document_id)
		{
			// clear out old selected row
			if (this.tr)
			{
				this.tr.classList.remove('selected');
			}

			// select new row
			var tr = findAncestor(elt, 'tr');

			tr.classList.add('selected');
			this.tr = tr;
			this.document_id = document_id;

			document.getElementById('linkButton').disabled = false;
		}

		documentPickerLinkToDocument()
		{
			if (!this.document_id) return;
			this.insertLink("/action/document/download?document_id=" + this.document_id);
		}

		documentPickerInsertLink(document_id)
		{
			this.document_id = document_id;
			this.documentPickerLinkToDocument();
		}

		linkToDocument()
		{
			var id = document.getElementById('files_value').value;
			this.insertLink("/action/document/download?document_id=" + id);
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new LinkPickerSingleton();
	};

})();

var picker;

function linkPicker(editor)
{
	picker = new LinkPicker().show(editor);
}

function linkPickerExternal(editor)
{
	picker = new LinkPicker().show(editor, "external_url_picker");
}

function closeLinkPicker()
{
	picker = new LinkPicker().hide();
}
