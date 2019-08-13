var Glossarizer = new Class({
	Implements: Options,
	
	container: null,
	url: null,
	terms: [],
	
	options:
	{
		selector: 		'p,div,li',
		firstOnly: 		true,
		termClass:		'glossary_term',
		tooltipClass: 	'glossary_tooltip'
	},

	initialize: function(container, url, options)
	{
		this.container = document.id(container);
		this.url = url;
		
		this.setOptions(options);
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
			this.terms[i].replace = "<span class='" + this.options.termClass + "'>" + 
									this.terms[i].term + "<div class='" + this.options.tooltipClass + "'>" + 
									this.terms[i].definition + "</div></span>";
		}
		
		this.container.getElements(this.options.selector).each(function(elt)
		{
			for(var i = 0; i < this.terms.length; ++i)
			{
				if (this.terms[i].regex == null) continue;
				this.iterateNode(elt, this.terms[i].regex, this.terms[i].replace);
			}
		});
	},
	
	iterateNode: function(node, expr, val) 
	{
	    if (node.nodeType === 3) 
	    {
	    	// Node.TEXT_NODE
	        var text = node.data.replace(expr, val);
	        if (text != node.data)
	        {
	        	// there's a Safari bug
	            node.data = text;
	        }

	    } 
	    else if (node.nodeType === 1) 
	    { 
	    	// Node.ELEMENT_NODE
	        for (var i = 0; i < node.childNodes.length; i++) 
	        {
	            this.iterateNode(node.childNodes[i], expr, val); // run recursive on DOM
	        }
	    }
	}
});