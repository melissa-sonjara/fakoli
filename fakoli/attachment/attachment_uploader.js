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
		
		initialize: function()
		{
		},
	
		setup: function(form, list, control)
		{
			form = $(form);
			
			form.iFrameFormRequest(
			{
				'onRequest': function() { this.postStart(); return true; }.bind(this),
				'onComplete': function(response) { this.postComplete(response);}.bind(this)
			});
			
			this.form = form;
			this.list = $(list);
			this.control = $(control);
			
			this.uploadDialog = new ModalDialog($("attachmentDialog"), {draggable: false, closeLink: 'closeAttachmentDialog'});
		},
		
		addAttachment: function()
		{
			this.form.reset();
			this.uploadDialog.show();
		},
		
		postStart: function()
		{
			$('attachmentDialogMessage').innerHTML = "Uploading file...";
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
				
				this.list.innerHTML += "<img src='" + icon + "' alt='Icon' style='display: inline-block;vertical-align: middle'/>&nbsp;" + name + " (" + size + ")<br/>";
				
				if (this.control.value) this.control.value += ",";
				this.control.value += id;
				
				this.uploadDialog.hide();
			}
		}
		
	});
	
	var instance;
	return function()
	{
		return instance ? instance : instance = new AttachmentUploaderSingleton();
	}
})();
