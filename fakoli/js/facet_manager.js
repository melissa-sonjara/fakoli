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
		
		if (this.container) this.container.facetManager = this; // public back reference
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
	
	preprocessComplete: function()
	{
		this.handlers.each(function(handler)
		{
			handler.preprocessComplete();
		});
	},
	
	filter: function(element)
	{	
		clear = true;
		var found = true;
		
		for(var i = 0; i < this.handlers.length; ++i)
		{
			if (!this.handlers[i].isClear())
			{
				clear = false;
				found &= this.handlers[i].filter(element);
			}
		}
		
		return clear || found;
	}
	
});


var MultiSelectFacetHandler = new Class(
{
	id: "",
	select: Class.Empty,
	checkboxes: Class.Empty,
	manager: Class.Empty,
	termLookup: {},
	
	initialize: function(id, select, manager)
	{
		this.id = id;
		this.select = select;
		this.checkboxes = this.select.getCheckboxes();
		this.manager = manager;
		this.manager.registerHandler(this);
		
		this.select.addEvent('selectionChanged', function()
		{
			this.manager.filterChanged();
		}.bind(this));
	},
	
	preprocess: function(item)
	{
		var term = item.get("data-" + this.id);

		if (term)
		{
			this.termLookup[term] = true;
		}
	},
	
	preprocessComplete: function()
	{
		var terms = Object.keys(this.termLookup).sort();
		
		terms.each(function(term)
		{
			this.select.addCheckbox(term, term);
		}.bind(this));

		this.checkboxes = this.select.getCheckboxes();
	},
	
	filter: function(item)
	{
		var term = item.get("data-" + this.id);
		
		for(var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked && term == this.checkboxes[i].value)
			{
				return true;
			}
		}
	},
	
	isClear: function()
	{
		for(var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked) return false;
		}
		
		return true;
	}		
});
