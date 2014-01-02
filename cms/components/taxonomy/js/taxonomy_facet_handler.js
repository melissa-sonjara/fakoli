var TaxonomyFacetHandler = new Class(
{	
	taxonomy: "",
	select: Class.Empty,
	checkboxes: Class.Empty,
	manager: Class.Empty,
	
	termLookup: {},
	
	initialize: function(taxonomy, select, manager)
	{
		this.taxonomy = taxonomy;
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
		var terms = item.get("data-taxonomy-" + this.taxonomy);
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