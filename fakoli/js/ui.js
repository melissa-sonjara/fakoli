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

// Annotate the document body with browser info
window.addEvent('domready', function()
{
	document.id(document.body).addClass(Browser.name).addClass((Browser.name + Browser.version).replace(".", "_"));
});

window.size = function()
{
	var w = 0;
	var h = 0;

	//IE
	if(!window.innerWidth)
	{
		//strict mode
		if(!(document.documentElement.clientWidth == 0))
		{
			w = document.documentElement.clientWidth;
			h = document.documentElement.clientHeight;
		}
		//quirks mode
		else
		{
			w = document.body.clientWidth;
			h = document.body.clientHeight;
		}
	}
	//w3c
	else
	{
		w = window.innerWidth;
		h = window.innerHeight;
	}
	return {width:w,height:h};
};

window.center = function()
{
	var hWnd = (arguments[0] != null) ? arguments[0] : {width:0,height:0};

	var _x = 0;
	var _y = 0;
	var offsetX = 0;
	var offsetY = 0;

	//IE
	if(!window.pageYOffset)
	{
		//strict mode
		if(!(document.documentElement.scrollTop == 0))
		{
			offsetY = document.documentElement.scrollTop;
			offsetX = document.documentElement.scrollLeft;
		}
		//quirks mode
		else
		{
			offsetY = document.body.scrollTop;
			offsetX = document.body.scrollLeft;
		}
	}
	//w3c
	else
	{
		offsetX = window.pageXOffset;
		offsetY = window.pageYOffset;
	}

	_x = ((this.size().width-hWnd.width)/2)+offsetX;
	_y = ((this.size().height-hWnd.height)/2)+offsetY;

	return{x:_x,y:_y};
};

window.scrollToElement = function(element, offset)
{
	if (typeof(offset) == "undefined") offset = 0;
	element = document.id(element);
	var coords = element.getCoordinates();
	var scroll = element.getScroll();
	
	new Fx.Scroll(document.body).start(scroll.x, coords.top + offset);
};

window.alignHeights = function(selector)
{
	if (!selector) selector = ".align_height";
	$$(selector).each(function(container)
	{
		var max = 0;
		var children = container.getChildren();
		children.each(function(child)
		{
			var height = child.getSize().y;
			if (height > max) max = height;
		});

		children.each(function(child)
		{
			child.setStyle('height', max);
		});		
	});
};

var Curtain = new Class(
{
	curtain: Class.Empty,
	shim: Class.Empty,
	
	initialize: function()
	{
		this.curtain = document.id('curtain');
		if (!this.curtain)
		{
			this.curtain = new Element('div', {'id': 'curtain'});
			var doc = document.id(document.body ? document.body : document.documentElement);
			
			doc.adopt(this.curtain);
		}
		
		this.shim = new IframeShim(this.curtain);
	},
	
	lower: function(onComplete)
	{
		var noFixed = (Browser.name == 'ie' && Browser.version < 7);
		
		var opacity = this.curtain.getStyle('opacity');
		var display = this.curtain.getStyle('display');
		if (opacity > 0 && display == 'block')
		{
			if (onComplete) onComplete();
			return;
		}
		
		var windowSize = window.size();

		if (document.body.hasClass('full-height'))
		{
			 w = "100%";
			 h = "100%";
		}
		else
		{
			w = windowSize.width;
			h = windowSize.height;
		}
		this.curtain.setStyles({top: 0,
								left: 0,
								width: w,
								height: h,
								display: 'block',
								opacity: 0,
								position: (noFixed) ? 'absolute' : 'fixed',
								visibility: 'visible'});
		
		this.shim.position();
		this.shim.show();
		
		if (onComplete)
		{
			new Fx.Tween(this.curtain).start('opacity', 0.5).chain(onComplete);
		}
		else
		{
			this.curtain.fade(0.7);
		}
	},
	
	raise: function(onComplete)
	{
		this.shim.hide();
		
		if (onComplete)
		{
			new Fx.Tween(this.curtain).start('opacity', 0.0).chain(onComplete);
		}
		else
		{
			this.curtain.fade(0.0);
		}
	},
	
	loadingCursor: function()
	{
		this.curtain.setStyle('cursor', 'progress');
	},
	
	normalCursor: function()
	{
		this.curtain.setStyle('cursor', 'auto');
	}
});

Window.implement(
{
	raiseCurtain: function(onComplete)
	{
		new Curtain().raise(onComplete);
	},
	
	lowerCurtain: function(onComplete)
	{
		new Curtain().lower(onComplete);
	}		

});

function modalPopup(title, url, width, height, returnPopup, draggable, clazz)
{
	var popup = new ModalDialog('modalPopup_' + String.uniqueID(), {'title': title, 'width': width, 'height': height, 'draggable': draggable, 'class': clazz, 'maximized': (width=='100%' && height=='100%')});
	popup.show(null, url);
	if (returnPopup) return popup;
}

function hideModalPopup(popup)
{
	if (popup) popup.hide();
}

function messagePopup(title, message, width, height, returnPopup, draggable, clazz)
{
	var popup = new ModalDialog('modalPopup_' + String.uniqueID(), {'title': title, 'width': width, 'height': height, 'draggable': draggable, 'class': clazz, 'maximized': (width=='100%' && height=='100%')});
	popup.options.body.set('html', message);
	popup.show();
	if (returnPopup) return popup; 
}


function floatingPopup(id, title, url, width, height, returnPopup, draggable, clazz)
{
	var popup = new FloatingDialog(id, {'title': title, 'width': width, 'height': height, 'draggable': draggable, 'class': clazz, 'maximized': (width=='100%' && height=='100%')});
	popup.show(null, url);
	if (returnPopup) return popup;
}

var AbstractDialog = new Class(
{
    Implements: [Options, Events],
    
    options: 
    {
		draggable: 	false,
		handle: 	Class.Empty,
		closeLink: 	Class.Empty,
		body:		Class.Empty,
		width: 		"500px",
		height:		"auto",
		title:		Class.Empty,
		top: 	   Class.Empty,
		left:	   Class.Empty,
		position:  Class.Empty,
		onHide:		function() {},
		'class':	null,
		maximized:	false
	},
	
    element: Class.Empty,
    remoteURL: null,
    
    initialize: function(element, options)
    {
    	this.element = document.id(element);
    	this.setOptions(options);
    	
    	if (!this.element)
    	{
    		this.element = this.createDialog(element);
    	}
    	else
    	{
    		this.dialog_header = this.element.getElement('.dialog_header');
    	}
    	
    	if (this.options.class)
    	{
    		this.element.addClass(this.options.class);
    	}
    	
    	if (this.element) this.element.setStyle('display', 'none');
    	if (this.options.body) 
    	{
    		this.options.body = document.id(this.options.body);
    	}
    	else
    	{
    		this.options.body = document.id(this.element.id + "Body");
    	}
    	
    	if (this.options.closeLink)
    	{
    		document.id(this.options.closeLink).addEvent('click', function(e) { new DOMEvent(e).stop(); this.hide(); if (this.disposeOnExit) { this.options.body.set('text', ''); } }.bind(this));
    	}
    	
    	if (this.options.title)	this.setTitle(this.options.title);
    	this.element.setStyles({'width': this.options.width, 'height': this.options.height});
    },
    
    setTitle: function(title)
    {
    	this.options.title = title;
    	if (this.element)
    	{
    		document.id(this.element.id + 'Title').set('text', title);
    	}
    },	
    
    createDialog: function(id)
    {   
    	var dialog = new Element('div', {'class': 'dialog', 
    									 'id': id});
    	
    	dialog.setStyles({'display': 'none',
    					  'width': this.options.width,
    					  'height': this.options.height});
    	
    	this.dialog_header = new Element('div', {'class': 'dialog_header', 'id': id + 'Header'});
    	
    	var padding = new Element('div');
    	padding.setStyles({'padding': '4px'});
    	
    	var body = new Element('div', {'id': id + 'Body', 'class': 'dialog_body'});
    	
    	padding.set('html', "<div style='float: right'>&nbsp;<a id='close" + id + "' href=''>" + AbstractDialog.closeButtonHTML + "</a></div>" +
    						"<span style='font-weight: bold' id='" + id + "Title'>" + this.options.title + "</span>");
    	
    	this.options.closeLink = "close" + id;
    	this.options.body = body;
     	
    	if (this.options.draggable)
    	{
    		this.options.handle = this.dialog_header;
    		this.options.handle.setStyle('cursor', 'move');    		
    	}
    	
    	padding.inject(this.dialog_header);    	
    	this.dialog_header.inject(dialog, 'top');
    	body.inject(dialog, 'bottom');
		var doc = document.id(document.body ? document.body : document.documentElement);
		
		doc.adopt(dialog);
		return dialog;
    }    
});

AbstractDialog.closeButtonHTML = "Close &times;";
AbstractDialog.onClose = null;

AbstractDialog.onClose = function(dialog)
{
	if (dialog.remoteURL)
	{
		dialog.element.getElements("textarea.richtext").each(function(rte)
		{
			tinymce.remove("#" + rte.id);
		});
	}
};


var ModalDialog = new Class(
{
	Extends: AbstractDialog,
	
    initialize: function(element, options)
    {
		this.parent(element, options);
		this.element.fade('hide');		
    },
    	
    center: function()
    {
    	var noFixed = (Browser.name == 'ie' && Browser.version < 7);
    	
    	
    	if (this.options.body)
    	{
    		this.options.body.setStyle('height', 'auto');
    	}

    	if (this.options.maximized)
    	{
    		this.element.addClass('maximized');
    		this.element.setStyles({position: (this.draggable || noFixed) ? 'absolute' : 'fixed', top: 0, left: 0, width: '100%', height: '100%', 'z-index': 10000});
    		return;
    	}
    	
    	this.element.removeClass('maximized');
    	
    	var curtain = document.id('curtain');
    	var windowSize = window.size();
    	
    	if (this.element.offsetWidth > windowSize.width)
    	{
    		this.element.setStyle('width', windowSize.width * 0.9);
    	}
    	
    	if (this.element.offsetHeight > windowSize.height && this.options.body)
    	{
    		this.options.body.setStyles({'height': windowSize.height * 0.9, 'overflow-y': 'auto'});
    	}
    	
    	var x = (windowSize.width - this.element.offsetWidth) / 2;
    	var y = (windowSize.height - this.element.offsetHeight) / 2;
    	this.element.setStyles({position: (this.draggable || noFixed) ? 'absolute' : 'fixed', top: y, left: x, 'z-index': 10000});
    },
    
    show: function(onComplete, fragmentURL)
    {
    	var reload = this.element.getStyle('display') != 'none';
    	
    	this.remoteURL = fragmentURL;

    	if (!reload)
    	{
    		ModalDialog.activeDialogs.push(this);
    	}
    	
    	if (this.options.draggable)
    	{
    		var options = 
    		{
    			preventDefault: true, 
    			onDrag: function(element, event) 
    			{ 
    				new DOMEvent(event).stop(); 
    			} 
    		};
    		
    		if (this.options.handle)
    		{
    			options.handle = this.options.handle;
    		}
    		
    		var drag = new Drag.Move(this.element, options);
    	}
    	
    	
    	if (fragmentURL && this.options.body)
    	{
    		new Curtain().loadingCursor();
    		
    		this.disposeOnExit = true;
    		if (!reload) this.options.body.set('text', "Loading...");
    		var request = new Request.HTML(
    		{
    			evalScripts: false,
    			evalResponse: false,
    			method: 'get', 
    			url: fragmentURL, 
    			onSuccess: function(tree, elements, html, script) 
    			{ 
    				this.options.body.set('text', '');
    				this.options.body.set('html', html);
    				Browser.exec(script);
    				this.element.fade('show');
    				this.center();
    				new Curtain().normalCursor();
    			}.bind(this)
    		});
    		request.send();
    	}
    	else
    	{
    		this.element.fade('show');
    	}
    	
    	window.lowerCurtain(function() {
    		this.element.setStyle('display', 'block');
    		this.center();
    		this.addResizeHook();
    		if (onComplete) onComplete(this);
    	}.bind(this));
    },
    
    addResizeHook: function()
    {
    	this.resizeHook = function() { this.center(); }.bind(this);
    	window.addEvent('resize', this.resizeHook);
    },
    
    removeResizeHook: function()
    {
    	if (this.resizeHook)
    	{
    		window.removeEvent('resize', this.resizeHook);
    		this.resizeHook = null;
    	}
    },
    
    hide: function(whenDone)
    {
    	this.fireEvent('hide', this);
    	if (AbstractDialog.onClose) { AbstractDialog.onClose(this); }
    	ModalDialog.activeDialogs.pop();
    	if (ModalDialog.activeDialogs.length == 0) window.raiseCurtain();
    	this.element.setStyle('display', 'none');
    	if (this.remoteURL) this.element.dispose();
    	this.removeResizeHook();
    	if (whenDone) whenDone();
    }
});

ModalDialog.activeDialogs = [];
ModalDialog.getActiveDialog = function()
{
	if (ModalDialog.activeDialogs.length == 0) return null;
	return ModalDialog.activeDialogs[ModalDialog.activeDialogs.length - 1];
};

ModalDialog.recenterActiveDialog = function()
{
	var dialog = ModalDialog.getActiveDialog();
	if (dialog) dialog.center();
};


var FloatingDialog = new Class(
{
	Extends: AbstractDialog,
	
	initialize: function(element, options)
	{
		this.parent(element, options);
    },
	
    position: function()
    {
		var x, y;
    	var noFixed = (Browser.name == 'ie' && Browser.version < 7);
    			
		if (this.top && this.left)
		{
			x = this.left;
			y = this.top;
		}
		else
		{
	    	var windowHeight = window.innerHeight ? window.innerHeight :
	    		document.documentElement.clientHeight ?
	    		document.documentElement.clientHeight : document.body.clientHeight; 
	    	
	    	x = (document.body.clientWidth - this.element.offsetWidth) / 2;
	    	y = (windowHeight - this.element.offsetHeight) / 2;
		}

		var pos;
		
		if (this.options.position) 
		{
			pos = this.options.position;
		}
		else
		{
			pos = (this.draggable || noFixed) ? 'absolute' : 'fixed';
		}
		
		if (this.options.height != 'auto')
		{
			var height = this.element.offsetHeight - this.dialog_header.offsetHeight;
			this.options.body.setStyles({'max-height': height, 'overflow': 'auto'});
		}
		
		this.element.setStyles({position: pos , top: y, left: x, 'z-index': 10000});
    },
    
    
    show: function(onComplete, fragmentURL)
    {
    	if (this.options.draggable)
    	{
    		var options = 
    		{
    			preventDefault: true, 
    			onDrag: function(element, event) 
    			{ 
    				new DOMEvent(event).stop(); 
    			} 
    		};
    		
    		if (this.options.handle)
    		{
    			options.handle = this.options.handle;
    		}
    		
    		var drag = new Drag.Move(this.element, options);
    	}
    	
    	this.position();
    	 	
    	this.remoteURL = fragmentURL;

    	if (fragmentURL && this.options.body)
    	{
       		this.disposeOnExit = true;
       	 	this.options.body.set('text', "Loading...");
    		var request = new Request.HTML(
    		{
    			evalScripts: false,
    			evalResponse: false,
    			method: 'get', 
    			url: fragmentURL, 
    			onSuccess: function(tree, elements, html, script) 
    			{ 
    				this.options.body.set('text', '');
    				this.options.body.set('html', html);
    				Browser.exec(script);
    				this.position();
    			}.bind(this)
    		});
    		request.send();
    	}
    	
    	this.element.setStyles({'display': 'block', 'opacity': 0});
    	new Fx.Tween(this.element).start('opacity', 1).chain(onComplete);
    },
    
    hide: function()
    {
    	this.fireEvent('hide', this);
    	if (AbstractDialog.onClose) { AbstractDialog.onClose(this); }
    	new Fx.Tween(this.element).start('opacity', 0).chain(function() {  this.element.setStyle('display', 'none');}.bind(this));
    }
});

var Interstitial = new Class({
	
	Implements: Options,
	
	options:
	{
		spinner:	'/fakoli/images/loader.gif',
		cssClass:	'interstitial',
		id:			'interstitial',
		width:		400,
		height:		66
	},

	message: "",
	interstitial: Class.Empty,
	
	initialize: function(message, options)
	{
		this.setOptions(options);
		this.message = message;
		
		this.interstitial = this.createInterstitial();
	},
	
	createInterstitial: function()
	{
		var div = new Element('div', {'class': this.options.cssClass, 
									  'id': this.options.id});
									  
		div.setStyles({'width': this.options.width, 
					   'height': this.options.height,
					   'display': 'none',
					   'padding': '10px'});

		var img = Asset.image(this.options.spinner, {'align': 'left'});
		img.setStyle("margin-right", 20);
		img.inject(div);
		
		var text = new Element('span', {'html': this.message});
		text.inject(div, 'bottom');
		
		var doc = document.id(document.body ? document.body : document.documentElement);

		doc.adopt(div);
		return div;
	},
	
	center: function()
	{
	   	var noFixed = (Browser.name == 'ie' && Browser.version < 7);
	   	
	   	var size = window.size();
	   	var coords = this.interstitial.getCoordinates();
	   	var x = (size.width - coords.width) / 2;
	   	var y = (size.height - coords.height) / 2;

	   	this.interstitial.setStyles({position: (this.draggable || noFixed) ? 'absolute' : 'fixed', top: y, left: x, 'z-index': 10005});	   	
	},
	
    addResizeHook: function()
    {
    	this.resizeHook = function() { this.center(); }.bind(this);
    	window.addEvent('resize', this.resizeHook);
    },
    
    removeResizeHook: function()
    {
    	if (this.resizeHook)
    	{
    		window.removeEvent('resize', this.resizeHook);
    		this.resizeHook = null;
    	}
    },
    
	show: function()
	{
	   	window.lowerCurtain(function() 
	   	{
    		this.interstitial.setStyle('display', 'block');
    		this.center(); 
    		this.addResizeHook();
    		Interstitial.current = this;
    	}.bind(this));
	},
	
	hide: function()
	{
    	this.removeResizeHook();
    	window.raiseCurtain();
    	this.interstitial.setStyle('display', 'none');
    	Interstitial.current = null;
	}
	
});

Interstitial.setDefaultSpinner = function(spinner)
{
	Interstitial.defaultSpinner = spinner;
	Asset.image(spinner);
}

Interstitial.setDefaultSpinner('/fakoli/images/loader.gif');

function interstitial(message, image)
{
	if (!image) image = Interstitial.defaultSpinner;
	
	var int = new Interstitial(message, {spinner: image});
	int.show();
	
	return int;
}

function hideInterstitial()
{
	if (Interstitial.current)
	{
		Interstitial.current.hide();
	}
}

var Notification = new Class({
	
	Implements: [Options, Events],
	
	options:
	{
		cssClass:		'notification',
		width:			400,
		height:			'auto',
		wait:			3000,
		onHide:			function () {},
		blocking:		false,
		buttonClass:	'button',
		buttonText:		'OK'
	},

	message: "",
	notification: Class.Empty,
	
	initialize: function(message, options)
	{
		this.setOptions(options);
		this.message = message;
		
		this.notification = this.createNotification();
		this.center();
		
		this.show();
		if (!this.options.blocking)
		{
			this.hide.delay(this.options.wait, this);
			this.fireEvent('hide', this, this.options.wait + 1000);
		}
	},

	createNotification: function()
	{
		var div = new Element('div', {'class': this.options.cssClass});
									  
		div.setStyles({'max-width': this.options.width, 
					   'height': this.options.height,
					   'display': 'block',
					   'opacity': 0});

		div.set('html', this.message);
		var doc = document.id(document.body ? document.body : document.documentElement);

		doc.adopt(div);
		
		if (this.options.blocking)
		{
			var button = new Element('a', {'class': this.options.buttonClass});
			button.set('html', this.options.buttonText);
			button.addEvent('click', function(e) { new DOMEvent(e).stop(); this.hide(); }.bind(this));
			
			div.adopt(button);
		}
		return div;
	},
	
	center: function()
	{
	   	var noFixed = (Browser.name == 'ie' && Browser.version < 7);
	   	
	   	var size = window.size();
	   	var coords = this.notification.getCoordinates();
	   	var x = (size.width - coords.width) / 2;
	   	var y = (size.height - coords.height) / 2;

	   	if (coords.width > size.width * 0.8)
	   	{
	   		this.notification.setStyle('width', size.width * 0.8);
	   		x = size.width * 0.1;
	   	}
	   	
	   	this.notification.setStyles({position: (this.draggable || noFixed) ? 'absolute' : 'fixed', top: y, left: x, 'z-index': 150});	   	
	},
	
	show: function()
	{
		this.notification.fade('in');
	},
	
	hide: function()
	{
		this.notification.fade('out');
	}
});

function notification(message, options)
{
	new Notification(message, options);
}

var ProgressiveSearch = new Class({
	
	Implements: Options,
	  
	options: 
	{
		search:			Class.Empty,
		minimumLength: 	4,
		height: 		'150px',
		width:			Class.Empty,
		cssClass:		'',
		parameter:		'',
		defaultSearch:	Class.Empty,
		browse:			false
	},
  
	element:	Class.Empty,
	list: 		Class.Empty,
	container:	Class.Empty,
	allowHide:	true,
	
	initialize: function(element, options)
	{
		this.setOptions(options);
		this.element = document.id(element);
		this.element.search = this;
		this.list = new Element('div', {'id': this.element.id + '_progressive_search', 'class': 'progressive_search'});
		this.list.setStyles({'display': 'none', 'position': 'absolute', 'max-height': this.options.height, 'overflow-y': 'auto'});
		this.container = new Element('div');
		
		this.list.adopt(this.container);
		
		this.container.select = function(value)
		{
			this.element.value = value;
			this.list.setStyle('display', 'none');
			return this;
		}.bind(this);
		
		this.container.search = function(value)
		{
			this.search(value);
			return this;
		}.bind(this);
		
		if (this.options.cssClass) this.list.addClass(this.options.cssClass);
		
		var body = document.id(document.body ? document.body : document.documentElement);
		body.adopt(this.list);
		
		this.element.addEvent('keyup', function() { this.onKeyPress();}.bind(this));
		this.element.addEvent('blur', function() { this.hideList();}.bind(this));
		this.element.addEvent('focus', function() { this.showDefaultList();}.bind(this));
		this.element.setProperty('autocomplete', 'off');
		this.list.addEvent('mouseover', function() { this.allowHide = false; }.bind(this));
		this.list.addEvent('mouseout', function() { this.allowHide = true; }.bind(this));
		
		if (this.options.browse)
		{
			this.browseButton = new Element('a', {'href': '#', 'class': 'button', 'text': 'Browse'});
			this.browseButton.setStyle('margin-left', 5);
			this.browseButton.addEvent('click', function(e) { new DOMEvent(e).stop(); this.element.focus(); this.browse();}.bind(this));
			this.browseButton.inject(this.element, 'after');
		}
	},
	
	onKeyPress: function()
	{
		var val = this.element.value;
		
		if (val == '') this.showDefaultList();
		
		if (val.length < this.options.minimumLength) 
		{
			this.list.setStyle('display', 'none');
			return;
		}
		
		var name = this.options.parameter ? this.options.parameter : this.element.id;
		var request = new Request(
	    		{
	    			method: 'get', 
	    			url: appendQueryString(this.options.search, name + "=" + encodeURIComponent(val)), 
	    			onSuccess: function(html) 
	    			{ 
	    				this.showList(html);
	    			}.bind(this)
	    		});
   		request.send();
	},
	
	browse: function()
	{
		var request = new Request(
	    		{
	    			method: 'get', 
	    			url: appendQueryString(this.options.search, "browse=1"), 
	    			onSuccess: function(html) 
	    			{ 
	    				this.showList(html);
	    			}.bind(this)
	    		});
   		request.send();
	},
	
	search: function(val)
	{
		this.element.value = val;
		this.onKeyPress();
	},
	
	showList: function(html)
	{
		this.container.set('html', html);
		var coords = this.element.getCoordinates();
		this.list.setStyles({'top': coords.bottom, 'left': coords.left, 'width': this.options.width ? this.options.width : coords.width, 'max-height': this.options.height, 'display': 'block'});
	},
	
	hideList: function(override)
	{
		if (!this.allowHide && !override) return;
		
		this.list.setStyle('display', 'none');
	},
	
	reset: function()
	{
		this.hideList(true);
		this.element.value = "";
	},
		
	showDefaultList: function()
	{
		if (this.element.value == '' && this.options.defaultSearch)
		{
			var request = new Request(
		    		{
		    			method: 'get', 
		    			url: this.options.defaultSearch, 
		    			onSuccess: function(html) 
		    			{ 
		    				this.showList(html);
		    			}.bind(this)
		    		});
	   		request.send();
	   	}
	}
});



var PaginatingList = new Class(
{
	Implements: Options,
  
	options: 
	{
    	per_page: 10,
    	display_mode: 'list-item'
	},
	
	list: 			Class.Empty,
	paginator: 		Class.Empty,
	current_page:	1,

	initialize: function(list, paginator, options)
	{
		this.list = document.id(list);
		if (!this.list) return;
		this.paginator = document.id(paginator);
		this.setOptions(options);
		this.pages = Math.ceil(this.list.getChildren("li").length  / this.options.per_page );

		if (this.list.facetManager)
		{
			this.list.facetManager.addEvent('filterChanged', function() { this.filterChanged()}.bind(this));
			this.list.facetManager.addEvent('filterCleared', function() { this.filterCleared()}.bind(this));
			this.preprocessFacets();
		}
		
		this.createPagination();
		
		this.toPage(this.current_page);
	},
	
	createPagination: function()
	{
		if (!this.paginator) return;
		
		this.paginator.empty();
	    this.createPaginationNode( '&#60;&#60;&#32;&#80;&#114;&#101;&#118;', function(evt)
	    {
	        var evt = new DOMEvent( evt );
	        this.toPrevPage();
	        evt.stop();
	        return false;
	    }).inject( this.paginator );
	      
	    var li = new Element('li', {'class': 'pager'} );
	    var a = new Element('a', {'href': "#", 'class': 'goto-page', 'html': 'Page ' + (this.current_page || 1) + " of " + this.pages});
	    a.inject(li);
	    li.inject(this.paginator);
	    
	    this.createPaginationNode( '&#78;&#101;&#120;&#116;&#32;&#62;&#62;', function(evt)
	    {
	        var evt = new DOMEvent( evt );
	        this.toNextPage();
	        evt.stop();
	        return false;
	    }).inject( this.paginator );
	    
	    if (this.pages <= 1)
	    {
	    	this.paginator.getParent().setStyle('display', 'none');
	    }
	},
	  
	createPaginationNode: function( text, evt ) 
	{
	     var span = new Element( 'span' ).set( 'html', text );
	     if (text == '&#60;&#60;&#32;&#80;&#114;&#101;&#118;'){
	       var a = new Element( 'a', { 'href': '#', 'class': 'paginate' }).addEvent( 'click', evt.bind( this ) );
	     } else if (text == '&#78;&#101;&#120;&#116;&#32;&#62;&#62;'){
	       var a = new Element( 'a', { 'href': '#', 'class': 'paginate' }).addEvent( 'click', evt.bind( this ) );
	     } else {
	       var a = new Element( 'a', { 'href': '#'}).addEvent( 'click', evt.bind( this ) );
	     }
	     var li = new Element( 'li' );
	     span.inject( a.inject( li ) );
	     return li;	    
	},
	
	countRows: function()
	{
		  var filtered = this.list.getElements(".filtered").length;
		  var all = this.list.getChildren().length;
		  return all - filtered;
	},
	  
	updatePageCount: function()
	{
		this.pages = Math.ceil(this.countRows() / this.options.per_page );
		this.toPage(1);
		this.updatePage();
	},
	
	updatePage: function()
	{
		if (!this.paginator) return;
		
		var pagers = this.paginator.getElements("a.goto-page");
    	pagers.each(function(p) { p.innerHTML = 'Page ' + ((this.pages > 0) ? (this.current_page || 1) : "0") + " of " + this.pages; }.bind(this));    	
	},
	
	toPrevPage: function()
	{
		this.toPage( this.current_page - 1 );
	},
	
	toNextPage: function()
	{
		this.toPage( this.current_page + 1 );
	},
	
	toPage: function(page_num)
	{
		if (!this.paginator) return;
	    page_num = page_num.toInt();
	    if (page_num > this.pages || page_num < 1) return;
	    this.current_page = page_num;
	    this.low_limit = this.options.per_page * ( this.current_page - 1 );
	    this.high_limit = this.options.per_page * this.current_page;
	    
	    var kids = this.list.getChildren(":not(.filtered)");
	    
	    for(var i = 0; i < this.low_limit; ++i)
	    {
	    	kids[i].setStyle('display', 'none');
	    }
	    
	    for(var i = this.low_limit; i < this.high_limit && i < kids.length; ++i)
	    {
	    	kids[i].setStyle('display', this.options.display_mode);
	    }
	    
	    for(var i = this.high_limit; i < kids.length; ++i)
	    {
	    	kids[i].setStyle('display', 'none');
	    }
	    
	    this.updatePage();
	},

	preprocessFacets: function()
	{
		this.list.getElements('li').each(function(elt)
		{
			this.list.facetManager.preprocess(elt);
		}.bind(this));
		
		this.list.facetManager.preprocessComplete();
	},
	
	filterChanged: function()
	{
		this.list.getElements('li').each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			
			var match = this.list.facetManager.filter(elt);
			
			if (match)
			{
				elt.addClass('filtermatch');
				elt.show();
			}
			else
			{
				elt.addClass('filtered');
				elt.hide();
			}
		}.bind(this));
		
 	    this.updatePageCount();
	},
	
	filterCleared: function()
	{
		this.list.getElements('li').each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			elt.show();
		});
		
  	    this.updatePageCount();
	}
});


var Splitter = new Class({

	Implements: [Options, Events],
	
	container: null,
	layout: null,
	panes: [],
	lastPosition: null,
	resizing: false,
	
	options: 
	{
		orientation: 'vertical',
		split: [50, 50],
		minimumSize: 10,
		onresize: Class.Empty
	},
	
	initialize: function(element, options)
	{
		this.container = document.id(element);
		this.setOptions(options);
		
		this.panes = this.container.getChildren();
		if (this.panes.length != 2)
		{
			alert("Splitting Headache!");
			return;
		}
		
		this.layout = new Element('div');
		this.layout.setStyles({"width": "100%", "height": "100%"});
		this.splitter = new Element('div').addClass(this.getSplitterClass());
		this.splitter.setStyle("float", "left");
		this.layout.wraps(this.panes[0]);
		this.splitter.inject(this.layout);
		this.panes[1].inject(this.layout);
		this.layout.inject(this.container);
		
		this.panes.each(function(elt) { elt.setStyles({"overflow-y": "auto", "float": "left"}); });
		
		this.splitter.addEvent('mousedown', function(e) { this.startResize(new DOMEvent(e)); }.bind(this));
		this.calculateLayout();
	},
	
	getSplitterClass: function()
	{
		return "splitter_" + this.options.orientation;
	},
	
	setOrientation: function(orientation)
	{
		this.splitter.removeClass(this.getSplitterClass());
		this.options.orientation = orientation;
		this.splitter.addClass(this.getSplitterClass());
		this.calculateLayout();
	},
	
	calculateLayout: function()
	{
		if (this.resizing) return;
		
		this.resizing = true;
		
		var vert = this.options.orientation == 'vertical';
		var prop = vert ? "height" : "width";
		
		if (vert)
		{
			var width = this.layout.getWidth();
			this.panes.each(function(elt) { elt.setStyle("width", width); });
		}
		else
		{
			var height = this.layout.getHeight();
			this.panes.each(function(elt) { elt.setStyle("height", height); });
		}

		var splitterSize = 9;//(vert) ? this.splitter.getHeight() : this.splitter.getWidth();
		
		var ratio = [0,0];
		ratio[0] = this.options.split[0] / (this.options.split[0] + this.options.split[1]);
		ratio[1] = this.options.split[1] / (this.options.split[0] + this.options.split[1]);
		
		
		var size = (vert) ? this.layout.getHeight() : this.layout.getWidth();
		size -= splitterSize;
		if (size < 0) size = 0;
		
		this.panes[0].setStyle(prop, Math.floor(ratio[0] * size));
		this.panes[1].setStyle(prop, Math.floor(ratio[1] * size));
        this.fireEvent('resize');
        
        this.resizing = false;
	},
	
	startResize: function(e)
	{
		var vert = this.options.orientation == 'vertical';
		var axis = (vert) ? "y" : "x";
		var minF = (vert) ? "top" : "left";
		var maxF = (vert) ? "bottom" : "right";
		
        var performDrag = function(e) 
        {
            var pos = mousePosition(e);
            var coords = this.container.getCoordinates();
            
            var range = coords[maxF] - coords[minF];
            var thumb = (pos[axis] - coords[minF]) / range * 100;
            if (thumb < this.options.minimumSize) thumb = this.options.minimumSize;
            if (thumb > (100 - this.options.minimumSize)) thumb = 100 - this.options.minimumSize;
            
            this.options.split[0] = thumb;
            this.options.split[1] = 100 - thumb;
            
            this.calculateLayout();
        }.bind(this);

		var endDrag = function(e) 
        {            
            document.removeEvent('mousemove', performDrag);
            document.removeEvent('mouseup', endDrag);
        };
        
        var mousePosition = function(e) 
        {
            return { x: e.event.clientX + document.documentElement.scrollLeft,
                y: e.event.clientY + document.documentElement.scrollTop};
        };
        
        document.addEvents(
		{
			'mousemove': performDrag,
			'mouseup': endDrag
		});
	}
	
});

var CrossFader = new Class(
{	
	Implements : [Options],
	container: Class.Empty,
	options:
	{
		duration: 5000,
		transition: 1000,
		navigation: false,
		navigationType: 'byItem', // alternatives: 'Prev' & 'Next' (bubbles vs left/right arrows)
		navigationPosition: 'bottomLeft',
		navigationEdge: 'bottomLeft',
		navigationContainerClass: 'crossfader_nav',
		navigationClass: 'crossfader_nav_item', //alternative: 'crossfader_nav_arrow' (bubbles vs left/right arrows)
		navigationCurrentClass: 'crossfader_current',
		navigationHighlightClass: 'crossfader_highlight',
		navigationPreviousClass: 'crossfader_previous',
		navigationNextClass: 'crossfader_next',
		prevLinkPosition: 'centerLeft',
		prevLinkEdge: 'centerLeft',
		nextLinkPosition: 'centerRight',
		nextLinkEdge: 'centerRight',
		navigationShowNumbers: false,
		firstElementPosition: 'absolute'
	},
	idx: 0,
	elements: Class.Empty,
	navigationContainer: Class.Empty,
	navigationLinks: [],
	
	initialize: function(container, options)
	{
		this.container = document.id(container);
		this.setOptions(options);
		
		if (!this.container) return;
		
		this.elements = this.container.getChildren();
		
		if (this.elements.length == 0) return;
		
		this.elements[0].setStyles({display: 'block', position: this.options.firstElementPosition, visibility: 'visible', opacity: 1});
	
		if (this.elements.length == 1) return;
		
		this.createNavigation();
		
		this.start();
	},
	
	createNavigation: function()
	{
		if (!this.options.navigation) return;
		
		this.navigationContainer = new Element('div', {'class': 'crossfader_nav'});
		this.navigationContainer.setStyles({'position': 'absolute'});
		
		if (this.options.navigationType == 'PrevNext')
		{
			
			console.log("Element: ");
				
			var leftArrow = new Element('a', {href: '#', 'class': this.options.navigationClass});
			leftArrow.addClass(this.options.navigationPreviousClass);
			var rightArrow = new Element('a', {href: '#', 'class': this.options.navigationClass});
			rightArrow.addClass(this.options.navigationNextClass);
				
			leftArrow.set('html', '&nbsp;');
			rightArrow.set('html', '&nbsp;');
				
			//leftArrow.addEvent('mouseenter', function(e) { leftArrow.addClass(this.options.navigationPreviousClass);}.bind(this));
			//leftArrow.addEvent('mouseleave', function(e) { leftArrow.removeClass(this.options.navigationPreviousClass);}.bind(this));
			leftArrow.addEvent('click', function(e) { this.goToPrevious(); return false; }.bind(this));
			leftArrow.inject(this.navigationContainer);
				
			//rightArrow.addEvent('mouseenter', function(e) { rightArrow.addClass(this.options.navigationNextClass);}.bind(this));
			//rightArrow.addEvent('mouseleave', function(e) { rightArrow.removeClass(this.options.navigationNextClass);}.bind(this));
			rightArrow.addEvent('click', function(e) { this.goToNext(); return false; }.bind(this));
			rightArrow.inject(this.navigationContainer);
				
			this.navigationLinks.push(leftArrow);
			this.navigationLinks.push(rightArrow);			

			this.navigationContainer.inject(document.body);
			
			var size = this.container.getSize();
			this.navigationContainer.position({ relativeTo: this.container, position: 'topLeft', edge: 'topLeft'});
			this.navigationContainer.setStyles({width: size.x, height: size.y});
			
			leftArrow.position({ relativeTo: this.navigationContainer, position: this.options.prevLinkPosition, edge: this.options.prevLinkEdge});
			rightArrow.position({ relativeTo: this.navigationContainer, position: this.options.nextLinkPosition, edge: this.options.nextLinkEdge});
			

			window.addEvent('resize', function()
			{	
				var size = this.container.getSize();
				this.navigationContainer.position({ relativeTo: this.container, position: 'topLeft', edge: 'topLeft'});
				this.navigationContainer.setStyles({width: size.x, height: size.y});
				
				leftArrow.position({ relativeTo: this.navigationContainer, position: this.options.prevLinkPosition, edge: this.options.prevLinkEdge});
				rightArrow.position({ relativeTo: this.navigationContainer, position: this.options.nextLinkPosition, edge: this.options.nextLinkEdge});
				
			}.bind(this));
			
		}
		else if (this.options.navigationType == 'byItem') // bubbles for each item
		{
			this.elements.each(function(elt, idx)
			{
				console.log("Element: " + idx);
				
				var blob = new Element('a', {href: '#', 'class': this.options.navigationClass});
				
				if (this.options.navigationShowNumbers) 
				{
					blob.set('text', idx + 1);
				}
				else
				{
					blob.set('html', '&nbsp;');
				}
				blob.addEvent('mouseenter', function(e) { blob.addClass(this.options.navigationHighlightClass);}.bind(this));
				blob.addEvent('mouseleave', function(e) { blob.removeClass(this.options.navigationHighlightClass);}.bind(this));
				blob.addEvent('click', function(e) { this.goTo(idx); return false; }.bind(this));
				blob.inject(this.navigationContainer);
				
				this.navigationLinks.push(blob);
				
				this.navigationContainer.inject(document.body);
				this.navigationContainer.position({	relativeTo: this.container, 
					position: this.options.navigationPosition, 
					edge: this.options.navigationEdge});
				
				window.addEvent('resize', function()
				{	
					this.navigationContainer.position({	relativeTo: this.container, 
						position: this.options.navigationPosition, 
						edge: this.options.navigationEdge});
				}.bind(this));
			}.bind(this));
		}
		
	},
	
	start: function()
	{
		if (this.options.duration == 0) return;
		this.timer = this.next.periodical(this.options.duration, this);
	},
	
	next: function()
	{
		if (this.paused) return;
		
		if (this.idx >= 0)
		{
			this.elements[this.idx].set('tween', {duration: this.options.transition}).fade('out');
			if (this.options.navigation && this.options.navigationType == "byItem")
			{
				this.navigationLinks[this.idx].removeClass(this.options.navigationCurrentClass);
			}
		}
		
		++this.idx;
		if (this.idx >= this.elements.length) this.idx = 0;
		
		this.elements[this.idx].setStyles({'opacity': 0});
		this.elements[this.idx].set('tween', {duration: this.options.transition}).fade('in');
		if (this.options.navigation && this.options.navigationType == "byItem")
		{
			this.navigationLinks[this.idx].addClass(this.options.navigationCurrentClass);
		}
	},
	
	goTo: function(idx)
	{
		this.paused = true;
		
		if (this.idx == idx) return;

		if (this.idx >= 0 && this.options.navigation && this.options.navigationType == "byItem")
		{
			this.navigationLinks[this.idx].removeClass(this.options.navigationCurrentClass);
		}

		this.elements[this.idx].set('tween', {duration: this.options.transition}).fade('out');
		this.idx = idx;
		this.elements[this.idx].setStyles({'opacity': 0});
		this.elements[this.idx].set('tween', {duration: this.options.transition}).fade('in');
		
		if (this.options.navigation && this.options.navigationType == "byItem")
		{
			this.navigationLinks[this.idx].addClass(this.options.navigationCurrentClass);
		}
	},
	
	goToPrevious: function()
	{	
		var idx = this.idx - 1;
		if (idx < 0)
		{
			idx = this.elements.length - 1;	
		}
		this.goTo(idx);
	},
	
	goToNext: function()
	{
		var idx = this.idx + 1;
		if (idx >= this.elements.length) 
		{
			idx = 0;
		}
		this.goTo(idx);
	}
	
});

var FocusWatcher = new Class({
	
	Implements : [Options, Events],
	
    options: {
            elementTypes : ['textarea','input','select','button','a'],
            onFocusChanged: function(){} 
    }, 
    
	focus: null,
	blur: null,
	elements: [],
	
	initialize: function(options)
	{
    	var watch = this;
    	this.setOptions(options);
    	this.options.elementTypes.each(function(type) { this.elements.combine(($$(type))); }.bind(this));
    	this.elements.each(function(elt)
    	{
    		elt.addEvents({'focus': function(e) { this.focus = elt; this.fireEvent('focusChanged'); }.bind(watch),
    					   'blur':  function(e) { this.blur = elt; }.bind(watch)
    					  });
    	});
	}
});
    	
var ScrollWatcher = new Class(
{
	initialize: function()
	{
	},
	
	watch: function(position, element, above, below)
	{
		element = document.id(element);
		if (!element) return this;
		
		window.addEvent('scroll', function() { this.onScroll(position, element, above, below);}.bind(this));
		return this;
	},
	
	onScroll: function(position, element, above, below)
	{
		if (position >= 0)
		{
			if (window.getScroll().y > position)
			{
				if (typeof below === "function")
				{
					below(element);
				}
				else
				{
					element.removeClass(above).addClass(below);
				}
				
				return;
			}
			
			if (typeof above === "function")
			{
				above(element);
			}
			else
			{
				element.removeClass(below).addClass(above);
			}
		}
		else
		{
			// Negative positions are relative to the bottom of the page
			if (window.getScroll().y > (window.getScrollSize().y + position - element.getHeight()))
			{
				if (typeof below === "function")
				{
					below(element);
				}
				else
				{
					element.removeClass(above).addClass(below);
				}
				
				return;
			}
			
			if (typeof above === "function")
			{
				above(element);
			}
			else
			{
				element.removeClass(below).addClass(above);
			}
		}
	}
});

window.addEvent('domready', function() 
{ 
	document.focusWatcher = new FocusWatcher(); 
	document.scrollWatcher = new ScrollWatcher(); 
});

function addReloadHandlers(container)
{
	container.getElements('[data-url]').each(function(elt)
	{
		elt.reload = function(onComplete)
		{
			var handler = elt.get('data-url');
			
			var request = new Request.HTML(
			{
				url: handler,
				evalScripts: false,
				evalResponse: false,
				method: 'get', 
				onSuccess: function(tree, elements, html, script) 
				{ 
					elt.set('html', html);
					Browser.exec(script);
					
					if (typeof onComplete != "undefined")
					{
						onComplete();
					}
				}
			});
			request.send();			
		};
		
		elt.load = function(url, onComplete)
		{
			elt.set('data-url', url);
			elt.reload(onComplete);
		};
	});

}
// Implement reload() for page elements that supply data-url
window.addEvent('domready', function()
{
	addReloadHandlers(document.body);
});

// Mix-in a reloadPanel() method for all elements - easily reload the innermost containing panel.
Element.implement('reloadPanel', function(onComplete)
{
	var element = document.id(this);
	do
	{
		if (element.reload) 
		{
			element.reload(onComplete);
			return;
		}
		element = element.getParent();
	}
	while(element);		
});

Element.implement('loadPanel', function(url, onComplete)
{
	var element = document.id(this);
	do
	{
		if (element.match('[data-url]') && element.load) 
		{
			element.load(url, onComplete);
			return;
		}
		element = element.getParent();
	}
	while(element);
	
	// No panels present - navigate page to URL.
	go(url);
});

function loadPanel(elt, url, onComplete)
{
	new Element(elt).loadPanel(url, onComplete);
}

Element.implement("fitText", function() 
{
    var e = this.getParent();
    var maxWidth = e.getSize().x;
    var maxHeight = e.getSize().y;
    var sizeX = this.getSize().x;
    var sizeY = this.getSize().y;
    if (sizeY <= maxHeight && sizeX <= maxWidth)
        return;

    var fontSize = this.getStyle("font-size").toInt();
    while( (sizeX > maxWidth || sizeY > maxHeight) && fontSize > 4 ) 
    {
        fontSize -= .5;
        this.setStyle("font-size", fontSize + "px");
        sizeX = this.getSize().x;
        sizeY = this.getSize().y;
    }
    return this;
});

window.addEvent('domready', function()
{
	$$(".fitText").fitText();
});

var CountIndicator = new Class(
{
	Implements: Options,
	container: Class.Empty,
	
	options:
	{
		cssClass: 'count_indicator',
		position: 'bottomRight',
		edge:	  'bottomRight',
		showZero: false,
		maximum: 99
	},
	
	initialize: function(container, options)
	{
		this.setOptions(options);
		this.container = document.id(container);
		this.refresh();
	},
	
	refresh: function()
	{
		this.container.getElements('[data-count]').each(function(element)
		{
			if (!element.countDisplay)
			{
				var div = new Element('div', {'class': this.options.cssClass});
				div.setStyles({display: 'block', width: 'auto', position: 'absolute'});
				document.body.adopt(div);
				element.countDisplay = div;
			}
			
			var count = element.get('data-count');
			element.countDisplay.set('text', (count > this.options.maximum) ? this.options.maximum + "+" : count);
			if (count == 0 && this.options.showZero == false)
			{
				element.countDisplay.setStyle('display', 'none');
			}
			
			element.countDisplay.position({relativeTo: element, position: this.options.position, edge: this.options.edge});
			
		}.bind(this));
	}
});

var ToggleManager = new Class(
{
	Implements: Options,
	
	container: null,
	
	options:
	{
		container: 	null,
		show:	  	function(elt) { elt.reveal(); },
		hide:	  	function(elt) { elt.dissolve(); },
		selector: 	'.toggle',
		cssOpen:	'open',
		cssClosed:	'closed',
		event:		'click'
	},
	
	initialize: function(options)
	{
		this.setOptions(options);
	
		this.container = this.options.container ? document.id(this.options.container) : document.body;
		
		if (!this.container) return;
		
		this.container.getElements(this.options.selector).each(function(toggle)
		{
			toggle.addEvent(this.options.event, function(event)
			{
				var elt = new DOMEvent(event).target;
				
				var target = elt.get('target');
				if (!target) elt.get('data-target');
				if (!target) return;
				
				target = document.id(target);
				
				if (elt.hasClass(this.options.cssOpen))
				{
					this.options.hide(target);
					elt.removeClass(this.options.cssOpen);
					elt.addClass(this.options.cssClosed);
				}
				else
				{
					this.options.show(target);
					elt.removeClass(this.options.cssClosed);
					elt.addClass(this.options.cssOpen);
				}
				
				return false;
				
			}.bind(this));
		}.bind(this));
	}
});		
	
var ConnectivityChecker = new Class(
{
    Implements: [Options, Events],
    onlineFlag: true,
    timerID: null,
    options:
    {
        onOnline:   function() {},
        onOffline:  function() {},
        timeout:    2000,
        period:     5000, 
        url:        "/action/component/ping"
    },

    initialize: function(options)
    {
        this.setOptions(options);
        this.start();
    },

    start: function()
    {
        if (this.timerID) return;
        this.checkConnectivity();
        this.timerID = this.checkConnectivity.periodical(this.options.period, this);
    },

    stop: function()
    {
        if (this.timerID)
        {
            clearInterval(this.timerID);
            this.timerID = null;
        }
    },

    checkConnectivity: function()
    {
        var online = true;

        var request = new Request(
        {
            url: this.options.url,
            timeout: this.options.timeout,
            onSuccess: function(response)
            {
                online = (response == "OK");
                this.updateConnectivity(online);
            }.bind(this),
            onFailure: function()
            {
                this.updateConnectivity(false);
            }.bind(this),
            onTimeout: function()
            {
                this.updateConnectivity(false);
            }.bind(this)
        });
        request.send();
    },

    updateConnectivity: function(online)
    {
        if (online != this.onlineFlag)
        {
            if (!online)
            {
                this.fireEvent('offline');
            }
            else
            {
                this.fireEvent('online');
            }

            this.onlineFlag = online;
        }        
    }
});