var ImagePicker = (function()
{
	class ImagePickerSingleton
	{
		constructor()
		{
			this.myLinks = "";
			this.searchResults = "";
			this.browse = "";
			this.selectedTitle = null;
			this.selectedKey = 0;
			this.originalWidth = 1;
			this.originalHeight = 1;
			this.mode = 'insert';
			this.editor = "";
			this.selectModeField = null;
			this.selectModePreview = null;
		}

		show(editor)
		{
			this.editor = editor;
			var title = this.mode == 'insert' ? "Insert an Image" : "Select an Image";
			this.dialog = modalPopup(title, "/action/image_picker/image_picker?Editor=" + this.editor.name, 800, 'auto', true, false);
		}

		hide()
		{
			this.dialog.hide();
		}

		setMode(mode, selectModeField, selectModePreview)
		{
			this.mode = mode;
			this.selectModeField = selectModeField;
			this.selectModePreview = selectModePreview;
		}

		showImages(gallery_id)
		{
			fetch('/action/image_picker/list_images?gallery_id=' + gallery_id)
				.then(r => r.text())
				.then((html) =>
				{
					document.getElementById('image_list').innerHTML = html;
				});
		}

		selectImage(image_id, title)
		{
			this.selectedTitle = title;
			this.selectedKey = image_id;

			document.getElementById('image_picker_preview').innerHTML = "<img src='/action/image/iconize?image_id=" + image_id + "&size=150' width='150' border='0' alt='" + title + "'/>";
			if (this.mode != "select")
			{
				this.getImageSize();
			}
		}

		insertImage()
		{
			if (this.selectedKey == 0) return;

			if (this.mode == "select")
			{
				var srcdoc = window.opener.document;

				srcdoc.getElementById(this.selectModeField).value = this.selectedKey;

				var preview = srcdoc.getElementById(this.selectModePreview);
				preview.src = "/action/image/iconize?image_id=" + this.selectedKey + "&size=150";
				window.close();
			}
			else
			{
				var align = 0;
				var align_elt = document.getElementById('alignment');
				if (align_elt)
					align = align_elt.value;

				var width = document.getElementById('width');
				var height = document.getElementById('height');
				var w = 0;
				var h = 0;

				if (width)
					w = parseInt(width.value);
				if (height)
					h = parseInt(height.value);

				var size = w > h ? w : h;

				var src = "/action/image/thumbnail?image_id=" + this.selectedKey + "&size=" + size;

				var a = (align != "" && align != "inline") ? " align=\"" + align + "\"" : "";

				var s = "";

				if (w != "" || h != "")
				{
					s = "style=\"";

					if (w != "")
					{
						s += "width: " + w + "px;";
					}

					if (h != "")
					{
						s += "height: " + h + "px;";
					}

					s += "\" ";
				}

				var img = "<img image_id=\"" + this.selectedKey + "\" src=\"" + src + "\" " + s + "border=\"0\"" + " alt=\"" + this.selectedTitle + "\"" + a + ">";

				if (this.editor)
					this.editor.insertContent(img);
				this.hide();
			}
		}

		getImageSize()
		{
			var width = document.getElementById('width');
			var height = document.getElementById('height');

			fetch("/action/image_picker/get_image_size?image_id=" + this.selectedKey)
				.then(r => r.text())
				.then((result) =>
				{
					if (!result.match(/^\d+,\d+/))
					{
						if (width)
							document.getElementById('width').value = "";
						if (height)
							document.getElementById('height').value = "";
					}

					var dimensions = result.split(",");
					if (width)
						width.value = dimensions[0];
					if (height)
						height.value = dimensions[1];
					this.originalWidth = dimensions[0];
					this.originalHeight = dimensions[1];
				});
		}

		changeWidth()
		{
			if (!document.getElementById('aspect').checked) return;

			var width = document.getElementById('width').value;
			if (width == "")
			{
				width = 0;
				document.getElementById('width').value = "0";
			}

			document.getElementById('height').value = (width / this.originalWidth * this.originalHeight).toFixed(0);
		}

		changeHeight()
		{
			if (!document.getElementById('aspect').checked) return;

			var height = document.getElementById('height').value;
			if (height == "")
			{
				height = 0;
				document.getElementById('height').value = "0";
			}

			document.getElementById('width').value = (height / this.originalHeight * this.originalWidth).toFixed(0);
		}

		maskInput(e)
		{
			var key;
			var keychar;

			if (window.event)
			{
				key = window.event.keyCode;
			}
			else if (e)
			{
				key = e.which;
			}
			else
			{
				return true;
			}

			keychar = String.fromCharCode(key);

			// control keys
			if ((key == null) || (key == 0) || (key == 8) ||
				(key == 9) || (key == 13) || (key == 27))
			{
				return true;
			}

			// numbers
			else if ((("0123456789.,").indexOf(keychar) > -1))
			{
				return true;
			}

			return false;
		}

		uploadImage()
		{
			var gallery_id = document.getElementById('gallery_id').value;
			this.popup = modalPopup("Upload Image", "/action/image_picker/image_upload?gallery_id=" + gallery_id, 'auto', 'auto', true);
		}

		showSelectImageDialog(field)
		{
			this.dialog = modalPopup("Select Image", "/action/image_picker/image_picker?Mode=select&Field=" + field + "&Preview=" + field + "_preview", '625px', 'auto', true);
		}

		setSelectedImage(field)
		{
			if (this.selectedKey == 0) return;

			var imageField = document.getElementById(field);
			if (imageField)
				imageField.value = this.selectedKey;

			var preview = document.getElementById(field + '_preview');
			if (preview)
				preview.src = "/action/image/thumbnail?image_id=" + this.selectedKey + "&size=150";
			this.dialog.hide();
		}

		clearImage(field)
		{
			var imageField = document.getElementById(field);
			if (imageField)
				imageField.value = 0;

			var preview = document.getElementById(field + '_preview');
			if (preview)
				preview.src = "/fakoli/images/noimage.gif";
			this.dialog.hide();
		}

		hideUploadPopup()
		{
			this.popup.hide();
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new ImagePickerSingleton();
	};

})();

function imagePicker(editor)
{
	new ImagePicker().show(editor);
}

function closeLinkPicker()
{
	new ImagePicker().hide();
}
