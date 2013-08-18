var ImagePicker =  (function()
{
	var ImagePickerSingleton = new Class(
	{
		myLinks: "",
		searchResults: "",
		browse: "",

		selectedTitle: Class.Empty,
		selectedKey: 0,

		originalWidth: 1,
		originalHeight: 1,

		mode: 'insert',
		
		editor: "",
		selectModeField: Class.Empty,
		selectModePreview: Class.Empty,
		
		initialize: function()
		{
		},
		
		show: function(editor)
		{
			this.editor = editor;
			this.dialog = modalPopup("Link Picker", "/action/image_picker/image_picker?Editor=" + this.editor.name, 800, 'auto', true, false); 
		},
		
		hide: function()
		{
			this.dialog.hide();
		},
			
	
		setMode: function( mode, selectModeField, selectModePreview)
		{
			this.mode = mode;
			this.selectModeField = selectModeField;
			this.selectModePreview = selectModePreview;
		},

		showImages: function(gallery_id)
		{
			var request = new Request.HTML(
		    		{
		    			evalScripts: false,
		    			evalResponse: false,
		    			method: 'get', 
		    			url: '/action/image_picker/list_images?gallery_id=' + gallery_id, 
		    			onSuccess: function(tree, elements, html, script) 
		    			{ 
		    				document.id('image_list').set('html', html);
		    				$exec(script);
		    			}.bind(this)
		    		});
		    		request.send();
		},
	
		selectImage: function(image_id, title)
		{
			this.selectedTitle = title;
			this.selectedKey = image_id;
			
			document.id('preview').innerHTML = "<img src='/action/image/thumbnail?image_id=" + image_id + "&size=150' width='150' border='0' alt='" + title + "'/>";
			if (this.mode != "select")
			{
				this.getImageSize();
			}
		},
	
		insertImage: function()
		{
			if (this.selectedKey == 0) return;
			
			if (this.mode == "select")
			{
				var srcdoc = window.opener.document;
		
				srcdoc.getElementById(this.selectModeField).value = this.selectedKey;
				
				var preview = srcdoc.getElementById(this.selectModePreview);
				preview.src = "/action/image/thumbnail?image_id=" + this.selectedKey + "&size=150";
				window.close();
			}
			else
			{   
				var align = 0;
			    var align_elt = document.id('alignment');
			    if(align_elt)
			    	align = align_elt.value;
			    
			    var width = document.id('width');
			    var height = document.id('height');
			    var w = 0;
			    var h = 0;
			    
			    if(width)
			    	w = parseInt(width.value);
			    if(height)
			    	h = parseInt(height.value);	    
			    
			    var size = w > h ? w: h;
			    
			    var src = "/action/image/thumbnail?image_id=" + this.selectedKey + "&size=" + size;
			    
			    var a = (align != "" && align != "inline") ? " align=\"" +align + "\"" : "";
			    
	
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
						
			    var img  = "<img image_id=\"" + this.selectedKey + "\" src=\"" + src + "\" " + s + "border=\"0\"" + " alt=\"" + this.selectedTitle + "\"" + a + ">";
			    
			    if(this.editor)
			    	this.editor.insertContent(img);
			    this.hide();
			}
		},
	
		getImageSize: function()
		{
			var width = document.id('width');
			var height = document.id('height');

			var request = new Request({
					method: 'get', 
	    			url: "/action/image_picker/get_image_size?image_id=" + this.selectedKey, 
	    			onSuccess: function(result) 
	    			{ 
						if (!result.match(/^\d+,\d+/))
						{
							if(width)
								document.id('width').value = "";
							if(height)
								document.id('height').value = "";
						}
			
						var dimensions = result.split(",");
						if(width)
							width.value = dimensions[0];
						if(height)
							height.value = dimensions[1];
						this.originalWidth = dimensions[0];
						this.originalHeight = dimensions[1];
	    			}.bind(this)
			});
			
			request.send();
		},

		changeWidth: function()
		{
			if (!document.id('aspect').checked) return;
			
			var width = document.id('width').value;
			if (width == "") 
			{
				width = 0;
				document.id('width').value = "0";
			}	
			
			document.id('height').value = (width/this.originalWidth * this.originalHeight).toFixed(0);
		},
		
		changeHeight: function()
		{
			if (!document.id('aspect').checked) return;
		
			var height = document.id('height').value;
			if (height == "") 
			{
				height = 0;
				document.id('height').value = "0";
			}	
			
			document.id('width').value = (height/this.originalHeight * this.originalWidth).toFixed(0);
		},
	
		maskInput: function(e)
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
			if ((key==null) || (key==0) || (key==8) ||
				(key==9) || (key==13) || (key==27) )
			{
				return true;
			}
		
			// numbers
			else if ((("0123456789.,").indexOf(keychar) > -1))
			{
				return true;
			}
		
			return false;
		},
	
		uploadImage: function()
		{
			var gallery_id = document.id('gallery_id').value;
			this.popup = modalPopup("Upload Image", "/action/image_picker/image_upload?gallery_id=" + gallery_id, 'auto', 'auto', true);
		},
		
		showSelectImageDialog: function(field)
		{
			this.dialog = modalPopup("Select Image", "/action/image_picker/image_picker?Mode=select&Field=" + field + "&Preview=" + field + "_preview", '625px', 'auto', true);			
		},
		
		setSelectedImage: function(field)
		{
			if (this.selectedKey == 0) return;
	
			var imageField = document.id(field);
			if(imageField)
				imageField.set("value", this.selectedKey);
			
			var preview = document.id(field + '_preview');
			if(preview)
				preview.src = "/action/image/thumbnail?image_id=" + this.selectedKey + "&size=150";
			this.dialog.hide();
		},
		
		clearImage: function(field)
		{
			var imageField = document.id(field);
			if(imageField)
				imageField.set("value", 0);
				
			var preview = document.id(field + '_preview');
			if(preview)
				preview.src = "/fakoli/images/noimage.gif";
			this.dialog.hide();
		},
		
		hideUploadPopup: function()
		{
			this.popup.hide();
		}
	});
		
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
