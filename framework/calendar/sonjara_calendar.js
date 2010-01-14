
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

// These should be consts, but are vars for backwards compatibility

var MonthSelectNone  = 0;
var MonthSelectStart = 1;
var MonthSelectEnd   = 2;

function Calendar(varName, formName, ctrlName)
{
	this.varName  = varName;
	this.formName = formName;
	this.ctrlName = ctrlName;
	this.divID    = varName + "_" + formName + "_" + ctrlName;
	
	this.control = null;
	this.form    = null;
	this.date    = null;
	this.month   = 0;
	this.year    = 0;
	this.monthSelect = MonthSelectNone;
	this.weekdays = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
	this.months =   ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
	this.longMonths = [	"January",
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
						"December"];
						
	this.daysInMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
	
	
	this.drawCalendar   	= drawCalendar;
	this.getDateString  	= getDateString;
	this.getMonthString 	= getMonthString;
	this.getLongDateString = getLongDateString;
	this.getDaysInMonth 	= getDaysInMonth;
	this.getDateForDay  	= getDateForDay;
	this.bind				= bind;
	this.bindDate			= bindDate;
	this.draw		    	= draw;
	this.show				= show;
	this.toggle				= toggle;
	this.hide				= hide;
	this.nextMonth		 	= nextMonth;
	this.prevMonth			= prevMonth;
	this.select  			= select;
	this.onMouseOut        	= onMouseOut;
	this.setMonthSelectMode = setMonthSelectMode;
	this.onDateChanged  = onDateChanged;
	// For IE5.5+, we need to use an IFRAME shim to ensure that
	// the calendar overlays SELECT and IFRAME sections properly.
	// However, for other browsers this can lead to visual artifacts,
	// plus from a neatness standpoint there's no need to carry
	// around an extra IFRAME if we don't need it...

	if (navigator.appVersion.indexOf("MSIE")!=-1)
	{
		var temp = navigator.appVersion.split("MSIE");
		var version = parseFloat(temp[1]);
		if (version>=5.5)
		{
			document.write('<iframe id="' + this.divID + 'Shim" style="display: none; position: absolute; z-index: 254"></iframe>');
		}
	}
	
	document.write('<div style="position: absolute; z-index: 255; visibility: hidden" id="' + this.divID + '" onmouseout="' + this.varName + '.onMouseOut(event)"></div>');
	
	function getDateString(yearFirst)
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
	}	

	function getMonthString()
	{
		return this.months[this.date.getMonth()];
	}

	function getDaysInMonth(month, year)
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
	}
	function getLongDateString()
	{
		return this.date.getDate() + " " + this.longMonths[this.date.getMonth()] + " " + this.date.getFullYear();		
	}
	
	function getDateForDay(day)
	{
		var d = (this.month + 1) + "/" + day + "/" +  this.year;
	
		return d;	
	}
	
	function drawCalendar()
	{
		var doc;
		
		doc  = "<TABLE CLASS='calendar' CELLPADDING='2'>";
		doc += "<TR>";
		doc += "<TD COLSPAN='7'><TABLE BORDER='0' WIDTH='100%'>";
		doc += "<TD CLASS='calnav'><A HREF=\"javascript:" + this.varName + ".prevMonth()\">&nbsp;&laquo;&nbsp;</a></TD>";
		doc += "<TD CLASS='calmonth'>";
		switch(this.monthSelect)
		{
		case MonthSelectNone:
			break;
			
		case MonthSelectStart:
			doc += "<A HREF=\"javascript:" + 
					this.varName + ".select('" + this.getDateForDay(1) + "')\">";
			break;
			
		case MonthSelectEnd:
			doc += "<A HREF=\"javascript:" + 
					this.varName + ".select('" + this.getDateForDay(this.getDaysInMonth(this.month, this.year)) + "')\">";
			break;
		}
		
		doc += this.months[this.month] + " " + this.year;
		
		if (this.monthSelect == MonthSelectStart ||
			this.monthSelect == MonthSelectEnd)
		{
			doc += "</A>";
		}	
		
		doc += "</TD>";
		doc += "<TD CLASS='calnav'><A HREF=\"javascript:" + this.varName + ".nextMonth()\">&nbsp;&raquo;&nbsp;</a></TD>";
		doc += "</TR></TABLE>";
		
		doc += "<TR>";
		
		for(i = 0; i < 7; ++i)
		{
			doc += "<TH>";
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
		
		doc += "<TR>";
		
		for(i = 0; i < startDay; ++i)
		{
			doc += "<TD class='calday'>&nbsp;</TD>";
			++counter;
		}
		
		while(day <= limit)
		{
			if ((counter % 7) == 0)
			{
				doc += "</TR><TR>";
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
			doc  += "<TD class='" + cellStyle + "'><A HREF=\"javascript:" + 
					this.varName + ".select('" + this.getDateForDay(day) + "')\">" + day + "</A></TD>";
			++day;
		}
		
		doc += "</TR></TABLE>";
				
		//alert(doc);
		return doc;
	}
	
	function bind()
	{
		this.div = document.getElementById(this.divID);
		this.shim = document.getElementById(this.divID + "Shim");
		
		this.form    = document.forms[formName];
		
		if (!this.form)
		{
			alert("Cannot find form");
			return;
		}
			
		this.control = document.forms[formName][ctrlName];
		
		if (!this.control)
		{
			alert("Cannot find control");
		}
	
		this.bindDate();
	}	
	
	function bindDate()
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
	}

	function show(parent)
	{
		if (!this.form) this.bind();
		this.draw();
		
		var top = getOffsetTop(parent);	
		var left =  getOffsetLeft(parent) + parent.offsetWidth;
		
		this.div.style.position="absolute";
		this.div.style.visibility="visible";
		this.div.style.left = left + "px";
		this.div.style.top = top + "px";
		
		if (this.shim)
		{
			this.shim.style.width = this.div.offsetWidth;
			this.shim.style.height = this.div.offsetHeight;
			this.shim.style.top = this.div.style.top;
			this.shim.style.left = this.div.style.left;
			this.shim.style.zIndex =this.div.style.zIndex - 1;
			
			this.shim.style.display = "block";
		}
	}
	
	
	function hide()
	{
		if (!this.form) this.bind();
		this.div.style.position="absolute";
		this.div.style.visibility = "hidden";
		
		if (this.shim)
		{
			this.shim.style.display = "none";
		}
	}
	
	function toggle(parent)
	{
		if (!this.form) this.bind();
		

		if (this.div.style.visibility == "visible")
		{
			this.hide();
		}
		else
		{
			this.bindDate();
			this.show(parent);
		}
	}	
			
	function draw()
	{
		this.div.innerHTML= this.drawCalendar();
	}
	
	function prevMonth()
	{
		this.month--;
		if (this.month < 0)
		{
			this.month = 11;
			this.year--;
		}
		
		this.draw();
	}

	function nextMonth()
	{
		this.month++;
		if (this.month > 11)
		{
			this.month = 0;
			this.year++;
		}
		
		this.draw();
	}

	function select(date)
	{
		this.control.value = date;
		this.bindDate();
		this.onDateChanged(date);
		this.draw();
	}
	
	function setMonthSelectMode(mode)
	{
		this.monthSelect = mode;
	}
		
	function onMouseOut(event)
	{
		var top = getOffsetTop(this.div);
		var left = getOffsetLeft(this.div);
		var width = this.div.offsetWidth;
		var height = this.div.offsetHeight;
		
		var ie;
		
		if (!event.pageX) ie = true;
		
		var x = (event.pageX > 0) ? event.pageX : event.x + scrollLeft();
		var y = (event.pageY > 0) ? event.pageY : event.y + scrollTop();
		
		window.status = (left  +"," + top +" " + width + "x" + height + ": " + x + ", " + y);
		
		// Fudge for IE's bad borders
		if (ie)
		{
			left++;
			top++;
			width--;
			height--;
		}
		
		if (x <= left || x >= left + width || y <= top || y >= top + height)
		{
			this.hide();
		}
	}
	
	function onDateChanged(date)
	{
	}
}

function getOffsetLeft (el) 
{
  	var ol = el.offsetLeft;
  	while ((el = el.offsetParent) != null)
	{
    	ol += el.offsetLeft;
    }
  
  	return ol;
}

function getOffsetTop (el) 
{
  	var ot = el.offsetTop;
  	while((el = el.offsetParent) != null)
  	{
   		ot += el.offsetTop;
   	}
   	
	return ot;
}

function scrollTop()
{
	if (document.documentElement && document.documentElement.scrollTop)
		theTop = document.documentElement.scrollTop;
	else if (document.body)
		theTop = document.body.scrollTop;
		
	return theTop;
}

function scrollLeft()
{
	if (document.documentElement && document.documentElement.scrollLeft)
		theTop = document.documentElement.scrollLeft;
	else if (document.body)
		theTop = document.body.scrollLeft;
		
	return theTop;
}