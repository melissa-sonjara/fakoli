var ReportManager = new Class(
{
	
});

ReportManager.updateFilters = function()
{
	var table = document.id('custom_report_table');
	
	var cboxes = table.getElements("input");
	
	cboxes.each(function(c) 
	{ 
		if (c.name.indexOf("table_") == 0)
		{
			var fieldset = c.value;
			if (c.checked)
			{
				document.id(fieldset).setStyle('display', 'block');
			}
			else
			{
				document.id(fieldset).setStyle('display', 'none');
			}
		}
		else if (c.name.indexOf("column_") == 0)
		{
			if (c.checked)
			{
				var div = findAncestor(c, "div").getParent();
				var containerID = div.id.replace("_contents", "");
				div = document.id(containerID);
				var t = div.getElement("input");
				if (!t.checked) 
				{
					t.checked = true;
					document.id(t.value).setStyle('display', 'block');
				}
			}
		}
	});
};

ReportManager.saveReport = function(report_id)
{
	ReportManager.dialog = modalPopup("Save Custom Report", "/action/report_manager/save_report?report_id=" + report_id, null, null, true);
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
	document.id('custom_report_title').value = document.id('CustomReport_form_title').value;
	document.id('custom_report_description').value = document.id('CustomReport_form_description').value;
	document.id('custom_report_mode').value = 'save';
	ReportManager.commitSave.delay(100);
		
	return false;
};

ReportManager.commitSave = function()
{
	var form = document.id('custom_report');
	var action = form.action;
	form.target = "";
	form.action = "";
	document.id('custom_report').submit();
	form.action = action;
	form.target = "_blank";
};


ReportManager.reportSaved = function(response)
{
	if (response == "OK")
	{
		ReportManager.dialog.hide();	
	}
	
	// We don't need to reload if saving from report results page
	var table = document.id('CustomReports');
	if(table)
		window.location.reload();
	
	document.id('CustomReport_form__error').set('html', response);
};

ReportManager.columnOrder = "";

ReportManager.setColumnOrder = function(ths)
{
	var arr = [];
	Array.each(ths, function(th) { arr.push(th.get('text').trim()); });
	ReportManager.columnOrder = arr.join("|");
};