var DataSyncManager = new Class(
{
	Implements: Options,
	
	form: 		 Class.Empty,
	container:   Class.Empty,
	
	statistics:	Class.Empty,
	matchingButton: Class.Empty,
	nonMatchingButton: Class.Empty,
	submitButton: Class.Empty,
	
	matching: [],
	nonmatching: [],
	
	options:
	{
		chunked: false,
		offset: 0,
		pageSize: 0,
		totalRecords: 0
	},
	
	initialize: function(form, container, options)
	{
		this.setOptions(options);
		
		this.form = document.id(form);
		this.container = document.id(container);
		
		this.matching = this.form.getElements("tr.matching");
		this.nonmatching = this.form.getElements("tr.new");
		
		matchingCount = this.matching ? this.matching.length : 0;
		nonmatchingCount = this.nonmatching ? this.nonmatching.length : 0;
		
		if (this.options.chunked)
		{
			this.importPosition = new Element('p');
			this.importPosition.setStyle('display', 'block');
			this.importPosition.setStyle('margin', 0);
			from = this.options.offset + 1;
			to = this.options.offset + this.options.pageSize;
			if (to > this.options.totalRecords) to = this.options.totalRecords;
			
			var html = "";
			
			if (this.options.offset > 0)
			{
				html += "<a class='button' href='?offset=" + (this.options.offset - this.options.pageSize) + "'>&laquo; Prev</a>&nbsp;";
			}
			
			html += "<strong>Showing Records " + from + " to "  + to + " of " + this.options.totalRecords + "</strong>";
			
			if (this.options.offset < this.options.totalRecords - this.options.pageSize)
			{
				html += "&nbsp;<a class='button' href='?offset=" + (this.options.offset + this.options.pageSize) + "'>Next &raquo;</a>";
			}
			
			this.importPosition.set('html', html);
			this.importPosition.inject(this.container);
		}
		
		this.statistics = new Element('p');
		this.statistics.set('html', matchingCount + " Matching Records. " + nonmatchingCount + " New Records.<br/>");
		this.statistics.inject(this.container);
		
		this.matchingButton = new Element('a', {'class': 'button', 'text': "Select Matching Records"});
		
		this.matchingButton.addEvent('click', function() { this.toggleSelectMatching(); }.bind(this));
		this.matchingButton.inject(this.container);
		
		this.nonMatchingButton = new Element('a', {'class': 'button', 'text': "Select New Records"});
		
		this.nonMatchingButton.addEvent('click', function() { this.toggleSelectNonMatching(); }.bind(this));
		this.nonMatchingButton.inject(this.container);
		
		this.submitButton = new Element('a', {'class': 'button important', 'text': "Import Selected Records"});
		this.submitButton.addEvent('click', function() { this.form.submit(); }.bind(this));
		
		this.submitButton.inject(this.container);
	},
	
	toggleSelectMatching: function()
	{
		var text = this.matchingButton.get('text');
		
		if (text == "Select Matching Records")
		{
			this.form.getElements("tr.matching input.selector").each(function(cbox) { cbox.checked = true; });
			text = "Deselect Matching Records";
		}
		else
		{
			this.form.getElements("tr.matching input.selector").each(function(cbox) { cbox.checked = false; });
			text = "Select Matching Records";			
		}
		
		this.matchingButton.set('text', text);
	},
	
	toggleSelectNonMatching: function()
	{
		var text = this.nonMatchingButton.get('text');
		
		if (text == "Select New Records")
		{
			this.form.getElements("tr.new input.selector").each(function(cbox) { cbox.checked = true; });
			text = "Deselect New Records";
		}
		else
		{
			this.form.getElements("tr.new input.selector").each(function(cbox) { cbox.checked = false; });
			text = "Select New Records";			
		}
		
		this.nonMatchingButton.set('text', text);
	}	
});