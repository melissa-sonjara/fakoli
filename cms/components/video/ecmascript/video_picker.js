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

var VideoPicker = (function()
{
	class VideoPickerSingleton
	{
		constructor()
		{
			this.editor              = null;
			this.selectedVideoId     = null;
			this.selectedVideoTitle  = "";
			this.selectedVideoWidth  = 0;
			this.selectedVideoHeight = 0;
			this.selectedVideoFormat = "";
			this.selectedImageID     = 0;

			this.updateUI();
		}

		setup(editor)
		{
			this.editor = editor;
		}

		selectVideo(link, id, video, image)
		{
			var tr = findAncestor(link, "tr");

			document.querySelectorAll('#videos tr').forEach(function(r) { r.classList.remove('selected'); });
			tr.classList.add('selected');

			this.selectedVideoId = id;

			var resolution = tr.children[1].textContent;
			var dimensions = resolution.split(" x ");
			this.selectedVideoWidth  = dimensions[0];
			this.selectedVideoHeight = dimensions[1];

			this.selectedVideoTitle  = link.textContent;
			this.selectedImageID     = tr.querySelector("span.image_id").textContent;
			this.selectedVideo       = video;
			this.selectedVideoImage  = image;
			this.selectedVideoFormat = tr.children[2].textContent;
		}

		show(editor)
		{
			this.editor = editor;
			this.dialog = modalPopup("Insert Video", "/action/video/video_picker?Editor=" + this.editor.name, 800, 'auto', true, false);
		}

		hide()
		{
			this.dialog.hide();
		}

		updateUI()
		{
			var button = document.getElementById('insert_video_button');
			if (!button) return;

			button.disabled = !this.selectedVideoId;
		}

		insert()
		{
			var mode       = document.getElementById('insert_mode').value;
			var transcript = document.getElementById('transcript').checked;
			var insertion  = "";

			switch (mode)
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
				insertion = "<a class='video " + this.selectedVideoFormat + "' href='" + this.selectedVideo +
							"' style='display: block; width:" + this.selectedVideoWidth + "px; height: " + this.selectedVideoHeight +
							"px; background-image: url(" + this.selectedVideoImage + "); background-size: cover;' data-width='" + this.selectedVideoWidth + "' data-height='" + this.selectedVideoHeight + "'></a>";
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

			var selection = this.editor.selection;
			selection.setContent(insertion);

			this.hide();
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new VideoPickerSingleton();
	};

})();


function videoLightbox(element, id)
{
	if (typeof element === 'string') element = document.getElementById(element);

	var title = "";
	var tag   = element.tagName.toUpperCase();

	switch (tag)
	{
	case "A":
	case "BUTTON":
		title = element.textContent;
		if (!title) title = element.title;
		break;

	case "IMG":
		title = element.alt;
		break;
	}

	if (!title) title = "Video";

	var w = element.getAttribute('data-width');
	var h = element.getAttribute('data-height');

	if (!w && !h)
	{
		var size       = httpRequest('/action/video/video_size?video_id=' + id);
		var dimensions = size.split('x');
		w = Number(dimensions[0]);
		h = Number(dimensions[1]);
	}

	modalPopup(title, "/action/video/video?video_id=" + id, (w + 16), (h + 52));
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
