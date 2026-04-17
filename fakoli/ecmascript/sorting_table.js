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
 * Adds client-side column sorting to a table, with automatic type detection
 * for dates, file sizes, currency, and numbers, plus optional zebra striping.
 */
class SortingTable extends EventEmitter
{
	constructor(table, options)
	{
		super();
		this.table = $el(table);
		this.options = Object.assign({
			zebra:              true,
			details:            false,
			paginator:          false,
			filter:             false,
			onSorted:           function() {},
			dont_sort_class:    'nosort',
			forward_sort_class: 'forward_sort',
			reverse_sort_class: 'reverse_sort'
		}, options || {});

		this.tbody       = this.table.querySelector('tbody');
		this.sort_column = -1;
		this.conversion_function  = false;
		this.conversion_matcher   = null;

		if (this.options.zebra)
		{
			SortingTable.stripe_table(Array.from(this.tbody.children));
		}

		this.headers = Array.from(this.table.querySelector('thead').querySelectorAll('th'));
		this.headers.forEach(function(header, index)
		{
			if (header.classList.contains(this.options.dont_sort_class)) return;
			header._sortColumn = index;
			header.addEventListener('click', function(evt)
			{
				if (evt.target != header) return;
				this.sortByHeader(evt.target);
				if (this.options.paginator) this.options.paginator.update_pages();
			}.bind(this));
		}.bind(this));

		this.load_conversions();
	}

	sortByHeader(header)
	{
		var rows = [];

		var before = this.tbody.previousElementSibling;
		this.tbody.remove();

		var trs = Array.from(this.tbody.children);
		var row;
		while ((row = trs.shift()))
		{
			row = { row: row.parentNode ? (row.parentNode.removeChild(row), row) : row };
			if (this.options.details)
			{
				var detailEl = trs.shift();
				if (detailEl && detailEl.parentNode) detailEl.parentNode.removeChild(detailEl);
				row.detail = detailEl;
			}
			rows.unshift(row);
		}

		if (this.sort_column >= 0 && this.sort_column == header._sortColumn)
		{
			if (header.classList.contains(this.options.reverse_sort_class))
			{
				header.classList.remove(this.options.reverse_sort_class);
				header.classList.add(this.options.forward_sort_class);
			}
			else
			{
				header.classList.remove(this.options.forward_sort_class);
				header.classList.add(this.options.reverse_sort_class);
			}
		}
		else
		{
			this.headers.forEach(function(h)
			{
				h.classList.remove(this.options.forward_sort_class);
				h.classList.remove(this.options.reverse_sort_class);
			}.bind(this));

			this.sort_column = header._sortColumn;

			if (header.getAttribute('data-sort'))
			{
				// Explicit sort order already provided via data attribute
				rows.forEach(function(row)
				{
					row.sort_column = this.sort_column;
					row.toString = function()
					{
						return row.row.querySelectorAll('td')[this.sort_column].getAttribute('data-sort');
					}.bind(this);
				}.bind(this));
				header.classList.add(this.options.forward_sort_class);
			}
			else
			{
				if (header._conversionFunction)
				{
					this.conversion_matcher  = header._conversionMatcher;
					this.conversion_function = header._conversionFunction;
				}
				else
				{
					this.conversion_function = false;
					var self = this;
					rows.some(function(row)
					{
						var td = row.row.getElementsByTagName('td')[self.sort_column];
						var to_match = td ? td.textContent : '';
						if (to_match == '') return false;
						self.conversions.some(function(conversion)
						{
							if (conversion.matcher.test(to_match))
							{
								self.conversion_matcher  = conversion.matcher;
								self.conversion_function = conversion.conversion_function;
								return true;
							}
							return false;
						});
						return !!(self.conversion_function);
					});

					header._conversionFunction = this.conversion_function;
					header._conversionMatcher  = this.conversion_matcher;
				}

				header.classList.add(this.options.forward_sort_class);
				rows.forEach(function(row)
				{
					var compare_value = this.conversion_function ? this.conversion_function.call(this, row) : '';
					row.toString = function() { return compare_value; };
				}.bind(this));
			}

			rows.sort();
		}

		var index = 0;
		while ((row = rows.shift()))
		{
			this.tbody.appendChild(row.row);
			if (row.detail) this.tbody.appendChild(row.detail);
			if (this.options.zebra && !row.row.classList.contains('filtered'))
			{
				row.row.classList.remove('alt');
				if (row.detail) row.detail.classList.remove('alt');
				if (index % 2)
				{
					row.row.classList.add('alt');
					if (row.detail) row.detail.classList.add('alt');
				}
				index++;
			}
		}

		if (before && before.parentNode)
		{
			before.parentNode.insertBefore(this.tbody, before.nextSibling);
		}
		else if (this.table)
		{
			this.table.appendChild(this.tbody);
		}

		this.fireEvent('sorted', this);
	}

	load_conversions()
	{
		var self = this;
		this.conversions = [
			// 1.75 MB, 301 GB, 34 KB, 8 TB
			{
				matcher: /([0-9.]{1,8}).*([KMGT]{1})B/,
				conversion_function: function(row)
				{
					var cell = row.row.getElementsByTagName('td')[self.sort_column].textContent;
					cell = self.conversion_matcher.exec(cell);
					if (!cell) return '0';
					var sort_val = (cell[2] == 'M') ? '1' : (cell[2] == 'G') ? '2' : (cell[2] == 'T') ? '3' : '0';
					var post;
					var i = cell[1].indexOf('.');
					if (i == -1)
					{
						post = '00';
					}
					else
					{
						var dec = cell[1].split('.');
						cell[1] = dec[0];
						post = dec[1].concat('00'.substr(0, 2 - dec[1].length));
					}
					return sort_val.concat('00000000'.substr(0, 2 - cell[1].length).concat(cell[1])).concat(post);
				}
			},
			// 1 day ago, 4 days ago, 38 years ago, 1 month ago
			{
				matcher: /(\d{1,2}) (.{3,6}) ago/,
				conversion_function: function(row)
				{
					var cell = row.row.getElementsByTagName('td')[self.sort_column].textContent;
					cell = self.conversion_matcher.exec(cell);
					if (!cell) return '0';
					var sort_val = (cell[2].indexOf('month') != -1) ? '1' : (cell[2].indexOf('year') != -1) ? '2' : '0';
					return sort_val.concat('00'.substr(0, 2 - cell[1].length).concat(cell[1]));
				}
			},
			// Currency or floating point
			{
				matcher: /^[^\d]?((\d+|,\d{3})*(\.\d{1,2})?)$/,
				conversion_function: function(row)
				{
					var cell = row.row.getElementsByTagName('td')[self.sort_column].textContent;
					cell = parseFloat(cell.replace(/^[^\d]/, "").replace(/,/g, ""));
					if (isNaN(cell)) cell = 0;
					cell = Math.round(cell * 100).toString();
					return '00000000000000000000000000000000'.substr(0, 32 - cell.length).concat(cell);
				}
			},
			// YYYY-MM-DD, YYYY/MM/DD
			{
				matcher: /(\d{4})[\-\/](\d{1,2})[\-\/](\d{1,2})/,
				conversion_function: function(row)
				{
					var cell = row.row.getElementsByTagName('td')[self.sort_column].textContent;
					var d = self.conversion_matcher.exec(cell);
					return d ? d[1] +
						'00'.substr(0, 2 - d[2].length).concat(d[2]) +
						'00'.substr(0, 2 - d[3].length).concat(d[3]) : cell;
				}
			},
			// MM/DD/YYYY, MM-DD-YYYY
			{
				matcher: /(\d{1,2})[\-\/](\d{1,2})[\-\/](\d{4})/,
				conversion_function: function(row)
				{
					var cell = row.row.getElementsByTagName('td')[self.sort_column].textContent;
					var d = self.conversion_matcher.exec(cell);
					return d ? d[3] +
						'00'.substr(0, 2 - d[1].length).concat(d[1]) +
						'00'.substr(0, 2 - d[2].length).concat(d[2]) : cell;
				}
			},
			// Numbers
			{
				matcher: /^\d+$/,
				conversion_function: function(row)
				{
					var cell = row.row.getElementsByTagName('td')[self.sort_column].textContent;
					return '00000000000000000000000000000000'.substr(0, 32 - cell.length).concat(cell);
				}
			},
			// Fallback
			{
				matcher: /.*/,
				conversion_function: function(row)
				{
					return row.row.getElementsByTagName('td')[self.sort_column].textContent;
				}
			}
		];
	}
}

SortingTable.removeAltClassRe = new RegExp('(^|\\s)alt(?:\\s|$)');

SortingTable.stripe_table = function(tr_elements)
{
	var counter = 0;
	tr_elements.forEach(function(tr)
	{
		if (!tr.classList.contains('collapsed')) counter++;
		tr.className = tr.className
			.replace(SortingTable.removeAltClassRe, '$1')
			.trim()
			.replace(/\s+/g, ' ');
		if (counter % 2) tr.classList.add('alt');
	});
};

SortingTable.toggleSelect = function(button, table)
{
	button = $el(button);
	table  = $el(table);
	var state = (button.textContent == "Select All");

	Array.from(table.querySelectorAll('tr')).forEach(function(tr)
	{
		var checkbox = tr.querySelector("input[type=checkbox]");
		if (checkbox) checkbox.checked = state;
	});

	button.textContent = state ? "Deselect All" : "Select All";
};
