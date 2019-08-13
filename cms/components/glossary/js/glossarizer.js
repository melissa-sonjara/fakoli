var Glossarizer = new Class({
	implements: Options,
	
	container: null,
	url: null,
	terms: [],
	
	options:
	{
		selector: 		'p,div,li',
		firstOnly: 		true,
		tooltipClass: 	'glossary_tooltip'
	},

	initialize: function(container, url, options)
	{
		this.container = document.id(container);
		this.url = url;
		
		this.setOptions();
		this.loadTerms();
	},
	
	loadTerms: function()
	{
		var request = new Request.JSON(
		{
			url: this.url,
			
			onSuccess: function(responseJSON, responseText)
			{
				this.terms = responseJSON;
				this.applyTerms();
			}.bind(this),
			
			onError: function(text, error)
			{
				notification(error);
			}
		}.bind(this));
		request.send();
	},
	
	applyTerms: function()
	{
		var flags = "i";
		if (!this.options.firstOnly) flags += "g";
		
		for(var i = 0; i < this.terms.length; ++i)
		{
			this.terms[i].regex = new RegExp('(?:^|\\b)(' + this.terms[i].term + '\\b', flags);
		}
		
		this.container.getElements(this.options.selector).each(function(elt)
		{
			for(var i = 0; i < this.terms.length; ++i)
			{
				if (this.terms[i].regex == null) continue;
				
			}
		});
	}
});