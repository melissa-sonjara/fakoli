var ReportManager = new Class(
{
	
});

ReportManager.updateFilters = function()
{
	var table = $('custom_report_table');
	
	var cboxes = table.getElements("input");
	
	cboxes.each(function(c) 
	{ 
		if (c.name.indexOf("table_") == 0)
		{
			var fieldset = c.value;
			if (c.checked)
			{
				$(fieldset).setStyle('display', 'block');
			}
			else
			{
				$(fieldset).setStyle('display', 'none');
			}
		}
	});
};