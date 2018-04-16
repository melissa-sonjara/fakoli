var ScrollingTable = new Class(
{
	table: null,
	
	initialize: function(table)
	{
		table = document.id(table);
		if (!table) return;
		
		this.table = table;
		window.addEvent('resize', function(e) { this.resize(); }.bind(this));
		
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