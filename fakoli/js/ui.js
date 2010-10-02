var Curtain = new Class(
{
	curtain: Class.Empty,

	initialize: function()
	{
		this.curtain = $('curtain');
	},
	
	lower: function(onComplete)
	{
		var opacity = this.curtain.get('opacity');
		var display = this.curtain.getStyle('display');
		if (opacity > 0 && display == 'block')
		{
			if (onComplete) onComplete();
			return;
		}
		
		var cw = window.innerWidth == undefined ?
				((document.documentElement.clientWidth == 0) ? document.body.clientWidth : document.documentElement.clientWidth)
												 : window.innerWidth;
		var ch = window.innerHeight == undefined ? 
				((document.documentElement.clientHeight == 0) ? document.body.clientHeight : document.documentElement.clientHeight)
												 : window.innerHeight;
		
		var dw = document.width == undefined ? document.body.offsetWidth: document.width;
		var dh = document.height == undefined ? document.body.offsetHeight: document.height;
		 
		var w = dw;
		var h = dh;
		var l = document.left;
		
		if (ch > h) h = ch;
		
		this.curtain.setStyles({top: 0,
								left: l,
								width: w,
								height: h,
								display: 'block',
								opacity: 0,
								position: 'absolute'});
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
		if (onComplete)
		{
			new Fx.Tween(this.curtain).start('opacity', 0.0).chain(onComplete);
		}
		else
		{
			this.curtain.fade(0.0);
		}
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

function modalPopup(title, url, width, height)
{
	var popup = new ModalDialog('modalPopup', {'title': title, 'width': width, 'height': height});
	popup.show(null, url);
}

var ModalDialog = new Class(
{
    Implements: [Options],
    
    options: 
    {
		draggable: 	false,
		handle: 	Class.Empty,
		closeLink: 	Class.Empty,
		body:		Class.Empty,
		width: 		"500px",
		height:		"auto",
		title:		Class.Empty
	},
    
    element: Class.Empty,
    
    initialize: function(element, options)
    {
    	this.element = $(element);
    	this.setOptions(options);
    	
    	if (!this.element)
    	{
    		this.element = this.createDialog(element);
    	}
    	
    	if (this.element) this.element.setStyle('display', 'none');
    	if (this.options.body) 
    	{
    		this.options.body = $(this.options.body);
    	}
    	else
    	{
    		this.options.body = $(this.element.id + "Body");
    	}
    	
    	if (this.options.closeLink)
    	{
    		$(this.options.closeLink).addEvent('click', function(e) { new Event(e).stop(); this.hide(); }.bind(this));
    	}
    	
    	if (this.options.title)	this.setTitle(this.options.title);
    	this.element.setStyles({'width': this.options.width, 'height': this.options.height});
    },
    
    setTitle: function(title)
    {
    	this.options.title = title;
    	if (this.element)
    	{
    		$(this.element.id + 'Title').set('text', title);
    	}
    },
    
    createDialog: function(id)
    {
    	var dialog = new Element('div', {'class': 'dialog', 
    									 'display': 'none', 
    									 'id': id, 
    									 'width': this.options.width, 
    									 'height': this.options.height});
    	
    	var dialog_header = new Element('div', {'class': 'dialog_header', 'id': id + 'Header'});
    	
    	var padding = new Element('div');
    	padding.setStyles({'padding': '4px'});
    	
    	var body = new Element('div', {'id': id + 'Body', 'class': 'dialog_body'});
    	
    	padding.set('html', "<div style='float: right'>&nbsp;<a id='close" + id + "' href=''>Close &times;</a></div>" +
    						"<span style='font-weight: bold' id='" + id + "Title'>" + this.options.title + "</span>");
    	
    	this.options.closeLink = "close" + id;
    	this.options.body = body;
     	
    	padding.inject(dialog_header);    	
    	dialog_header.inject(dialog, 'top');
    	body.inject(dialog, 'bottom');
		var doc = $(document.body ? document.body : document.documentElement);
		
		doc.adopt(dialog);
		return dialog;
    },
    	
    center: function()
    {
    	if (this.options.body)
    	{
    		this.options.body.setStyle('height', 'auto');
    	}
    	
    	var curtain = $('curtain');
    	var windowHeight = window.innerHeight ? window.innerHeight :
    		document.documentElement.clientHeight ?
    		document.documentElement.clientHeight : document.body.clientHeight; 
    	
    	if (this.element.offsetHeight > windowHeight && this.options.body)
    	{
    		this.options.body.setStyles({'height': windowHeight * 0.9, 'overflow-y': 'auto'});
    	}
    	
    	var x = (document.body.clientWidth - this.element.offsetWidth) / 2;
    	var y = (windowHeight - this.element.offsetHeight) / 2;
    	this.element.setStyles({position: (this.draggable || window.ie6) ? 'absolute' : 'fixed', top: y, left: x, 'z-index': 150});
    },
    
    show: function(onComplete, fragmentURL)
    {
    	var reload = this.element.getStyle('display') != 'none';
    	
    	if (this.options.draggable)
    	{
    		var options = 
    		{
    			preventDefault: true, 
    			onDrag: function(element, event) 
    			{ 
    				new Event(event).stop(); 
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
    				this.options.body.adopt(tree);
    				$exec(script);
    				this.center();
    			}.bind(this)
    		});
    		request.send();
    	}
    	
    	window.lowerCurtain(function() {
    		this.element.setStyle('display', 'block');
    		this.center();
    		if (onComplete) onComplete(this);
    	}.bind(this));
    },
    
    hide: function()
    {
    	window.raiseCurtain();
    	this.element.setStyle('display', 'none');
    }
});


var FloatingDialog = new Class(
{
	Implements: [Options],
	
	options:
	{
		draggable: false,
		handle:    Class.Empty,
		body:	   Class.Empty,
		top: 	   Class.Empty,
		left:	   Class.Empty,
		position:  Class.Empty
	},
	
	element: Class.Empty,
	
	initialize: function(element, options)
	{
		this.element = $(element);
		this.setOptions(options);
		if (this.element) this.element.setStyle('display', 'none');
    	if (this.options.body) this.options.body = $(this.options.body);
    	if (this.options.closeLink)
    	{
    		$(this.options.closeLink).addEvent('click', function(e) { new Event(e).stop(); this.hide(); }.bind(this));
    	}
    },
	
    position: function()
    {
		var x, y;
		
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
			pos = (this.draggable || window.ie6) ? 'absolute' : 'fixed';
		}
		
		this.element.setStyles({position: pos , top: y, left: x, 'z-index': 150});
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
    				new Event(event).stop(); 
    			} 
    		};
    		
    		if (this.options.handle)
    		{
    			options.handle = this.options.handle;
    		}
    		
    		var drag = new Drag.Move(this.element, options);
    	}
    	
    	this.position();
    	
    	if (fragmentURL && this.options.body)
    	{
    		this.options.body.set('text', "Loading...");
    		var request = new Request(
    		{
    			method: 'get', 
    			url: fragmentURL, 
    			onSuccess: function(html) 
    			{ 
    				this.options.body.set('html', html);
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
    	new Fx.Tween(this.element).start('opacity', 0).chain(function() { this.element.setStyle('display', 'none');}.bind(this));
    }
});


var ProgressiveSearch = new Class({
	
	Implements: Options,
	  
	options: 
	{
		search:			Class.Empty,
		minimumLength: 	4,
		height: 		'150px',
		width:			Class.Empty,
		cssClass:		''
	},
  
	element:	Class.Empty,
	list: 		Class.Empty,
	container:	Class.Empty,
	allowHide:	true,
	
	initialize: function(element, options)
	{
		this.setOptions(options);
		this.element = $(element);
		this.list = new Element('div', {'id': this.element.id + '_progressive_search', 'class': 'progressive_search'});
		this.list.setStyles({'display': 'none', 'position': 'absolute', 'max-height': this.options.height, 'overflow-y': 'auto'});
		this.container = new Element('div');
		
		this.list.adopt(this.container);
		
		if (this.options.cssClass) this.list.addClass(this.options.cssClass);
		
		var body = $(document.body ? document.body : document.documentElement);
		body.adopt(this.list);
		
		this.element.addEvent('keyup', function() { this.onKeyPress();}.bind(this));
		this.element.addEvent('blur', function() { this.hideList();}.bind(this));
		this.list.addEvent('mouseover', function() { this.allowHide = false; }.bind(this));
		this.list.addEvent('mouseout', function() { this.allowHide = true; }.bind(this));
	},
	
	onKeyPress: function()
	{
		var val = this.element.value;
		
		if (val.length < this.options.minimumLength) 
		{
			this.list.setStyle('display', 'none');
			return;
		}
		
		var name = this.element.id;
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
	
	showList: function(html)
	{
		this.container.set('html', html);
		var coords = this.element.getCoordinates();
		this.list.setStyles({'top': coords.bottom, 'left': coords.left, 'width': this.options.width ? this.options.width : coords.width, 'max-height': this.options.height, 'display': 'block'});
	},
	
	hideList: function()
	{
		if (!this.allowHide) return;
		
		this.list.setStyle('display', 'none');
	}
});



var PaginatingList = new Class(
{
	Implements: Options,
  
	options: 
	{
    	per_page: 10
	},
	
	list: 			Class.Empty,
	paginator: 		Class.Empty,
	current_page:	1,

	initialize: function(list, paginator, options)
	{
		this.list = $(list);
		this.paginator = $(paginator);
		this.setOptions(options);
		this.pages = Math.ceil(this.list.getElements("li").length  / this.options.per_page );
		
		this.createPagination();
		
		this.toPage(this.current_page);
	},
	
	createPagination: function()
	{
		this.paginator.empty();
	    this.createPaginationNode( '&#171;', function(evt)
	    {
	        var evt = new Event( evt );
	        this.toPrevPage();
	        evt.stop();
	        return false;
	    }).injectInside( this.paginator );
	      
	    var li = new Element('li', {'class': 'pager'} );
	    var a = new Element('a', {'href': "#", 'class': 'goto-page', 'html': 'Page ' + (this.current_page || 1) + " of " + this.pages});
	    a.injectInside(li);
	    li.injectInside(this.paginator);
	    
	    this.createPaginationNode( '&#187;', function(evt)
	    {
	        var evt = new Event( evt );
	        this.toNextPage();
	        evt.stop();
	        return false;
	    }).injectInside( this.paginator );
	},
	  
	createPaginationNode: function( text, evt ) 
	{
	     var span = new Element( 'span' ).set( 'html', text );
	     if (text == '&#171;'){
	       var a = new Element( 'a', { 'href': '#', 'class': 'previous-page' }).addEvent( 'click', evt.bind( this ) );
	     } else if (text == '&#187;'){
	       var a = new Element( 'a', { 'href': '#', 'class': 'next-page' }).addEvent( 'click', evt.bind( this ) );
	     } else {
	       var a = new Element( 'a', { 'href': '#'}).addEvent( 'click', evt.bind( this ) );
	     }
	     var li = new Element( 'li' );
	     span.injectInside( a.injectInside( li ) );
	     return li;	    
	},
	
	updatePage: function()
	{
		var pagers = this.paginator.getElements("a.goto-page");
    	pagers.each(function(p) { p.innerHTML = 'Page ' + (this.current_page || 1) + " of " + this.pages; }.bind(this));    	
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
	    page_num = page_num.toInt();
	    if (page_num > this.pages || page_num < 1) return;
	    this.current_page = page_num;
	    this.low_limit = this.options.per_page * ( this.current_page - 1 );
	    this.high_limit = this.options.per_page * this.current_page;
	    
	    var kids = this.list.getChildren();
	    
	    for(var i = 0; i < this.low_limit; ++i)
	    {
	    	kids[i].setStyle('display', 'none');
	    }
	    
	    for(var i = this.low_limit; i < this.high_limit && i < kids.length; ++i)
	    {
	    	kids[i].setStyle('display', 'list-item');
	    }
	    
	    for(var i = this.high_limit; i < kids.length; ++i)
	    {
	    	kids[i].setStyle('display', 'none');
	    }
	    
	    this.updatePage();
	}
});
