var ScrollingTable = new Class(
{
	table: null,
	
	initialize: function(table)
	{
		table = document.id('table');
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
		
		tds = tbody.getElement('tr').getElements('td');
		
		for(var i = 0; i < ths.length; ++i)
		{
			ths[i].set('width', tds[i].getWidth());
			ths[i].set('height', 23);
		}
		
	}
});