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
	var AttachmentUploaderSingleton = new Class(
	{
		uploadDialog: Class.Empty,
		form: Class.Empty,
		list: Class.Empty,
		cssClass: "",
		deleteIcon: "",
		useCamera: false,
		
		initialize: function()
		{
		},
	
		setup: function(form, list, control, cssClass, deleteIcon, useCamera)
		{
			this.list = document.id(list);
			this.control = document.id(control);
			this.cssClass = cssClass;
			this.deleteIcon = deleteIcon;
			this.useCamera = useCamera;
			
			var attachmentDialog = document.id("attachmentDialog");
			if (!attachmentDialog)
			{
				this.loadDialog(form);
			}
			else
			{
				this.configureForm(form);
				this.uploadDialog = new ModalDialog(document.id("attachmentDialog"), {draggable: false, closeLink: 'closeAttachmentDialog'});
			}
		},
		
		loadDialog: function(form)
		{
			var url = '/action/attachment/dialog?use_camera=' + ((this.useCamera) ? '1' : '0');
			
			this.uploadDialog = new ModalDialog('attachmentDialog_' + String.uniqueID(), {'title': "Add an Attachment", 'width': '500px', 'height': 'auto'});
			this.uploadDialog.show(function() 
			{
				this.form = this.uploadDialog.options.body.getElement('form');
				this.configureForm(this.form);
			}.bind(this), url);
		},
		
		configureForm: function(form)
		{
			form = document.id(form);
			
			form.iFrameFormRequest(
			{
				'onRequest': function() { this.postStart(); return true; }.bind(this),
				'onComplete': function(response) { this.postComplete(response);}.bind(this)
			});
			
			this.form = form;			
		},
		
		addAttachment: function()
		{
			this.form.reset();
			document.id('attachmentDialogMessage').innerHTML = "";
			this.uploadDialog.show();
		},
		
		postStart: function()
		{
			document.id('attachmentDialogMessage').innerHTML = "Uploading file...";
		},
		
		postComplete: function(response)
		{
			var match = response.match(/^(.*?):(.*?):(.*?):(.*)$/);
			
			if (match)
			{
				var id = match[1];
				var name = match[2];
				var icon = match[3];
				var size = match[4];
				
				this.list.innerHTML += 
					"<li id='attachment_" +id + "' class='" + this.cssClass + "'><span><img src='" + icon + "' alt='Icon' style='display: inline-block;vertical-align: middle'/>&nbsp;" +
					"<a href='/action/attachment/download?attachment_id=" + id + "'>" + name + "</a>&nbsp;(" + size + ")&nbsp;" +
					"<a href='#' onclick='new AttachmentUploader().deleteAttachment(\"" + name + "\", " + id + "); return false' title='Delete this Attachment'>" +
					"<img src='" + this.deleteIcon + "' style='display:inline-block; vertical-align: middle' alt='Delete this Attachment'/></a></span></li>";
				
				if (this.control.value) this.control.value += ",";
				this.control.value += id;
				
				this.uploadDialog.hide();
			}
			else
			{
				document.id('attachmentDialogMessage').innerHTML = response;
			}
		},
		
		deleteAttachment: function(filename, id)
		{
			if (!confirm("Are you sure you want to delete " + filename + "?")) return;
			
			var request = new Request(
			{
				'url': 		'/action/attachment/delete?attachment_id=' + id, 
			    'method': 	'get',
				'onSuccess': function(responseText) 
				{ 
					if (responseText == "OK") 
					{
						document.id('attachment_' + id).dispose();
						
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
				}.bind(new AttachmentUploader()),
				'onFailure':	function() { alert("Failed to communicate with server"); }
			});
			
			request.send();
		}
		
	});
	
	var instance;
	return function()
	{
		return instance ? instance : instance = new AttachmentUploaderSingleton();
	};
})();
