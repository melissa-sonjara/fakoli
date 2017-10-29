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

var ToolTip = new Class(
{
	Implements: [Options],
	options:
	{
		'css': 		'tooltip_box',
		'width':	'auto',
		'loading':	"Loading...",
		'delay':	0,
		'fade':		false
	},
	
	id:			'',
	handlerURL: '',
	div:		null,
	
	initialize: function(id, options)
	{
		this.setOptions(Object.merge(ToolTip.globalOptions, options));
		this.id = id;
	},
	
	show: function(link, evt, handlerURL)
	{
		ToolTip.currentLink = document.id(link);
		
		var event = new DOMEvent(evt).stop();
		
		loadTooltip = function (link, event, tip, handlerURL)
		{
			if (link != ToolTip.currentLink) return;
						
			if (tip.div == null) 
			{
				tip.create();
				tip.position(event);
			}
			else if(tip.handlerURL == handlerURL)
			{
				tip.position(event);
				return;
			}
			
			tip.handlerURL = handlerURL;
			
			var cursor = link.getStyle('cursor');
		
			link.setStyle('cursor', 'progress');
		   	
			var request = new Request.HTML(
		    {	 	
		    	evalScripts: false,
				evalResponse: false,
				method: 'get', 
				url: handlerURL, 
				onSuccess: function(tree, elements, html, script) 
				{
		    		tip.div.set('text', '');
					tip.div.adopt(tree);
					tip.position(event);
					link.setStyle('cursor', cursor);
					Browser.exec(script);
				}
			});
			request.send();
		};
			
		loadTooltip.delay(this.options.delay, null, [ToolTip.currentLink, event, this, handlerURL]);
	},

	showText: function(link, evt, text)
	{
		ToolTip.currentLink = document.id(link);
		
		var event = new DOMEvent(evt).stop();
		
		textTooltip = function(link, event, tip, text)
		{
			//if (link != ToolTip.currentLink) return;
			
			if (tip.div == null) 
			{
				tip.create();
			}

			tip.position(event);
			tip.div.set('html', text);
		};
		
		textTooltip.delay(this.options.delay, null, [ToolTip.currentLink, event, this, text]);	
	},
	
	create: function()
	{
		this.div = new Element('div', {'id': this.id, 'class': this.options.css});
		this.div.set('html', this.options.loading);
		var doc = document.body ? document.body : document.documentElement;		
		this.div.inject(doc, 'top'); 
	},
	
	position: function(event)
	{
		var z = window.getZIndex(ToolTip.currentLink);
		++z;
		
		if (!this.options.position)
		{
			this.div.setStyles({'display':  	'block',
								'opacity':		0,
							    'left':	   		event.page.x + 4,
							    'top':      	event.page.y,
							    'position':		'absolute',
							    'width':		this.options.width,
							    'z-index':		z});
		}
		else
		{
			this.div.position({'relativeTo': ToolTip.currentLink, 'position': this.options.position, 'edge': this.options.edge});
			this.div.setStyles({'display':  	'block',
								'opacity':		0,
							    'position':		'absolute',
							    'width':		this.options.width,
							    'z-index':		z});
		}
		
		if (this.options.fade)
		{
			this.div.fade('in');
		}
		else
		{
			this.div.setStyles({'display': 'block', 'visibility': 'visible', 'opacity': 1});
		}
	},

	hide: function()
	{
		ToolTip.currentLink = null;
		if (this.div == null) return;
		
		if (this.options.fade)
		{
			this.div.fade('out');
		}
		else
		{
			this.div.setStyles({'display': 'none', 'visibility': 'hidden', 'opacity': 0});
		}
	}
});

ToolTip.globalOptions = {};
ToolTip.tooltips = {};
ToolTip.currentLink = null;

ToolTip.getToolTip = function(id, options)
{
	if (!ToolTip.tooltips[id])
	{
		ToolTip.tooltips[id] = new ToolTip(id, options);
	}
	return ToolTip.tooltips[id];
};

function showToolTip(link, evt, id, handlerURL, width)
{
	var tip = ToolTip.getToolTip(id);
	if (width) tip.options.width = width;
	tip.show(link, evt, handlerURL);
};

function showTextToolTip(link, evt, id, text, width, position, edge)
{
	var tip = ToolTip.getToolTip(id);
	if (width) tip.options.width = width;
	if (position) tip.options.position = position;
	if (edge) tip.options.edge = edge;
	tip.showText(link, evt, text);
}

function hideToolTip (id)
{
	var tip = ToolTip.getToolTip(id);
	tip.hide();
};
