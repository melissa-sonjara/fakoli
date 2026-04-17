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

var pictureSource;
var destinationType;

document.addEventListener("deviceready", onDeviceReady, false);

// Cordova is ready to be used!
//
function onDeviceReady()
{
	pictureSource = navigator.camera.PictureSourceType;
	destinationType = navigator.camera.DestinationType;
}

var PhotoAttachmentUploader = (function()
{
	class PhotoAttachmentUploaderSingleton
	{
		constructor()
		{
			this.list = null;
			this.cssClass = "";
			this.deleteIcon = "";
		}

		setup(list, control, cssClass, deleteIcon)
		{
			this.list = document.getElementById(list);
			this.control = document.getElementById(control);
			this.cssClass = cssClass;
			this.deleteIcon = deleteIcon;
			console.log("PhotoAttachmentUploader::setup complete");
		}

		capturePhoto()
		{
			navigator.camera.getPicture(
				function(imageData) { new PhotoAttachmentUploader().onPhotoURISuccess(imageData); },
				function() { new PhotoAttachmentUploader().onFail(message); },
				{
					quality: 50,
					destinationType: destinationType.FILE_URI
				}
			);
		}

		choosePhoto()
		{
			// Retrieve image file location from specified source
			navigator.camera.getPicture(
				function(imageURI) { new PhotoAttachmentUploader().onPhotoURISuccess(imageURI); },
				function() { new PhotoAttachmentUploader().onFail(message); },
				{
					quality: 50,
					destinationType: destinationType.FILE_URI,
					sourceType: pictureSource.PHOTOLIBRARY
				}
			);
		}

		// Called when a photo is successfully retrieved
		//
		onPhotoURISuccess(imageURI)
		{
			this.uploadPhoto(imageURI);
		}

		// Called if something bad happens.
		//
		onFail(message)
		{
			messagePopup("Error", message);
		}

		uploadPhoto(imageURI)
		{
			console.log("Uploading Photo");
			var options = new FileUploadOptions();
			options.fileKey = "attachmentFile";
			options.fileName = imageURI.substr(imageURI.lastIndexOf('/') + 1);
			options.mimeType = "image/jpeg";

			var url = new URL(document.location.href);

			var ft = new FileTransfer();
			ft.upload(
				imageURI,
				encodeURI(url.protocol + "//" + url.host + "/action/attachment/upload"),
				function(r) { new PhotoAttachmentUploader().onUploadSuccess(r); },
				function(error) { new PhotoAttachmentUploader().onUploadFail(error); },
				options
			);
		}

		onUploadSuccess(r)
		{
			console.log("Code = " + r.responseCode);
			console.log("Response = " + r.response);
			console.log("Sent = " + r.bytesSent);
			this.postComplete(r.response);
		}

		onUploadFail(error)
		{
			messagePopup("Error", "Upload Failed");
			console.log("upload error code " + error.code);
			console.log("upload error source " + error.source);
			console.log("upload error target " + error.target);
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
					"<a href='#' onclick='new PhotoAttachmentUploader().deleteAttachment(\"" + name + "\", " + id + "); return false' title='Delete this Photo'>" +
					"<img src='" + this.deleteIcon + "' style='display:inline-block; vertical-align: middle' alt='Delete this Photo'/></a></span></li>";

				if (this.control.value) this.control.value += ",";
				this.control.value += id;
			}
			else
			{
				messagePopup("Photo Upload Failed", response);
			}
		}

		deleteAttachment(filename, id)
		{
			if (!confirm("Are you sure you want to delete " + filename + "?")) return;

			fetch('/action/attachment/delete?attachment_id=' + id)
				.then(function(r) { return r.text(); })
				.then(function(responseText)
				{
					if (responseText == "OK")
					{
						document.getElementById('attachment_' + id).remove();

						var uploader = new PhotoAttachmentUploader();
						if (uploader.control.value)
						{
							var regexp = new RegExp("\\b" + id + "\\b");
							uploader.control.value = uploader.control.value.replace(regexp, "");
							uploader.control.value = uploader.control.value.replace(",,", ",");
						}
					}
					else
					{
						messagePopup("Error", responseText);
					}
				})
				.catch(function() { messagePopup("Network Error", "Failed to communicate with server"); });
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new PhotoAttachmentUploaderSingleton();
	};
})();
