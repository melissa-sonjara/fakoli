var ContextMenu = new Class(
{
	Implements: [Options],
	
	elements: [],
	menu: 	null,
	trigger:	'contextmenu',
	options:
	{
		position: 'pointer',
		offsetX: 0,
		offsetY: 0
	},
	
	initialize: function(menu, elementSelector, trigger, options)
	{
		this.menu = document.id(menu);

		if (!this.menu) return;

		this.trigger = trigger;
		this.setOptions(options);
		
		var me = this;
		
		elementSelector = $splat(elementSelector);
		elementSelector.each(function(selector)
		{
			$$(selector).each(function(elt)
			{
				elt.addEvent(trigger, function(e)
				{
					ContextMenu.root = elt;
					var event = new DOMEvent(e).stop();
					me.show(event);
				});
			});
		});
		
		var doc = document.id(document.body ? document.body : document.documentElement);
		
		doc.addEvent('click', function(e) { me.hide(); });
		
		this.menu = this.menu.dispose();
		doc.adopt(this.menu);
		
	},
	
	show: function(event)
	{
		var coords = ContextMenu.root.getCoordinates();		
		var bc = document.id(document.body).getCoordinates();
		
		var left = (event && this.options.position == 'pointer') ? event.page.x : coords.left;
		var top =  (event && this.options.position == 'pointer') ? event.page.y : coords.bottom - bc.top;
		
		this.menu.setStyles(
				{'top': top + this.options.offsetY,
				 'left': left + this.options.offsetX,
				 'display': 'block',
				 'opacity': 0,
				 'z-index': 500});
		this.menu.fade('in');
	},
	
	hide: function()
	{
		if (this.menu) this.menu.setStyles({'display': 'none', 'opacity': 0});
	}
});

// Static global for root element of displayed menu
ContextMenu.root = null;