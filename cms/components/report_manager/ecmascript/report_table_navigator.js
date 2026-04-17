class ReportTableNavigator
{
	constructor(container)
	{
		this.container = document.getElementById(container);
		this.tables = [];
		this.filters = [];

		this.tables = Array.from(this.container.querySelectorAll(".report_table"));

		this.tables.forEach(table =>
		{
			var columnContainer = table.querySelector(".columns");
			var heading = table.querySelector('h3');
			var columns = Array.from(columnContainer.querySelectorAll(".column"));
			var selectAll = table.querySelector(".select_all");
			var showAll = table.querySelector(".show_all");
			var helpIcon = table.querySelector("a.report_table_help_icon");

			if (!table.classList.contains('selected_table'))
			{
				columnContainer.style['display'] = 'none';
			}

			heading.addEventListener('click', e => { this.toggleTable(table); });

			columns.forEach(column =>
			{
				column.table = table;
				column.cbox = column.querySelector("input[type=checkbox");
				column.addEventListener('click', e => { this.toggleColumn(column, e); });
				if (!column.classList.contains("selected_column") && !column.classList.contains("favorite"))
				{
					column.style['display'] = 'none';
				}
			});

			selectAll.addEventListener('click', e => { this.toggleSelectAll(table, selectAll); });
			showAll.addEventListener('click', e => { this.toggleShowAll(table, showAll); });

			table.columns = columns;
			table.selectedCheckbox = table.querySelector("input[type=checkbox]");
			table.columnContainer = columnContainer;

			if (helpIcon)
			{
				helpIcon.addEventListener('mouseover', e => { this.showHelp(helpIcon); });
				helpIcon.addEventListener('mouseout', e => { this.hideHelp(helpIcon); });
			}
		});

		this.filters = Array.from(this.container.querySelectorAll(".report_filter"));

		this.filters.forEach(filter =>
		{
			var table = document.getElementById("table_" + filter.id);
			table.filter = filter;
			filter.filterForm = filter.querySelector(".filter");
			var heading = filter.querySelector('h3');

			heading.addEventListener('click', e => { e.stopPropagation(); e.preventDefault(); this.toggleFilter(filter); });
		});
	}

	toggleTable(table)
	{
		var columns = table.columnContainer;
		if (window.getComputedStyle(columns).getPropertyValue('display') == 'none')
		{
			columns.style.display = 'block';
			if (table.selectedCheckbox.checked)
			{
				table.filter.filterForm.style.display = 'block';
			}
		}
		else
		{
			columns.style.display = 'none';
			table.filter.filterForm.style.display = 'none';
		}
	}

	toggleColumn(column, event)
	{
		var input = column.cbox;

		if (event.target.id != input.id)
		{
			if (input.checked)
			{
				input.checked = false;
				column.classList.remove('selected_column');
			}
			else
			{
				input.checked = true;
				column.classList.add('selected_column');
			}
		}
		else
		{
			if (input.checked)
			{
				column.classList.add('selected_column');
			}
			else
			{
				column.classList.remove('selected_column');
			}
		}

		this.setTableStatus(column.table);
	}


	setTableStatus(table)
	{
		var checked = table.columnContainer.querySelectorAll("input[type=checkbox]:checked");
		var columns = table.querySelector(".columns");

		if (checked.length > 0)
		{
			table.classList.add('selected_table');
			table.selectedCheckbox.checked = true;

			this.filters.forEach(f =>
			{
				if (f != table.filter && window.getComputedStyle(f).getPropertyValue("display") == "block" && !f.classList.contains("collapsed"))
				{
					f.filterForm.style.display = 'none';
					f.classList.add("collapsed");
				}
			});

			if (window.getComputedStyle(table.filter).getPropertyValue('display') != 'block')
			{
				table.filter.style.display = 'block';
				table.filter.style.opacity = 0;
				table.filter.style.transition = 'opacity 0.3s';
				requestAnimationFrame(() => { table.filter.style.opacity = 1; });
			}
		}
		else
		{
			table.classList.remove('selected_table');
			table.selectedCheckbox.checked = false;
			table.filter.style.transition = 'opacity 0.3s';
			table.filter.style.opacity = 0;
			table.filter.addEventListener('transitionend', function handler()
			{
				table.filter.style.display = 'none';
				table.filter.removeEventListener('transitionend', handler);
			});
		}
	}

	toggleSelectAll(table, link)
	{
		var text = link.textContent;

		var columns = table.columns;
		if (text == "Select All")
		{
			columns.forEach(column =>
			{
				if (window.getComputedStyle(column).getPropertyValue('display') != 'none')
				{
					column.cbox.checked = true;
					column.classList.add('selected_column');
				}
			});

			link.textContent = "Deselect All";
		}
		else
		{
			columns.forEach(column =>
			{
				if (window.getComputedStyle(column).getPropertyValue('display') != 'none')
				{
					column.cbox.checked = false;
					column.classList.remove('selected_column');
				}
			});

			link.textContent = 'Select All';
		}
		this.setTableStatus(table);
	}

	toggleShowAll(table, link)
	{
		var text = link.textContent;

		var columns = table.columns;
		if (text == "Show All Fields")
		{
			columns.forEach(column =>
			{
				column.style['display'] = 'block';
			});

			link.textContent = 'Show Commonly Used Fields';
		}
		else
		{
			columns.forEach(column =>
			{
				if (!column.classList.contains('favorite') && !column.cbox.checked)
				{
					column.style['display'] = 'none';
				}
			});

			link.textContent = 'Show All Fields';
		}
	}

	toggleFilter(filter)
	{
		var form = filter.filterForm;
		if (window.getComputedStyle(form).getPropertyValue('display') == 'none')
		{
			form.style.display = 'block';
			filter.classList.remove("collapsed");
		}
		else
		{
			form.style.display = 'none';
			filter.classList.add("collapsed");
		}
	}

	showHelp(icon)
	{
		var help = icon.nextElementSibling;
		// Position relative to icon (bottom-left aligned to top-left of icon)
		var rect = icon.getBoundingClientRect();
		help.style.position = 'absolute';
		help.style.left = (rect.left + window.scrollX) + 'px';
		help.style.top = (rect.bottom + window.scrollY) + 'px';
		help.style.opacity = 0;
		help.style.display = 'block';
		help.style.transition = 'opacity 0.3s';
		requestAnimationFrame(() => { help.style.opacity = 1; });
	}

	hideHelp(icon)
	{
		var help = icon.nextElementSibling;
		help.style.transition = 'opacity 0.3s';
		help.style.opacity = 0;
	}
}
