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

var FakoliMenu = new Class({
	
	root:	Class.Empty,
	
	initialize: function(elt)
	{
		this.root = $(elt);
		var menu = this;
		
		document.focusWatcher.addEvent('focusChanged', function() { if (!this.root.hasChild(document.focusWatcher.focus)) this.clearFocus(); }.bind(menu));
		
		// Extension of suckerfish to allow keyboard navigation of dropdowns.
		
		$$("#" + this.root.id + " > ul > li > a").each(function(elt) 
		{
			elt.addEvents({'focus': function(e) 
									{
										var event = new Event(e).stop();
										this.updateFocus(event, elt);
									}.bind(menu)
			});
		});
		
		//if (Browser.Engine.trident)
		//{
			// Suckerfish style dropdown implementation for IE6 mouseovers.
			
			$$("#" + this.root.id + " > ul > li").each(function (elt)
			{
				elt.addEvents(
				{
					'mouseover': function() 
					{ 
						elt.addClass("sfhover"); 
						var ul = elt.getElement('ul');
						if (ul) ul.position({'relativeTo': elt, position: 'bottomLeft'}); 
					},
						
					'mouseout': function() 
					{
						elt.removeClass("sfhover");  
						var ul = elt.getElement('ul');
						if (ul) ul.setStyle('left', -2000);
					} 
				 });
			});
		//}
	},
	
	updateFocus: function(event, elt)
	{
		this.clearFocus();
		
		var parent = elt.getParent();
		parent.addClass('sfhover');
	},
	
	clearFocus: function()
	{
		this.root.getElements("ul > li").each(function(elt) { elt.removeClass('sfhover'); });
	}
});
