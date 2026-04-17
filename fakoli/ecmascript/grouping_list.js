// Requires ui.js helpers ($el, fadeIn, fadeOut, etc.)

/**
 * Manages an accordion-style grouped list, toggling child items open or
 * collapsed when their section heading is clicked, with optional facet filtering.
 */
class GroupingList
{
	constructor(div, options)
	{
		this.div = $el(div);
		this.options = Object.assign({
			mode:       'accordian',
			groupClass: 'subheading'
		}, options || {});

		this.subheadings  = null;
		this.content_divs = null;
		this.openFirst    = false;

		if (this.options.mode != "fixed")
		{
			this.subheadings  = Array.from(this.div.querySelectorAll("h2"));
			this.content_divs = Array.from(this.div.children).filter(function(c) { return c.tagName == 'DIV'; });

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

			this.subheadings.forEach(function(r)
			{
				r.classList.add('collapsed');
			});

			if (this.openFirst && this.content_divs.length > 0)
			{
				var firstH2s = this.content_divs[0].querySelectorAll('h2');
				if (firstH2s.length > 0)
				{
					firstH2s[0].classList.remove('collapsed');
					firstH2s[0].classList.add('expanded');
				}
			}
		}

		if (this.div.facetManager)
		{
			var self = this;
			this.div.facetManager.addEvent('filterChanged', function() { self.filterChanged(); });
			this.div.facetManager.addEvent('filterCleared', function() { self.filterCleared(); });
			this.preprocessFacets();
		}

		this.update();
	}

	update()
	{
		if (!this.content_divs) return;

		this.content_divs.forEach(function(div)
		{
			var subheading = div.querySelector('h2');
			var lists = Array.from(div.querySelectorAll(':scope > ul'));

			if (subheading && subheading.classList.contains('expanded'))
			{
				lists.forEach(function(list) { list.style.display = ''; });
			}
			else
			{
				lists.forEach(function(list) { list.style.display = 'none'; });
			}
		});
	}

	handleClick(item)
	{
		this.subheadings.forEach(function(r)
		{
			if (r.id != item.id)
			{
				r.classList.remove('expanded');
				r.classList.add('collapsed');
			}
		});

		if (item.classList.contains('collapsed'))
		{
			item.classList.remove('collapsed');
			item.classList.add('expanded');
		}
		else
		{
			item.classList.remove('expanded');
			item.classList.add('collapsed');
		}

		this.update();
	}

	preprocessFacets()
	{
		var self = this;
		Array.from(this.div.querySelectorAll('li')).forEach(function(elt)
		{
			self.div.facetManager.preprocess(elt);
		});
		this.div.facetManager.preprocessComplete();
	}

	filterChanged()
	{
		var self = this;
		Array.from(this.div.querySelectorAll('li')).forEach(function(elt)
		{
			elt.classList.remove("filtered");
			elt.classList.remove("filtermatch");

			var match = self.div.facetManager.filter(elt);
			if (match)
			{
				elt.classList.add('filtermatch');
				elt.style.display = '';
			}
			else
			{
				elt.classList.add('filtered');
				elt.style.display = 'none';
			}
		});
	}

	filterCleared()
	{
		Array.from(this.div.querySelectorAll('li')).forEach(function(elt)
		{
			elt.classList.remove("filtered");
			elt.classList.remove("filtermatch");
			elt.style.display = '';
		});
	}
}
