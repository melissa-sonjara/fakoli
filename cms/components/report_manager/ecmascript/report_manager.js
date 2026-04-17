class ReportManager
{

}

ReportManager.updateFilters = function()
{
	var table = document.getElementById('custom_report_table');

	var cboxes = table.querySelectorAll("input");

	cboxes.forEach(function(c)
	{
		if (c.name.indexOf("table_") == 0)
		{
			var fieldset = c.value;
			if (c.checked)
			{
				document.getElementById(fieldset).style['display'] = 'block';
			}
			else
			{
				document.getElementById(fieldset).style['display'] = 'none';
			}
		}
		else if (c.name.indexOf("column_") == 0)
		{
			if (c.checked)
			{
				var div = findAncestor(c, "div").parentElement;
				var containerID = div.id.replace("_contents", "");
				div = document.getElementById(containerID);
				var t = div.querySelector("input");
				if (!t.checked)
				{
					t.checked = true;
					document.getElementById(t.value).style['display'] = 'block';
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
	document.getElementById('custom_report_title').value = document.getElementById('CustomReport_form_title').value;
	document.getElementById('custom_report_description').value = document.getElementById('CustomReport_form_description').value;
	document.getElementById('custom_report_mode').value = 'save';
	setTimeout(ReportManager.commitSave, 100);

	return false;
};

ReportManager.commitSave = function()
{
	var form = document.getElementById('custom_report');
	var action = form.action;
	form.target = "";
	form.action = "";
	document.getElementById('custom_report').submit();
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
	var table = document.getElementById('CustomReports');
	if (table)
		window.location.reload();

	document.getElementById('CustomReport_form__error').innerHTML = response;
};

ReportManager.columnOrder = "";

ReportManager.setColumnOrder = function(ths)
{
	var arr = [];
	Array.from(ths).forEach(function(th) { arr.push(th.textContent.trim()); });
	ReportManager.columnOrder = arr.join("|");
};

ReportManager.exportToExcel = function(url)
{
	if (ReportManager.columnOrder)
	{
		url += "&__column_order=" + encodeURIComponent(ReportManager.columnOrder);
	}

	go(url);
};

ReportManager.createReportDialog = function()
{
	modalPopup("Select Report Target", "/action/report_manager/select_report_target", "500px", "auto");
};

ReportManager.createReportSelect = function()
{
	var checked = document.body.querySelectorAll("input[name='target_type']:checked");
	if (!checked.length) return;

	var target_type = checked[0];
	if (!target_type) return;

	var target = target_type.value;
	if (!target) return;
	go('custom_report?target=' + target);
};

ReportManager.shareReport = function(report_id)
{
	fetch("/action/report_manager/share?report_id=" + report_id + "&shared=1")
		.then(r => r.text())
		.then(response =>
		{
			if (response == "OK")
			{
				location.reload();
			}
			else
			{
				notification(response);
			}
		});
};

ReportManager.unshareReport = function(report_id)
{
	fetch("/action/report_manager/share?report_id=" + report_id + "&shared=0")
		.then(r => r.text())
		.then(response =>
		{
			if (response == "OK")
			{
				location.reload();
			}
			else
			{
				notification(response);
			}
		});
};

ReportManager.deleteReport = function(report_id)
{
	if (!confirm("Are you sure you want to delete this report?")) return;

	fetch("/action/report_manager/delete?report_id=" + report_id)
		.then(r => r.text())
		.then(response =>
		{
			if (response == "OK")
			{
				location.reload();
			}
			else
			{
				notification(response);
			}
		});
};
