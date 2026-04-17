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


var AttachmentUploader = (function()
{
	class AttachmentUploaderSingleton
	{
		constructor()
		{
			this.uploadDialog = null;
			this.form = null;
			this.list = null;
			this.control = null;
			this.cssClass = "";
			this.deleteIcon = "";
			this.useCamera = false;
		}

		setup(form, list, control, cssClass, deleteIcon, useCamera)
		{
			this.list = document.getElementById(list);
			this.control = document.getElementById(control);
			this.cssClass = cssClass;
			this.deleteIcon = deleteIcon;
			this.useCamera = useCamera;

			var attachmentDialog = document.getElementById("attachmentDialog");
			if (!attachmentDialog)
			{
				this.loadDialog(form);
			}
			else
			{
				this.configureForm(form);
				this.uploadDialog = new ModalDialog(document.getElementById("attachmentDialog"), {draggable: false, closeLink: 'closeAttachmentDialog'});
			}
		}

		loadDialog(form)
		{
			var url = '/action/attachment/dialog?use_camera=' + ((this.useCamera) ? '1' : '0');

			// Generate a unique id for the dialog container
			var uid = 'attachmentDialog_' + Math.random().toString(36).slice(2);
			this.uploadDialog = new ModalDialog(uid, {'title': "Add an Attachment", 'width': '500px', 'height': 'auto'});
			this.uploadDialog.show((dialogBody) =>
			{
				this.form = dialogBody.querySelector('form');
				this.configureForm(this.form);
			}, url);
		}

		configureForm(form)
		{
			if (typeof form === 'string')
			{
				form = document.getElementById(form);
			}

			// iFrameFormRequest is a legacy MooTools plugin; wire up a native
			// submit handler that posts via fetch into a hidden iframe instead.
			// If a native iFrameFormRequest equivalent is available globally,
			// prefer it; otherwise fall back to a simple fetch-based approach.
			if (typeof form.iFrameFormRequest === 'function')
			{
				form.iFrameFormRequest(
				{
					'onRequest': () => { this.postStart(); return true; },
					'onComplete': (response) => { this.postComplete(response); }
				});
			}
			else
			{
				form.addEventListener('submit', (e) =>
				{
					e.preventDefault();
					this.postStart();
					var formData = new FormData(form);
					fetch(form.action || '', { method: 'POST', body: formData })
						.then(r => r.text())
						.then(response => { this.postComplete(response); });
				});
			}

			this.form = form;
		}

		addAttachment()
		{
			this.form.reset();
			document.getElementById('attachmentDialogMessage').innerHTML = "";
			this.uploadDialog.show();
		}

		postStart()
		{
			document.getElementById('attachmentDialogMessage').innerHTML = "Uploading file...";
		}

		postComplete(response)
		{
			var match = response.match(/^(.*?):(.*?):(.*?):(.*)$/);

			if (match)
			{
				var id = match[1];
				var name = match[2];
				var icon = match[3];
				var size = match[4];

				this.list.innerHTML +=
					"<li id='attachment_" + id + "' class='" + this.cssClass + "'><span><img src='" + icon + "' alt='Icon' style='display: inline-block;vertical-align: middle'/>&nbsp;" +
					"<a href='/action/attachment/download?attachment_id=" + id + "'>" + name + "</a>&nbsp;(" + size + ")&nbsp;" +
					"<a href='#' onclick='new AttachmentUploader().deleteAttachment(\"" + name + "\", " + id + "); return false' title='Delete this Attachment'>" +
					"<img src='" + this.deleteIcon + "' style='display:inline-block; vertical-align: middle' alt='Delete this Attachment'/></a></span></li>";

				if (this.control.value) this.control.value += ",";
				this.control.value += id;

				this.uploadDialog.hide();
			}
			else
			{
				document.getElementById('attachmentDialogMessage').innerHTML = response;
			}
		}

		deleteAttachment(filename, id)
		{
			if (!confirm("Are you sure you want to delete " + filename + "?")) return;

			fetch('/action/attachment/delete?attachment_id=' + id, { method: 'GET' })
				.then(r => r.text())
				.then((responseText) =>
				{
					if (responseText == "OK")
					{
						var el = document.getElementById('attachment_' + id);
						if (el) el.remove();

						if (this.control.value)
						{
							var regexp = new RegExp("\\b" + id + "\\b");
							this.control.value = this.control.value.replace(regexp, "");
							this.control.value = this.control.value.replace(",,", ",");
						}
					}
					else
					{
						alert(responseText);
					}
				})
				.catch(() => { alert("Failed to communicate with server"); });
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new AttachmentUploaderSingleton();
	};
})();
