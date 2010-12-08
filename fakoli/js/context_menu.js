var ContextMenu = new Class(
{
	Implements: [Options],
	
	elements: [],
	menu: 	null,
	root:	null,
	trigger:	'contextmenu',
	options:
	{
		position: 'pointer',
		offsetX: 0,
		offsetY: 0
	},
	
	initialize: function(menu, elementSelector, trigger, options)
	{
		this.menu = $(menu);
		this.trigger = trigger;
		this.setOptions(options);
		
		var me = this;
		
		$$(elementSelector).each(function(elt)
		{
			elt.addEvent(trigger, function(e)
			{
				me.root = elt;
				var event = new Event(e).stop();
				me.show(event);
			});
		});
		
		$(document.body).addEvent('click', function(e) { me.hide(); });
		
	},
	
	show: function(event)
	{
		var coords = this.root.getCoordinates();		
		var bc = $(document.body).getCoordinates();
		
		var left = (event && this.options.position == 'pointer') ? event.page.x : coords.left;
		
		this.menu.setStyles(
				{'top': coords.bottom - bc.top + this.options.offsetY,
				 'left': left + this.options.offsetX,
				 'display': 'block',
				 'opacity': 0,
				 'z-index': 500});
		this.menu.fade('in');
	},
	
	hide: function()
	{
		this.menu.setStyles({'display': 'none', 'opacity': 0});
	}
});