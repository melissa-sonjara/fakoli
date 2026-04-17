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

/**
 * Adds a column-select and text-filter control above a table, hiding rows
 * that do not match the entered filter string.
 */
class FilteringTable
{
	constructor(table, filterElement, options)
	{
		this.table         = $el(table);
		this.filterElement = $el(filterElement);
		this.options = Object.assign({
			column:    0,
			startsWith: false,
			paginator:  false,
			cull:       true
		}, options || {});

		this.thead         = this.table.querySelector('thead');
		this.tbody         = this.table.querySelector('tbody');
		this.filterSelect  = null;
		this.filterControl = null;

		this.createFilter();
	}

	clearFilter()
	{
		Array.from(this.tbody.children).forEach(function(row)
		{
			row.classList.remove('filtered');
			row.classList.remove('filtermatch');
		});
	}

	filter()
	{
		this.clearFilter();

		if (this.filterControl.value == "")
		{
			if (this.options.paginator) this.options.paginator.update_pages();
			return;
		}

		var val = this.filterControl.value.toLowerCase();
		var self = this;

		Array.from(this.tbody.children).forEach(function(row)
		{
			var tds  = Array.from(row.children);
			var colIdx = parseInt(self.filterSelect.value, 10);
			var text = tds[colIdx] ? (tds[colIdx].textContent || tds[colIdx].innerText) : '';

			if (!text)
			{
				row.classList.add('filtered');
				return;
			}

			var idx = text.toLowerCase().indexOf(val);
			if ((self.options.startsWith && idx == 0) || (!self.options.startsWith && idx >= 0))
			{
				row.classList.add('filtermatch');
			}
			else
			{
				row.classList.add('filtered');
			}
		});

		if (this.options.paginator)
		{
			this.options.paginator.update_pages();
		}
	}

	createFilter()
	{
		var headers = Array.from(this.thead.querySelectorAll('th'));

		var label = document.createElement('label');
		label.innerHTML = "Filter by&nbsp;";
		this.filterElement.appendChild(label);

		this.filterSelect = document.createElement('select');
		this.filterSelect.id = this.table.id + "_filterselect";

		headers.forEach(function(h, i)
		{
			var text = h.innerText || h.textContent;
			if (/^(?:\s|&nbsp;)*$/.test(text)) return;
			var opt = document.createElement('option');
			opt.value = i;
			opt.innerHTML = text;
			this.filterSelect.appendChild(opt);
		}.bind(this));

		this.filterElement.appendChild(this.filterSelect);

		this.filterControl = document.createElement('input');
		this.filterControl.type = 'text';
		this.filterControl.addEventListener('keyup', function(e)
		{
			e.preventDefault();
			e.stopPropagation();
			this.filter();
		}.bind(this));

		this.filterElement.appendChild(this.filterControl);
	}
}
