
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

var Button = new Class(
{
	parent: 	Class.Empty,
	name: 		"",
	image:		"",
	tooltip:	"",
	fn:			Class.Empty,
	
	initialize: function(parent, name, image, tooltip, fn)
	{
		this.parent = parent;
		this.name = name;
		this.image = image;
		this.tooltip = tooltip;
		this.fn = fn;
	},
	
	toHTML: function()
	{

		var doc = "<img id='" + this.parent.getID() + "_" + this.name + 
				  "' src='" + this.getImageURI() + "' " +
				  " class='rteButton' alt='" + this.tooltip + 
				  "' title='" + this.tooltip + 
				  "' onclick='theEditors[\"" + this.parent.name + "\"]." + this.fn + "();'>";
		
		return doc;
	},
	
	getImageURI: function()
	{
		if (this.image.substr(0, 1) == "/") return this.image;
		if (this.image.substr(0, 7) == "http://") return this.image;
		return this.parent.getImagePath() + this.image;
	}
});

var Separator = new Class(
{
	parent: Class.Empty,
	
	initialize: function(parent)
	{
		this.parent = parent;
	},

	toHTML: function()
	{
		return "<img class='rteSeparator' src='" + this.parent.getImagePath() + "separator.gif' border='0' alt=''>";
	}
});

var DropDownEntry = new Class(
{
	text: 	"",
	value: 	"",
	style: 	"",
	cl: 	"",
	def: 	"",
	
	initialize: function(text, value, style, cl, def)
	{
		this.text = text;
		this.value = value;
		this.style = style;
		this.cl = cl;
		this.def = def;
	},
	
	toHTML: function()
	{
		var doc = "<option ";
		if (this.cl) doc += "class='" + this.cl + "' ";
		if (this.style) doc += "style='" + this.style + "' ";
		//if (this.def) doc += "selected ";
		doc += "value='" + this.value + "'>" + this.text + "</option>";
		
		return doc;
	}
});

var DropDown = new Class(
{
	parent: 	Class.Empty,
	title:		"",
	name:		"",
	entries:	[],
	fn:			Class.Empty,
	
	initialize: function(parent, title, name, fn, entries)
	{
		this.parent = parent;
		this.title = title;
		this.name = name;
		this.entries = entries;
		this.fn = fn;
	},
	
	toHTML: function()
	{
		var doc = "<select name='" + this.parent.name + "_" + this.name + 
				  "' id='" + this.parent.name + "_" + this.name + 
				  "' onchange='theEditors[\"" + this.parent.name + "\"]." + this.fn + "()' style='height:20px'>";
		doc += "<option value='' selected>" + this.title + "</option>";
		
		for(var i = 0 ; i < this.entries.length ; ++i)
		{
			doc += this.entries[i].toHTML();
		}
		
		doc += "</select>";
		
		return doc;
	},
		
	clearSelection: function()
	{
	    var ctrl = document.getElementById(this.parent.name + "_" + this.name);
	    if (ctrl == null) return;
	    
	    for(var i = 1; i < ctrl.options.length; ++i)
	    {
	        ctrl.options[i].selected = false;
	    }
	    
	    ctrl.options[0].selected = true;
	}
	
});

var SymbolTable = new Class(
{
	div:	Class.Empty,
	editor:	Class.Empty,
	shim:	Class.Empty,
	
	initialize: function(editor)
	{
		this.editor = editor;
		
		this.div = new Element('div', {'class': 'rte_symbol_table'});
		this.div.setStyles({position: 'absolute', display: 'none', zIndex: 255});
		
		for (var i = 160; i < 256; ++i)
		{
			var a = new Element('a', {'html': "&#" + i + ";", 'href': '#'});
			a.inject(this.div);
			a.addEvent('click', function(e) { new Event(e).stop(); this.editor.insertAtSelection(e.target.get('html')); this.div.fade('out'); return false; }.bind(this));
		}
		
		this.div.addEvent('mouseleave', function(e) { this.div.fade('out'); }.bind(this));
		var doc = $(document.body ? document.body : document.documentElement);
		
		doc.adopt(this.div);
		
		this.shim = new IframeShim(this.div);
	},
	
	show: function()
	{
		var button = document.id(this.editor.getID() + "_symbol");
		this.div.setStyles({display: 'block', opacity: 0});
		this.div.position({relativeTo: button, edge: 'topLeft', position: 'bottomLeft'});
		this.div.fade('in');
	}
});

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

isIE = Browser.Engine.trident; 
isGecko = Browser.Engine.gecko;
isSafari = Browser.Engine.webkit; // Also Chrome
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

var theEditors = {};

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

var RichTextEditor = new Class({
	
	Implements:		[Options, Events],
	name:			"",
	clientID:		"",
	html:			"",
	width:			"",
	height:			"",
	buttons:		[],
	readOnly:		false,
	baseHref:		"",
	cssFile:		"",
	showStyleBar:	true,
	hasSelection: 	false,
	symbolTable:	Class.Empty,
	toolbar: 		[],
	stylebar: 		[],		
		
	options:
	{
		onSetupToolbar: function(editor) { return editor.setupDefaultToolbar();},
		onSetupStylebar: function(editor) { return editor.setupDefaultStylebar();},
		absoluteLinks: false
	},
		

	setupDefaultToolbar: function()
	{
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
				new Button(this, 'symbol', 'symbols.gif', 'Insert Symbol', 'onInsertSymbol'),
				new Button(this, 'clear', 'clear.gif', 'Clear All Formatting', 'onClearFormatting'),
				new Button(this, 'link', 'hyperlink.gif', 'Insert Link', 'onInsertLink'),
				new Button(this, 'image', 'image.gif', 'Insert Image', 'onInsertImage')/*,
				new Button(this, 'table', 'insert_table.gif', 'Insert Table', 'onInsertTable')*/
			];

	},

	setupDefaultStylebar: function()
	{
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
		
	},
	
	initialize: function(name, clientID, html, width, height, buttons, readOnly, options)
	{
		this.setOptions(options);
		
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
		
		this.symbolTable = new SymbolTable(this);
		this.fireEvent('setupToolbar', this);				
		this.fireEvent('setupStylebar', this);
	},
	
	
	draw: function()
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
		
		var container = $(this.clientID + '_container');
		if (!container)
		{
			document.write(doc);
		}
		else
		{
			container.set('html', doc);
		}
		
		
		var rte = $(this.name + "_iframe");
		if (rte.resizable) rte.resizable();
		rte.addEvent('load', function() { this.enableDesignMode(); }.bind(this));
	},
		
	drawToolbar: function(toolbar, toolbarName)
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
	},

	setToolbarVisibility: function(toolbarName, state)
	{
		var toolbarDiv = document.getElementById(this.name + "_" + toolbarName);
		if (toolbarDiv)
		{
			toolbarDiv.style.visibility = state;
		}
	},
			   		
	drawEditor: function()
	{		
		var doc = "<div><iframe id='" + this.name + "_iframe' name='" + this.name + "' width='" + this.width + "' height='" + this.height + 
				  "' src='" + rteRoot + "blank.htm' style='border: solid 1px black'></iframe></div>";
		doc += '<textarea id="' + this.clientID + '" name="' + this.clientID + '" style="display:none"></textarea>';
		if (!this.readOnly) 
		{
			doc += '<br><input type="checkbox" id="chkSrc' + this.name + '" onclick="theEditors[\'' + this.name + '\'].toggleHTMLSrc(\'' + this.name + '\',' + this.buttons + ');" />&nbsp;<label for="chkSrc' + this.name + '">View Source</label>';
		}
		
		doc += '<iframe width="154" height="104" id="cp' + this.name + '" src="palette.htm" marginwidth="0" marginheight="0" scrolling="no" style="visibility:hidden; position: absolute;"></iframe>';
		doc += '</div>';
	
		return doc;
	},
	
	
	enableDesignMode: function() 
	{
		if (this.loading) return;
		
		var frameHtml = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"; 
		frameHtml += "<html id=\"" + this.name + "\">\n";
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
	
		if (isIE) 
		{
			this.loading = true;
			
			var oRTE = this.findFrame().document;
			//alert(oRTE);
			oRTE.charset = "ISO-8859-1";
			oRTE.open();
			oRTE.write(frameHtml);
			oRTE.close();
			if (!this.readOnly) 
			{
				if (Browser.version < 9)
				{
					oRTE.designMode = "On";
				}
				else
				{
					oRTE.body.contentEditable = true;
				}
				//this.findFrame().document.attachEvent("onkeypress", this.evt_ie_keypress = function(event) {ieKeyPress(event, rte);});
			}
			
			var name = this.name;
			var self = this;
			
			oRTE.onselectionchange = function() { selectionChangedCallback(name); };
			setTimeout(function() 
			{ 	
				oRTE.body.onbeforepaste = function(evt) { return self.ieBeforePaste(evt); };
				oRTE.body.onpaste = function() { return self.iePaste(); };
			}, 1000);
			
			$(this.name + "_iframe").removeEvents('load');
			this.loading = false;
		} 
		else 
		{
			try 
			{
				if (!this.readOnly) 
				{
					$(this.name + "_iframe").contentDocument.designMode = "on";
				}
				
				try 
				{
					var oRTE = $(this.name + "_iframe").contentWindow.document;
					
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
					//alert("Error preloading content.");
				}
			} 
			catch (e) 
			{
				//gecko may take some time to enable design mode.
				//Keep looping until able to set.
				if (isGecko) 
				{
					console.log(e);
					//this.enableDesignMode().delay(500);
				} 
				else 
				{
					return false;
				}
			}
		}
	},
	
	findFrame: function()
	{
		return document.frames[this.name];
	},
	
	toggleHTMLSrc: function() 
	{
		var oHdnField = $(this.clientID);
		var toolbar = $(this.name + "_toolbar");
		var stylebar = $(this.name + "_stylebar");
		var rte = $(this.name + "_iframe");
		var src = $(this.clientID);
		var chkSrc = $("chkSrc" + this.name);
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
	},

	getEditor: function()
	{
		if (document.all)
		{
			return this.findFrame().document;
		}
		else
		{
			return $(this.name + "_iframe").contentWindow.document;
		}
	},
	
    setHTML: function(html)
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
			var oRTE = $(this.name + "_iframe").contentWindow.document;
			oRTE.body.innerHTML = html;
		}
    },
    
	setHiddenVal: function() 
	{
		//set hidden form field value for current rte
		var oHdnField = $(this.clientID);
		
		if (oHdnField.value == null) oHdnField.value = "";
		
		if (document.all) 
		{
    		oHdnField.value = this.findFrame().document.body.innerHTML;
		} 
		else 
		{
			// Strip errant <br> tags in Firefox
			var doc = $(this.name + "_iframe").contentWindow.document;
			var preBlocks = doc.getElementsByTagName("PRE");
			for(var i = 0; i < preBlocks.length; ++i)
			{
				preBlocks[i].innerHTML = unescape(preBlocks[i].innerHTML.replace(/<br\/?>/gi, "\n"));
			}
			oHdnField.value = doc.body.innerHTML;
		}

		var val = oHdnField.value;
		// Escape entities
		for(var i = 160; i < 256; ++i)
		{
			val = val.replace(String.fromCharCode(i), "&#" + i + ";");
		}
		
		oHdnField.value = val.trim();
		
		//alert(oHdnField.value);
		
		//if there is no content (other than formatting) set value to nothing
		if (this.stripHTML(oHdnField.value.replace("&nbsp;", " ")) == "" &&
			oHdnField.value.toLowerCase().search("<hr") == -1 &&
			oHdnField.value.toLowerCase().search("<img") == -1) 
		{
			oHdnField.value = "";
		}
	},

	stripHTML: function(oldString) 
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
	},
		
	trim: function(inputString) 
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
	},

	rteCommand: function(command, option) 
	{
		//function to perform command
		var oRTE;
		if (document.all) 
		{
			oRTE = this.findFrame();
		} 
		else 
		{
			oRTE = $(this.name + "_iframe").contentWindow;
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
	},

	onBold: function()
	{
		this.rteCommand('bold', '');
	},
	
	onItalic: function()
	{
		this.rteCommand('italic', '');
	},
	
	onUnderline: function()
	{
		this.rteCommand('underline', '');
	},
		
	onAlignLeft: function()
	{
		this.rteCommand('justifyleft', '');
	},
	
	onAlignCenter: function()
	{
		this.rteCommand('justifycenter', '');
	},
		
	onAlignRight: function()
	{
		this.rteCommand('justifyright', '');
	},
		
	onJustify: function()
	{
		this.rteCommand('justifyfull', '');
	},
	
	onHorizontalRule: function()
	{
		this.rteCommand('inserthorizontalrule', '');
	},
	
	onNumberedList: function()
	{
		this.rteCommand('insertorderedlist', '');
	},
	
	onBulletList: function()
	{
		this.rteCommand('insertunorderedlist', '');
	},
	
	onOutdent: function()
	{
		this.rteCommand('outdent', '');
	},
	
	onIndent: function()
	{
		this.rteCommand('indent', '');
	},
	
	onInsertLink: function()
	{
	    var url = prompt("Enter a URL: ", "");
	    if (url)
	    {
    	    this.formatSelection("<a href=\"" + url + "\">");
    	}
	},
	
	onInsertImage: function()
	{
	    var url = prompt("Enter an Image URL: ", "");
	    if (url)
	    {
    	    this.insertAtSelection("<img border=\"0\" src=\"" + url + "\">");
    	}
	},
		
	onFontChange: function()
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
	},
	
	onStyleChange: function()
	{
		var block = document.getElementById(this.name + "_style").value;
		if (block == "") return;
		//this.rteCommand('formatblock', block);
		//this.cleanCloseTags();
		this.formatSelection(block);
		this.clearStyleBar();
	},
	
	clearStyleBar: function()
	{
	    for(dropdown in this.stylebar)
	    {
	        this.stylebar[dropdown].clearSelection();
	    }
	},
	    
	onSelectionChanged: function()
	{
	    this.hasSelection = true;
	},
	    
	formatSelection: function(tag)
	{
		if (window.getSelection)
		{
			var elt = this.parseTag(tag);
			var selection = $(this.name + "_iframe").contentWindow.getSelection();
			
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
	},
	
	insertAtSelection: function(tag)
	{
		if (window.getSelection)
		{
			var elt = this.parseTag(tag);
			var selection = $(this.name + "_iframe").contentWindow.getSelection();
			
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
	},
	
	parseTag: function(tag)
	{
		var tagPattern = new RegExp("<(\\w+)\\s*(.*)>", "g");
		var attrPattern = new RegExp("(\\w+)=[\"'](.*?)[\"']");
        		
		var matches = tagPattern.exec(tag);
		
		var elt = null;
		
		if (matches == null || matches.length == 0) return document.createTextNode(tag);
		
		elt = document.createElement(matches[1]);
		
		var attrs = matches[2].match(/\w+=[\"'].*?[\"']/g);
		
    	if (attrs)
		{
			for(var i = 0; i < attrs.length; ++i)
			{
				var attrMatches = attrPattern.exec(attrs[i]);
				elt.setAttribute(attrMatches[1], attrMatches[2]);
			}
		}
		
		var content = tag.match(/^<.*?>(.*)<\/.*>$/);		
		if (content) elt.innerHTML = content[1];
		
		return elt;
	},
	
	cleanCloseTags: function()
	{
		var pattern = /<\/(\w*?)\s+.*?>/g;
		
		if (document.all)
		{
		}
		else
		{
			var html = $(this.name + "_iframe").contentWindow.document.body.innerHTML;
			//alert(html);
			html = html.replace(pattern, "</$1>");
			//alert(html);
			$(this.name + "_iframe").contentWindow.document.body.innerHTML = html;
		}
	},
	
	onInsertSymbol: function()
	{
		this.symbolTable.show();
	},
	
	onTextColor: function()
	{
	
	},
	
	getID: function()
	{
		return this.name;
	},
	
	getImagePath: function()
	{
		return (rteImagePath != "") ? rteImagePath : "images/";
	},
	
	onGeckoKeyPress: function(evt)
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
	},
	
	onGeckoPaste: function(evt)
	{
		this.insertCleaningContainer();
		var self = this;
		setTimeout(function() { self.cleanPaste(); }, 50);
	},
	
	ieBeforePaste: function(event)
	{	
		var oRTE = this.getEditor();
		var cleaner = oRTE.getElementById('HTMLCleansingNode');
		if (cleaner) cleaner.parentNode.removeChild(cleaner);
	
		this.insertCleaningContainer();
	
		return false;
	},
	
	iePaste: function()
	{	
		var self = this;
		setTimeout(function() { self.cleanPaste(); }, 50);
	},
	
	cleanPaste: function()
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
	},
	
	insertCleaningContainer: function()
	{
		this.insertAtSelection("<div id='HTMLCleansingNode' style='display: none'>_</div>");
		var oRTE = this.getEditor();
		this.selectNodeContents(oRTE.getElementById('HTMLCleansingNode'));
	},
	
	onClearFormatting: function()
	{
	    this.setHiddenVal();
	    var oHdnField = document.getElementById(this.clientID);
	    var text = this.cleanHTML(oHdnField.value);
	    oHdnField.value = text;
	    this.setHTML(oHdnField.value);
	},
	    
	cleanHTML: function(text)
	{
		text = text.replace(/<\!--\s*\[if.*?supportLists\s*\]-->(.*?)<\!--\[endif\]\s*-->/gi, "$1");
		text = text.replace(/<\!--\s*\[if(?:.|[\n\r])*?<\!\[endif\]\s*-->/gi, "");
	    text = text.replace(/<style.*?>(?:.|[\n\r])*?<\/style>/gi, "");
	    text = text.replace(/<script.*?>(?:.|[\n\r])*?<\/script>/gi, "");
		text = text.replace(/<link(?:.|[\n\r])*?>/gi, "");

	    text = text.replace(/<(\/?(?:p|a[^>]*|h\d|ul|ol|li|br|hr|table|tbody|thead|tfoot|tr|td)).*?>/gi, "[_*_[$1]_*_]");
	    text = text.replace(/<[\s\S]*?>/gi, "");
	    text = text.replace(/\[_\*_\[/gi, "<");
	    text = text.replace(/\]_\*_\]/gi, ">");
	    text = text.replace(/<p>\s*<\/p>/g, "");
	    text = text.replace(/<table>/, "<table class='list'>");
	    return text;
	},
	
	selectNodeContents: function(node)
	{
		if (!node) return false;
		
		var oRTE = this.getEditor();

		if (window.getSelection)
		{
			range = oRTE.createRange();
			range.selectNodeContents(node);
			var selection = $(this.name + "_iframe").contentWindow.getSelection();
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
	},
	
	prepareForSubmit: function()
	{
		var chkSrc = document.getElementById("chkSrc" + this.name);
		if (!chkSrc.checked) 
		{
		    this.setHiddenVal();
		}
		
		this.relativizeLinks();
	},
	
	relativizeLinks: function()
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
	},
	
	addToolbarButton: function(name, image, tooltip, handler, width, height)
	{
			
	    var fn = "onButton" + name;
		if (typeof handler == 'function')
		{
			this[fn] = function()
			{
				handler(this);
			};
		}
		else
		{
			url = handler;
			url += (url.indexOf("?") > -1) ? "&" : "?";
			url += "Editor=" + this.name;
	    
		    this[fn] =  function() 
		                { 
		                    var w = window.open(url, name, 
		                                        'width='+width+',height='+height+',toolbar=0,scrollbars=1,resizable=1,status=0');
		                    w.focus();
		                };
		}
		
        this.toolbar.push(new Button(this, name, image, tooltip, fn));
	},
	
	overrideToolbarButton: function(name, handler)
	{
		this.toolbar.filter(function(button) { return button.name == name;})
					.each(function(button)
					{
					    var fn = "onButton" + name;
						if (typeof handler == 'function')
						{
							this[fn] = function()
							{
								handler(this);
							};
						}
						else
						{
							url = handler;
							url += (url.indexOf("?") > -1) ? "&" : "?";
							url += "Editor=" + this.name;
					    
						    this[fn] =  function() 
						                { 
						                    var w = window.open(url, name, 
						                                        'width='+width+',height='+height+',toolbar=0,scrollbars=1,resizable=1,status=0');
						                    w.focus();
						                };
						}
						
						button.fn = fn;
						
					}.bind(this));
	},
	
	addStyle: function(name, defn)
	{
	    this.stylebar[2].entries.push(new DropDownEntry(name, defn, '', '', false));
	},
	
	toString: function()
	{
	    var str = "[RichTextEditor]:\nname = " + this.name + "\n" +
	              "clientID = " + this.clientID + "\n" +
                  "width = " + this.width + "\n" + 
            	  "height = " + this.height + "\n" +
            	  "readOnly = " + this.readOnly + "\n" +
            	  "baseHref = " + this.baseHref + "\n" +
            	  "cssFile = " + this.cssFile  + "\n" +
            	  "hasSelection = " + this.hasSelection;
	    
	    return str;
    }
});

function getNumericSize(s)
{
    var n = new Number(s.match(/\d+/));
    
    return n;
}