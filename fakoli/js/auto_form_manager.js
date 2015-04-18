var AutoFormManager = new Class(
{
	Implements: Options,
	form: Class.Empty,
	partialSaveButton: Class.Empty,
	options: 
	{
		partialSaveContainer:	'',
		partialSaveLabel: 'Save'
	},
	
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
		
		if (this.options.partialSaveContainer)
		{
			this.addPartialSaveButton();
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
	},
	
	addPartialSaveButton: function()
	{
		var container = document.id(this.options.partialSaveContainer);
		this.partialSaveButton = new Element('a', {class: 'button partial_save', html: this.options.partialSaveLabel});
		this.partialSaveButton.addEvent('click', function() { this.partialSave();}.bind(this));
		container.adopt(this.partialSaveButton);
		
		this.form.getElements("input,select,textarea").each(function(elt)
		{
			elt.addEvent('change', function ()
			{
				this.partialSaveButton.removeClass('saved').addClass('dirty');
			}.bind(this));
		}.bind(this));
		
		this.form.getElements("input,textarea").each(function(elt)
		{
			elt.addEvent('keypress', function ()
			{
				this.partialSaveButton.removeClass('saved').addClass('dirty');
			}.bind(this));
		}.bind(this));
	},
	
	partialSave: function()
	{
		var uri = new URI();
		var action = this.form.action + "?" + uri.get("query");

		this.partialSaveButton.removeClass('error').addClass('saving');
		
		var request = new Request.JSON(
		{
			url: action,
			data: this.form,
			headers: {'X-Partial-Save': 'true'},
			onSuccess: function(responseJSON, responseText)
			{
				if (responseJSON.status == 'success')
				{
					this.partialSaveButton.removeClass('dirty').removeClass('saving');
					this.partialSaveButton.addClass('saved');
				}
				else
				{
					this.partialSaveButton.removeClass('saving');
					this.partialSaveButton.addClass('error');
					notification(repsonseJSON.error);
				}
			}.bind(this),
			onError: function(error)
			{
				notification("Error contacting server");
			}.bind(this)
		});
		
		request.post();
	}
});