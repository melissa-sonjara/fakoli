/**
 * Toggles an element between its normal width and the full viewport width
 * using a CSS elastic animation, controlled by a button with hover images.
 */
class Sproing
{
	constructor(element, button, options)
	{
		this.element     = $el(element);
		this.button      = $el(button);
		this.subElements = [];
		this.options = Object.assign({
			margin:     0,
			width:      960,
			in_image:   "",
			in_hover:   "",
			out_image:  "",
			out_hover:  "",
			sproinged:  false,
			additional: []
		}, options || {});

		var self = this;
		this.button.addEventListener('click',     function() { self.toggleSproing(); });
		this.button.addEventListener('mouseover', function() { self.overSproinger(); });
		this.button.addEventListener('mouseout',  function() { self.outSproinger(); });

		this.options.additional.forEach(function(elt)
		{
			elt = $el(elt);
			var w = elt.offsetWidth;
			self.subElements.push({ difference: self.options.width - w, element: elt });
		});
	}

	toggleSproing()
	{
		var self = this;
		if (this.options.sproinged)
		{
			this._animateWidth(this.element, this.options.width);
			this.subElements.forEach(function(sub)
			{
				self._animateWidth(sub.element, self.options.width - sub.difference);
			});
			setStyles(this.element, { 'margin-left': 'auto', 'margin-right': 'auto' });
			this.options.sproinged = false;
			this.button.src = this.options.in_image;
		}
		else
		{
			var w = (window.innerWidth || document.documentElement.clientWidth) - (this.options.margin * 2);
			this._animateWidth(this.element, w);
			this.subElements.forEach(function(sub)
			{
				self._animateWidth(sub.element, w - sub.difference);
			});
			this.options.sproinged = true;
			this.button.src = this.options.out_image;
		}
	}

	_animateWidth(el, targetWidth)
	{
		// CSS transition-based elastic width animation replacing Fx.Tween + Elastic easing
		el.style.transition = 'width 600ms cubic-bezier(0.68, -0.55, 0.27, 1.55)';
		el.style.width = targetWidth + 'px';
	}

	overSproinger()
	{
		this.button.src = this.options.sproinged ? this.options.out_hover : this.options.in_hover;
	}

	outSproinger()
	{
		this.button.src = this.options.sproinged ? this.options.out_image : this.options.in_image;
	}
}
