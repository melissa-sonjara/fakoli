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
 * Splits a table's rows into pages with Prev/Next navigation controls,
 * supporting zebra striping, detail rows, and integration with FacetManager.
 */
class PaginatingTable
{
	constructor(table, ids, options)
	{
		this.table = $el(table);
		this.options = Object.assign({
			per_page:     10,
			current_page: 1,
			offset_el:    false,
			cutoff_el:    false,
			details:      false,
			link_count:   10,
			zebra:        false
		}, options || {});

		this.excelLink = $el(this.table.id + "_excel");
		this.excelQS   = this.excelLink ? this.excelLink.getAttribute('href') : null;

		this.tbody = this.table.querySelector('tbody');

		if (this.options.offset_el) this.options.offset_el = $el(this.options.offset_el);
		if (this.options.cutoff_el) this.options.cutoff_el = $el(this.options.cutoff_el);

		if (Array.isArray(ids))
		{
			this.paginators = ids.map(function(id) { return $el(id); });
		}
		else
		{
			this.paginators = [$el(ids)];
		}

		if (this.options.details)
		{
			this.options.per_page = this.options.per_page * 2;
		}

		if (this.table.facetManager)
		{
			var self = this;
			this.table.facetManager.addEvent('filterChanged', function() { self.filterChanged(); });
			this.table.facetManager.addEvent('filterCleared', function() { self.filterCleared(); });
			this.preprocessFacets();
			this.filterChanged();
		}
		else
		{
			this.update_pages();
		}
	}

	countRows()
	{
		var filtered = this.tbody.querySelectorAll(".filtered").length;
		var all = this.tbody.children.length;
		return all - filtered;
	}

	update_pages()
	{
		this.pages = Math.ceil(this.countRows() / this.options.per_page);
		this.create_pagination();
		this.restripeTable();
		this.to_page(1);
	}

	to_page(page_num)
	{
		page_num = parseInt(page_num, 10);
		if (page_num > this.pages || page_num < 1) return;
		this.current_page = page_num;
		this.low_limit  = this.options.per_page * (this.current_page - 1);
		this.high_limit = this.options.per_page * this.current_page;
		var trs = Array.from(this.tbody.children);
		if (trs.length < this.high_limit) this.high_limit = trs.length;

		var c = 0;
		trs.forEach(function(tr, i)
		{
			if (tr.classList.contains("filtered"))
			{
				tr.style.display = 'none';
				return;
			}
			tr.style.display = (this.low_limit <= c && this.high_limit > c) ? '' : 'none';
			c++;
		}.bind(this));

		var self = this;
		this.paginators.forEach(function(paginator)
		{
			paginator.querySelectorAll("a.goto-page").forEach(function(p)
			{
				p.innerHTML = 'Page ' + (self.current_page || 1) + " of " + self.pages;
			});
		});

		if (this.options.offset_el)
		{
			this.options.offset_el.textContent = Math.ceil(this.low_limit / (this.options.details ? 2 : 1) + 1);
		}
		if (this.options.cutoff_el)
		{
			this.options.cutoff_el.textContent = this.high_limit / (this.options.details ? 2 : 1);
		}
	}

	to_next_page() { this.to_page(this.current_page + 1); }
	to_prev_page() { this.to_page(this.current_page - 1); }

	create_pagination()
	{
		var self = this;
		this.paginators.forEach(function(paginator)
		{
			paginator.innerHTML = '';

			self.create_pagination_node('\u00ab\u00a0Prev', function(evt)
			{
				evt.preventDefault();
				evt.stopPropagation();
				self.to_prev_page();
				return false;
			}).appendTo(paginator);

			var li = document.createElement('li');
			li.className = 'pager';
			var a = document.createElement('a');
			a.href = "#";
			a.className = 'goto-page';
			a.innerHTML = 'Page ' + (self.current_page || 1) + " of " + self.pages;
			li.appendChild(a);
			paginator.appendChild(li);

			self.create_pagination_node('Next\u00a0\u00bb', function(evt)
			{
				evt.preventDefault();
				evt.stopPropagation();
				self.to_next_page();
				return false;
			}).appendTo(paginator);

			paginator.style.display = (self.pages < 2) ? 'none' : 'block';
		});
	}

	create_pagination_node(text, evtHandler)
	{
		var span = document.createElement('span');
		span.innerHTML = text;
		var a = document.createElement('a');
		a.href = '#';
		a.className = 'paginate';
		a.addEventListener('click', evtHandler);
		a.appendChild(span);
		var li = document.createElement('li');
		li.appendChild(a);
		// Convenience helper used above
		li.appendTo = function(parent) { parent.appendChild(this); };
		return li;
	}

	restripeTable()
	{
		if (!this.options.zebra) return;
		var counter = 0;
		Array.from(this.tbody.querySelectorAll('tr')).forEach(function(tr)
		{
			if (tr.classList.contains('filtered')) return;
			if (!tr.classList.contains('collapsed')) counter++;
			tr.classList.remove('alt');
			if (counter % 2) tr.classList.add('alt');
		});
	}

	preprocessFacets()
	{
		var self = this;
		Array.from(this.tbody.querySelectorAll('tr')).forEach(function(elt)
		{
			self.table.facetManager.preprocess(elt);
		});
		this.table.facetManager.preprocessComplete();
	}

	filterChanged()
	{
		var self = this;
		Array.from(this.tbody.querySelectorAll('tr')).forEach(function(elt)
		{
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

		this.update_pages();

		if (this.excelLink)
		{
			this.excelLink.setAttribute('href', this.excelQS + "&" + this.table.facetManager.getQueryString());
		}
	}

	filterCleared()
	{
		Array.from(this.tbody.querySelectorAll('tr')).forEach(function(elt)
		{
			elt.classList.remove("filtered");
			elt.classList.remove("filtermatch");
		});

		this.update_pages();

		if (this.excelLink)
		{
			this.excelLink.setAttribute('href', this.excelQS);
		}
	}
}
