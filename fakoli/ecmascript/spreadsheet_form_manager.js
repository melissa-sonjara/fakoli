// Requires ui.js for EventEmitter, $el.

/**
 * Manages a spreadsheet-style inline form, collecting structured row data
 * from indexed field names and firing a formChanged event on input changes.
 */
class SpreadsheetFormManager extends EventEmitter
{
	constructor(id, options)
	{
		super();
		this.id = id;
		this.options = Object.assign({
			onFormChanged: function() {}
		}, options || {});

		this.spreadsheet = $el(id);

		var self = this;
		Array.from(this.spreadsheet.querySelectorAll("input")).forEach(function(elt)
		{
			elt.addEventListener('change', function() { self.fireEvent('formChanged'); });
		});
	}

	getData()
	{
		var formData = new FormData(this.spreadsheet.closest ? this.spreadsheet.closest('form') : this.spreadsheet);
		var rawData  = {};

		// Convert FormData to plain object
		for (var pair of formData.entries())
		{
			rawData[pair[0]] = pair[1];
		}

		var data  = [];
		var regex = new RegExp(this.id + '_(\\d+)__(.*)');

		Object.keys(rawData).forEach(function(key)
		{
			var mch = regex.exec(key);
			if (mch == null) return;
			var idx   = mch[1];
			var field = mch[2];
			if (data[idx] === undefined) data[idx] = {};
			data[idx][field] = rawData[key];
		});

		return data;
	}

	showError(error)
	{
		var el = $el(this.id + "__error");
		if (el)
		{
			el.innerHTML = error;
			el.style.display = 'table-cell';
		}
	}
}
