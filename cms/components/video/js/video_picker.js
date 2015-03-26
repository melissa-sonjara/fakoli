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
 
var VideoPicker =  (function()
{
	var VideoPickerSingleton = new Class(
	{
		editor: null,
		selectedVideoId: null,
		selectedVideoTitle: "",
		selectedVideoWidth: 0,
		selectedVideoHeight: 0,
		selectedImageID: 0,
		
		initialize: function()
		{
			this.updateUI();
		},
		
		setup: function(editor)
		{
			this.editor = editor;
		},
		
		selectVideo: function(link, id, video, image)
		{
			var tr = findAncestor(link, "tr");
			
			$$('#videos tr').each(function(r) { r.removeClass('selected'); });
			tr.addClass('selected');
			this.selectedVideoId = id;
			
			var resolution = tr.getChildren()[1].get('text');
			
			var dimensions = resolution.split(" x ");
			this.selectedVideoWidth = dimensions[0];
			this.selectedVideoHeight = dimensions[1];
			
			this.selectedVideoTitle = link.get('text');
			this.selectedImageID = tr.getElement("span.image_id").get("text");	
			this.selectedVideo = video;
			this.selectedVideoImage = image;
		},
		
		show: function(editor)
		{
			this.editor = editor;
			this.dialog = modalPopup("Insert Video", "/action/video/video_picker?Editor=" + this.editor.name, 600, 620, true, false); 
		},
		
		hide: function()
		{
			this.dialog.hide();
		},
		
		updateUI: function()
		{
			var button = document.id('insert_video_button');
			
			if (!button) return;
			
			if (this.selectedVideoId)
			{
				button.setStyle('disabled', false);
			}
			else
			{
				button.setStyle('disabled', true);
			}
		},
		
		insert: function()
		{
			var mode = document.id('insert_mode').value;
			var transcript = document.id('transcript').checked;
			
			var insertion = "";
			
			switch(mode)
			{
			case "button":
				
				insertion = "<button class='button' onclick=\"videoLightbox(this, " + this.selectedVideoId + "); return false;\">" + this.selectedVideoTitle + "</button>";
				break;
				
			case "link":
				
				insertion = "<a href='#' onclick='videoLightbox(this, " + this.selectedVideoId + "); return false;'>" + this.selectedVideoTitle + "</a>";
				break;
				
			case "thumbnail":
				
				insertion = "<img src='" + this.selectedVideoImage + "' style='cursor: pointer' alt='" + this.selectedVideoTitle + "' onclick=\"videoLightbox(this, " + this.selectedVideoId + "); return false;\"/>";
				break;
				
			case "embed":
				
				insertion = "<a class='video' href='" + this.selectedVideo + "' style='display: block; width:" + this.selectedVideoWidth + "px; height: " + this.selectedVideoHeight +"px; background-image: url(" + this.selectedVideoImage + ")'></a>";
				break;
				
			case "download":
				var download = this.selectedVideo.replace("/stream", "/download");
				
				insertion = "<a href='" + download + "'>Download Video</a>";
				break;
			}
			
			if (transcript)
			{
				insertion = "<span>" + insertion + "<br/><a href='#' onclick='videoTranscript(" + this.selectedVideoId + "); return false;'>View Transcript</a></span>";
			}
			
		    this.editor.insertContent(insertion);
		    this.hide();
		}
	});

	var instance;
	return function()
	{
		return instance ? instance : instance = new VideoPickerSingleton();
	};
	
})();


function videoLightbox(element, id)
{
	var title = "";
	var tag = element.tagName.toUpperCase();
	switch(tag)
	{
	case "A":
	case "BUTTON":
		
		title = element.get('text');
		if (!title) title = element.title;
		break;
		
	case "IMG":
		
		title = element.alt;
		break;
	}

	if (!title) title = "Video";

	var size = httpRequest('/action/video/video_size?video_id=' + id);
	var dimensions = size.split('x');
	modalPopup(title, "/action/video/video?video_id=" + id, (Number(dimensions[0]) + 16),(Number(dimensions[1]) + 52));
}

function videoTranscript(id)
{
	popup("/transcript?video_id=" + id, "video_transcript_" + id, "500px", "600px");
}

function videoPicker(editor)
{
	new VideoPicker().show(editor);
}

function closeVideoPicker()
{
	new VideoPicker().hide();
}