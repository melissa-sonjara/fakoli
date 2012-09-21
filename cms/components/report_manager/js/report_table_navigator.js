var ReportTableNavigator = new Class(
{
	container: Class.Empty,
	tables: [],
	filters: [],
	
	initialize: function(container)
	{
		this.container = document.id(container);
		
		this.tables = this.container.getElements(".report_table");
		
		this.tables.each(function(table)
		{
			var columnContainer = table.getElement(".columns");
			var heading = table.getElement('h3');
			var columns = columnContainer.getElements(".column");
			var selectAll = table.getElement(".select_all");
			var showAll = table.getElement(".show_all");
			var helpIcon = table.getElement("a.report_table_help_icon");
			
			if (!table.hasClass('selected_table'))
			{
				columnContainer.setStyle('display', 'none');
			}
			
			heading.addEvent('click', function(e) { this.toggleTable(table); }.bind(this));
			
			columns.each(function(column)
			{
				column.table = table;
				column.cbox = column.getElement("input[type=checkbox");
				column.addEvent('click', function(e) { this.toggleColumn(column, e); }.bind(this));
				if (!column.hasClass("selected_column") && !column.hasClass("favorite"))
				{
					column.setStyle('display', 'none');
				}
			}.bind(this));
				
			selectAll.addEvent('click', function(e) { this.toggleSelectAll(table, selectAll); }.bind(this));
			showAll.addEvent('click', function(e) { this.toggleShowAll(table, showAll); }.bind(this));
			
			table.columns = columns;
			table.selectedCheckbox = table.getElement("input[type=checkbox]");
			table.columnContainer = columnContainer;
			
			if (helpIcon)
			{
				helpIcon.addEvent('mouseover', function(e) { this.showHelp(helpIcon); }.bind(this));
				helpIcon.addEvent('mouseout', function(e) { this.hideHelp(helpIcon); }.bind(this));
			}
			
		}.bind(this));
		
		this.filters = this.container.getElements(".report_filter");
		
		this.filters.each(function(filter)
		{
			var table = document.id("table_" + filter.id);
			table.filter = filter;
			filter.filterForm = filter.getElement(".filter");
			var heading = filter.getElement('h3');
			
			heading.addEvent('click', function(e) { new Event(e).stop(); this.toggleFilter(filter); }.bind(this));			
		}.bind(this));
	},
	
	toggleTable: function(table)
	{
		var columns = table.columnContainer;
		if (columns.getStyle('display') == 'none')
		{
			columns.reveal();
			if (table.selectedCheckbox.checked)
			{
				table.filter.filterForm.reveal();
			}
		}
		else
		{
			columns.dissolve();
			table.filter.filterForm.dissolve();
		}
	},
	
	toggleColumn: function(column, event)
	{
		var input = column.cbox;
	
		if (event.target.id != input.id)
		{			
			if (input.checked)
			{
				input.checked = false;
				column.removeClass('selected_column');
			}
			else
			{
				input.checked = true;
				column.addClass('selected_column');
			}
		}
		else
		{
			if (input.checked)
			{
				column.addClass('selected_column');
			}
			else
			{
				column.removeClass('selected_column');
			}
		}
		
		this.setTableStatus(column.table);
	},
	
	
	setTableStatus: function(table)
	{
		var checked = table.columnContainer.getElements("input[type=checkbox]:checked");
		var columns = table.getElement(".columns");
		
		if (checked.length > 0)
		{
			table.tween('background-color', '#002a6c');
			columns.morph({'background-color': '#d7dee7', 'color': '#000000'});
			table.selectedCheckbox.checked = true;
			
			this.filters.each(function (f)
			{
				if (f != table.filter && f.getStyle("display") == "block" && !f.hasClass("collapsed"))
				{
					f.filterForm.dissolve();
					f.addClass("collapsed");
				}
			});
			
			if (table.filter.getStyle('display') != 'block')
			{
				table.filter.setStyles({'display': 'block', 'opacity': 0}).fade('in');
			}
		}
		else
		{
			table.tween('background-color', '#AEBFDA');
			columns.morph({'background-color': '#ffffff', 'color': '#666666'});
			table.selectedCheckbox.checked = false;
			fade = new Fx.Tween(table.filter, {property: 'opacity'});
			fade.start(0).chain(function() { table.filter.setStyle('display', 'none'); });
		}
	},
	
	toggleSelectAll: function(table, link)
	{
		var text = link.get('text');
		
		var columns = table.columns;
		if (text == "Select All")
		{
			columns.each(function(column)
			{
				if (column.getStyle('display') != 'none')
				{
					column.cbox.checked = true;
					column.addClass('selected_column');
				}
			});

			link.set('text', "Deselect All");
		}
		else
		{			
			columns.each(function(column)
			{
				if (column.getStyle('display') != 'none')
				{
					column.cbox.checked = false;
					column.removeClass('selected_column');
				}
			});
			
			link.set('text', 'Select All');
		}
		this.setTableStatus(table);	
	},
	
	toggleShowAll: function(table, link)
	{

		var text = link.get('text');
		
		var columns = table.columns;
		if (text == "Show All Fields")
		{
			columns.each(function(column)
			{
				column.setStyle('display', 'block');
			});
			
			link.set('text', 'Show Commonly Used Fields');
		}
		else
		{
			columns.each(function(column)
			{
				if (!column.hasClass('favorite') && !column.cbox.checked)
				{
					column.setStyle('display', 'none');
				}
			});
			
			link.set('text', 'Show All Fields');
		}
	},
	
	toggleFilter: function(filter)
	{
		var form = filter.filterForm;
		if (form.getStyle('display') == 'none')
		{
			form.reveal();
			filter.removeClass("collapsed");
		}
		else
		{
			form.dissolve();
			filter.addClass("collapsed");
		}
	},
	
	showHelp: function(icon)
	{
		var help = icon.getNext();
		help.position({relativeTo: icon, position: 'bottomLeft', edge: 'topLeft'});
		help.fade('in');
	},
	
	hideHelp: function(icon)
	{
		var help = icon.getNext();
		help.fade('out');
	}
});