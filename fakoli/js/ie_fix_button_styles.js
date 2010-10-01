window.addEvent('domready', function()
{
	// Hack only for Internet Explorer
	if (!Browser.Engine.trident) return;
	
	$$('.button').each(function(e)
	{
		// Styling issues do not affect 'a' tags
		if (e.tagName == "A") return;
		
		// Handle incorrect handling of button border rendering
		// by Internet Explorer by wrapping any button or input
		// tags with the class of button in a span.
		
		var elt = new Element('span', {'class': 'button'});
		var fl = e.getStyle('float');
		if (fl == "right")
		{
			elt.setStyles({'display': 'block', 'float': fl});
			e.setStyles({'display': 'inline-block', 'float': 'none'});
		}
		elt.wraps(e);
		e.setStyles({'border': 'none', 'height': '22px'});
	});
});