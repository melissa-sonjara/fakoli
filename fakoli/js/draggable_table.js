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

 
var DraggableTable = new Class({
		
	sortables: Class.Empty,
	options: {clone: true, revert: true, opacity: 1},
	handler: "",
	key: "",
	
	initialize: function(list, handler, key)
	{
		this.handler = handler;
		this.key = key;
		
		this.options.onStart = function(element, clone)
		{
			etds = element.getElements("td");
			ctds = clone.getElements("td");
			
			etds.each(function(td, i)
			{
				ctds[i].setStyle('width', td.getSize().x);
			});
			
			clone.addClass("dragRow");
		};
		
		this.options.onComplete = function()
		{
			var trs = $$(list).getElements("tr").flatten();
			var qs = (this.handler.indexOf("?") < 0) ? "?" : "&";
			
			var expr = this.key + "_";
			
			var c = 1;
			
			trs.each(function(tr, i)
			{
				if (tr.id)
				{
					pk = tr.id.replace(expr, "");
					qs += this.key + "[" + pk + "]=" + c + "&";
					++c;
				}
			}.bind(this));
			
    		var request = new Request.HTML(
    		{
    			evalScripts: false,
    			evalResponse: false,
    			method: 'get', 
    			url: handler + qs
    		});
    		request.send();
    		
    		return true;
		}.bind(this);
		
		this.sortables = new Sortables(list, this.options);
	}
});