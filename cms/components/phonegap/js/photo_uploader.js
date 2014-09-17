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

window.addEvent('load', function()
{
	document.addEventListener("deviceready", function() { new PhotoUploader().onDeviceReady(); },false);
});


var PhotoUploader = (function()
{
	var PhotoUploaderSingleton = new Class(
	{
		control: Class.Empty,
		thumbnail: Class.Empty,
		cssClass: "",
		deleteIcon: "",
		statusLabel: Class.Empty,
		galleryID: 0,
		statusText: "Waiting for Camera...",
		
		initialize: function()
		{
		},
	
		// Cordova is ready to be used!
		//
	    onDeviceReady: function() 
		{
			pictureSource=navigator.camera.PictureSourceType;
			destinationType=navigator.camera.DestinationType;		
			this.setStatusText('Camera is ready');
		},

		setup: function(control, thumbnail, statusLabel, galleryID)
		{
			this.thumbnail = document.id(thumbnail);
			this.control = document.id(control);
			this.statusLabel = document.id(statusLabel);
			this.galleryID = galleryID;
			
			this.statusLabel.set('text', this.statusText);
			
			console.log("PhotoUploader::setup complete");
		},
		
		setStatusText: function(text)
		{
			this.statusText = text;
			if (this.statusLabel) this.statusLabel.set('text', text);
		},
		
		capturePhoto: function()
		{
			navigator.camera.getPicture(function(imageData) { new PhotoUploader().onPhotoURISuccess(imageData);}, 
										function(message) { new PhotoUploader().onFail(message); },
										{ 
											quality: 50,
											destinationType: Camera.DestinationType.FILE_URI,
											sourceType: Camera.PictureSourceType.CAMERA
										});
		},
		
		choosePhoto: function() 
		{
			// Retrieve image file location from specified source
			navigator.camera.getPicture(function(imageURI) { new PhotoUploader().onPhotoURISuccess(imageURI);}, 
										function(message) { new PhotoUploader().onFail(message); }, 
										{
											quality: 50,
											destinationType: Camera.DestinationType.FILE_URI,
											sourceType: Camera.PictureSourceType.PHOTOLIBRARY 
										});
		},

		// Called when a photo is successfully retrieved
		//
		onPhotoURISuccess: function(imageURI) 
		{
			this.uploadPhoto(imageURI);
		},

		// Called if something bad happens.
		//
	    onFail: function(message) 
	    {
	    	messagePopup("Error", message);
		},

        uploadPhoto: function(imageURI) 
        {
        	console.log("Uploading Photo");
            var options = new FileUploadOptions();
            options.fileKey="photo";
            options.fileName=imageURI.substr(imageURI.lastIndexOf('/')+1);
            options.mimeType="image/jpeg";

            var url = document.location.href.toURI();
            
            var ft = new FileTransfer();
            ft.onprogress = function(progressEvent)
            {
            	if (progressEvent.lengthComputable)
            	{
            		percentage = (100 * progressEvent.loaded / progressEvent.total).toPrecision(0);
            		this.setStatusText("Uploading " + percentage + "%");
            	}
            }.bind(this);
            
            ft.upload(imageURI, encodeURI(url.get('scheme') + "://" + url.get('host') + "/action/phonegap/photo_upload?gallery_id=" + this.galleryID), 
            		  function(r) { new PhotoUploader().onUploadSuccess(r); },
            		  function(error) { new PhotoUploader().onUploadFail(error); },
            		  options);
        },

        onUploadSuccess: function(r) 
        {
            console.log("Code = " + r.responseCode);
            console.log("Response = " + r.response);
            console.log("Sent = " + r.bytesSent);
            this.postComplete(r.response);
        },

        onUploadFail: function(error) 
        {
        	messagePopup("Error", "Upload Failed");
            console.log("upload error code " + error.code);
            console.log("upload error source " + error.source);
            console.log("upload error target " + error.target);
        },

		postStart: function()
		{
			this.statusLabel.set("text", "Uploading file...");
		},
		
		postComplete: function(response)
		{
			
			var match = response.match(/^(\d+)$/);
			
			if (match)
			{
				var id = match[1];

				this.thumbnail.set('html', '<img src="/action/image/thumbnail?image_id=' + id + '&size=300" alt="Uploaded Photo"/>');
				this.control.value = id;	
				this.setStatusText("Photo Uploaded");
			}
			else
			{
				messagePopup("Photo Upload Failed", response);
			}
		},
		
		deletePhoto: function(filename, id)
		{
			if (!confirm("Are you sure you want to delete " + filename + "?")) return;
			
			var request = new Request(
			{
				'url': 		'/action/phonegap/photo_delete?attachment_id=' + id, 
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
						messagePopup("Error", responseText);
					}
				}.bind(new PhotoUploader()),
				'onFailure':	function() { messagePopup("Network Error", "Failed to communicate with server"); }
			});
			
			request.send();
		}
		
	});
	
	var instance = null;
	return function()
	{
		return instance ? instance : instance = new PhotoUploaderSingleton();
	};
})();
