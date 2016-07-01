/**
 * Spreadsheet Form Manager class 
 */
var SpreadsheetFormManager = new Class({
	Implements: [Options, Events],
	spreadsheet: Class.Empty,

	options:
	{
	},
	
	initialize: function(id, options)
	{
		this.setOptions(options);
		this.spreadsheet = document.id(id);
	},
	
	getData: function()
	{
		var rawData = this.spreadsheet.toQueryString().parseQueryString();
		
		//TODO: parse out into records
		
		return rawData;
	}
});