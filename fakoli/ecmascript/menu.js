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

// Requires ui.js for $el, setStyles, fadeIn, reveal, positionRelative, EventEmitter.
// Requires document.focusWatcher (defined in ui.js).

/**
 * Manages a navigation menu with dropdown sub-menus, touch support,
 * responsive collapse/expand, and keyboard accessibility.
 */
class FakoliMenu
{
	constructor(elt, options)
	{
		this.root   = $el(elt);
		this.toggle = null;
		this.reduced  = false;
		this.opening  = false;
		this.options = Object.assign({
			position:            'bottomLeft',
			edge:                'topLeft',
			effect:              'fade',
			subMenuPosition:     'topRight',
			subMenuEdge:         'topLeft',
			responsiveToggle:    '',
			responsivePosition:  'bottomRight',
			responsiveEdge:      'topRight',
			mode:                'pulldown',
			inlineSubSubMenus:   false
		}, options || {});

		var menu = this;

		if (this.options.responsiveToggle != '')
		{
			this.toggle = $el(this.options.responsiveToggle);
			if (this.toggle)
			{
				this.toggle.addEventListener('click', function() { menu.toggleResponsiveMenu(); });
			}
		}

		if (document.focusWatcher)
		{
			document.focusWatcher.addEvent('focusChanged', function()
			{
				menu.updateFocus(document.focusWatcher.focus);
			});
		}

		if (!this.root) return;

		// Suckerfish-style dropdown for mouseovers
		document.querySelectorAll("#" + this.root.id + " > ul > li").forEach(function(elt)
		{
			var uls = Array.from(elt.querySelectorAll('ul'));

			uls.forEach(function(ul)
			{
				if (menu.options.effect == 'fade')
				{
					ul.style.opacity = '0';
				}

				var parent = ul.parentElement;
				var sub = (parent.parentElement.parentElement.id == menu.root.id);
				parent.classList.add(sub ? "submenu" : "subsubmenu");

				if (!sub && menu.options.inlineSubSubMenus) return;

				parent.addEventListener('touchstart', function(event)
				{
					event.preventDefault();
					event.stopPropagation();

					if (menu.reduced && menu.isShown(parent))
					{
						menu.hideMenu(parent);
						return;
					}
					menu.clearFocus();
					menu.showMenu(parent);

					if (!parent.blockClick)
					{
						parent.blockClick = function(event)
						{
							event.preventDefault();
							event.stopPropagation();
							return false;
						};
						parent.addEventListener('click', parent.blockClick);

						ul.querySelectorAll('a').forEach(function(child)
						{
							child.addEventListener('touchend', function(event)
							{
								event.preventDefault();
								event.stopPropagation();
								menu.clearFocus();
								go(child.href);
							});
						});
					}
				});

				parent.addEventListener('mouseover', function()
				{
					if (!menu.reduced && menu.options.mode == 'pulldown')
					{
						menu.showMenu(parent);
					}
					else
					{
						if (ul.style.display == "none")
						{
							menu.opening = true;
						}
					}
				});

				parent.addEventListener('mouseout', function()
				{
					menu.opening = false;
					if (!menu.reduced && menu.options.mode == 'pulldown') menu.hideMenu(parent);
				});

				Array.from(parent.children).filter(function(c) { return c.tagName == 'A'; }).forEach(function(a)
				{
					a.addEventListener('click', function()
					{
						var style = ul.style.display;
						if (document.body.classList.contains('safari') && menu.options.mode == 'accordion')
						{
							// Safari can't handle focus changes on links
							menu.updateFocus(a);
							return false;
						}
						else
						{
							return (style != 'none' && !menu.opening);
						}
					});
				});
			});
		});
	}

	isShown(elt)
	{
		return elt.classList.contains("sfhover");
	}

	showMenu(elt)
	{
		if (elt.classList.contains('subsubmenu') && this.options.inlineSubSubMenus) return;

		var ul = elt.querySelector('ul');
		elt.classList.add("sfhover");

		if (ul)
		{
			var offset = this.options.position;
			var edge   = this.options.edge;

			if (elt.classList.contains("subsubmenu"))
			{
				offset = this.options.subMenuPosition;
				edge   = this.options.subMenuEdge;
			}

			if (!this.reduced && this.options.mode == 'pulldown')
			{
				positionRelative(ul, { relativeTo: elt, position: offset, edge: edge });

				var winWidth = window.innerWidth;
				var coords   = ul.getBoundingClientRect();
				if (coords.right > winWidth)
				{
					var sbarWidth = 0;
					try
					{
						if (window.scrollbars && window.scrollbars.visible &&
						    !/mac/i.test(navigator.platform))
						    {
							sbarWidth = 17;
						}
					} catch(e) {}
					ul.style.left = (ul.offsetLeft - (coords.right - winWidth) - sbarWidth) + 'px';
				}
			}

			if (this.options.effect == 'fade')
			{
				fadeIn(ul);
			}
			else if (this.options.effect == 'reveal')
			{
				reveal(ul);
			}
		}
	}

	hideMenu(elt)
	{
		if (elt.classList.contains('subsubmenu') && this.options.inlineSubSubMenus) return;

		var ul = elt.querySelector('ul');
		elt.classList.remove("sfhover");

		if (ul)
		{
			if ((this.reduced || this.options.mode == 'accordion') &&
			    (this.options.effect == 'fade' || this.options.effect == 'reveal'))
			    {
				ul.style.opacity = '0';
			}

			if (this.reduced || this.options.mode == 'accordion') return;

			ul.style.left = '-10000px';
		}
	}

	updateFocus(elt)
	{
		if (!this.root) return;
		this.clearFocus();
		if (!this.root.contains(elt)) return;
		this.showMenu(elt.parentElement);
	}

	clearFocus()
	{
		if (!this.root) return;
		var self = this;
		Array.from(this.root.querySelectorAll("ul > li")).forEach(function(elt)
		{
			var focusEl = document.focusWatcher ? document.focusWatcher.focus : null;
			if (!focusEl || !elt.contains(focusEl)) self.hideMenu(elt);
		});
	}

	toggleResponsiveMenu()
	{
		this.reduced = true;

		if (this.root.classList.contains("sfhover"))
		{
			this.root.classList.remove("sfhover");
			this.root.style.display = 'none';
		}
		else
		{
			this.root.classList.add("sfhover");

			if (this.options.responsivePosition != 'fixed')
			{
				positionRelative(this.root, {
					relativeTo: this.toggle,
					position:   this.options.responsivePosition,
					edge:       this.options.responsiveEdge
				});
			}

			this.root.style.display = 'block';

			if (this.options.effect == 'fade')
			{
				fadeIn(this.root);
			}
			else if (this.options.effect == 'reveal')
			{
				reveal(this.root);
			}
		}
	}
}
