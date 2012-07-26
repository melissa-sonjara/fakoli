/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

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