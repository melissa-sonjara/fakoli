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
		else if (c.name.indexOf("column_") == 0)
		{
			if (c.checked)
			{
				var div = findAncestor(c, "div").getParent();
				var containerID = div.id.replace("_contents", "");
				div = $(containerID);
				var t = div.getElement("input");
				if (!t.checked) 
				{
					t.checked = true;
					$(t.value).setStyle('display', 'block');
				}
			}
		}
	});
};