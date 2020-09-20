// Requires mootools Class, Array, Function, Element, Element.Selectors,
// Element.Event, and you should probably get Window.DomReady if you're smart.
//
 
var GroupingTable = new Class({
 
	Implements: Options,
  
	options: 
	{
		mode: 		'tree',
		groupClass: 'subheading'
	},
  
	table: null,
	thead: null,
	tbody: null,
	subheadings: null,
	
	initialize: function(table, options ) 
	{
		this.table = document.id(table);
		this.setOptions(options);

		this.thead = this.table.getElement('thead');
		this.tbody = this.table.getElement('tbody');
		this.subheadings = this.tbody.getElements("tr." + this.options.groupClass);
		
		var self = this;
		
		this.subheadings.each(function(h) { h.addEvent('click', function(e) { new DOMEvent(e).stop(); self.handleClick(h); }); });
		
		if (this.table.facetManager)
		{
			this.table.facetManager.addEvent('filterChanged', function() { this.filterChanged(); }.bind(this));
			this.table.facetManager.addEvent('filterCleared', function() { this.filterCleared(); }.bind(this));
			this.preprocessFacets();
		}
		
		this.update();
	},
	
	preprocessFacets: function()
	{
		this.tbody.getChildren('tr').each(function(elt)
		{
			if (elt.hasClass('subheading')) return;
			this.table.facetManager.preprocess(elt);
		}.bind(this));
		
		this.table.facetManager.preprocessComplete();
	},
	
	filterChanged: function()
	{
		this.tbody.getChildren('tr').each(function(elt)
		{
			if (elt.hasClass('subheading')) return;

			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			
			var match = this.table.facetManager.filter(elt);
			
			if (match)
			{
				elt.addClass('filtermatch');
			}
			else
			{
				elt.addClass('filtered');
			}
		}.bind(this));
		
 	    this.update();
	},
	
	filterCleared: function()
	{
		this.tbody.getChildren('tr').each(function(elt)
		{
			if (elt.hasClass('subheading')) return;

			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
		});
		
  	    this.update();
	},
	
	update: function()
	{
		var rows = this.tbody.getChildren();
		
		var expanded = true;
		for(r = 0; r < rows.length; ++r)
		{
			if (rows[r].hasClass('subheading')) 
			{
				expanded = rows[r].hasClass('expanded');
			}
			else
			{
				rows[r].setStyle('display', expanded ? '' : 'none');
			}
		}
	},
	
	handleClick: function(row)
	{
		if (this.options.mode == 'accordion')
		{
			this.subheadings.each(function(r) { r.removeClass('expanded'); r.addClass('collapsed'); });
			row.removeClass('collapsed');
			row.addClass('expanded');
		}
		else if (this.options.mode == 'tree')
		{
			if (row.hasClass('expanded'))
			{
				row.removeClass('expanded');
				row.addClass('collapsed');
			}
			else
			{
				row.removeClass('collapsed');
				row.addClass('expanded');
			}
		}
		
		this.update();
	}
});
	
GroupingTable.toggleGroup = function(button, groupClass)
{
	tr = document.id(button).getParent().getParent();
	var state = (button.get('text') == "Select All");
	tr = tr.getNext();
	
	while(tr && !tr.hasClass(groupClass))
	{
		tr.getElement("input[type=checkbox]").checked = state;
		tr = tr.getNext();
	}
	
	button.set('text', state ? "Deselect All" : "Select All");
};
	
	
	