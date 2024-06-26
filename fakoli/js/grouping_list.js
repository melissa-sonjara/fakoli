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
		this.div = document.id(div);
		this.setOptions(options);

		if (this.options.mode != "fixed")
		{
			this.subheadings = this.div.getElements("h2");
			this.content_divs = this.div.getChildren('div');
		
			var self = this;
		
			this.subheadings.each(function(h) 
			{ 
				h.addEvent('click', function(e) 
				{ 
					new DOMEvent(e).stop(); 
					self.handleClick(h); 
				}); 
			});
		
			this.subheadings.each(function(r) 
			{ 
				r.addClass('collapsed');
			});
			
			// Default Set first to open
			if(this.openFirst)
			{
				var subheadings = this.content_divs[0].getChildren('h2');
				subheadings[0].removeClass('collapsed'); 
				subheadings[0].addClass('expanded');
			}
		}
		

		if (this.div.facetManager)
		{
			this.div.facetManager.addEvent('filterChanged', function() { this.filterChanged()}.bind(this));
			this.div.facetManager.addEvent('filterCleared', function() { this.filterCleared()}.bind(this));
			this.preprocessFacets();
		}
		
		this.update();
	},
	

	update: function()
	{
		if (!this.content_divs) return;
		
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
	},

	preprocessFacets: function()
	{
		this.div.getElements('li').each(function(elt)
		{
			this.div.facetManager.preprocess(elt);
		}.bind(this));
		
		this.div.facetManager.preprocessComplete();
	},
	
	filterChanged: function()
	{
		this.div.getElements('li').each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			
			var match = this.div.facetManager.filter(elt);
			
			if (match)
			{
				elt.addClass('filtermatch');
				elt.show();
			}
			else
			{
				elt.addClass('filtered');
				elt.hide();
			}
		}.bind(this));
		
 	    this.updatePageCount();
	},
	
	filterCleared: function()
	{
		this.div.getElements('li').each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			elt.show();
		});
		
  	    this.updatePageCount();
	}
	
});
