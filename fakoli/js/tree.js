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

var Tree = new Class({
	
	initialize: function()
	{
	}
});

Tree.toggleFolder = function(id, openStyle, closedStyle)
{
	var div  = $(id + "_contents");
	var link = $(id);

	if (div.style.display == "none" || div.style.display == "")
	{
		div.style.display = "block";
		link.className = openStyle;
	}
	else
	{
		div.style.display="none";
		link.className = closedStyle;
	}
};

Tree.loadOnDemand = function(id, fragmentURL, force)
{
	var link = $(id );
	var div = $(id + "_contents");
	var cursor = link.getStyle('cursor');
	
	if (!div.loaded || force)
	{
		if (force) div.set('text', '');
		
		link.setStyle('cursor', 'progress');
		
		var request = new Request.HTML(
		{
			evalScripts: false,
			evalResponse: false,
			method: 'get', 
			url: fragmentURL, 
			onSuccess: function(tree, elements, html, script) 
			{ 
				div.set('text', '');
				div.adopt(tree);
				link.setStyle('cursor', cursor);
				$exec(script);
			}
		});
		request.send();
		div.loaded = true;
	}
};
		
Tree.clearCheckBoxes = function(id, except)
{
	var div = $(id + "_table");
	div.getElements("input").each(function(box)
	{
		if (box != except)
		{
			box.checked = false;
		}
	});
	
	except.form[id].value = except.value;
};
