var Shape = new Class(
{
	Implements: [Options, Events],
	
	paper: Class.Empty,
	
	options: 
	{
		draggable: true,
		shown: true,
		onClick: Class.Empty
	},
	
	primitives: {},
	shapeSet: Class.Empty,
	origin: {x: 0, y: 0},
	connectors: [],
	clickHander: null,
	
	initialize: function(paper, options)
	{
		this.paper = paper;
		this.setOptions(options);
		
		this.shapeSet = this.paper.set();
		this.clickHandler = this.doClick.bind(this);
	},
	
	doClick: function()
	{
		if (this.dragging || !this.options.shown) return;
		this.fireEvent('click');
	},
	
	addPrimitive: function(name, primitive)
	{
		this.primitives[name] = primitive;
		this.shapeSet.push(primitive);
		
		if (this.options.draggable)
		{
			primitive.drag(this.dragMove, this.startDrag, this.endDrag, this, this, this);
		}
		
		primitive.attr({'opacity': (this.options.shown ? 1 : 0)});
		if (this.options.shown)
		{
			primitive.click(this.clickHandler);
		}
		return this;
	},

	addConnector: function(connector)
	{
		this.connectors.push(connector);
	},
	
	attr: function()
	{
		if (arguments.length == 1)
		{
			this.shapeSet.attr(arguments[0]);
		}
		else if (arguments.length == 2)
		{
			name = arguments[0];
			attrs = arguments[1];
			
			p = this.primitives[name];
			p.attr(attrs);
		}
		return this;
	},
	
	animate: function(params, length, transition, callback)
	{
		this.shapeSet.animate(params, length, transition, callback);
		return this;
	},
	
	animatePrimitive: function(name, params, length, transition, callback)
	{
		this.primitives[name].animate(params, length, transition, callback);
		return this;
	},
	
	show: function(length, transition)
	{
		if (this.options.shown) return;
		
		this.options.shown = true;

		Object.values(this.primitives).each(function(elt) 
		{ 
			elt.show(); 
		}.bind(this));
		
		this.connectors.each(function(c)
		{
			if (c.to.options.shown)
			{
				c.show(length, transition);
			}
		});
		
		if (length)
		{
			this.animate({'opacity': 1}, length, transition);
		}
		else
		{
			this.attr({'opacity': 0});
		}		
	},
	
	hide: function(length, transition)
	{
		this.options.shown = false;
		this.connectors.each(function(c)
		{
			c.hide(length, transition);
		});
		
		if (length)
		{
			this.animate({'opacity': 0}, length, transition, function() 
			{
				Object.values(this.primitives).each(function(elt) { elt.hide();/* elt.unclick(this.clickHandler); */});
			}.bind(this));
		}
		else
		{
			this.attr({'opacity': 0});
			Object.values(this.primitives).each(function(elt) { elt.hide();/* elt.unclick(this.clickHandler);*/});
		}
	},
	
	move: function(x, y, length, transition)
	{
		var dx = x - this.origin.x;
		var dy = y - this.origin.y;
		
		Object.values(this.primitives).each(function(elt)
		{
			if (length)
			{
				elt.animate({'x': elt.attrs.x + dx, 'y': elt.attrs.y + dy}, length, transition);
			}
			else
			{
				elt.attr({'x': elt.attrs.x + dx, 'y': elt.attrs.y + dy});
			}
				
		}.bind(this));
		
		this.origin.x = x;
		this.origin.y = y;
		
		this.connectors.each(function(conn)				
		{
			conn.layout(length, transition);
		});	
	},
	
	getBBox: function()
	{
		var bb = {x: 200000, y: 200000, x2: -200000, y2: -200000, width: -200000, height: -200000};
		
		Object.values(this.primitives).each(function(p)
		{
			var bb2 = p.getBBox();
			if (bb2.x < bb.x) bb.x = bb2.x;
			if (bb2.y < bb.y) bb.y = bb2.y;
			if (bb2.x2 > bb.x2) bb.x2 = bb2.x2;
			if (bb2.y2 > bb.y2) bb.y2 = bb2.y2;
		});
		
		bb.width = bb.x2 - bb.x;
		bb.height = bb.y2 - bb.y;
		
		dx = this.origin.x - bb.x;
		dy = this.origin.y - bb.y;
		bb.x = this.origin.x;
		bb.y = this.origin.y;
		bb.x2 += dx;
		bb.y2 += dy;
		
		return bb;
	},
	
	startDrag: function(x, y)
	{
		this.dragging = true;
		this.dragX = this.origin.x;
		this.dragY = this.origin.y;
	},
	
	dragMove: function(dx, dy)
	{
		console.debug("(dx,dy):"+dx+","+dy);
		this.move(this.dragX + dx, this.dragY + dy);
	},
	
	endDrag: function(x, y)
	{
		this.dragging = false;
	}
});


var TextBox = new Class({
	
	Extends: Shape,
	
	initialize: function(paper, x, y, w, h, text, radius, options)
	{
		this.parent(paper, options);
		this.setOptions(options);
		
		this.origin.x = x;
		this.origin.y = y;
		
		if (!radius) radius = 0;
		
		this.addPrimitive('box', this.paper.rect(x, y, w, h, radius));		
		this.addPrimitive('text', this.paper.text(x + (w / 2), y + (h / 2), text));
	},
	
	setText: function(text)
	{
		this.primitives.text.attr('text', text);
		return this;
	},
	
	click: function(handler)
	{
		this.shapeSet.click(handler);
		return this;
	}
});


var Connector = new Class({

	Extends: Shape,
	
	from: 	Class.Empty,
	to:   	Class.Empty,
	attrs:	Class.Empty,
	
	initialize: function(paper, from, to, options, attrs)
	{
		this.parent(paper, options);
		this.from = from;
		this.to = to;
		this.attrs = attrs;
		
		this.from.addConnector(this);
		this.to.addConnector(this);
		this.layout();
	},
	
	layout: function(length, transition)
	{
		//TODO: Cribbed from raphaeljs.com demo - redo (or at least understand!)
		var bb1 = this.from.getBBox(),
        	bb2 = this.to.getBBox(),
        	p = [{x: bb1.x + bb1.width / 2, y: bb1.y - 1},				// NORTH
        	     {x: bb1.x + bb1.width / 2, y: bb1.y + bb1.height + 1},	// SOUTH
        	     {x: bb1.x - 1, y: bb1.y + bb1.height / 2},				// WEST
        	     {x: bb1.x + bb1.width + 1, y: bb1.y + bb1.height / 2},	// EAST
		        {x: bb2.x + bb2.width / 2, y: bb2.y - 1},
		        {x: bb2.x + bb2.width / 2, y: bb2.y + bb2.height + 1},
		        {x: bb2.x - 1, y: bb2.y + bb2.height / 2},
		        {x: bb2.x + bb2.width + 1, y: bb2.y + bb2.height / 2}],
		    d = {}, dis = [];
		
	    for (var i = 0; i < 4; i++) 
	    {
	        for (var j = 4; j < 8; j++) 
	        {
	            var dx = Math.abs(p[i].x - p[j].x),
	                dy = Math.abs(p[i].y - p[j].y);
	            if ((i == j - 4) || (((i != 3 && j != 6) || p[i].x < p[j].x) && ((i != 2 && j != 7) || p[i].x > p[j].x) && ((i != 0 && j != 5) || p[i].y > p[j].y) && ((i != 1 && j != 4) || p[i].y < p[j].y))) 
	            {
	                dis.push(dx + dy);
	                d[dis[dis.length - 1]] = [i, j];
	            }
	        }
	    }
	    
	    if (dis.length == 0) 
	    {
	        var res = [0, 4];
	    } 
	    else 
	    {
	        res = d[Math.min.apply(Math, dis)];
	    }
	    var x1 = p[res[0]].x,
	        y1 = p[res[0]].y,
	        x4 = p[res[1]].x,
	        y4 = p[res[1]].y;

	    dx = Math.max(Math.abs(x1 - x4) / 2, 80);
	    dy = Math.max(Math.abs(y1 - y4) / 2, 80);
	    var x2 = [x1, x1, x1 - dx, x1 + dx][res[0]].toFixed(3),
	        y2 = [y1 - dy, y1 + dy, y1, y1][res[0]].toFixed(3),
	        x3 = [0, 0, 0, 0, x4, x4, x4 - dx, x4 + dx][res[1]].toFixed(3),
	        y3 = [0, 0, 0, 0, y1 + dy, y1 - dy, y4, y4][res[1]].toFixed(3);
	    
	    var path = ["M", x1.toFixed(3), y1.toFixed(3), "C", x2, y2, x3, y3, x4.toFixed(3), y4.toFixed(3)].join(",");
	    	    
	    if (this.options.arrowTo)
	    {
	    
	    }
	    	    
	    if (this.primitives["line"])
	    {
	    	if (!length)
	    	{
	    		this.attr("line", {'path': path});
	    	}
	    	else
	    	{
	    		this.animatePrimitive("line", {'path': path}, length, transition);
	    	}
	    }
	    else 	    
	    {
	    	this.addPrimitive("line", this.paper.path(path));
	    	this.attr("line", this.attrs);
	    }
	    
	    if (this.options.arrowFrom)
	    {
	    	var arrowPath;
	    	
	    	switch(res[0])
	    	{
	    		case 0:	// NORTH
	    			arrowPath = ["M", x1.toFixed(3), y1.toFixed(3), "l,4,-12,l,-8,0,Z"].join(",");
	    			break;
	    			
	    		case 1:	// SOUTH
	    			arrowPath = ["M", x1.toFixed(3), y1.toFixed(3), "l,4,12,l,-8,0,Z"].join(",");
	    			break;
	    			
	    		case 2: // WEST
	    			arrowPath = ["M", x1.toFixed(3), y1.toFixed(3), "l,-12,4,l,0,-8,Z"].join(",");
	    			break;
	    			
	    		case 3: // EAST
	    			arrowPath = ["M", x1.toFixed(3), y1.toFixed(3), "l,12,4,l,0,-8,Z"].join(",");
	    			break;	
	    	}
	    			
	    	if (this.primitives["arrowFrom"])
			{
				if (!length)
				{
					this.attr("arrowFrom", {'path': arrowPath});
				}
				else
				{
					this.animatePrimitive("arrowFrom", {'path': arrowPath}, length, transition);
				}
			}
			else
			{
				this.addPrimitive("arrowFrom", this.paper.path(arrowPath));
		    	this.attr("arrowFrom", {'stroke-width': 1, 'stroke': this.attrs.stroke, 'fill': this.attrs.stroke});
			}
	    }

	}
});

