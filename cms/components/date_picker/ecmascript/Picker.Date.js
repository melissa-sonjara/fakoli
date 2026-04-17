/*
---
name: Picker.Date
description: Creates a DatePicker, can be used for picking years/months/days and time, or all of them
authors: Arian Stolwijk
requires: [Picker, Picker.Attach, Locale.en-US.DatePicker, More/Locale, More/Date]
provides: Picker.Date
...
*/


(function()
{

class PickerDate extends PickerAttach
{
	constructor(attachTo, options)
	{
		var mergedOptions = Object.assign({
			/*
			onSelect: function(date){},

			minDate: new Date('3/4/2010'), // Date object or a string
			maxDate: new Date('3/4/2011'), // same as minDate
			availableDates: {}, //
			invertAvailable: false,

			format: null,*/

			timePicker: false,
			timePickerOnly: false, // deprecated, use onlyView = 'time'
			timeWheelStep: 1, // 10,15,20,30

			yearPicker: true,
			yearsPerPage: 20,

			startDay: 1, // Sunday (0) through Saturday (6) - be aware that this may affect your layout, since the days on the right might have a different margin
			rtl: false,

			startView: 'days', // allowed values: {time, days, months, years}
			openLastView: false,
			pickOnly: false, // 'years', 'months', 'days', 'time'
			canAlwaysGoUp: ['months', 'days'],
			updateAll: false, //whether or not to update all inputs when selecting a date

			weeknumbers: false,

			// if you like to use your own translations
			months_abbr: null,
			days_abbr: null,
			years_title: function(date, options)
			{
				var year = date.getFullYear();
				return year + '-' + (year + options.yearsPerPage - 1);
			},
			months_title: function(date, options)
			{
				return date.getFullYear();
			},
			days_title: function(date, options)
			{
				return dateFormat(date, '%b %Y');
			},
			time_title: function(date, options)
			{
				return (options.pickOnly == 'time') ? Locale.get('DatePicker.select_a_time') : dateFormat(date, '%d %B, %Y');
			}
		}, options);

		super(attachTo, mergedOptions);

		options = this.options;

		// If we only want to use one picker / backwards compatibility
		['year', 'month', 'day', 'time'].some(function(what)
		{
			if (options[what + 'PickerOnly'])
			{
				options.pickOnly = what;
				return true;
			}
			return false;
		});
		if (options.pickOnly)
		{
			options[options.pickOnly + 'Picker'] = true;
			options.startView = options.pickOnly;
		}

		// backward compatibility for startView
		var newViews = ['days', 'months', 'years'];
		['month', 'year', 'decades'].some(function(what, i)
		{
			return (options.startView == what) && (options.startView = newViews[i]);
		});

		options.canAlwaysGoUp = options.canAlwaysGoUp ? [].concat(options.canAlwaysGoUp) : [];

		// Set the min and max dates as Date objects
		if (options.minDate)
		{
			if (!(options.minDate instanceof Date)) options.minDate = new Date(options.minDate);
			clearTime(options.minDate);
		}
		if (options.maxDate)
		{
			if (!(options.maxDate instanceof Date)) options.maxDate = new Date(options.maxDate);
			clearTime(options.maxDate);
		}

		if (!options.format)
		{
			options.format = (options.pickOnly != 'time') ? Locale.get('Date.shortDate') : '';
			if (options.timePicker) options.format = (options.format) + (options.format ? ' ' : '') + Locale.get('Date.shortTime');
		}

		// Some link or input has fired an event!
		this.addEvent('attached', function(event, element)
		{
			// This is where we store the selected date
			if (!this.currentView || !options.openLastView) this.currentView = options.startView;

			this.date = limitDate(new Date(), options.minDate, options.maxDate);
			var tag = element.tagName.toLowerCase();
			var input;
			if (tag == 'input') input = element;
			else
			{
				var index = this.toggles.indexOf(element);
				if (this.inputs[index]) input = this.inputs[index];
			}
			this.getInputDate(input);
			this.input = input;
			this.setColumns(this.originalColumns);
		}.bind(this), true);
	}

	getInputDate(input)
	{
		this.date = new Date();
		if (!input) return;
		var date = parseDateString(input.value);
		if (date == null || !isValidDate(date))
		{
			var storeDate = input._datepickerValue;
			if (storeDate) date = parseDateString(storeDate);
		}
		if (date != null && isValidDate(date)) this.date = date;
	}

	// Control the previous and next elements

	constructPicker()
	{
		super.constructPicker();

		if (!this.options.rtl)
		{
			var previous = this.previous = document.createElement('div');
			previous.className = 'previous';
			previous.innerHTML = '&#171;';
			this.header.appendChild(previous);

			var next = this.next = document.createElement('div');
			next.className = 'next';
			next.innerHTML = '&#187;';
			this.header.appendChild(next);
		}
		else
		{
			var next = this.next = document.createElement('div');
			next.className = 'previous';
			next.innerHTML = '&#171;';
			this.header.appendChild(next);

			var previous = this.previous = document.createElement('div');
			previous.className = 'next';
			previous.innerHTML = '&#187;';
			this.header.appendChild(previous);
		}
	}

	hidePrevious(_next, _show)
	{
		this[_next ? 'next' : 'previous'].style.display = _show ? 'block' : 'none';
		return this;
	}

	showPrevious(_next)
	{
		return this.hidePrevious(_next, true);
	}

	setPreviousEvent(fn, _next)
	{
		var el = this[_next ? 'next' : 'previous'];
		if (el._prevEventFn)
		{
			el.removeEventListener('click', el._prevEventFn);
			el._prevEventFn = null;
		}
		if (fn)
		{
			el._prevEventFn = fn;
			el.addEventListener('click', fn);
		}
		return this;
	}

	hideNext()
	{
		return this.hidePrevious(true);
	}

	showNext()
	{
		return this.showPrevious(true);
	}

	setNextEvent(fn)
	{
		return this.setPreviousEvent(fn, true);
	}

	setColumns(columns, view, date, viewFx)
	{
		var ret = super.setColumns(columns);
		var method;

		if ((view || this.currentView)
			&& (method = 'render' + capitalize(view || this.currentView))
			&& this[method]
		) this[method](date || cloneDate(this.date), viewFx);

		return ret;
	}

	// Render the Pickers

	renderYears(date, fx)
	{
		var options = this.options;
		var pages = options.columns;
		var perPage = options.yearsPerPage;
		var _columns = [];
		var _dates = [];
		this.dateElements = [];

		// start neatly at interval (eg. 1980 instead of 1987)
		date = cloneDate(date);
		decrementDate(date, 'year', date.getFullYear() % perPage);

		var iterateDate = cloneDate(date);
		decrementDate(iterateDate, 'year', Math.floor((pages - 1) / 2) * perPage);

		for (var i = pages; i--;)
		{
			var _date = cloneDate(iterateDate);
			_dates.push(_date);
			_columns.push(renderers.years(
				timesSelectors.years(options, cloneDate(_date)),
				options,
				cloneDate(this.date),
				this.dateElements,
				function(date)
				{
					if (options.pickOnly == 'years') this.select(date);
					else this.renderMonths(date, 'fade');
					this.date = date;
				}.bind(this)
			));
			incrementDate(iterateDate, 'year', perPage);
		}

		this.setColumnsContent(_columns, fx);
		this.setTitle(_dates, options.years_title);

		// Set limits
		var limitLeft = (options.minDate && date.getFullYear() <= options.minDate.getFullYear());
		var limitRight = (options.maxDate && (date.getFullYear() + options.yearsPerPage) >= options.maxDate.getFullYear());
		this[(limitLeft ? 'hide' : 'show') + 'Previous']();
		this[(limitRight ? 'hide' : 'show') + 'Next']();

		this.setPreviousEvent(function()
		{
			this.renderYears(decrementDate(date, 'year', perPage), 'left');
		}.bind(this));

		this.setNextEvent(function()
		{
			this.renderYears(incrementDate(date, 'year', perPage), 'right');
		}.bind(this));

		// We can't go up!
		this.setTitleEvent(null);

		this.currentView = 'years';
	}

	renderMonths(date, fx)
	{
		var options = this.options;
		var years = options.columns;
		var _columns = [];
		var _dates = [];
		var iterateDate = cloneDate(date);
		decrementDate(iterateDate, 'year', Math.floor((years - 1) / 2));
		this.dateElements = [];

		for (var i = years; i--;)
		{
			var _date = cloneDate(iterateDate);
			_dates.push(_date);
			_columns.push(renderers.months(
				timesSelectors.months(options, cloneDate(_date)),
				options,
				cloneDate(this.date),
				this.dateElements,
				function(date)
				{
					if (options.pickOnly == 'months') this.select(date);
					else this.renderDays(date, 'fade');
					this.date = date;
				}.bind(this)
			));
			incrementDate(iterateDate, 'year', 1);
		}

		this.setColumnsContent(_columns, fx);
		this.setTitle(_dates, options.months_title);

		// Set limits
		var year = date.getFullYear();
		var limitLeft = (options.minDate && year <= options.minDate.getFullYear());
		var limitRight = (options.maxDate && year >= options.maxDate.getFullYear());
		this[(limitLeft ? 'hide' : 'show') + 'Previous']();
		this[(limitRight ? 'hide' : 'show') + 'Next']();

		this.setPreviousEvent(function()
		{
			this.renderMonths(decrementDate(date, 'year', years), 'left');
		}.bind(this));

		this.setNextEvent(function()
		{
			this.renderMonths(incrementDate(date, 'year', years), 'right');
		}.bind(this));

		var canGoUp = options.yearPicker && (options.pickOnly != 'months' || options.canAlwaysGoUp.indexOf('months') >= 0);
		var titleEvent = (canGoUp) ? function()
		{
			this.renderYears(date, 'fade');
		}.bind(this) : null;
		this.setTitleEvent(titleEvent);

		this.currentView = 'months';
	}

	renderDays(date, fx)
	{
		var options = this.options;
		var months = options.columns;
		var _columns = [];
		var _dates = [];
		var iterateDate = cloneDate(date);
		decrementDate(iterateDate, 'month', Math.floor((months - 1) / 2));
		this.dateElements = [];

		for (var i = months; i--;)
		{
			var _date = cloneDate(iterateDate);
			_dates.push(_date);
			_columns.push(renderers.days(
				timesSelectors.days(options, cloneDate(_date)),
				options,
				cloneDate(this.date),
				this.dateElements,
				function(date)
				{
					if (options.pickOnly == 'days' || !options.timePicker) this.select(date);
					else this.renderTime(date, 'fade');
					this.date = date;
				}.bind(this),
				this.titleID
			));
			incrementDate(iterateDate, 'month', 1);
		}

		this.setColumnsContent(_columns, fx);
		this.setTitle(_dates, options.days_title);

		var yearmonth = parseInt(dateFormat(date, '%Y%m'), 10);
		var limitLeft = (options.minDate && yearmonth <= parseInt(dateFormat(options.minDate, '%Y%m'), 10));
		var limitRight = (options.maxDate && yearmonth >= parseInt(dateFormat(options.maxDate, '%Y%m'), 10));
		this[(limitLeft ? 'hide' : 'show') + 'Previous']();
		this[(limitRight ? 'hide' : 'show') + 'Next']();

		this.setPreviousEvent(function()
		{
			this.renderDays(decrementDate(date, 'month', months), 'left');
		}.bind(this));

		this.setNextEvent(function()
		{
			this.renderDays(incrementDate(date, 'month', months), 'right');
		}.bind(this));

		var canGoUp = options.pickOnly != 'days' || options.canAlwaysGoUp.indexOf('days') >= 0;
		var titleEvent = (canGoUp) ? function()
		{
			this.renderMonths(date, 'fade');
		}.bind(this) : null;
		this.setTitleEvent(titleEvent);

		this.currentView = 'days';
	}

	renderTime(date, fx)
	{
		var options = this.options;
		this.setTitle(date, options.time_title);

		var originalColumns = this.originalColumns = options.columns;
		this.currentView = null; // otherwise you'd get crazy recursion
		if (originalColumns != 1) this.setColumns(1);

		this.setContent(renderers.time(
			options,
			cloneDate(date),
			function(date)
			{
				this.select(date);
			}.bind(this)
		), fx);

		// Hide « and » buttons
		this.hidePrevious()
			.hideNext()
			.setPreviousEvent(null)
			.setNextEvent(null);

		var canGoUp = options.pickOnly != 'time' || options.canAlwaysGoUp.indexOf('time') >= 0;
		var titleEvent = (canGoUp) ? function()
		{
			this.setColumns(originalColumns, 'days', date, 'fade');
		}.bind(this) : null;
		this.setTitleEvent(titleEvent);

		this.currentView = 'time';
	}

	select(date, all)
	{
		this.date = date;
		var formatted = dateFormat(date, this.options.format);
		var time = date.toISOString();
		var inputs = (!this.options.updateAll && !all && this.input) ? [this.input] : this.inputs;

		inputs.forEach(function(input)
		{
			input.value = formatted;
			input._datepickerValue = time;
			input.dispatchEvent(new Event('change', {bubbles: true}));
		});

		this.fireEvent('select', [date].concat(inputs));
		this.close();
		return this;
	}
}


// Renderers only output elements and calculate the limits!

var timesSelectors = {

	years: function(options, date)
	{
		var times = [];
		for (var i = 0; i < options.yearsPerPage; i++)
		{
			times.push(+date);
			incrementDate(date, 'year', 1);
		}
		return times;
	},

	months: function(options, date)
	{
		var times = [];
		date.setMonth(0);
		for (var i = 0; i <= 11; i++)
		{
			times.push(+date);
			incrementDate(date, 'month', 1);
		}
		return times;
	},

	days: function(options, date)
	{
		var times = [];
		date.setDate(1);
		while (date.getDay() != options.startDay) date.setDate(date.getDate() - 1);
		for (var i = 0; i < 42; i++)
		{
			times.push(+date);
			incrementDate(date, 'day', 1);
		}
		return times;
	}

};

var renderers = {

	years: function(years, options, currentDate, dateElements, fn)
	{
		var container = document.createElement('table');
		container.className = 'years';
		var today = new Date();
		var rows = [];
		var element, classes;

		years.forEach(function(_year, i)
		{
			var date = new Date(_year);
			var year = date.getFullYear();
			if (i % 4 === 0)
			{
				var tr = document.createElement('tr');
				rows.push(tr);
				container.appendChild(tr);
			}
			classes = 'year year' + i;
			if (year == today.getFullYear()) classes += ' today';
			if (year == currentDate.getFullYear()) classes += ' selected';
			element = document.createElement('td');
			element.className = classes;
			element.textContent = year;
			rows[rows.length - 1].appendChild(element);

			dateElements.push({element: element, time: _year});

			if (isUnavailable('year', date, options)) element.classList.add('unavailable');
			else element.addEventListener('click', fn.bind(null, date));
		});

		return container;
	},

	months: function(months, options, currentDate, dateElements, fn)
	{
		var today = new Date();
		var month = today.getMonth();
		var thisyear = today.getFullYear();
		var selectedyear = currentDate.getFullYear();
		var container = document.createElement('table');
		container.className = 'months';
		var monthsAbbr = options.months_abbr || Locale.get('Date.months_abbr');
		var rows = [];
		var element, classes;

		months.forEach(function(_month, i)
		{
			var date = new Date(_month);
			var year = date.getFullYear();
			if (i % 3 === 0)
			{
				var tr = document.createElement('tr');
				rows.push(tr);
				container.appendChild(tr);
			}

			classes = 'month month' + (i + 1);
			if (i == month && year == thisyear) classes += ' today';
			if (i == currentDate.getMonth() && year == selectedyear) classes += ' selected';
			element = document.createElement('td');
			element.className = classes;
			element.textContent = monthsAbbr[i];
			rows[rows.length - 1].appendChild(element);
			dateElements.push({element: element, time: _month});

			if (isUnavailable('month', date, options)) element.classList.add('unavailable');
			else element.addEventListener('click', fn.bind(null, date));
		});

		return container;
	},

	days: function(days, options, currentDate, dateElements, fn, titleID)
	{
		var month = new Date(days[14]).getMonth();
		var todayString = new Date().toDateString();
		var currentString = currentDate.toDateString();
		var weeknumbers = options.weeknumbers;
		var container = document.createElement('table');
		container.className = 'days' + (weeknumbers ? ' weeknumbers' : '');
		container.setAttribute('role', 'grid');
		if (titleID) container.setAttribute('aria-labelledby', titleID);
		var header = document.createElement('thead');
		container.appendChild(header);
		var body = document.createElement('tbody');
		container.appendChild(body);
		var titles = document.createElement('tr');
		titles.className = 'titles';
		header.appendChild(titles);
		var localeDaysShort = options.days_abbr || Locale.get('Date.days_abbr');
		var day, classes, element, weekcontainer, dateString;
		var where = options.rtl ? 'top' : 'bottom';

		if (weeknumbers)
		{
			var wkTh = document.createElement('th');
			wkTh.className = 'title day weeknumber';
			wkTh.textContent = Locale.get('DatePicker.week');
			titles.appendChild(wkTh);
		}

		for (day = options.startDay; day < (options.startDay + 7); day++)
		{
			var th = document.createElement('th');
			th.className = 'title day day' + (day % 7);
			th.textContent = localeDaysShort[(day % 7)];
			th.setAttribute('role', 'columnheader');
			if (where === 'top')
			{
				titles.insertBefore(th, titles.firstChild);
			}
			else
			{
				titles.appendChild(th);
			}
		}

		days.forEach(function(_date, i)
		{
			var date = new Date(_date);

			if (i % 7 == 0)
			{
				weekcontainer = document.createElement('tr');
				weekcontainer.className = 'week week' + Math.floor(i / 7);
				weekcontainer.setAttribute('role', 'row');
				body.appendChild(weekcontainer);
				if (weeknumbers)
				{
					var wkTh = document.createElement('th');
					wkTh.className = 'day weeknumber';
					wkTh.textContent = getWeekNumber(date);
					wkTh.setAttribute('scope', 'row');
					wkTh.setAttribute('role', 'rowheader');
					weekcontainer.appendChild(wkTh);
				}
			}

			dateString = date.toDateString();
			classes = 'day day' + date.getDay();
			if (dateString == todayString) classes += ' today';
			if (date.getMonth() != month) classes += ' otherMonth';
			element = document.createElement('td');
			element.className = classes;
			element.textContent = date.getDate();
			element.setAttribute('role', 'gridcell');

			if (where === 'top')
			{
				weekcontainer.insertBefore(element, weekcontainer.firstChild);
			}
			else
			{
				weekcontainer.appendChild(element);
			}

			if (dateString == currentString)
			{
				element.classList.add('selected');
				element.setAttribute('aria-selected', 'true');
			}
			else
			{
				element.setAttribute('aria-selected', 'false');
			}

			dateElements.push({element: element, time: _date});

			if (isUnavailable('date', date, options)) element.classList.add('unavailable');
			else element.addEventListener('click', fn.bind(null, cloneDate(date)));
		});

		return container;
	},

	time: function(options, date, fn)
	{
		var container = document.createElement('div');
		container.className = 'time';

		// make sure that the minutes are timeWheelStep * k
		var initMinutes = Math.round(date.getMinutes() / options.timeWheelStep) * options.timeWheelStep;

		if (initMinutes >= 60) initMinutes = 0;
		date.setMinutes(initMinutes);

		var hoursInput = document.createElement('input');
		hoursInput.className = 'hour';
		hoursInput.type = 'text';
		hoursInput.title = Locale.get('DatePicker.use_mouse_wheel');
		hoursInput.value = padZero(date.getHours());
		hoursInput.maxLength = 2;
		hoursInput.addEventListener('click', function(event)
		{
			event.target.focus();
			event.stopPropagation();
		});
		hoursInput.addEventListener('wheel', function(event)
		{
			event.stopPropagation();
			event.preventDefault();
			hoursInput.focus();
			var value = parseInt(hoursInput.value, 10);
			value = (event.deltaY < 0) ? ((value < 23) ? value + 1 : 0)
				: ((value > 0) ? value - 1 : 23);
			date.setHours(value);
			hoursInput.value = padZero(date.getHours());
		});
		container.appendChild(hoursInput);

		var separator = document.createElement('div');
		separator.className = 'separator';
		separator.textContent = ':';
		container.appendChild(separator);

		var minutesInput = document.createElement('input');
		minutesInput.className = 'minutes';
		minutesInput.type = 'text';
		minutesInput.title = Locale.get('DatePicker.use_mouse_wheel');
		minutesInput.value = padZero(date.getMinutes());
		minutesInput.maxLength = 2;
		minutesInput.addEventListener('click', function(event)
		{
			event.target.focus();
			event.stopPropagation();
		});
		minutesInput.addEventListener('wheel', function(event)
		{
			event.stopPropagation();
			event.preventDefault();
			minutesInput.focus();
			var value = parseInt(minutesInput.value, 10);
			value = (event.deltaY < 0) ? ((value < 59) ? (value + options.timeWheelStep) : 0)
				: ((value > 0) ? (value - options.timeWheelStep) : (60 - options.timeWheelStep));
			if (value >= 60) value = 0;
			date.setMinutes(value);
			minutesInput.value = padZero(date.getMinutes());
		});
		container.appendChild(minutesInput);

		var okButton = document.createElement('input');
		okButton.className = 'ok';
		okButton.type = 'submit';
		okButton.value = Locale.get('DatePicker.time_confirm_button');
		okButton.addEventListener('click', function(event)
		{
			event.stopPropagation();
			date.setHours(parseInt(hoursInput.value, 10));
			date.setMinutes(parseInt(minutesInput.value, 10));
			fn(cloneDate(date));
		});
		container.appendChild(okButton);

		return container;
	}

};


PickerDate.defineRenderer = function(name, fn)
{
	renderers[name] = fn;
	return this;
};

PickerDate.getRenderer = function(name)
{
	return renderers[name];
};

var limitDate = function(date, min, max)
{
	if (min && date < min) return min;
	if (max && date > max) return max;
	return date;
};

var isUnavailable = function(type, date, options)
{
	var minDate = options.minDate;
	var maxDate = options.maxDate;
	var availableDates = options.availableDates;
	var year, month, day, ms;

	if (!minDate && !maxDate && !availableDates) return false;
	clearTime(date);

	if (type == 'year')
	{
		year = date.getFullYear();
		return (
			(minDate && year < minDate.getFullYear()) ||
			(maxDate && year > maxDate.getFullYear()) ||
			(
				(availableDates != null && !options.invertAvailable) && (
					availableDates[year] == null ||
					Object.keys(availableDates[year]).length == 0 ||
					Object.keys(availableDates[year]).filter(function(k)
					{
						return (availableDates[year][k].length > 0);
					}).length == 0
				)
			)
		);
	}

	if (type == 'month')
	{
		year = date.getFullYear();
		month = date.getMonth() + 1;
		ms = parseInt(dateFormat(date, '%Y%m'), 10);
		return (
			(minDate && ms < parseInt(dateFormat(minDate, '%Y%m'), 10)) ||
			(maxDate && ms > parseInt(dateFormat(maxDate, '%Y%m'), 10)) ||
			(
				(availableDates != null && !options.invertAvailable) && (
					availableDates[year] == null ||
					availableDates[year][month] == null ||
					availableDates[year][month].length == 0
				)
			)
		);
	}

	// type == 'date'
	year = date.getFullYear();
	month = date.getMonth() + 1;
	day = date.getDate();

	var dateAllow = (minDate && date < minDate) || (maxDate && date > maxDate);
	if (availableDates != null)
	{
		dateAllow = dateAllow
			|| availableDates[year] == null
			|| availableDates[year][month] == null
			|| availableDates[year][month].indexOf(day) < 0;
		if (options.invertAvailable) dateAllow = !dateAllow;
	}

	return dateAllow;
};


// ── Date utility helpers ───────────────────────────────────────────────────

function cloneDate(date)
{
	return new Date(date.getTime());
}

function clearTime(date)
{
	date.setHours(0, 0, 0, 0);
	return date;
}

function incrementDate(date, unit, amount)
{
	if (unit === 'year')
	{
		date.setFullYear(date.getFullYear() + amount);
	}
	else if (unit === 'month')
	{
		date.setMonth(date.getMonth() + amount);
	}
	else if (unit === 'day')
	{
		date.setDate(date.getDate() + amount);
	}
	return date;
}

function decrementDate(date, unit, amount)
{
	return incrementDate(date, unit, -amount);
}

function padZero(n)
{
	return (n < 10 ? '0' : '') + n;
}

function capitalize(str)
{
	if (!str) return str;
	return str.charAt(0).toUpperCase() + str.slice(1);
}

function getWeekNumber(date)
{
	var d = cloneDate(date);
	d.setHours(0, 0, 0, 0);
	d.setDate(d.getDate() + 4 - (d.getDay() || 7));
	var yearStart = new Date(d.getFullYear(), 0, 1);
	return Math.ceil((((d - yearStart) / 86400000) + 1) / 7);
}

/**
 * Simple strftime-style date formatter.
 * Supports: %Y, %m, %d, %H, %M, %b, %B
 */
function dateFormat(date, fmt)
{
	var monthsAbbr = Locale.get('Date.months_abbr') || ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
	var months     = Locale.get('Date.months')      || ['January','February','March','April','May','June','July','August','September','October','November','December'];
	return fmt
		.replace('%Y', date.getFullYear())
		.replace('%m', padZero(date.getMonth() + 1))
		.replace('%d', padZero(date.getDate()))
		.replace('%H', padZero(date.getHours()))
		.replace('%M', padZero(date.getMinutes()))
		.replace('%b', monthsAbbr[date.getMonth()])
		.replace('%B', months[date.getMonth()]);
}

/**
 * Parse a date string into a Date object, returning null on failure.
 */
function parseDateString(str)
{
	if (!str) return null;
	var d = new Date(str);
	return isNaN(d.getTime()) ? null : d;
}

function isValidDate(date)
{
	return date instanceof Date && !isNaN(date.getTime());
}


// Expose on global scope
window.DatePicker = Picker.Date = PickerDate;

})();
