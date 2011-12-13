// Requires mootools Class, Array, Function, Element, Element.Selectors,
// Element.Event, and you should probably get Window.DomReady if you're smart.
//
// Note 10/2011 - this is a work in progress
 
var GroupingList = new Class({
 
	Implements: Options,
  
	options: 
	{
		mode: 		'accordian',
		groupClass: 'subheading'
	},
  
	div: null,
	subheadings: null,
	content_divs: null,
	openFirst: false,
	
	initialize: function(div, options) 
	{
		this.div = $('grouped_list');
		this.setOptions(options);

		this.subheadings = this.div.getElements("h2");
		this.content_divs = this.div.getChildren('div');
	
		var self = this;
	
		this.subheadings.each(function(h) 
		{ 
			h.addEvent('click', function(e) 
			{ 
				new Event(e).stop(); 
				self.handleClick(h); 
			}); 
		});
	
		this.subheadings.each(function(r) 
		{ 
			r.addClass('collapsed');
		});

		
		this.content_divs.each(function(cd)
		{
			cd.addEvent('click', function(e) 
			{ 
				new Event(e).stop(); 
				self.handleClick(cd); 
			}); 
		});
		
		
		// Default Set first to open
		if(this.openFirst)
		{
			var subheadings = this.content_divs[0].getChildren('h2');
			subheadings[0].removeClass('collapsed'); 
			subheadings[0].addClass('expanded');
		}
		this.update();
	},
	

	update: function()
	{
		var subheadings;
		var subheading;
		var lists;
			
		var expanded = true;
		
		this.content_divs.each(function(div)
		{
			subheadings = div.getChildren('h2');
			subheading = subheadings[0]; // there will only be one
			lists = div.getChildren('ul');
		
			if(subheading.hasClass('expanded'))
			{
				lists.each(function(list)
				{
					list.setStyle('display', '');
				});
			}
			else
			{
				lists.each(function(list)
				{
					list.setStyle('display', 'none');
				});
			}
		});
	
	},
	
	handleClick: function(item)
	{
		this.subheadings.each(function(r) 
		{ 
			if(r.get('id') != item.get('id'))
			{
				r.removeClass('expanded'); 
				r.addClass('collapsed');
			}
		});
		
		if(item.hasClass('collapsed'))
		{
		item.removeClass('collapsed');
			item.addClass('expanded');
		}
		else
		{
			item.removeClass('expanded');
			item.addClass('collapsed');
		}
	
		this.update();
	},
	
	morphItems: function()
	{
		$$("#grouped_list a").each(function(elt) 
		{		
			elt.addEvents(
			{
				'mouseover': function()
				{
					this.morph({'background-color': '#990000', 'border-color': '#666666'});
				}.bind(elt),
				'mouseout': function()
				{
					this.morph({'background-color': '#EEEEEE', 'border-color': '#cccccc'});
				}.bind(elt)
			});
		});
	}	
});
