// Requires ui.js for $el and EventEmitter helpers.

/**
 * Manages a table with collapsible group heading rows, supporting accordion
 * and tree expand/collapse modes and optional facet filtering.
 */
class GroupingTable
{
	constructor(table, options)
	{
		this.table = $el(table);
		this.options = Object.assign({
			mode:       'tree',
			groupClass: 'subheading'
		}, options || {});

		this.thead = this.table.querySelector('thead');
		this.tbody = this.table.querySelector('tbody');
		this.subheadings = Array.from(this.tbody.querySelectorAll("tr." + this.options.groupClass));

		var self = this;
		this.subheadings.forEach(function(h)
		{
			h.addEventListener('click', function(e)
			{
				e.preventDefault();
				e.stopPropagation();
				self.handleClick(h);
			});
		});

		if (this.table.facetManager)
		{
			var self = this;
			this.table.facetManager.addEvent('filterChanged', function() { self.filterChanged(); });
			this.table.facetManager.addEvent('filterCleared', function() { self.filterCleared(); });
			this.preprocessFacets();
		}

		this.update();
	}

	preprocessFacets()
	{
		var self = this;
		Array.from(this.tbody.children).forEach(function(elt)
		{
			if (elt.classList.contains('subheading')) return;
			self.table.facetManager.preprocess(elt);
		});
		this.table.facetManager.preprocessComplete();
	}

	filterChanged()
	{
		var self = this;
		Array.from(this.tbody.children).forEach(function(elt)
		{
			if (elt.classList.contains('subheading')) return;
			elt.classList.remove("filtered");
			elt.classList.remove("filtermatch");
			var match = self.table.facetManager.filter(elt);
			if (match)
			{
				elt.classList.add('filtermatch');
			}
			else
			{
				elt.classList.add('filtered');
			}
		});
		this.update();
	}

	filterCleared()
	{
		Array.from(this.tbody.children).forEach(function(elt)
		{
			if (elt.classList.contains('subheading')) return;
			elt.classList.remove("filtered");
			elt.classList.remove("filtermatch");
		});
		this.update();
	}

	update()
	{
		var rows = Array.from(this.tbody.children);
		var expanded = true;

		for (var r = 0; r < rows.length; ++r)
		{
			if (rows[r].classList.contains('subheading'))
			{
				expanded = rows[r].classList.contains('expanded') || this.options.mode == 'fixed';
			}
			else
			{
				rows[r].style.display = expanded ? '' : 'none';
			}
		}
	}

	handleClick(row)
	{
		if (this.options.mode == 'accordion')
		{
			this.subheadings.forEach(function(r)
			{
				r.classList.remove('expanded');
				r.classList.add('collapsed');
			});
			row.classList.remove('collapsed');
			row.classList.add('expanded');
		}
		else if (this.options.mode == 'tree')
		{
			if (row.classList.contains('expanded'))
			{
				row.classList.remove('expanded');
				row.classList.add('collapsed');
			}
			else
			{
				row.classList.remove('collapsed');
				row.classList.add('expanded');
			}
		}
		this.update();
	}
}

GroupingTable.toggleGroup = function(button, groupClass)
{
	var tr = button.parentElement.parentElement;
	var state = (button.textContent == "Select All");
	tr = tr.nextElementSibling;

	while (tr && !tr.classList.contains(groupClass))
	{
		var cb = tr.querySelector("input[type=checkbox]");
		if (cb) cb.checked = state;
		tr = tr.nextElementSibling;
	}

	button.textContent = state ? "Deselect All" : "Select All";
};
