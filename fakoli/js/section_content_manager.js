var SectionContentManager = new Class(
{
	tree: Class.Empty,
		
	initialize: function(tree)
	{
		this.tree = document.id(tree);
	},
	
	allProperties: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=all&" + this.getQueryParams(), '650px', 'auto', true);		
	},
		
	setTemplate: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=template&" + this.getQueryParams(), '650px', 'auto', true);		
	},

	setRole: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=role&" + this.getQueryParams(), '650px', 'auto', true);		
	},

	setPermissions: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=permissions&" + this.getQueryParams(), '650px', 'auto', true);		
	},
	
	setSSL: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=ssl&" + this.getQueryParams(), '650px', 'auto', true);		
	},
	
	setPageTitle: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=override_page_title&" + this.getQueryParams(), '650px', 'auto', true);		
	},

	setBodyClass: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=body_class&" + this.getQueryParams(), '650px', 'auto', true);		
	},
	
	cancel: function()
	{
		this.popup.hide();
	},
	
	getValues: function()
	{
		this.values = Tree.selectedValues(this.tree.id);		
	},
	
	getQueryParams: function()
	{
		return "section_content_id[]=" + this.values.join("&section_content_id[]=");
	},
	
	propertiesChanged: function(response)
	{
		if (response == "OK")
		{
			this.popup.hide();
			this.tree.getElements("input:checked").each(function(input)
			{
				var div = findAncestor(input, "div");
				div.set('class', 'flagged_node_leaf');
			});
		}
		else
		{
			document.id("SectionContent_form__error").set('html', response);
			document.id("SectionContent_form__error").setStyle('display', 'table-cell');
		}
	},
	
	clearProperties: function()
	{
		this.getValues();
		if (this.values.length == 0) return;
		
		var request = new Request(
		{
			method: 'get', 
			url: "/action/section/clear_content_properties?" + this.getQueryParams(), 
			onSuccess: function(response) 
			{ 
				if (response == "OK")
				{
					this.tree.getElements("input:checked").each(function(input)
					{
						var div = findAncestor(input, "div");
						div.set('class', 'file_node_leaf');
					});
				}
			}.bind(this)
		});
		request.send();
	},
	

	positionModules: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Position Modules", "/action/section/content_modules?" + this.getQueryParams(), '750px', 'auto', true);		
	},
	
	modulesChanged: function(response)
	{
		if (response == "OK")
		{
			this.popup.hide();
			this.tree.getElements("input:checked").each(function(input)
			{
				var div = findAncestor(input, "div");
				div.set('class', 'flagged_node_leaf');
			});
		}
		else
		{
			document.id("SectionContentModules_form__error").set('html', response);
			document.id("SectionContentModules_form__error").setStyle('display', 'table-cell');
		}
	},
	
	
});

