/*window.addEvent('domready', function()
{
	$$("#global > ul > li").each(function (elt)
	{
		elt.addEvents({'mouseover': function() { elt.addClass("sfhover"); },
					   'mouseout': function() { elt.removeClass("sfhover"); } 
					 });
	});
});*/

var FakoliMenu = new Class({
	
	root:	Class.Empty,
	
	initialize: function(elt)
	{
		this.root = $(elt);
		var menu = this;
		$$("#" + this.root.id + " > ul > li > a").each(function(elt) 
		{
			elt.addEvent('focus', function(e) 
			{
				var event = new Event(e).stop();
				this.updateFocus(event, elt);
			}.bind(menu));
		});
	},
	
	updateFocus: function(event, elt)
	{
		this.clearFocus();
		
		var parent = elt.getParent();
		var target = event.event.target;
		var found = false;
		do
		{
			if (target == parent)
			{
				found = true;
				break;
			}
			target = target.getParent();
		}
		while(target);
		
		if (found)
		{
			parent.addClass('sfhover');
		}
	},
	
	clearFocus: function()
	{
		this.root.getElements("ul > li").each(function(elt) { elt.removeClass('sfhover'); });
	}
});
