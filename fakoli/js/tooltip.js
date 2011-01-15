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

function showToolTip(link, evt, id, handlerURL, width)
{
	var link = $(link);
	currentLink = $(link);
	
	var event = new Event(evt).stop();
	loadTooltip = function (link, event, id, handlerURL)
	{
		if (link != currentLink) return;
		
		var div = $(id);
		if(div == null) 
		{
			div = new Element('div', {'id': 'tooltip', 'class': 'tooltip_box'});
			var doc = document.body ? document.body : document.documentElement;
			
			div.inject(doc, 'top'); 
		}
		
		var cursor = link.getStyle('cursor');
	
		link.setStyle('cursor', 'progress');
	   	
		var request = new Request.HTML(
	    {	 	
	    	evalScripts: false,
			evalResponse: false,
			method: 'get', 
			url: handlerURL, 
			onSuccess: function(tree, elements, html, script) 
			{
	    		div.set('text', '');
				div.adopt(tree);
				div.setStyles({'display':  'block',
							   'left':	   event.page.x + 4,
							   'top':      event.page.y,
							   'position': 'absolute',
							   'width':		width});
				link.setStyle('cursor', cursor);
				$exec(script);
			}
		});
		request.send();
	};
		
	loadTooltip.delay(1000, null, [link, event, id, handlerURL]);
};

function hideToolTip (id)
{
	currentLink = null;
	
	var div = $(id);
	if (div == null) return;

	div.style.display = 'none';
};
