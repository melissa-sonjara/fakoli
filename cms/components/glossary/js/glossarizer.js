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
			}.bind(this)
		});
		request.send();
	},
	
	applyTerms: function()
	{
		var flags = "i";
		if (!this.options.firstOnly) flags += "g";
		
		for(var i = 0; i < this.terms.length; ++i)
		{
			this.terms[i].regex = new RegExp('(?:^|\\b)' + this.terms[i].term + '\\b', flags);
			this.terms[i].replace = this.terms[i].term + "<div class='" + this.options.tooltipClass + "'>" + 
									this.terms[i].definition + "</div>";
			this.terms[i].strlen = this.terms[i].term.length;
			this.terms[i].matched = false;
		}
		
		this.container.getElements(this.options.selector).each(function(elt)
		{
			for(var i = 0; i < this.terms.length; ++i)
			{
				if (this.terms[i].regex == null) continue;
				this.iterateNode(elt, this.terms[i]);
			}
		}.bind(this));
		
		document.getElements("span.glossary_term").each(function(span) 
		{
			span.addEvent('click', 
			function(e) 
			{ 
				if (span.hasClass('tapped')) 
					span.removeClass('tapped');
				else 
					span.addClass('tapped');
			})
		});
	},
	
	iterateNode: function(node, term) 
	{
		if (term.matched) return;

		var expr = term.regex;
		var val = term.replace;
		var offset = term.strlen;
		
	    if (node.nodeType === 3) 
	    {
	    	// Node.TEXT_NODE
	    	var match = expr.exec(node.data);
	    	
	    	if (match != null)
	    	{
	    		var parent = document.id(node.parentElement);
	    		
	    		var span = new Element('span', {'class': this.options.termClass})
	    		span.set('html', val);
	    		var after = document.createTextNode(node.data.substring(match.index + offset));
	    		var next = node.nextSibling;
	    		
	    		node.data = node.data.substring(0, match.index);
	    		span.inject(node, 'after');
	    		parent.insertBefore(after, next);
	    		
	    		if (this.options.firstOnly)
	    		{
	    			term.matched = true;
	    		}
	    	}
	    } 
	    else if (node.nodeType === 1) 
	    { 
	    	// Node.ELEMENT_NODE
	    	
	    	if (node.className == this.options.termClass) return;
	    	
	        for (var i = 0; i < node.childNodes.length; i++) 
	        {
	            this.iterateNode(node.childNodes[i], term); // run recursive on DOM
	        }
	    }
	}
});