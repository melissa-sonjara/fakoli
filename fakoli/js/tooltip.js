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
		'loading':	"Loading..."
	},
	
	id:			'',
	handlerURL: '',
	div:		null,
	
	initialize: function(id, options)
	{
		this.setOptions(ToolTip.globalOptions);
		this.options.merge(options);
		this.id = id;
	},
	
	show: function(link, evt, handlerURL)
	{
		ToolTip.currentLink = document.id(link);
		
		var event = new Event(evt).stop();
		
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
					$exec(script);
				}
			});
			request.send();
		};
			
		loadTooltip.delay(1000, null, [ToolTip.currentLink, event, this, handlerURL]);
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
		if (!this.options.position)
		{
			this.div.setStyles({'display':  	'block',
								'opacity':		0,
							    'left':	   		event.page.x + 4,
							    'top':      	event.page.y,
							    'position':		'absolute',
							    'width':		this.options.width});
		}
		else
		{
			this.div.position({'relativeTo': ToolTip.currentLink, 'position': this.options.position, 'edge': this.options.edge});
			this.div.setStyles({'display':  	'block',
								'opacity':		0,
							    'position':		'absolute',
							    'width':		this.options.width});
		}
		
		this.div.fade('in');
	},

	hide: function()
	{
		ToolTip.currentLink = null;
		if (this.div == null) return;
		
		this.div.fade('out');
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

function hideToolTip (id)
{
	var tip = ToolTip.getToolTip(id);
	tip.hide();
};
