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
	
	getGroupID: function(groupName)
	{
		return this.form.id + "_" + codify(groupName) + "_group";
	},
	
	addGroupClass: function(group, cssClass)
	{
		var gid = this.getGroupID(group);
		var g = document.id(gid);
		g.addClass(cssClass);
	},
	
	removeGroupClass: function(group, cssClass)
	{
		var gid = this.getGroupID(group);
		var g = document.id(gid);
		g.removeClass(cssClass);		
	},
	
	setLabel: function(field, text)
	{
		var id = this.form.id + "_" + field + "_label";
		var label = document.id(id);
		label.set('text', text);
	},
	
	dirty: function()
	{
		if (this.partialSaveButton)
		{
			this.partialSaveButton.removeClass('saved').addClass('dirty');
		}
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
		var action = this.form.action ? this.form.action : uri.toString();
		
		if (this.partialSaveButton) this.partialSaveButton.removeClass('error').addClass('saving');
		
		var request = new Request.JSON(
		{
			url: action,
			data: this.form,
			headers: {'X-Partial-Save': 'true'},
			onSuccess: function(responseJSON, responseText)
			{
				if (responseJSON.status == 'success')
				{
					if (this.partialSaveButton) 
					{
						this.partialSaveButton.removeClass('dirty').removeClass('saving');
						this.partialSaveButton.addClass('saved');
					}
					
					var pkfield = this.form.getElement("input[name='" + responseJSON.primary_key + "']");
					if (pkfield && pkfield.value == '') 
					{
						pkfield.value = responseJSON.primary_key_value;
					}
				}
				else
				{
					if (this.partialSaveButton) 
					{
						this.partialSaveButton.removeClass('saving');
						this.partialSaveButton.addClass('error');
					}
					notification(responseJSON.error);
				}
			}.bind(this),
			onError: function(error)
			{
				notification("Error contacting server");
			}.bind(this)
		});
		
		request.post();
	},
	
	getSubmitButton: function()
	{
		var submitID = this.form.id + "_submitButton";
		var submitButton = document.id(submitID);
		return submitButton;
	},
	
	setSubmitLabel: function(text)
	{
		var submitButton = this.getSubmitButton();
		if (submitButton.tagName == 'A' || submitButton.tagName == "BUTTON")
		{
			submitButton.set('html', text);
		}
		else
		{
			submitButton.set('value', text);
		}
	},
	
	setSubmitEnabled: function(enabled)
	{
		var submitButton = this.getSubmitButton(); 
		submitButton.disabled = !enabled;
	},
	
	submit: function()
	{
		var dosubmit = this.form.fireEvent('submit', this.form);
		if (dosubmit) this.form.submit();
	},
	
	rawSubmit: function()
	{
		if (this.form.setLoading) this.form.setLoading(true);
		this.form.submit();
	},
	
	showError: function(error)
	{
		document.id(this.form.id + "__error").set('html', error).setStyle('display', 'table-cell');
	},
	
	clearError: function()
	{
		document.id(this.form.id + "__error").set('html', '').setStyle('display', 'none');
	}
});

AutoFormManager.getManager = function(id)
{
	var form = document.id(id);
	if (form) return form.manager;
	return null;
};

AutoFormManager.toggleGroup = function(group, state)
{
	if (state) 
	{  
		document.id(group).setStyle('opacity', 0).removeClass('collapsed').addClass('expanded').fade('in'); 
	} 
	else 
	{ 
		document.id(group).removeClass('expanded').addClass('collapsed'); 
	} 
	
	ModalDialog.recenterActiveDialog();
};