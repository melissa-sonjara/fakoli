
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

 thE SOFTWARE IS PROVIDED "AS IS", WIthOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO thE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL thE AUthORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OthER LIABILITY,
 WHEthER IN AN ACTION OF CONtrACT, TORT OR OthERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WIth thE SOFTWARE OR thE USE OR
 OthER DEALINGS IN thE SOFTWARE.

*****************************************************************/

// These should be consts, but are vars for backwards compatibility

var MonthSelectNone  = 0;
var MonthSelectStart = 1;
var MonthSelectEnd   = 2;

var Calendar = new Class(
{
	varName: 		"",
	formName:		"",
	ctrlName:		"",
	divID:			"",
	
	control:		Class.Empty,
	form: 			Class.Empty,
	date: 			Class.Empty,
	calendar:		Class.Empty,
	shim:			Class.Empty,
	month: 			0,
	year: 			0,
	monthSelect:	MonthSelectNone,
	weekdays:		["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"],
	months:			["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"],
	longMonths:		[	"January",
						"February",
						"March",
						"April",
						"May",
						"June",
						"July",
						"August",
						"September",
						"October",
						"November",
						"December"],
						
	daysInMonth:	[31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31],
	
	initialize: function(varName, formName, ctrlName)
	{
		this.varName = varName;
		if (formName)
		{
			this.formName = formName;
			this.ctrlName = ctrlName;
			this.divID    = varName + "_" + formName + "_" + ctrlName;
		}
		else
		{
			this.ctrlName = ctrlName;
			this.divID = varName + "_" + ctrlName;
		}
		
		this.calendar = new Element('div', {id: this.divID});
		this.calendar.setStyles({'position': 'absolute', 'z-index': 255, 'display': 'none', 'opacity': 0});
		this.calendar.addEvent('mouseout', function(e) {this.onMouseOut(e);}.bind(this));

		this.shim = new IframeShim(this.calendar);
		
		window.addEvent("domready", function() { document.body.grab(this.calendar, 'top'); }.bind(this));
	
	},
	
	getDateString: function(yearFirst)
	{
		var d;
		if (yearFirst)
		{
			d = this.date.getFullYear() + "/" + this.date.getMonth() + "/" + this.date.getDate();
		}
		else
		{
			d = this.date.getDate() + "/" + this.date.getMonth() + "/" + this.date.getFullYear();
		}
		
		return d;
	},	

	getMonthString: function()
	{
		return this.months[this.date.getMonth()];
	},

	getDaysInMonth: function(month, year)
	{
		var dim = this.daysInMonth[month];
	
		if (month == 1) // only check for Feb
		{
			if ((year % 4) == 0) 
			{
				if (year % 100 != 0 || (year % 400) == 0)
				{
					dim +=1;
				}
			}
		}
		
		return dim;
	},

	getLongDateString: function()
	{
		return this.date.getDate() + " " + this.longMonths[this.date.getMonth()] + " " + this.date.getFullYear();		
	},
	
	getDateForDay: function(day)
	{
		var d = (this.month + 1) + "/" + day + "/" +  this.year;
	
		return d;	
	},
	
	drawCalendar: function()
	{
		var doc;
		
		doc  = "<table class='calendar' cellpadding='2'>";
		doc += "<tr>";
		doc += "<td colspan='7'><table border='0' width='100%'>";
		doc += "<td class='calnav'><a href=\"javascript:" + this.varName + ".prevMonth()\">&nbsp;&laquo;&nbsp;</a></td>";
		doc += "<td class='calmonth'>";
		switch(this.monthSelect)
		{
		case MonthSelectNone:
			break;
			
		case MonthSelectStart:
			doc += "<a href=\"javascript:" + 
					this.varName + ".select('" + this.getDateForDay(1) + "')\">";
			break;
			
		case MonthSelectEnd:
			doc += "<a href=\"javascript:" + 
					this.varName + ".select('" + this.getDateForDay(this.getDaysInMonth(this.month, this.year)) + "')\">";
			break;
		}
		
		doc += this.months[this.month] + " " + this.year;
		
		if (this.monthSelect == MonthSelectStart ||
			this.monthSelect == MonthSelectEnd)
		{
			doc += "</a>";
		}	
		
		doc += "</td>";
		doc += "<td class='calnav'><a href=\"javascript:" + this.varName + ".nextMonth()\">&nbsp;&raquo;&nbsp;</a></td>";
		doc += "</tr></table>";
		
		doc += "<tr>";
		
		for(i = 0; i < 7; ++i)
		{
			doc += "<th>";
			doc += this.weekdays[i];
		}
				
		var counter = 0;
		var day = 1;
		
		var limit = this.getDaysInMonth(this.month, this.year);
		
		var fd = new Date();
		fd.setDate(1);
		fd.setMonth(this.month);
		fd.setFullYear(this.year);
		
		var startDay = fd.getDay();
		
		doc += "<tr>";
		
		for(i = 0; i < startDay; ++i)
		{
			doc += "<td class='calday'>&nbsp;</td>";
			++counter;
		}
		
		while(day <= limit)
		{
			if ((counter % 7) == 0)
			{
				doc += "</tr><tr>";
			}
			
			var today = new Date();
			
			if (this.date.getDate() == day &&
				this.date.getMonth() == this.month &&
				this.date.getFullYear() == this.year)
			{
				cellStyle = "calselday";
			}
			else if (day == today.getDate() &&
					 this.month == today.getMonth() &&
					 this.year == today.getFullYear())
			{
				cellStyle = "caltoday";
			}
			else
			{	
				cellStyle = "calday";
			}
			
			++counter;
			doc  += "<td class='" + cellStyle + "'><a href=\"javascript:" + 
					this.varName + ".select('" + this.getDateForDay(day) + "')\">" + day + "</a></td>";
			++day;
		}
		
		doc += "</tr></table>";
				
		//alert(doc);
		return doc;
	},
	
	bindControl: function()
	{
		if (this.formName)
		{
			this.form = document.forms[this.formName];
			
			if (!this.form)
			{
				alert("Cannot find form");
				return;
			}
				
			this.control = document.forms[this.formName][this.ctrlName];
			
			if (!this.control)
			{
				alert("Cannot find control");
			}
		}
		else
		{
			this.control = document.getElementById(this.ctrlName);
			if (!this.control)
			{
				alert("Cannot find control");
			}
			this.form = this.control.form;
		}
		this.bindDate();
	},
	
	bindDate: function()
	{
		if (this.control.value)
		{
			this.date = new Date(this.control.value);
		}
		else
		{
			this.date = new Date();
		}	
	
		this.day   = this.date.getDate();
		this.month = this.date.getMonth();
		this.year  = this.date.getFullYear();
	},

	show: function(parent)
	{
		parent = document.id(parent);
		
		var zIndex = calculateZIndex(parent) + 1;
		
		if (!this.form) this.bindControl();
		this.draw();
		
		this.calendar.setStyles({position: "absolute", display: 'block', 'opacity': 0, 'z-index': zIndex});
		this.calendar.position({'relativeTo': parent, 'position': 'topRight', 'offset': {'x': 0, 'y': 0} });
		this.calendar.fade('in');
		
		if (this.shim)
		{
			this.shim.position();
			this.shim.show();
		}
	},
	
	
	hide: function()
	{
		if (!this.form) this.bindControl();
		this.calendar.fade('out');
		//this.calendar.setStyles({'position': "absolute", 'display': 'none'});
		
		if (this.shim)
		{
			this.shim.hide();
		}
	},
	
	toggle: function(parent)
	{
		if (!this.form) this.bindControl();
		

		if (this.calendar.getStyle('opacity') > 0)
		{
			this.hide();
		}
		else
		{
			this.bindDate();
			this.show(parent);
		}
	},
			
	draw: function()
	{
		this.calendar.set('html', this.drawCalendar());
	},
	
	prevMonth: function()
	{
		this.month--;
		if (this.month < 0)
		{
			this.month = 11;
			this.year--;
		}
		
		this.draw();
	},

	nextMonth: function()
	{
		this.month++;
		if (this.month > 11)
		{
			this.month = 0;
			this.year++;
		}
		
		this.draw();
	},

	select: function(date)
	{
		this.control.value = date;
		this.bindDate();
		this.onDateChanged(date);
		// Hide calendar after select on touch screen devices
		if (Browser.platform == "ios" || Browser.platform == "android" || Browser.platform == "webos")
		{
			this.hide();
		}
		else
		{
			this.draw();
		}
	},
	
	setMonthSelectMode: function(mode)
	{
		this.monthSelect = mode;
	},
		
	onMouseOut: function(event)
	{
		var coords = this.calendar.getCoordinates();
		
		var x = event.page.x;
		var y = event.page.y;
		
		window.status = "(" + x + "," + y + ") [" + coords.left + ", " + coords.width + "] <" + coords.top + "," + coords.height + ">";
		
		if (x <= coords.left || x >= coords.left + coords.width || y <= coords.top || y >= coords.top + coords.height)
		{
			this.hide();
		}
	},
	
	onDateChanged: function(date)
	{
		if (typeof(this.control.onchange) != 'undefined')
		{
			this.control.onchange();
		}
	}
});
