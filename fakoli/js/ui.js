var Curtain = new Class(
{
	curtain: Class.Empty,

	initialize: function()
	{
		this.curtain = $('curtain');
	},
	
	lower: function(onComplete)
	{
		var cw = window.innerWidth == undefined ?
				((document.documentElement.clientWidth == 0) ? document.body.clientWidth : document.documentElement.clientWidth)
												 : window.innerWidth;
		var ch = window.innerHeight == undefined ? 
				((document.documentElement.clientHeight == 0) ? document.body.clientHeight : document.documentElement.clientHeight)
												 : window.innerHeight;
		
		var dw = document.width == undefined ? document.body.offsetWidth: document.width;
		var dh = document.height == undefined ? document.body.offsetHeight: document.width;
		 
		var w = dw;
		var h = document.height;
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



var ModalDialog = new Class(
{
    Implements: [Options],
    
    options: 
    {
		draggable: false,
		handle: Class.Empty,
		closeLink: Class.Empty,
		body:	Class.Empty
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
    		this.options.body.set('text', "Loading...");
    		var request = new Request(
    		{
    			method: 'get', 
    			url: fragmentURL, 
    			onSuccess: function(html) 
    			{ 
    				this.options.body.set('html', html);
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
		left:	   Class.Empty
	},
	
	element: Class.Empty,
	
	initialize: function(element, options)
	{
		this.element = $(element);
		this.setOptions(options);
		if (this.element) this.element.setStyle('display', 'none');
    	if (this.options.body) this.options.body = $(this.options.body);
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

		this.element.setStyles({position: (this.draggable || window.ie6) ? 'absolute' : 'fixed', top: y, left: x, 'z-index': 150});
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
