class Glossarizer
{
	constructor(container, url, options)
	{
		this.container = null;
		this.url = null;
		this.terms = [];

		this.options = Object.assign(
		{
			selector:     'p,div,li',
			firstOnly:    true,
			termClass:    'glossary_term',
			tooltipClass: 'glossary_tooltip',
			excludeClass: 'no_glossarize'
		}, options);

		this.container = document.getElementById(container);
		this.url = url;

		if (document.body.classList.contains(this.options.excludeClass))
		{
			return;
		}

		this.loadTerms();
	}

	loadTerms()
	{
		fetch(this.url)
			.then(r => r.json())
			.then(function(responseJSON)
			{
				this.terms = responseJSON;
				this.applyTerms();
			}.bind(this))
			.catch(function(error)
			{
				notification(error);
			}.bind(this));
	}

	applyTerms()
	{
		if (!this.container) return;

		var flags = "i";
		if (!this.options.firstOnly) flags += "g";

		for (var i = 0; i < this.terms.length; ++i)
		{
			this.terms[i].regex = new RegExp('(?:^|\\b)' + this.terms[i].term + '\\b', flags);
			this.terms[i].replace = this.terms[i].term + "<div class='" + this.options.tooltipClass + "'>" +
									this.terms[i].definition + "</div>";
			this.terms[i].strlen = this.terms[i].term.length;
			this.terms[i].matched = false;
		}

		this.container.querySelectorAll(this.options.selector).forEach(function(elt)
		{
			for (var i = 0; i < this.terms.length; ++i)
			{
				if (this.terms[i].regex == null) continue;
				if (this.isExcluded(elt)) continue;
				this.iterateNode(elt, this.terms[i]);
			}
		}.bind(this));

		document.querySelectorAll("span.glossary_term").forEach(function(span)
		{
			span.addEventListener('click',
			function(e)
			{
				if (span.classList.contains('tapped'))
					span.classList.remove('tapped');
				else
					span.classList.add('tapped');
			});
		});
	}

	isExcluded(elt)
	{
		if (elt.classList.contains(this.options.excludeClass)) return true;
		if (!elt.parentElement || elt == this.container) return false;
		return this.isExcluded(elt.parentElement);
	}

	iterateNode(node, term)
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
				var parent = node.parentElement;

				var span = document.createElement('span');
				span.className = this.options.termClass;
				span.innerHTML = val;
				var after = document.createTextNode(node.data.substring(match.index + offset));
				var next = node.nextSibling;

				node.data = node.data.substring(0, match.index);
				node.insertAdjacentElement('afterend', span);
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
			if (node.classList.contains(this.options.termClass) || node.classList.contains(this.options.excludeClass)) return;

			for (var i = 0; i < node.childNodes.length; i++)
			{
				this.iterateNode(node.childNodes[i], term); // run recursive on DOM
			}
		}
	}
}
