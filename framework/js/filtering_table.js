// Requires mootools Class, Array, Function, Element, Element.Selectors,
// Element.Event, and you should probably get Window.DomReady if you're smart.
//
 
var FilteringTable = new Class({
 
	Implements: Options,
  
	options: 
	{
		column: 0,
		startsWith: false,
		paginator: false,
		cull: true
	},
  
	table: null,
	thead: null,
	tbody: null,
	filterElement: null,
	filterSelect: null,
	filterLabel: null,
	filterControl: null,
	
	initialize: function(table, filterElement, options ) 
	{
		this.table = $(table);
		this.filterElement = $(filterElement);
		this.setOptions(options);
    
		this.thead = this.table.getElement('thead');
		this.tbody = this.table.getElement('tbody');
		this.createFilter();
	},
	
	clearFilter: function()
	{
		var trs = this.tbody.getChildren();
		
		trs.each(function(row) { row.removeClass('filtered'); row.removeClass('filtermatch'); });
	},
	
	filter: function()
	{
		this.clearFilter();
		
		if (this.filterControl.value == "") 
		{
			if (this.options.paginator) this.options.paginator.update_pages();
			return;
		}
		
		var trs = this.tbody.getChildren();
		var val = this.filterControl.value.toLowerCase();
		
		trs.each(function(row)
		{
			var tds = row.getChildren();
			var text = tds[this.filterSelect.value].textContent;
			var idx = text.toLowerCase().indexOf(val);
			if ((this.startsWith && idx == 0) || (!this.startsWith && idx >= 0))
			{
				row.addClass('filtermatch');
			}
			else
			{
				row.addClass('filtered');
			}
		}.bind(this));
		
		if (this.options.paginator) this.options.paginator.update_pages();
	},
	
	createFilter: function()
	{
		var headers = this.thead.getElements('th');
		var text = headers[this.options.column].textContent;
		this.filterLabel = new Element( 'label' ).set( 'html', "Filter by&nbsp;");
		this.filterLabel.injectInside(this.filterElement);
		
		this.filterSelect = new Element('select', {id: this.table.id + "_filterselect"});
		headers.each(function(h, i) { this.filterSelect.options[i] = new Element('option', {'value': i, 'html': h.textContent}); }.bind(this));
		
		this.filterSelect.injectInside(this.filterElement);
		
		this.filterControl = new Element( 'input', 
								{
									'type': 'text',
									'events':	
									{
										'keyup': function(e) { new Event(e).stop(); this.filter(); }.bind(this)
									}
								});
		
		this.filterControl.injectInside(this.filterElement);
	}
});