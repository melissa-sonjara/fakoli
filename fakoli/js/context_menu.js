var ContextMenu = new Class(
{
	elements: [],
	menu: 	null,
	root:	null,
	trigger:	'contextmenu',
	
	initialize: function(menu, elementSelector, trigger)
	{
		this.menu = $(menu);
		this.trigger = trigger;
		var me = this;
		
		$$(elementSelector).each(function(elt)
		{
			elt.addEvent(trigger, function(e)
			{
				me.root = elt;
				event = new Event(e).stop();
				me.show();
			});
		});
		
		$(document.body).addEvent('click', function(e) { me.hide(); });
		
	},
	
	show: function()
	{
		var coords = this.root.getCoordinates();
		var bc = $(document.body).getCoordinates();
		this.menu.setStyles(
				{'top': coords.bottom - bc.top,
				 'left': coords.left,
				 'display': 'block',
				 'opacity': 0});
		this.menu.fade('in');
	},
	
	hide: function()
	{
		this.menu.setStyles({'display': 'none', 'opacity': 0});
	}
});