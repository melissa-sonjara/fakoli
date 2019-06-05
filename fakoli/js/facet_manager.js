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
	},

	getSelectedValues: function()
	{
		var values = {};
		this.handlers.each(function(handler)
		{
			values[handler.getName()] = handler.getSelectedValue();
		});
		
		return values;
	},
	
	getQueryString: function()
	{
		var values = this.getSelectedValues();
		var uri = new URI();
		uri.setData(values);
		return uri.get('query');
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
	
	getName: function()
	{
		return this.id;
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
		var terms = Object.keys(this.termLookup);
		terms.sort();
		
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
	},
	
	getSelectedValue: function()
	{
		var values = "";
		
		for(var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked)
			{
				if (values != "")
				{
					values += ",";
				}
				values += this.checkboxes[i].value;
			}
		}
		
		return values;
	}
});

var StringFacetHandler = new Class(
{
	id: "",
	textField: Class.Empty,
	manager: Class.Empty,
	currentValue: "",
	
	initialize: function(id, textField, manager)
	{
		this.id = id;
		this.textField = document.id(textField);
		this.manager = manager;
		this.manager.registerHandler(this);
		
		this.textField.addEvent('input', function(e) 
		{
			var value = this.textField.value.toLowerCase();
			if (value == this.currentValue) return;
			this.currentValue = value;
			this.manager.filterChanged();
		}.bind(this));
	},
	
	getName: function()
	{
		return this.id;
	},
	
	preprocess: function(item)
	{

	},
	
	preprocessComplete: function()
	{

	},
	
	filter: function(item)
	{
		var term = item.get("data-" + this.id);
		
		return (term.indexOf(this.currentValue) != -1);
	},
	
	isClear: function()
	{
		return this.textField.value.length < 3;
	},
	
	getSelectedValue: function()
	{
		return this.textField.value;
	}

});

var SelectFacetHandler = new Class(
{
	id: "",
	select: Class.Empty,
	manager: Class.Empty,
	currentValue: "",
	
	initialize: function(id, textField, manager)
	{
		this.id = id;
		this.select = document.id(textField);
		this.manager = manager;
		this.manager.registerHandler(this);
		
		this.select.addEvent('change', function(e) 
		{
			var value = this.select.value;
			if (value == this.currentValue) return;
			this.currentValue = value;
			this.manager.filterChanged();
		}.bind(this));
	},
	
	getName: function()
	{
		return this.id;
	},
	
	preprocess: function(item)
	{

	},
	
	preprocessComplete: function()
	{

	},
	
	filter: function(item)
	{
		var term = item.get("data-" + this.id);
		
		return (term.indexOf(this.currentValue) != -1);
	},
	
	isClear: function()
	{
		return this.select.selectedIndex == -1;
	},
	
	getSelectedValue: function()
	{
		return this.select.value;
	}			
});

var CrossReferenceFacetHandler = new Class(
{	
	taxonomy: "",
	select: Class.Empty,
	checkboxes: Class.Empty,
	manager: Class.Empty,
	termLookup: {},
	
	initialize: function(name, select, manager)
	{
		this.name = name;
		this.select = select;
		this.checkboxes = this.select.getCheckboxes();
		this.manager = manager;
		this.manager.registerHandler(this);
		
		this.select.addEvent('selectionChanged', function()
		{
			this.manager.filterChanged();
		}.bind(this));
	},
	
	getName: function()
	{
		return this.taxonomy;
	},
	
	preprocess: function(item)
	{
		var terms = item.get("data-" + this.name);
		var id = item.get("id");

		if (terms)
		{
			terms = terms.split(",");
			terms.each(function(term) { this.termLookup[id + ":" + term] = true; }.bind(this));
		}
	},
	
	preprocessComplete: function()
	{
	},
	
	filter: function(item)
	{
		var id = item.get('id');
		
		for(var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked && this.termLookup[id + ":" + this.checkboxes[i].value])
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
	},
	
	getSelectedValue: function()
	{
		var values = "";
		
		for(var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked)
			{
				if (values != "")
				{
					values += ",";
				}
				values += this.checkboxes[i].value;
			}
		}
		
		return values;
	}
});
