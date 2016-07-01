/**
 * Spreadsheet Form Manager class 
 */
var SpreadsheetFormManager = new Class({
	Implements:  [Options, Events],
	id:			 null,
	spreadsheet: Class.Empty,

	options:
	{
		onFormChanged: function () {}
	},
	
	initialize: function(id, options)
	{
		this.setOptions(options);
		this.id = id;
		this.spreadsheet = document.id(id);
		this.spreadsheet.getElements("input").each(function(elt)
		{
			elt.addEvent('change', function() { this.fireEvent('formChanged'); }.bind(this));
		}.bind(this));
	},
	
	getData: function()
	{
		var rawData = this.spreadsheet.toQueryString().parseQueryString();
		
		//TODO: parse out into records
		
		var data = [];
		var regex = new RegExp(this.id + '_(\\d+)__(.*)');
		Object.keys(rawData).each(function(key)
		{
			var match = regex.exec(key);
			if (match == null) return;
			var idx = match[1];
			var field = match[2];
			
			if (data[idx] === undefined)
			{
				data[idx] = {};
			}
			
			data[idx][field] = rawData[key];
		});
		
		return data;	
	}
});