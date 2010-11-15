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
	
	toggleFolder: function(id, openStyle, closedStyle)
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
	},

	loadOnDemand: function(id, url)
	{
		var div = $(id + "_contents");
		if (!div.loaded)
		{
			html = httpRequest(url);
			div.innerHTML = html;
			div.loaded = true;
		}
	},
		
	clearCheckBoxes: function(id, except)
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
	}
});