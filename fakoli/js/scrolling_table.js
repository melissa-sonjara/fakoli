var ScrollingTable = new Class(
{
	table: null,
	
	initialize: function(table)
	{
		table = document.id(table);
		if (!table) return;
		
		this.table = table;
		window.addEvent('resize', function(e) { this.resize(); }.bind(this));
		window.addEvent('load', function(e) { this.resize(); }.bind(this));
		
		this.resize();
	},
	
	resize: function()
	{
		thead = this.table.getElement('thead');
		tbody = this.table.getElement('tbody');
		
		ths = thead.getElements('th');
		var widths = [];
		
		ths.each(function(th)
		{
			widths.push(th.getWidth());
		});
		
		// Adjust for scrollbar
		widths[0] -= (Browser.platform != "mac") ? 17 : 0;
		widths[widths.length-1] += (Browser.platform != "mac") ? 17 : 0;
		
		thead.setStyle('display', 'block');
		thead.getElement('tr').setStyles({'display': 'block', 'width': '100%'});
		ths.each(function(th, idx)
		{
			th.setStyle('width', widths[idx] + 'px');
		});
		
		trs = tbody.getElements('tr');
		trs.each(function(tr)
		{
			var tds = tr.getElements('td');
			var idx = 0;
			tds.each(function(td)
			{
				var w = 0;
				var colspan = td.colSpan;
				while(colspan--)
				{
					w += widths[idx++];
				}
				td.setStyle('width', w+'px');
			});
		});
	}
});