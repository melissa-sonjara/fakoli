var SectionContentManager = new Class(
{
	tree: Class.Empty,
		
	initialize: function(tree)
	{
		this.tree = $(tree);
	},
	
	allProperties: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=all&" + this.getQueryParams(), '320px', 'auto', true);		
	},
		
	setTemplate: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=template&" + this.getQueryParams(), '320px', 'auto', true);		
	},

	setRole: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=role&" + this.getQueryParams(), '320px', 'auto', true);		
	},

	setSSL: function()
	{
		this.getValues();
		if (this.values.length == 0) return;

		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=ssl&" + this.getQueryParams(), '320px', 'auto', true);		
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
			$("SectionContent_form__error").set('html', response);
			$("SectionContent_form__error").setStyle('display', 'table-cell');
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
	}
});

