/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

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
			var text = tds[this.filterSelect.value].textContent || tds[this.filterSelect.value].innerText;
			if (!text)
			{
				row.addClass('filtered');
				return;
			}
			
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
		
		if (this.options.paginator) 
		{
			this.options.paginator.update_pages();
		}
	},
	
	createFilter: function()
	{
		var headers = this.thead.getElements('th');
		var text = headers[this.options.column].textContent;
		this.filterLabel = new Element( 'label' ).set( 'html', "Filter by&nbsp;");
		this.filterLabel.injectInside(this.filterElement);
		
		this.filterSelect = new Element('select', {id: this.table.id + "_filterselect"});
		var self = this;
		headers.each(function(h, i) 
		{ 
			var text = h.innerText || h.textContent;
			if (text.match(/^(?:\s|&nbsp;)*$/)) return;
			var elt = new Element('option', {'value': i, 'html': text});
			self.filterSelect.appendChild(elt); 
		}
		.bind(this));
		
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