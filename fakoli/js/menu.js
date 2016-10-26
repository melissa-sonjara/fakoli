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
	
	Implements: [Options],
	
	root:	Class.Empty,
	toggle: Class.Empty,
	options: 
	{
		position: 'bottomLeft',
		edge: 'topLeft',
		effect: 'fade',
		subMenuPosition: 'topRight',
		subMenuEdge: 'topLeft',
		responsiveToggle: '',
		responsivePosition: 'bottomRight',
		responsiveEdge: 'topRight',
		mode: 'pulldown'
	},
	
	initialize: function(elt, options)
	{
		this.root = document.id(elt);
		this.setOptions(options);
		var menu = this;
		
		if (this.options.responsiveToggle != '')
		{
			this.toggle = document.id(this.options.responsiveToggle);
			if (this.toggle != null) this.toggle.addEvent('click', function() {this.toggleResponsiveMenu(); }.bind(this));
		}
		
		document.focusWatcher.addEvent('focusChanged', function() 
		{ 
			this.updateFocus(document.focusWatcher.focus);
		}.bind(menu));
		
		if (!this.root) return;
		
		// Suckerfish style dropdown implementation for mouseovers.
		
		$$("#" + this.root.id + " > ul > li").each(function (elt)
		{
			var uls = elt.getElements('ul');
			
			uls.each(function(ul)
			{
				if (menu.options.effect == 'fade')
				{
					ul.setStyle('opacity', 0);
				}
				
				var parent = ul.getParent();
				parent.addClass(parent.getParent().getParent().id == menu.root.id ? "submenu" : "subsubmenu");
				
				parent.addEvents(
				{
					'touchstart': function(event)
					{
						new DOMEvent(event).stop();
						
						menu.clearFocus();
						menu.showMenu(parent);
						if (!parent.blockClick)
						{
							parent.blockClick = function(event) { new DOMEvent(event).stop(); return false; };
							parent.addEvent('click', elt.blockClick.bind(elt));
							
							ul.getElements('a').each(function(child)
							{
								child.addEvent('touchend', function(event) { new DOMEvent(event).stop(); this.clearFocus(); go(child.href);}.bind(menu));
							});
						}							
					},
					
					'mouseover': function() 
					{ 
						if (!menu.reduced && menu.options.mode == 'pulldown') 
						{
							menu.showMenu(parent);
						}
						else
						{
							style = ul.getStyle('display');
							if (style == "none")
							{
								
								menu.opening = true;
							}
						}
					},
						
					'mouseout': function() 
					{
						menu.opening = false;
						if (!menu.reduced && menu.options.mode == 'pulldown') menu.hideMenu(parent);
					}
				 });
				
				parent.getChildren("a").addEvents(
				{
						
					"click": function()
					{
						style = ul.getStyle('display');
						if (Browser.name == 'safari' && menu.options.mode == 'accordion')
						{
							// Safari can't handle focus changes on links (!)
							// so handle navigation via clicks instead
							menu.updateFocus(this);
							return false;
						}
						else
						{
							return (style != 'none' && !menu.opening);
						}
					}
				});
			});
		});

	},
	
	showMenu: function(elt)
	{
		var ul = elt.getElement('ul');
		elt.addClass("sfhover"); 
		
		
		var offset = this.options.position;
		var edge = this.options.edge;
		if (ul) 
		{
			if (elt.hasClass("subsubmenu"))
			{
				offset = this.options.subMenuPosition;
				edge = this.options.subMenuEdge;
			}

			if (!this.reduced && this.options.mode == 'pulldown')
			{
				ul.position({'relativeTo': elt, 'position': offset, 'edge': edge});
			}
			
			if (this.options.effect == 'fade')
			{
				ul.fade('in');
			}
			else if (this.options.effect == 'reveal')
			{
				ul.reveal();
			}
		}
	},
	
	hideMenu: function(elt)
	{
		var ul = elt.getElement('ul');
		elt.removeClass("sfhover");  
		if (ul)
		{
			if ((this.reduced || this.options.mode == 'accordion') && (this.options.effect == 'fade' || this.options.effect == 'reveal'))
			{
				ul.setStyle('opacity', '0');
			}
			
			if (this.reduced || this.options.mode == 'accordion') return;

			ul.setStyle('left', -10000);
		}
	},
	
	updateFocus: function(elt)
	{
		if (!this.root) return;
		
		this.clearFocus();
		
		if (!this.root.contains(elt)) return;
		
		this.showMenu(elt.getParent());
	},
	
	clearFocus: function()
	{
		if (!this.root) return;
		this.root.getElements("ul > li").each(function(elt) { if (!elt.contains(document.focusWatcher.focus)) this.hideMenu(elt); }.bind(this));
	},
	
	toggleResponsiveMenu: function()
	{
		this.reduced = true;
		
		if (this.root.hasClass("sfhover"))
		{
			this.root.removeClass("sfhover");
			this.root.setStyle('display', 'none');
;		}
		else
		{
			this.root.addClass("sfhover");
			if (this.options.responsivePosition != 'fixed')
			{
				this.root.position({'relativeTo': this.toggle, 'position': this.options.responsivePosition, 'edge': this.options.responsiveEdge});
			}
			
			this.root.setStyle('display', 'block');
			if (this.options.effect == 'fade')
			{
				this.root.fade('in');
			}
			else if (this.options.effect == 'reveal')
			{
				this.root.reveal();
			}
		}
	}
});
