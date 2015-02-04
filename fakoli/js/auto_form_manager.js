var AutoFormManager = new Class(
{
	Implements: Options,
	form: Class.Empty,
	options: {},
	
	initialize: function(form, options)
	{
		// Form over function?
		
		this.form = document.id(form);
		this.setOptions(options);
		this.form.manager = this;
		
		if (this.form.onInitalize) this.form.onInitialize();
	},
	
	hideField: function(field)
	{
		this.form.getElements("." + form.id + '_' + field + '_field').each(function(element)
		{
			element.setStyle('display', 'none');
		});
	},
	
	showField: function(field)
	{
		this.form.getElements("." + form.id + '_' + field + '_field').each(function(element)
		{
			element.setStyle('display', (element.tagName.toLower() == "tr") ? 'table-row' : 'block');
		});
	}	
});