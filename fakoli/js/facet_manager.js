var FacetManager = new Class(
{
	Implements: [Events, Options],
	container:	Class.Empty,
	elements: Class.Empty,
	handlers: [],
	
	options:
	{
		onFilterChanged: function() {},
		onFilterCleared: function() {}
	},
	
	initialize: function(container, options)
	{
		this.container = document.id(container);
		this.setOptions(options);
		
		this.container.facetManager = this; // public back reference
	},
	
	filterChanged: function()
	{
		var clear = true;
		
		for(var i = 0; i < this.handlers.length; ++i)
		{
			clear = this.handlers[i].isClear();
			if (!clear) break;
		}
		
		if (clear)
		{
			this.fireEvent('filterCleared');
		}
		else
		{
			this.fireEvent('filterChanged');
		}
	},
	
	registerHandler: function(handler)
	{
		this.handlers.push(handler);
	},
	
	preprocess: function(element)
	{	
		this.handlers.each(function(handler)
		{
			var id = element.get('id');
			if (!id)
			{
				id = String.uniqueID();
				element.set('id', id);
			}
			
			handler.preprocess(element);
		});
	},
	
	filter: function(element)
	{
		for(var i = 0; i < this.handlers.length; ++i)
		{
			var found = this.handlers[i].filter(element);
			if (found) return true;
		}
		
		return false;
	}
	
});