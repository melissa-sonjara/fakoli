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
		
		if (!this.form) return;
		
		this.form.manager = this;
		
		if (this.form.onInitialize) 
		{
			this.form.onInitialize();
		}
	},
	
	hideField: function(field)
	{
		var clazz = "." + this.form.id + '_' + field + '_field';
		this.form.getElements(clazz).each(function(element)
		{
			element.setStyle('display', 'none');
		});
	},
	
	showField: function(field)
	{
		var clazz = "." + this.form.id + '_' + field + '_field';
		this.form.getElements(clazz).each(function(element)
		{
			element.setStyle('display', (element.tagName.toLowerCase() == "tr") ? 'table-row' : 'block');
		});
	},
	
	setLabel: function(field, text)
	{
		var id = this.form.id + "_" + field + "_label";
		var label = document.id(id);
		label.set('text', text);
	}
});