class DataSyncManager
{
	constructor(form, container, options)
	{
		this.form = null;
		this.container = null;

		this.statistics = null;
		this.matchingButton = null;
		this.nonMatchingButton = null;
		this.submitButton = null;
		this.batchButton = null;
		this.batchMatchingButton = null;
		this.batchNewButton = null;

		this.matching = [];
		this.nonmatching = [];

		this.options = Object.assign(
		{
			chunked: false,
			offset: 0,
			pageSize: 0,
			totalRecords: 0
		}, options);

		this.form = document.getElementById(form);
		this.container = document.getElementById(container);

		this.matching = this.form.querySelectorAll("tr.matching");
		this.nonmatching = this.form.querySelectorAll("tr.new");

		var matchingCount = this.matching ? this.matching.length : 0;
		var nonmatchingCount = this.nonmatching ? this.nonmatching.length : 0;

		if (this.options.chunked)
		{
			this.importPosition = document.createElement('p');
			this.importPosition.style.display = 'block';
			this.importPosition.style.margin = 0;
			var from = this.options.offset + 1;
			var to = this.options.offset + this.options.pageSize;
			if (to > this.options.totalRecords) to = this.options.totalRecords;

			var html = "";

			if (this.options.offset > 0)
			{
				html += "<a class='button' href='?offset=" + (this.options.offset - this.options.pageSize) + "'>&laquo; Prev</a>&nbsp;";
			}

			html += "<strong>Showing Records " + from + " to " + to + " of " + this.options.totalRecords + "</strong>";

			if (this.options.offset < this.options.totalRecords - this.options.pageSize)
			{
				html += "&nbsp;<a class='button' href='?offset=" + (this.options.offset + this.options.pageSize) + "'>Next &raquo;</a>";
			}

			html += "&nbsp;";

			this.importPosition.innerHTML = html;
			this.container.appendChild(this.importPosition);

			this.batchButton = document.createElement('a');
			this.batchButton.className = 'button';
			this.batchButton.textContent = 'Import All Records...';
			this.batchButton.addEventListener('click', () => { this.batchImport(); return false; });
			this.container.appendChild(this.batchButton);

			this.batchMatchingButton = document.createElement('a');
			this.batchMatchingButton.className = 'button';
			this.batchMatchingButton.textContent = 'Update All Matching Records...';
			this.batchMatchingButton.addEventListener('click', () => { this.batchImportMatching(); return false; });
			this.container.appendChild(this.batchMatchingButton);

			this.batchNewButton = document.createElement('a');
			this.batchNewButton.className = 'button';
			this.batchNewButton.textContent = 'Import All New Records...';
			this.batchNewButton.addEventListener('click', () => { this.batchImportNew(); return false; });
			this.container.appendChild(this.batchNewButton);
		}

		this.statistics = document.createElement('p');
		this.statistics.innerHTML = matchingCount + " Matching Records. " + nonmatchingCount + " New Records.&nbsp;";
		this.container.appendChild(this.statistics);

		this.matchingButton = document.createElement('a');
		this.matchingButton.className = 'button';
		this.matchingButton.textContent = "Select Matching Records";
		this.matchingButton.addEventListener('click', () => { this.toggleSelectMatching(); });
		this.statistics.appendChild(this.matchingButton);

		this.nonMatchingButton = document.createElement('a');
		this.nonMatchingButton.className = 'button';
		this.nonMatchingButton.textContent = "Select New Records";
		this.nonMatchingButton.addEventListener('click', () => { this.toggleSelectNonMatching(); });
		this.statistics.appendChild(this.nonMatchingButton);

		this.submitButton = document.createElement('a');
		this.submitButton.className = 'button important';
		this.submitButton.textContent = "Import Selected Records";
		this.submitButton.addEventListener('click', () => { this.form.submit(); });
		this.statistics.appendChild(this.submitButton);
	}

	toggleSelectMatching()
	{
		var text = this.matchingButton.textContent;

		if (text == "Select Matching Records")
		{
			this.form.querySelectorAll("tr.matching input.selector").forEach((cbox) => { cbox.checked = true; });
			text = "Deselect Matching Records";
		}
		else
		{
			this.form.querySelectorAll("tr.matching input.selector").forEach((cbox) => { cbox.checked = false; });
			text = "Select Matching Records";
		}

		this.matchingButton.textContent = text;
	}

	toggleSelectNonMatching()
	{
		var text = this.nonMatchingButton.textContent;

		if (text == "Select New Records")
		{
			this.form.querySelectorAll("tr.new input.selector").forEach((cbox) => { cbox.checked = true; });
			text = "Deselect New Records";
		}
		else
		{
			this.form.querySelectorAll("tr.new input.selector").forEach((cbox) => { cbox.checked = false; });
			text = "Select New Records";
		}

		this.nonMatchingButton.textContent = text;
	}

	batchImport()
	{
		new BackgroundProcess("Importing Records", "/action/data_sync/batch_import?matching=1&new=1", {hideOnComplete: false, closeAction: function() { window.location.reload(); }});
	}

	batchImportMatching()
	{
		new BackgroundProcess("Importing Records", "/action/data_sync/batch_import?matching=1&new=0", {hideOnComplete: false, closeAction: function() { window.location.reload(); }});
	}

	batchImportNew()
	{
		new BackgroundProcess("Importing Records", "/action/data_sync/batch_import?matching=0&new=1", {hideOnComplete: false, closeAction: function() { window.location.reload(); }});
	}
}
