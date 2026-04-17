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

// Annotate the document body with browser info

class FilteringCalendar
{
	constructor(calendar_id, options)
	{
		this.options = Object.assign({ per_day: 5 }, options);

		///< The id of the calendar table
		this.list = null;
		///< The tag for the event summary that will store the event id and data attribute tags
		this.elt_tag = null;

		this.list = document.getElementById(calendar_id);

		if (!this.list) return;
		this.elt_tag = "div";

		if (this.list.facetManager)
		{
			this.list.facetManager.addEventListener('filterChanged', () => { this.filterChanged(); });
			this.list.facetManager.addEventListener('filterCleared', () => { this.filterCleared(); });
			this.preprocessFacets();
		}

		if (this.options.per_day)
		{
			document.querySelectorAll('.event').forEach((day) =>
			{
				if (this.getDayCount(day) > this.options.per_day)
				{
					this.toggleHidden(day);
				}
			});
		}
		this.update_days();
	}

	getDayCount(day)
	{
		return day.querySelectorAll(this.elt_tag).length;
	}

	preprocessFacets()
	{
		this.list.querySelectorAll(this.elt_tag).forEach((elt) =>
		{
			this.list.facetManager.preprocess(elt);
		});

		this.list.facetManager.preprocessComplete();
	}

	filterChanged()
	{
		this.list.querySelectorAll(this.elt_tag).forEach((elt) =>
		{
			elt.classList.remove("filtered");
			elt.classList.remove("filtermatch");

			var match = this.list.facetManager.filter(elt);

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

		this.update_days();
	}

	/**
	 * When the filter changes, show or hide "more" link based on the
	 * whether the number of events show exceeds the max to display per
	 * day option.
	 */
	update_days()
	{
		if (!this.options.per_day) return;

		document.querySelectorAll('.event').forEach((day) =>
		{
			this.filterDayList(day);
		});
	}

	filterDayList(day)
	{
		if (!this.options.per_day) return;
		if (this.getDayCount(day) < this.options.per_day) return;

		this.toggleHidden(day);
	}

	toggleHidden(day)
	{
		var count = 0;
		day.querySelectorAll(this.elt_tag).forEach((elt) =>
		{
			if (!elt.classList.contains('filtered'))
			{
				count += 1;
			}

			if (count > this.options.per_day)
			{
				setTimeout(() => { elt.classList.add("over_max"); }, 1000);
			}
			else
			{
				setTimeout(() => { elt.classList.remove("over_max"); }, 1000);
			}
		});

		this.toggleShowMoreLink(day, count);
	}

	getMoreElt(day)
	{
		var day_num = day.getAttribute("data-day");
		var id = 'show_all_' + day_num;

		return document.getElementById(id);
	}

	toggleShowMoreLink(day, unfiltered_count)
	{
		if (unfiltered_count <= this.options.per_day)
		{
			this.removeShowAllLink(day);
		}
		else
		{
			this.insertShowAllLink(day);
		}
	}

	insertShowAllLink(day)
	{
		var elt = this.getMoreElt(day);
		if (elt) return;

		var day_num = day.getAttribute("data-day");
		var id = 'show_all_' + day_num;

		var show_elt = document.createElement('a');
		show_elt.id = id;
		show_elt.className = 'show_all';
		show_elt.href = '#';
		var me = this;
		show_elt.addEventListener('click', function(e) { me.toggleShowAll(this); });

		show_elt.textContent = 'Show All';
		day.appendChild(show_elt);
	}

	removeShowAllLink(day)
	{
		var elt = this.getMoreElt(day);
		if (!elt) return;

		elt.remove();
	}

	toggleShowAll(elt)
	{
		var day = findAncestor(elt, 'td');

		if (elt.textContent == 'Show All')
		{
			day.querySelectorAll(this.elt_tag).forEach((summary_elt) =>
			{
				setTimeout(() => { summary_elt.classList.remove("over_max"); }, 1000);
			});

			elt.textContent = 'Show Less';
		}
		else
		{
			elt.textContent = 'Show All';
			this.toggleHidden(day);
		}
	}

	filterCleared()
	{
		this.list.querySelectorAll(this.elt_tag).forEach((elt) =>
		{
			if (elt.getAttribute("class") != 'events_more')
			{
				elt.classList.remove("filtered");
				elt.classList.remove("filtermatch");
				elt.style.display = '';
			}
		});

		this.update_days();
	}
}
