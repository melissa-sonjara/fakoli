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

ReportManager.saveReport = function()
{
	ReportManager.dialog = modalPopup("Save Custom Report", "/action/report_manager/save_report", null, null, true);
};

/*
 * Update an already saved report such as a presaved
 * report - saved on generate in case user wants to keep it.
 */
ReportManager.updateReport = function(report_id)
{
	ReportManager.dialog = modalPopup("Save Custom Report", "/action/report_manager/update_report?report_id=" + report_id, null, null, true);
};


ReportManager.onSaveReport = function()
{
	$('custom_report_title').value = $('CustomReport_form_title').value;
	$('custom_report_description').value = $('CustomReport_form_description').value;
	$('custom_report_mode').value = 'save';
	ReportManager.commitSave.delay(100);
		
	return false;
};

ReportManager.commitSave = function()
{
	$('custom_report').submit();
};


ReportManager.reportSaved = function(response)
{
	if (response == "OK")
	{
		ReportManager.dialog.hide();	
		window.location.reload();
	}
		
	$('CustomReport_form__error').set('html', response);
};
