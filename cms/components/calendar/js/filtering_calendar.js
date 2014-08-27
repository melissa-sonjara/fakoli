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

var FilteringCalendar = new Class(
{
	Implements: Options,
	  
	  options: {
	    per_day: 5,
	  },
	  
	list: 	Class.Empty,		///< The id of the calendar table
	elt_tag: null,				///< The tag for the event summary that will store the event id and data attribute tags

	initialize: function(calendar_id, options)
	{
		this.list = document.id(calendar_id);
		this.setOptions(options);
		   
		if (!this.list) return;
		this.elt_tag = "div";
	
		if (this.list.facetManager)
		{
			this.list.facetManager.addEvent('filterChanged', function() { this.filterChanged();}.bind(this));
			this.list.facetManager.addEvent('filterCleared', function() { this.filterCleared();}.bind(this));
			this.preprocessFacets();
		}
		
		if(this.options.per_day)
		{
			$$('.event').each(function(day)
			{
				if(this.getDayCount(day) > this.options.per_day)
				{
					this.toggleHidden(day);
				}
			}.bind(this));
		}
	    this.update_days();
	},
	
	
	getDayCount: function(day)
	{
		return day.getElements(this.elt_tag).length;
	},
		
	preprocessFacets: function()
	{
		this.list.getElements(this.elt_tag).each(function(elt)
		{
			this.list.facetManager.preprocess(elt);
		}.bind(this));
		
		this.list.facetManager.preprocessComplete();
	},
	
	filterChanged: function()
	{
		this.list.getElements(this.elt_tag).each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			
			var match = this.list.facetManager.filter(elt);
			
			if (match)
			{
				elt.addClass('filtermatch');
				elt.show();
			}
			else
			{
				elt.addClass('filtered');
				elt.hide();
			}
		}.bind(this));
		
	    this.update_days();
	},
	
	/**
	 * When the filter changes, show or hide "more" link based on the
	 * whether the number of events show exceeds the max to display per
	 * day option.
	 */
	update_days: function()
	{
		if(!this.options.per_day) return;
		
		$$('.event').each(function(day)
		{
			this.filterDayList(day);
			
		}.bind(this));
	},
	
	
	filterDayList: function(day)
	{
		if(!this.options.per_day) return;		
		if(this.getDayCount(day) < this.options.per_day) return;
		
		this.toggleHidden(day);
	},
	
	toggleHidden: function(day)
	{
		var count = 0;
		day.getElements(this.elt_tag).each(function(elt)
		{
			if(!elt.hasClass('filtered'))
			{
				count += 1;
			}
					
			if(count > this.options.per_day)
			{
				(function(){ elt.addClass("over_max"); }).bind(this).delay(1000);
			}
			else
			{
				(function(){ elt.removeClass("over_max"); }).bind(this).delay(1000);
			}		
		}.bind(this));
		
		this.toggleShowMoreLink(day, count);
	},
	
	getMoreElt: function(day)
	{
		var day_num = day.get("data-day");
		var id = 'show_all_' + day_num;
	
		return document.id(id);
	},

	toggleShowMoreLink: function(day, unfiltered_count)
	{
		if(unfiltered_count <= this.options.per_day)
		{
			this.removeShowAllLink(day);
		}
		else
		{
			this.insertShowAllLink(day);		
		}
	},
	
	insertShowAllLink: function(day)
	{
		var elt = this.getMoreElt(day);
		if(elt) return;
		
		var day_num = day.get("data-day");
		var id = 'show_all_' + day_num;
		
		var show_elt = new Element('a', {'id': id, 'class': 'show_all', 'href': '#'});
		me = this;
		show_elt.addEvent('click', function(e) { me.toggleShowAll(this);});
		
		show_elt.set('text', 'Show All');
		show_elt.inject(day, 'bottom');
	},
	
	removeShowAllLink: function(day)
	{
		var elt = this.getMoreElt(day);
		if(!elt) return;
		
		elt.dispose();
	},
	
	toggleShowAll: function(elt)
	{
		var day = findAncestor(elt, 'td');
		
		if(elt.get('text') == 'Show All')
		{
			day.getElements(this.elt_tag).each(function(summary_elt)
			{
				(function(){ summary_elt.removeClass("over_max"); }).bind(this).delay(1000);
			});			
		
			elt.set('text', 'Show Less');
		}
		else
		{
			elt.set('text', 'Show All');
			this.toggleHidden(day);
		}
	},
	
	filterCleared: function()
	{
		this.list.getElements(this.elt_tag).each(function(elt)
		{
			if(elt.getAttribute("class") != 'events_more')
			{
				elt.removeClass("filtered");
				elt.removeClass("filtermatch");
				elt.show();
			}
		});
		
 	    this.update_days();
	}
});
