var ImagePicker = new Class({
	
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
	
	initialize: function(editor, mode, selectModeField, selectModePreview)
	{
		this.editor = editor;
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
	    				$('image_list').set('html', html);
	    				$exec(script);
	    			}.bind(this)
	    		});
	    		request.send();
	},
	
	selectImage: function(image_id, title)
	{
		this.selectedTitle = title;
		this.selectedKey = image_id;
		
		$('preview').innerHTML = "<img src='/action/image/thumbnail?image_id=" + image_id + "&size=150' width='150' border='0' alt='" + title + "'/>";
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
		    var align = $('alignment').value;
		    
		    var w = $('width').value;
		    var h = $('height').value;	    
		    
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
		    
		    var editor = window.opener.theEditors[this.editor];
		    editor.insertAtSelection(img);
		    window.close();
		}
	},
	
	getImageSize: function()
	{
		var request = new Request({
				method: 'get', 
    			url: "/action/image_picker/get_image_size?image_id=" + this.selectedKey, 
    			onSuccess: function(result) 
    			{ 
					if (!result.match(/^\d+,\d+/))
					{
						$('width').value = "";
						$('height').value = "";
					}
		
					var dimensions = result.split(",");
					$('width').value = dimensions[0];
					$('height').value = dimensions[1];
					this.originalWidth = dimensions[0];
					this.originalHeight = dimensions[1];
    			}.bind(this)
		});
		
		request.send();
	},

	changeWidth: function()
	{
		if (!$('aspect').checked) return;
		
		var width = $('width').value;
		if (width == "") 
		{
			width = 0;
			$('width').value = "0";
		}	
		
		$('height').value = (width/this.originalWidth * this.originalHeight).toFixed(0);
	},
	
	changeHeight: function()
	{
		if (!$('aspect').checked) return;
	
		var height = $('height').value;
		if (height == "") 
		{
			height = 0;
			$('height').value = "0";
		}	
		
		$('width').value = (height/this.originalHeight * this.originalWidth).toFixed(0);
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
		var gallery_id = $('gallery_id').value;
		this.popup = modalPopup("Upload Image", "/action/image_picker/image_upload?gallery_id=" + gallery_id, 'auto', 'auto', true);
	},
	
	hideUploadPopup: function()
	{
		this.popup.hide();
	}
});
