/*
---
name: Picker.Date.Range
description: Select a Range of Dates
authors: Arian Stolwijk
requires: [Picker, Picker.Date]
provides: Picker.Date.Range
...
*/

class PickerDateRange extends PickerDate
{
	constructor(attachTo, options)
	{
		var mergedOptions = Object.assign({
			getStartEndDate: function(input)
			{
				return input.value.split('-').map(function(dateStr)
				{
					var parsed = new Date(dateStr.trim());
					return isValidDate(parsed) ? parsed : null;
				}).filter(function(d) { return d !== null; });
			},
			setStartEndDate: function(input, dates)
			{
				input.value = dates.map(function(date)
				{
					return dateFormat(date, this.options.format);
				}.bind(this)).join(' - ');
			},
			footer: true,
			columns: 3
		}, options);

		super(attachTo, mergedOptions);
	}

	getInputDate(input)
	{
		if (!input) return;

		var dates = input._datepickerValue;
		if (dates && dates.length)
		{
			dates = [].concat(dates).map(function(d)
			{
				return new Date(d);
			});
		}
		if (!dates || !dates.length || dates.some(function(date)
		{
			return !isValidDate(date);
		}))
		{
			dates = this.options.getStartEndDate.call(this, input);
			if (!dates.length || !dates.every(function(date)
			{
				return isValidDate(date);
			})) dates = [this.date];
		}
		if (dates.length == 1) this.date = this.startDate = this.endDate = dates[0];
		else if (dates.length == 2)
		{
			this.date = this.startDate = dates[0];
			this.endDate = dates[1];
		}
	}

	constructPicker()
	{
		super.constructPicker();
		var footer = this.footer;
		var self = this;
		if (!footer) return;

		var events = {
			click: function()
			{
				this.focus();
			},
			blur: function()
			{
				var date = new Date(this.value);
				if (isValidDate(date)) self[(this === startInput ? 'start' : 'end') + 'Date'] = date;
				self.updateRangeSelection();
			},
			keydown: function(event)
			{
				if (event.key === 'Enter') self.selectRange();
			}
		};

		var startInput = this.startInput = document.createElement('input');
		Object.keys(events).forEach(function(ev)
		{
			startInput.addEventListener(ev, events[ev]);
		});
		footer.appendChild(startInput);

		var span = document.createElement('span');
		span.textContent = ' - ';
		footer.appendChild(span);

		var endInput = this.endInput = document.createElement('input');
		Object.keys(events).forEach(function(ev)
		{
			endInput.addEventListener(ev, events[ev]);
		});
		footer.appendChild(endInput);

		var applyButton = this.applyButton = document.createElement('button');
		applyButton.className = 'apply';
		applyButton.textContent = Locale.get('DatePicker.apply_range');
		applyButton.addEventListener('click', function()
		{
			self.selectRange();
		});
		footer.appendChild(applyButton);

		var cancelButton = this.cancelButton = document.createElement('button');
		cancelButton.className = 'cancel';
		cancelButton.textContent = Locale.get('DatePicker.cancel');
		cancelButton.addEventListener('click', function()
		{
			self.close(false);
		});
		footer.appendChild(cancelButton);
	}

	renderDays()
	{
		super.renderDays.apply(this, arguments);
		this.updateRangeSelection();
	}

	select(date)
	{
		if (this.startDate && (this.endDate == this.startDate || date > this.endDate) && date >= this.startDate) this.endDate = date;
		else
		{
			this.startDate = date;
			this.endDate = date;
		}
		this.updateRangeSelection();
	}

	selectRange()
	{
		this.date = this.startDate;
		var dates = [this.startDate, this.endDate];
		var input = this.input;

		this.options.setStartEndDate.call(this, input, dates);
		input._datepickerValue = dates.map(function(date)
		{
			return date.toISOString();
		});
		input.dispatchEvent(new Event('change', {bubbles: true}));

		this.fireEvent('select', [dates, input]);
		this.close();
		return this;
	}

	updateRangeSelection()
	{
		var start = this.startDate;
		var end = this.endDate || start;

		if (this.dateElements)
		{
			for (var i = this.dateElements.length; i--;)
			{
				var el = this.dateElements[i];
				if (el.time >= start && el.time <= end) el.element.classList.add('selected');
				else el.element.classList.remove('selected');
			}
		}

		var formattedFirst = dateFormat(start, this.options.format);
		var formattedEnd = dateFormat(end, this.options.format);

		this.startInput.value = formattedFirst;
		this.endInput.value = formattedEnd;

		return this;
	}
}

// Expose
Picker.Date.Range = PickerDateRange;
