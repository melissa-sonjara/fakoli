
/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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

function Button(parent, name, image, tooltip, fn)
{
	this.parent = parent;
	this.name = name;
	this.image = image;
	this.tooltip = tooltip;
	this.fn = fn;
	
	this.toHTML = function()
	{

		var doc = "<img id='" + this.parent.getID() + "_" + this.name + 
				  "' src='" + this.getImageURI() + "' " +
				  " class='rteButton' alt='" + tooltip + 
				  "' title='" + tooltip + 
				  "' onclick='theEditors[\"" + this.parent.name + "\"]." + this.fn + "();'>";
		
		return doc;
	};
	
	this.getImageURI = function()
	{
		if (this.image.substr(0, 1) == "/") return this.image;
		if (this.image.substr(0, 7) == "http://") return this.image;
		return this.parent.getImagePath() + this.image;
	};
}

function Separator(parent)
{
	this.parent = parent;

	this.toHTML = function()
	{
		return "<img class='rteSeparator' src='" + this.parent.getImagePath() + "separator.gif' border='0' alt=''>";
	};
}

function DropDownEntry(text, value, style, cl, def)
{
	this.text = text;
	this.value = value;
	this.style = style;
	this.cl = cl;
	this.def = def;
	
	this.toHTML = function()
	{
		var doc = "<option ";
		if (this.cl) doc += "class='" + this.cl + "' ";
		if (this.style) doc += "style='" + this.style + "' ";
		//if (this.def) doc += "selected ";
		doc += "value='" + this.value + "'>" + this.text + "</option>";
		
		return doc;
	};
}

function DropDown(parent, title, name, fn, entries)
{
	this.parent = parent;
	this.name = name;
	this.entries = entries;
	this.fn = fn;
	
	this.toHTML = function()
	{
		var doc = "<select name='" + this.parent.name + "_" + this.name + 
				  "' id='" + this.parent.name + "_" + this.name + 
				  "' onchange='theEditors[\"" + this.parent.name + "\"]." + this.fn + "()' style='height:20px'>";
		doc += "<option value='' selected>" + title + "</option>";
		
		for(var i = 0 ; i < this.entries.length ; ++i)
		{
			doc += this.entries[i].toHTML();
		}
		
		doc += "</select>";
		
		return doc;
	};
	
	this.clearSelection = function()
	{
	    var ctrl = document.getElementById(this.parent.name + "_" + this.name);
	    if (ctrl == null) return;
	    
	    for(i = 1; i < ctrl.options.length; ++i)
	    {
	        ctrl.options[i].selected = false;
	    }
	    
	    ctrl.options[0].selected = true;
	};
}

var isIE;
var isGecko;
var isSafari;
var isKonqueror;
var ua = navigator.userAgent.toLowerCase();

var rteCSSpath = "";
var rteImagePath = "";
var rteRoot = "";

function setRichTextEnv(cssPath, imagePath, root)
{
	rteCSSPath = cssPath;
	rteImagePath = imagePath;
	rteRoot = root;
}

isIE = ((ua.indexOf("msie") != -1) && (ua.indexOf("opera") == -1) && (ua.indexOf("webtv") == -1)); 
isGecko = (ua.indexOf("gecko") != -1);
isSafari = (ua.indexOf("safari") != -1);
isKonqueror = (ua.indexOf("konqueror") != -1);
	
var styleCache = [];
	
function getStyles()
{
	if (styleCache.length > 0) return;
	
	if (isIE)
	{
		ieGetStyles();
	}
	else if (isGecko)
	{
		geckoGetStyles();
	}
}

function ieGetStyles()
{
	//alert("ieGetStyles()");
}

function geckoGetStyles()
{
	for(var i =0; i < document.styleSheets.length; ++i)
	{
		var sheet = document.styleSheets[i];
		
		var rules = sheet.cssRules;
		
		for(var j = 0; j < rules.length; ++j)
		{
			if (rules[j].selectorText)
			{
				var patterns = rules[j].selectorText.split(",");
				var style = rules[j].style.cssText;
				
				for(var k = 0; k < patterns.length; ++k)
				{
					styleCache[patterns[k].toLowerCase()] = style;
				}
			}
		}
	}
}

function getStyle(pattern)
{
	getStyles();

	var style = styleCache[pattern.toLowerCase()];
	if (!style) style = "";
	return style;
}

// Keep a record of the editors on the page, indexed by name (ID)

var theEditors = [];

function prepareAllEditorsForSubmit()
{
    //alert(theEditors.length);
    
    for(editor in theEditors)
    {
        theEditors[editor].prepareForSubmit();
    }
}

function geckoKeyPress(evt)
{
	theEditors[evt.target.id].onGeckoKeyPress(evt);
}

function selectionChangedCallback(editor)
{
    theEditors[editor].onSelectionChanged();
}
    
function RichTextEditor(name, clientID, html, width, height, buttons, readOnly)
{
	this.name	      = name;
	this.clientID     = clientID;
	this.html	      = html;
	this.width        = width;
	this.height       = height;
	this.buttons      = buttons;
	this.readOnly     = readOnly;
	this.baseHref     = "";
	this.cssFile      = "";
	this.showStyleBar = true;
	
	if (arguments.length > 7)
	{
		this.cssFile = arguments[7];
	}
	
	this.hasSelection = false;
	
	this.toolbar = [
					 	new Button(this, 'bold', 'bold.gif', 'Bold', 'onBold'),
						new Button(this, 'italic', 'italic.gif', 'Italic', 'onItalic'),
						new Button(this, 'underline', 'underline.gif', 'Underline', 'onUnderline'),
						new Separator(this),
						new Button(this, 'alignleft', 'left_just.gif', 'Align Left', 'onAlignLeft'),
						new Button(this, 'aligncenter', 'centre.gif', 'Center', 'onAlignCenter'),
						new Button(this, 'alignright', 'right_just.gif', 'Align Right', 'onAlignRight'),
						new Button(this, 'justify', 'justifyfull.gif', 'Justify', 'onJustify'),
						new Separator(this),
						new Button(this, 'hr', 'hr.gif', 'Horizontal Rule', 'onHorizontalRule'),
						new Separator(this),
						new Button(this, 'orderedlist', 'numbered_list.gif', 'Numbered List', 'onNumberedList'),
						new Button(this, 'bulletList', 'list.gif', 'Bullet List', 'onBulletList'),
						new Separator(this),
						new Button(this, 'outdent', 'outdent.gif', 'Decrease Indent', 'onOutdent'),
						new Button(this, 'indent', 'indent.gif', 'Increase Indent', 'onIndent'),
						/*new Button(this, 'textcolor', 'textcolor.gif', 'Text Color', 'onTextColor'),
						new Button(this, 'backcolor', 'bgcolor.gif', 'Background Color', 'onBackgroundColor'),*/
						new Separator(this),
						new Button(this, 'clear', 'clear.gif', 'Clear All Formatting', 'onClearFormatting'),
						new Button(this, 'link', 'hyperlink.gif', 'Insert Link', 'onInsertLink'),
						new Button(this, 'image', 'image.gif', 'Insert Image', 'onInsertImage')/*,
						new Button(this, 'table', 'insert_table.gif', 'Insert Table', 'onInsertTable')*/
					];
	
	this.stylebar =
				[

					new DropDown(this, 'Font', 'font', 'onFontChange',
					[
						new DropDownEntry('Arial', 'Arial', 'font-family: Arial; font-size:10pt', '', true),
						new DropDownEntry('Courier New', 'Courier New', 'font-family: Courier New; font-size:10pt', '', false),
						new DropDownEntry('Tahoma', 'Tahoma', 'font-family: Tahoma; font-size:10pt', '', false),
						new DropDownEntry('Times New Roman', 'Times New Roman', 'font-family: Times New Roman; font-size:10pt', '', false),
						new DropDownEntry('Trebuchet MS', 'Trebuchet MS', 'font-family: Trebuchet MS; font-size:10pt', '', false),
						new DropDownEntry('Verdana', 'Verdana', 'font-family: Verdana; font-size:10pt', '', false)
					]),
					
					new DropDown(this, 'Size', 'fontsize', 'onFontChange',
					[
						new DropDownEntry('6pt', '6pt', '', '', false),
						new DropDownEntry('8pt', '8pt', '', '', false),
						new DropDownEntry('10pt', '10pt', '', '', true),
						new DropDownEntry('12pt', '12pt', '', '', false),
						new DropDownEntry('14pt', '14pt', '', '', false),
						new DropDownEntry('16pt', '16pt', '', '', false),
						new DropDownEntry('24pt', '24pt', '', '', false),
						new DropDownEntry('36pt', '36pt', '', '', false)
					]),
					new DropDown(this, 'Style', 'style', 'onStyleChange',
					[
						new DropDownEntry('Paragraph', '<p>', '', '', false),
						new DropDownEntry('Heading 1', '<h1>', '', '', true),
						new DropDownEntry('Heading 2', '<h2>', '', '', false),
						new DropDownEntry('Heading 3', '<h3>', '', '', false),
						new DropDownEntry('Heading 4', '<h4>', '', '', false)
					])					
				];
						
	//this.draw();
	//this.enableDesignMode();

	theEditors[this.name] = this;
		
	this.draw = function()
    {
        var w = this.width;
        if (this.width.indexOf("%") == -1)
        {
            w = (getNumericSize(this.width) + 4) + "px";
        }
        
        var doc = "<div class='rte'";
        
        if (this.width == "100%")
        {
    		doc += ">";
    	}
    	else
    	{
    	    doc += " style='width: " + w + "'>";
    	}
    	
    	if (this.showStyleBar)
    	{
			doc += this.drawToolbar(this.stylebar, "stylebar");
		}
		
		doc += this.drawToolbar(this.toolbar, "toolbar");
		doc += this.drawEditor();
		doc += "</div>";
		//alert(doc);
		document.write(doc);
	};
		
	this.drawToolbar = function(toolbar, toolbarName)
	{
	    if (this.readOnly) return "";
	    
		var doc = "<div class='rteToolbar' id='" + this.name + "_" + toolbarName + "'>";
				
		for(i = 0; i < toolbar.length; ++i)
		{
			var t = toolbar[i].toHTML(); 
			//alert(i + ": " + t);
			doc += t;
		}
		
		doc += "</div>";
		
		return doc;
	};

	this.setToolbarVisibility = function(toolbarName, state)
	{
		var toolbarDiv = document.getElementById(this.name + "_" + toolbarName);
		if (toolbarDiv)
		{
			toolbarDiv.style.visibility = state;
		}
	};
			   		
	this.drawEditor = function()
	{
		var doc = "<div><iframe id='" + this.name + "' name='" + this.name + "' width='" + width + "' height='" + height + 
				  "' src='" + rteRoot + "blank.htm' style='border: solid 1px black'></iframe></div>";
		doc += '<textarea id="' + this.clientID + '" name="' + this.clientID + '" style="display:none"></textarea>';
		if (!this.readOnly) 
		{
			doc += '<br><input type="checkbox" id="chkSrc' + this.name + '" onclick="theEditors[\'' + this.name + '\'].toggleHTMLSrc(\'' + this.name + '\',' + buttons + ');" />&nbsp;<label for="chkSrc' + this.name + '">View Source</label>';
		}
		
		doc += '<iframe width="154" height="104" id="cp' + this.name + '" src="palette.htm" marginwidth="0" marginheight="0" scrolling="no" style="visibility:hidden; position: absolute;"></iframe>';
		doc += '</div>';
	
		return doc;
	};
	
	
	this.enableDesignMode = function() 
	{
		var frameHtml = "<html id=\"" + this.name + "\">\n";
		frameHtml += "<head>\n";
		frameHtml += '<meta http-equiv="content-type" content="text/html; charset=ISO-8859-1" />';
		
		//to reference your stylesheet, set href property below to your stylesheet path and uncomment
		if (this.baseHref.length > 0)
		{
			frameHtml += "<base href=\"" + this.baseHref + "\">";
		}
		
		if (this.cssFile) 
		{
			var css = this.cssFile.split(",");
			for(var i = 0; i < css.length; ++i)
			{
				frameHtml += "<link media=\"all\" type=\"text/css\" href=\"" + css[i] + "\" rel=\"stylesheet\">\n";
			}
		}
		// else {
		frameHtml += "<style>\n";
		frameHtml += "body {\n";
		frameHtml += "	background: #FFFFFF;\n";
		frameHtml += "	margin: 0px;\n";
		frameHtml += "	padding: 0px;\n";
		frameHtml += "}\n";
		frameHtml += "</style>\n";
		
		if (this.readOnly)
		{
		    frameHtml += "<script type='text/javascript'>\ndocument.onselectstart= function() { return false; };\n";
		    frameHtml += "if (window.sidebar) { document.onmousedown=function() { return false; }; document.onclick=function(){return true; }; }\n";
		    frameHtml += "</script>\n";		    
		}
		//}
		frameHtml += "</head>\n";
		frameHtml += "<body>\n";
		frameHtml += this.html + "\n";
		frameHtml += "</body>\n";
		frameHtml += "</html>";
	
		if (document.all) 
		{
			var oRTE = this.findFrame().document;
			//alert(oRTE);
			oRTE.charset = "ISO-8859-1";
			oRTE.open();
			oRTE.write(frameHtml);
			oRTE.close();
			if (!this.readOnly) 
			{
				oRTE.designMode = "On";
				//this.findFrame().document.attachEvent("onkeypress", this.evt_ie_keypress = function(event) {ieKeyPress(event, rte);});
			}
			
			var name = this.name;
			var self = this;
			
			oRTE.onselectionchange = function() { selectionChangedCallback(name); };
			setTimeout(function() 
			{ 	
				oRTE.body.onbeforepaste = function(evt) { self.ieBeforePaste(evt); };
				oRTE.body.onpaste = function() { self.iePaste(); };
			}, 1000);
		} 
		else 
		{
			try 
			{
				if (!this.readOnly) document.getElementById(this.name).contentDocument.designMode = "on";
				
				try 
				{
					var oRTE = document.getElementById(this.name).contentWindow.document;
					
					//alert(oRTE);
					oRTE.open();
					oRTE.write(frameHtml);
					oRTE.close();
					if (isGecko && !this.readOnly) 
					{
						//attach a keyboard handler for gecko browsers to make keyboard shortcuts work
						oRTE.addEventListener("keypress", geckoKeyPress, true);
					}
				} 
				catch (e) 
				{
					alert("Error preloading content.");
				}
			} 
			catch (e) 
			{
				//gecko may take some time to enable design mode.
				//Keep looping until able to set.
				if (isGecko) 
				{
					setTimeout(this.name + ".enableDesignMode();", 10);
				} 
				else 
				{
					return false;
				}
			}
		}
	};
	
	this.findFrame = function()
	{
		return document.frames(this.name);
	};
	
	this.toggleHTMLSrc = function() 
	{
		var oHdnField = document.getElementById(this.clientID);
		var toolbar = document.getElementById(this.name + "_toolbar");
		var stylebar = document.getElementById(this.name + "_stylebar");
		var rte = document.getElementById(this.name);
		var src = document.getElementById(this.clientID);
		var chkSrc = document.getElementById("chkSrc" + this.name);
		if (chkSrc.checked) 
		{
			this.setToolbarVisibility("stylebar", "hidden");
			this.setToolbarVisibility("toolbar", "hidden");
			
			var width = rte.clientWidth;
			var height = rte.clientHeight;
			
			rte.parentNode.style.display="none";
			this.setHiddenVal();
			this.relativizeLinks();
            src.style.display="block";
            
            src.style.width = width + "px";
            src.style.height = height + "px";
		} 
		else 
		{
			this.setToolbarVisibility("stylebar", "visible");
			this.setToolbarVisibility("toolbar", "visible");
			
			rte.parentNode.style.display="block";
            src.style.display="none";
            	
            this.setHTML(src.value);
		}
	};

	this.getEditor = function()
	{
		if (document.all)
		{
			return this.findFrame().document;
		}
		else
		{
			return document.getElementById(this.name).contentWindow.document;
		}
	};
	
    this.setHTML = function(html)
    {
		if (document.all) 
		{
			//fix for IE
			var output = escape(html);
			output = output.replace("%3CP%3E%0D%0A%3CHR%3E", "%3CHR%3E");
			output = output.replace("%3CHR%3E%0D%0A%3C/P%3E", "%3CHR%3E");
			this.findFrame().document.body.innerHTML = unescape(output);
		} 
		else 
		{
			var oRTE = document.getElementById(this.name).contentWindow.document;
			oRTE.body.innerHTML = html;
		}
    };
    
	this.setHiddenVal = function() 
	{
		//set hidden form field value for current rte
		var oHdnField = document.getElementById(this.clientID);
		
		if (oHdnField.value == null) oHdnField.value = "";
		
		if (document.all) 
		{
    		oHdnField.value = this.findFrame().document.body.innerHTML;
		} 
		else 
		{
			oHdnField.value = document.getElementById(this.name).contentWindow.document.body.innerHTML;
		}
		
		//alert(oHdnField.value);
		
		//if there is no content (other than formatting) set value to nothing
		if (this.stripHTML(oHdnField.value.replace("&nbsp;", " ")) == "" &&
			oHdnField.value.toLowerCase().search("<hr") == -1 &&
			oHdnField.value.toLowerCase().search("<img") == -1) oHdnField.value = "";
	};

	this.stripHTML = function(oldString) 
	{
		//function to strip all html
		var newString = oldString.replace(/(<([^>]+)>)/ig,"");
		
		//replace carriage returns and line feeds
	    newString = newString.replace(/\r\n/g," ");
	    newString = newString.replace(/\n/g," ");
	    newString = newString.replace(/\r/g," ");
		
		//trim string
		newString = this.trim(newString);
		
		return newString;
	};
		
	this.trim = function(inputString) 
	{
	   	// Removes leading and trailing spaces from the passed string. Also removes
	   	// consecutive spaces and replaces it with one space. If something besides
	   	// a string is passed in (null, custom object, etc.) then return the input.
	   	if (typeof inputString != "string") return inputString;
	   	var retValue = inputString;
	   	var ch = retValue.substring(0, 1);
		
	   	while (ch == " ") 
	   	{ 
	   		// Check for spaces at the beginning of the string
	      	retValue = retValue.substring(1, retValue.length);
	      	ch = retValue.substring(0, 1);
	   	}
	   	ch = retValue.substring(retValue.length - 1, retValue.length);
		
	   	while (ch == " ") 
	   	{ 
	   		// Check for spaces at the end of the string
	      	retValue = retValue.substring(0, retValue.length - 1);
	      	ch = retValue.substring(retValue.length - 1, retValue.length);
	   	}
		
		// Note that there are two spaces in the string - look for multiple spaces within the string
	   	while (retValue.indexOf("  ") != -1) 
		{
			// Again, there are two spaces in each of the strings
	    	retValue = retValue.substring(0, retValue.indexOf("  ")) + retValue.substring(retValue.indexOf("  ") + 1, retValue.length);
	   	}
	   	return retValue; // Return the trimmed string back to the user
	};

	this.rteCommand = function(command, option) 
	{
		//function to perform command
		var oRTE;
		if (document.all) 
		{
			oRTE = this.findFrame();
		} 
		else 
		{
			oRTE = document.getElementById(this.name).contentWindow;
		}
		
		try 
		{
			oRTE.focus();
		  	oRTE.document.execCommand(command, false, option);
			oRTE.focus();
		} 
		catch (e) 
		{
	//		alert(e);
	//		setTimeout("rteCommand('" + rte + "', '" + command + "', '" + option + "');", 10);
		}
	};

	this.onBold = function()
	{
		this.rteCommand('bold', '');
	};
	
	this.onItalic = function()
	{
		this.rteCommand('italic', '');
	};
	
	this.onUnderline = function()
	{
		this.rteCommand('underline', '');
	};
		
	this.onAlignLeft = function()
	{
		this.rteCommand('justifyleft', '');
	};
	
	this.onAlignCenter = function()
	{
		this.rteCommand('justifycenter', '');
	};
		
	this.onAlignRight = function()
	{
		this.rteCommand('justifyright', '');
	};
		
	this.onJustify = function()
	{
		this.rteCommand('justifyfull', '');
	};
	
	this.onHorizontalRule = function()
	{
		this.rteCommand('inserthorizontalrule', '');
	};
	
	this.onNumberedList = function()
	{
		this.rteCommand('insertorderedlist', '');
	};
	
	this.onBulletList = function()
	{
		this.rteCommand('insertunorderedlist', '');
	};
	
	this.onOutdent = function()
	{
		this.rteCommand('outdent', '');
	};
	
	this.onIndent = function()
	{
		this.rteCommand('indent', '');
	};
	
	this.onInsertLink = function()
	{
	    var url = prompt("Enter a URL: ", "");
	    if (url)
	    {
    	    this.formatSelection("<a href=\"" + url + "\">");
    	}
	};
	
	this.onInsertImage = function()
	{
	    var url = prompt("Enter an Image URL: ", "");
	    if (url)
	    {
    	    this.insertAtSelection("<img border=\"0\" src=\"" + url + "\">");
    	}
	};
		
	this.onFontChange = function()
	{
		var font = document.getElementById(this.name + "_font").value;
		var size = document.getElementById(this.name + "_fontsize").value;
		if (font == "" && size == "") return;
		
		var block = "<span style='";
		if (font != "") block += "font-family: " + font + ";";
		if (size != "") block += " font-size: " + size;
		block += "'>";
		this.formatSelection(block);
		this.clearStyleBar();
	};
	
	this.onStyleChange = function()
	{
		var block = document.getElementById(this.name + "_style").value;
		if (block == "") return;
		//this.rteCommand('formatblock', block);
		//this.cleanCloseTags();
		this.formatSelection(block);
		this.clearStyleBar();
	};
	
	this.clearStyleBar = function()
	{
	    for(dropdown in this.stylebar)
	    {
	        this.stylebar[dropdown].clearSelection();
	    }
	};
	    
	this.onSelectionChanged = function()
	{
	    this.hasSelection = true;
	};
	    
	this.formatSelection = function(tag)
	{
		if (window.getSelection)
		{
			var elt = this.parseTag(tag);
			var selection = document.getElementById(this.name).contentWindow.getSelection();
			
			selection.getRangeAt(0).surroundContents(elt);
		}
		else
		{
			var selection = this.findFrame().document.selection;
			var range = selection.createRange();
			var html = range.htmlText;
			html = tag + html;
			
			var tagPattern = new RegExp("<(\\w+)");
			var matches = tagPattern.exec(tag);
			html += "</" + matches[1] + ">";
			range.pasteHTML(html);
		}
	};
	
	this.insertAtSelection = function(tag)
	{
		if (window.getSelection)
		{
			var elt = this.parseTag(tag);
			var selection = document.getElementById(this.name).contentWindow.getSelection();
			
			var range = selection.getRangeAt(0);
			range.deleteContents();
			range.insertNode(elt);
		}
		else
		{
		    var doc = this.findFrame().document;
		    var selection = doc.selection;
		 
		    //Workaround for bug in IE8
		    doc.body.focus();
		    
		    if (!this.hasSelection)
		    {
		        // No insertion point - Insert at beginning of document
		        doc.body.innerHTML = tag + doc.body.innerHTML;
		    }
		    else
		    { 
			    var range = selection.createRange();
			    range.pasteHTML(tag);
			}
		}
	};
	
	this.parseTag = function(tag)
	{
		var tagPattern = new RegExp("<(\\w+)\\s*(.*)>", "g");
		var attrPattern = new RegExp("(\\w+)=[\"'](.*?)[\"']");
        		
		var matches = tagPattern.exec(tag);
		
		var elt = null;
		
		if (matches == null || matches.length == 0) return null;
		
		elt = document.createElement(matches[1]);
		
		var attrs = matches[2].match(/\w+=[\"'].*?[\"']/g);
		
    	if (attrs)
		{
			for(i = 0; i < attrs.length; ++i)
			{
				var attrMatches = attrPattern.exec(attrs[i]);
				elt.setAttribute(attrMatches[1], attrMatches[2]);
			}
		}
		
		var content = tag.match(/^<.*?>(.*)<\/.*>$/);		
		if (content) elt.innerHTML = content[1];
		
		return elt;
	};
	
	this.cleanCloseTags = function()
	{
		var pattern = /<\/(\w*?)\s+.*?>/g;
		
		if (document.all)
		{
		}
		else
		{
			var html = document.getElementById(this.name).contentWindow.document.body.innerHTML;
			//alert(html);
			html = html.replace(pattern, "</$1>");
			//alert(html);
			document.getElementById(this.name).contentWindow.document.body.innerHTML = html;
		}
	};
	
	this.onTextColor = function()
	{
	
	};
	
	this.getID = function()
	{
		return this.name;
	};
	
	this.getImagePath = function()
	{
		return (rteImagePath != "") ? rteImagePath : "images/";
	};
	
	this.onGeckoKeyPress = function(evt)
	{
		if (evt.ctrlKey) 
		{
			var key = String.fromCharCode(evt.charCode).toLowerCase();
			var cmd = '';
			switch (key) 
			{
				case 'b': cmd = "bold"; break;
				case 'i': cmd = "italic"; break;
				case 'u': cmd = "underline"; break;
				case 'v': this.onGeckoPaste(evt); return;
			};
	
			if (cmd) 
			{
				this.rteCommand(cmd, null);
				
				// stop the event bubble
				evt.preventDefault();
				evt.stopPropagation();
			}
	 	}		
	};
	
	this.onGeckoPaste = function(evt)
	{
		this.insertCleaningContainer();
		var self = this;
		setTimeout(function() { self.cleanPaste(); }, 50);
	};
	
	this.ieBeforePaste = function()
	{
		var oRTE = this.getEditor();
		var cleaner = oRTE.getElementById('HTMLCleansingNode');
		if (cleaner) cleaner.parentNode.removeChild(cleaner);
		
		this.insertCleaningContainer();
	};
	
	this.iePaste = function()
	{
		var self = this;
		setTimeout(function() { self.cleanPaste(); }, 50);
	};
	
	this.cleanPaste = function()
	{
		var oRTE = this.getEditor();
		var cleaner = oRTE.getElementById('HTMLCleansingNode');
		var pure = this.cleanHTML(cleaner.innerHTML);
		var newText = document.createElement('span');
		newText.innerHTML = pure;

		if (document.all)
		{
			cleaner.parentNode.removeChild(cleaner);
			this.insertAtSelection(pure);
		}
		else
		{
			cleaner.parentNode.replaceChild(newText, cleaner);
		}		
	};
	
	this.insertCleaningContainer = function()
	{
		this.insertAtSelection("<div id='HTMLCleansingNode'>_</div>");
		var oRTE = this.getEditor();
		this.selectNodeContents(oRTE.getElementById('HTMLCleansingNode'));
	};
	
	this.onClearFormatting = function()
	{
	    this.setHiddenVal();
	    var oHdnField = document.getElementById(this.clientID);
	    var text = this.cleanHTML(oHdnField.value);
	    oHdnField.value = text;
	    this.setHTML(oHdnField.value);
	};
	    
	this.cleanHTML = function(text)
	{
		text = text.replace(/<\!--\s*\[if.*?supportLists\s*\]-->(.*?)<\!--\[endif\]\s*-->/gi, "$1");
		text = text.replace(/<\!--\s*\[if(?:.|[\n\r])*?<\!\[endif\]\s*-->/gi, "");
	    text = text.replace(/<style.*?>(?:.|[\n\r])*?<\/style>/gi, "");
	    text = text.replace(/<script.*?>(?:.|[\n\r])*?<\/script>/gi, "");
		text = text.replace(/<link(?:.|[\n\r])*?>/gi, "");

	    text = text.replace(/<(\/?(?:p|a[^>]*|h\d|ul|ol|li|br|hr|table|tbody|thead|tfoot|tr|td)).*?>/gi, "[_*_[$1]_*_]");
	    text = text.replace(/<.*?>/gi, "");
	    text = text.replace(/\[_\*_\[/gi, "<");
	    text = text.replace(/\]_\*_\]/gi, ">");
	    text = text.replace(/<p>\s*<\/p>/g, "");
	    text = text.replace(/<table>/, "<table class='list'>");
	    return text;
	};
	
	this.selectNodeContents = function(node)
	{
		if (!node) return false;
		
		var oRTE = this.getEditor();

		if (window.getSelection)
		{
			range = oRTE.createRange();
			range.selectNodeContents(node);
			var selection = document.getElementById(this.name).contentWindow.getSelection();
			selection.removeAllRanges();
			selection.addRange(range);
		}
		else
		{
			try
			{
				range = oRTE.body.createTextRange();
				range.moveToElementText(node);
				range.select();
			}
			catch(e)
			{
			}
		}
	};
	
	this.prepareForSubmit = function()
	{
		var chkSrc = document.getElementById("chkSrc" + this.name);
		if (!chkSrc.checked) 
		{
		    this.setHiddenVal();
		}
		
		this.relativizeLinks();
	};
	
	this.relativizeLinks = function()
	{
	    // If there is a Base HREF defined, we should convert any absolute
	    // links to that path into relative links before saving. The absolute
	    // links are introduced by the browser when pasting HTML into the RTE.
	    
	    if (this.baseHref != "")
	    {
	        var oHdnField = document.getElementById(this.clientID);
	        var text = oHdnField.value;
	        
	        var expr = this.baseHref.replace(/(\/|\.)/g, "\$1");
	        
	        expr = "(href|src)\\s*=\\s*([\"'])" + expr;
	        
	        if (expr.charAt(expr.length - 1) == '/')
	        {
	            expr = expr.substring(0, expr.length - 1);
	        }
	        var regex = new RegExp(expr, "gi");
	        text = text.replace(regex, "$1=$2");
	        
	        oHdnField.value = text;
	    }
	};
	
	this.addToolbarButton = function(name, image, tooltip, url, width, height)
	{
	    var fn = "onButton" + name;
	    url += (url.indexOf("?") > -1) ? "&" : "?";
	    url += "Editor=" + this.name;
	    
	    this[fn] =  function() 
	                { 
	                    var w = window.open(url, name, 
	                                        'width='+width+',height='+height+',toolbar=0,scrollbars=1,resizable=1,status=0');
	                    w.focus();
	                };
	
        this.toolbar.push(new Button(this, name, image, tooltip, fn));
	};
	
	this.addStyle = function(name, defn)
	{
	    this.stylebar[2].entries.push(new DropDownEntry(name, defn, '', '', false));
	};
	
	this.toString = function()
	{
	    var str = "[RichTextEditor]:\nname = " + this.name + "\n" +
	              "clientID = " + this.clientID + "\n" +
                  "width = " + this.width + "\n" + 
            	  "height = " + this.height + "\n" +
            	  "readOnly = " + this.readOnly + "\n" +
            	  "baseHref = " + this.baseHref + "\n" +
            	  "cssFile = " + this.cssFile  + "\n" +
            	  "hasSelection = " + this.hasSelection;
    };
}

function getOffsetLeft (el) 
{
  	var ol = el.offsetLeft;
  	while ((el = el.offsetParent) != null)
	{
    	ol += el.offsetLeft;
    }
  
  	return ol;
}

function getOffsetTop (el) 
{
  	var ot = el.offsetTop;
  	while((el = el.offsetParent) != null)
  	{
   		ot += el.offsetTop;
   	}
   	
	return ot;
}

function scrollTop()
{
	if (document.documentElement && document.documentElement.scrollTop)
		theTop = document.documentElement.scrollTop;
	else if (document.body)
		theTop = document.body.scrollTop;
		
	return theTop;
}

function scrollLeft()
{
	if (document.documentElement && document.documentElement.scrollLeft)
		theTop = document.documentElement.scrollLeft;
	else if (document.body)
		theTop = document.body.scrollLeft;
		
	return theTop;
}

function getNumericSize(s)
{
    var n = new Number(s.match(/\d+/));
    
    return n;
}