// Requires mootools Class, Array, Function, Element, Element.Selectors,
// Element.Event, and you should probably get Window.DomReady if you're smart.
//
// Note 10/2011 - this is a work in progress
 
var GroupingList = new Class({
 
	Implements: Options,
  
	options: 
	{
		mode: 		'tree',
		groupClass: 'subheading'
	},
  
	div: null,
	subheadings: null,
	
	initialize: function(div, options) 
	{
		this.div = $(div);
		this.setOptions(options);

		this.subheadings = this.div.getElements("ul." + this.options.groupClass);
		
		var self = this;
		
		this.subheadings.each(function(h) 
		{ 
			h.addEvent('click', function(e) 
			{ 
				new Event(e).stop(); 
				self.handleClick(h); 
			}); 
		});
		this.update();
	},
	
	update: function()
	{
		var items = this.div.getChildren();
		
		var expanded = true;
		for(r = 0; r < items.length; ++r)
		{
			if (items[r].hasClass('subheading')) 
			{
				expanded = items[r].hasClass('expanded');
			}
			else
			{
				items[r].setStyle('display', expanded ? '' : 'none');
			}
		}
	},
	
	handleClick: function(item)
	{
		if (this.options.mode == 'accordion')
		{
			this.subheadings.each(function(item) 
			{ 
				item.removeClass('expanded'); 
				item.addClass('collapsed'); 
			});
			item.removeClass('collapsed');
			item.addClass('expanded');
		}
		else if (this.options.mode == 'tree')
		{
			if (item.hasClass('expanded'))
			{
				item.removeClass('expanded');
				item.addClass('collapsed');
			}
			else
			{
				item.removeClass('collapsed');
				item.addClass('expanded');
			}
		}
		
		this.update();
	}
});
