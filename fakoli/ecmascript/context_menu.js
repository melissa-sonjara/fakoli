/**
 * Attaches a context menu (right-click or custom trigger) to one or more
 * elements and positions it at the pointer or relative to the target element.
 */
class ContextMenu
{
	constructor(menu, elementSelector, trigger, options)
	{
		this.elements = [];
		this.menu = $el(menu);
		this.trigger = trigger || 'contextmenu';
		this.options = Object.assign({
			position: 'pointer',
			offsetX: 0,
			offsetY: 0
		}, options || {});

		if (!this.menu) return;

		var me = this;

		if (!Array.isArray(elementSelector)) elementSelector = [elementSelector];
		elementSelector.forEach(function(selector)
		{
			document.querySelectorAll(selector).forEach(function(elt)
			{
				elt.addEventListener(trigger, function(e)
				{
					ContextMenu.root = elt;
					e.preventDefault();
					e.stopPropagation();
					me.show(e);
				});
			});
		});

		var doc = document.body || document.documentElement;
		doc.addEventListener('click', function() { me.hide(); });

		// Detach from current parent and re-attach to document root
		if (this.menu.parentNode) this.menu.parentNode.removeChild(this.menu);
		doc.appendChild(this.menu);
	}

	show(event)
	{
		var coords = getCoordinates(ContextMenu.root);
		var bc = getCoordinates(document.body);

		var left = (event && this.options.position == 'pointer') ? event.pageX : coords.left;
		var top  = (event && this.options.position == 'pointer') ? event.pageY : coords.bottom - bc.top;

		setStyles(this.menu, {
			top:      (top  + this.options.offsetY) + 'px',
			left:     (left + this.options.offsetX) + 'px',
			display:  'block',
			opacity:  '0',
			'z-index': '500'
		});
		fadeIn(this.menu);
	}

	hide()
	{
		if (this.menu)
		{
			this.menu.style.display = 'none';
			this.menu.style.opacity = '0';
		}
	}
}

// Static global for root element of displayed menu
ContextMenu.root = null;
