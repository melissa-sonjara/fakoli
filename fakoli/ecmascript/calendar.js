/**************************************************************

 Copyright (c) 2006,2007,2008 Sonjara, Inc

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

var MonthSelectNone  = 0;
var MonthSelectStart = 1;
var MonthSelectEnd   = 2;

class Calendar
{
	/**
	 * @param {string} varName    Global variable name this instance is stored under.
	 * @param {string} [formName] Name of the containing form (optional).
	 * @param {string} ctrlName   Name / id of the input field.
	 */
	constructor(varName, formName, ctrlName)
	{
		this.varName  = varName;
		this.ctrlName = ctrlName;
		this.formName = formName || '';
		this.divID    = formName
			? varName + '_' + formName + '_' + ctrlName
			: varName + '_' + ctrlName;

		this.control     = null;
		this.form        = null;
		this.date        = null;
		this.month       = 0;
		this.year        = 0;
		this.monthSelect = MonthSelectNone;

		this.weekdays   = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
		this.months     = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
		this.longMonths = ['January', 'February', 'March', 'April', 'May', 'June',
		                   'July', 'August', 'September', 'October', 'November', 'December'];
		this.daysInMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];

		// Build popup div
		var div = document.createElement('div');
		div.id        = this.divID;
		div.className = 'calendarPopup';
		div.style.cssText = 'position:absolute;z-index:255;display:none;opacity:0;';

		// Event delegation for nav and day clicks
		div.addEventListener('click', this._onClick.bind(this));
		div.addEventListener('mouseleave', this._onMouseLeave.bind(this));

		this.calendar = div;

		// Attach to body when DOM is ready
		var self = this;
		if (document.readyState === 'loading')
		{
			document.addEventListener('DOMContentLoaded', function()
			{
				document.body.insertBefore(self.calendar, document.body.firstChild);
			});
		}
		else
		{
			document.body.insertBefore(this.calendar, document.body.firstChild);
		}
	}

	// ── Date helpers ───────────────────────────────────────────────────────

	getDateString(yearFirst)
	{
		if (yearFirst)
		{
			return this.date.getFullYear() + '/' + this.date.getMonth() + '/' + this.date.getDate();
		}
		return this.date.getDate() + '/' + this.date.getMonth() + '/' + this.date.getFullYear();
	}

	getMonthString()
	{
		return this.months[this.date.getMonth()];
	}

	getDaysInMonth(month, year)
	{
		var dim = this.daysInMonth[month];
		if (month === 1 && (year % 4) === 0 && (year % 100 !== 0 || (year % 400) === 0))
		{
			dim += 1;
		}
		return dim;
	}

	getLongDateString()
	{
		return this.date.getDate() + ' ' + this.longMonths[this.date.getMonth()] + ' ' + this.date.getFullYear();
	}

	getDateForDay(day)
	{
		return (this.month + 1) + '/' + day + '/' + this.year;
	}

	// ── Rendering ──────────────────────────────────────────────────────────

	drawCalendar()
	{
		var doc = "<table class='calnav'><tr>";
		doc += "<td class='calnav'><a href='#' data-action='prevMonth'>&nbsp;&laquo;&nbsp;</a></td>";
		doc += "<td class='calmonth'>";

		if (this.monthSelect === MonthSelectStart)
		{
			doc += "<a href='#' data-action='select' data-date='" + this.getDateForDay(1) + "'>";
		}
		else if (this.monthSelect === MonthSelectEnd)
		{
			doc += "<a href='#' data-action='select' data-date='" +
				this.getDateForDay(this.getDaysInMonth(this.month, this.year)) + "'>";
		}

		doc += this.months[this.month] + ' ' + this.year;

		if (this.monthSelect === MonthSelectStart || this.monthSelect === MonthSelectEnd)
		{
			doc += '</a>';
		}

		doc += "</td>";
		doc += "<td class='calnav'><a href='#' data-action='nextMonth'>&nbsp;&raquo;&nbsp;</a></td>";
		doc += "</tr></table>";

		doc += "<table class='calendar' cellpadding='2'><tr>";

		for (var i = 0; i < 7; ++i)
		{
			doc += '<th>' + this.weekdays[i] + '</th>';
		}

		doc += '</tr><tr>';

		var counter  = 0;
		var day      = 1;
		var limit    = this.getDaysInMonth(this.month, this.year);
		var fd       = new Date(this.year, this.month, 1);
		var startDay = fd.getDay();
		var today    = new Date();

		for (i = 0; i < startDay; ++i)
		{
			doc += "<td class='calday empty'>&nbsp;</td>";
			++counter;
		}

		while (day <= limit)
		{
			if ((counter % 7) === 0)
			{
				doc += '</tr><tr>';
			}

			var cellStyle;
			if (this.date.getDate() === day &&
				this.date.getMonth() === this.month &&
				this.date.getFullYear() === this.year)
			{
				cellStyle = 'calselday';
			}
			else if (day === today.getDate() &&
			         this.month === today.getMonth() &&
			         this.year === today.getFullYear())
			{
				cellStyle = 'caltoday';
			}
			else
			{
				cellStyle = 'calday';
			}

			doc += "<td class='" + cellStyle + "'>" +
				"<a href='#' data-action='select' data-date='" + this.getDateForDay(day) + "'>" +
				day + '</a></td>';

			++counter;
			++day;
		}

		while ((counter % 7) !== 0)
		{
			doc += "<td class='calday empty'>&nbsp;</td>";
			++counter;
		}

		doc += '</tr></table>';
		return doc;
	}

	// ── Control binding ────────────────────────────────────────────────────

	bindControl()
	{
		if (this.formName)
		{
			this.form = document.forms[this.formName];
			if (!this.form)
			{
				alert('Cannot find form');
				return;
			}
			this.control = this.form[this.ctrlName];
			if (!this.control)
			{
				alert('Cannot find control');
			}
		}
		else
		{
			this.control = document.getElementById(this.ctrlName);
			if (!this.control)
			{
				alert('Cannot find control');
			}
			this.form = this.control ? this.control.form : null;
		}
		this.bindDate();
	}

	bindDate()
	{
		this.date  = this.control.value ? new Date(this.control.value) : new Date();
		this.day   = this.date.getDate();
		this.month = this.date.getMonth();
		this.year  = this.date.getFullYear();
	}

	// ── Show / hide ────────────────────────────────────────────────────────

	show(parent)
	{
		parent = (typeof parent === 'string') ? document.getElementById(parent) : parent;

		parent = document.id(parent);
		
		var zIndex = calculateZIndex(parent) + 1;
		
		if (!this.form) this.bindControl();
		this.draw();
		
		this.calendar.setStyles({position: "absolute", display: 'block', 'opacity': 0, 'z-index': zIndex});
		this.calendar.position({'relativeTo': parent, 'position': 'topRight', 'offset': {'x': 0, 'y': 0} });
		this.calendar.fade('in');
		
	}

	hide()
	{
		if (!this.form) this.bindControl();
		this.calendar.fade('out');
	}

	toggle(parent)
	{
		if (!this.form) this.bindControl();

		if (parseFloat(this.calendar.style.opacity) > 0)
		{
			this.hide();
		}
		else
		{
			this.bindDate();
			this.show(parent);
		}
	}

	// ── Navigation / selection ─────────────────────────────────────────────

	draw()
	{
		this.calendar.innerHTML = this.drawCalendar();
	}

	prevMonth()
	{
		this.month--;
		if (this.month < 0)
		{
			this.month = 11;
			this.year--;
		}
		this.draw();
	}

	nextMonth()
	{
		this.month++;
		if (this.month > 11)
		{
			this.month = 0;
			this.year++;
		}
		this.draw();
	}

	select(date)
	{
		this.control.value = date;
		this.bindDate();
		this.onDateChanged(date);

		var ua = navigator.userAgent.toLowerCase();
		if (/iphone|ipad|ipod|android|webos/.test(ua))
		{
			this.hide();
		}
		else
		{
			this.draw();
		}
	}

	setMonthSelectMode(mode)
	{
		this.monthSelect = mode;
	}

	// ── Event handlers ─────────────────────────────────────────────────────

	/** @private Delegated click handler for nav arrows and day cells. */
	_onClick(event)
	{
		var target = event.target.closest('[data-action]');
		if (!target) return;

		event.preventDefault();

		var action = target.getAttribute('data-action');
		if (action === 'prevMonth')
		{
			this.prevMonth();
		}
		else if (action === 'nextMonth')
		{
			this.nextMonth();
		}
		else if (action === 'select')
		{
			this.select(target.getAttribute('data-date'));
		}
	}

	/** @private Hide when the pointer leaves the calendar popup. */
	_onMouseLeave(event)
	{
		var rect = this.calendar.getBoundingClientRect();
		var x    = event.pageX - (window.scrollX || window.pageXOffset);
		var y    = event.pageY - (window.scrollY || window.pageYOffset);

		if (x <= rect.left || x >= rect.right || y <= rect.top || y >= rect.bottom)
		{
			this.hide();
		}
	}

	onDateChanged(date)
	{
		if (this.control && typeof this.control.onchange === 'function')
		{
			this.control.onchange();
		}
	}
}
