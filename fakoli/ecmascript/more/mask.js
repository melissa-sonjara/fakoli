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

/**
 * IframeShim — positions an invisible iframe behind another element to cover
 * windowed controls (plugins, legacy select elements) in older browsers.
 * In modern browsers this is effectively a no-op.
 *
 * ES equivalent of MooTools More IframeShim.
 */
class IframeShim
{
	/**
	 * @param {HTMLElement} element
	 * @param {Object} [options]
	 * @param {number}  options.zIndex   z-index for the shim iframe.
	 * @param {number}  options.margin   Inset margin in px (default 0).
	 * @param {Object}  options.offset   {x, y} additional pixel offset.
	 */
	constructor(element, options)
	{
		options = options || {};
		this.element = element;
		this.zIndex  = options.zIndex != null ? options.zIndex : null;
		this.margin  = options.margin || 0;
		this.offset  = options.offset || { x: 0, y: 0 };
		this._shim   = null;

		this._build();
	}

	/** @private Create the iframe shim. */
	_build()
	{
		var iframe = document.createElement('iframe');
		iframe.src         = 'about:blank';
		iframe.scrolling   = 'no';
		iframe.frameBorder = '0';
		iframe.style.cssText = 'position:absolute;border:none;display:none;';

		var elZ = parseInt(window.getComputedStyle(this.element).zIndex, 10) || 1;
		var z   = (this.zIndex != null && elZ > this.zIndex) ? this.zIndex : elZ - 1;
		if (z < 0) z = 1;
		iframe.style.zIndex = z;

		this.element.parentNode.insertBefore(iframe, this.element.nextSibling);
		this._shim = iframe;
	}

	/** Reposition the shim to cover the element. */
	position()
	{
		if (!this._shim) return this;
		var rect   = this.element.getBoundingClientRect();
		var scrollX = window.scrollX || window.pageXOffset;
		var scrollY = window.scrollY || window.pageYOffset;
		var margin  = this.margin;
		var shim    = this._shim;

		shim.style.width  = (rect.width  - margin * 2) + 'px';
		shim.style.height = (rect.height - margin * 2) + 'px';
		shim.style.left   = (rect.left   + scrollX + margin + this.offset.x) + 'px';
		shim.style.top    = (rect.top    + scrollY + margin + this.offset.y) + 'px';
		return this;
	}

	/** Show the shim. */
	show()
	{
		if (this._shim)
		{
			this._shim.style.display = 'block';
			this.position();
		}
		return this;
	}

	/** Hide the shim. */
	hide()
	{
		if (this._shim) this._shim.style.display = 'none';
		return this;
	}

	/** Remove the shim from the DOM (but keep reference). */
	dispose()
	{
		if (this._shim && this._shim.parentNode)
		{
			this._shim.parentNode.removeChild(this._shim);
		}
		return this;
	}

	/** Remove and nullify the shim. */
	destroy()
	{
		this.dispose();
		this._shim = null;
		return this;
	}
}

/**
 * Mask — creates a semi-transparent overlay element covering another element
 * or the full page.
 *
 * ES equivalent of MooTools More Mask.
 */
class Mask
{
	/**
	 * @param {HTMLElement} [target]   Element to mask (default document.body).
	 * @param {Object}      [options]
	 * @param {string}   options.class           CSS class for the mask div (default 'mask').
	 * @param {Object}   options.style           Additional inline styles for the mask.
	 * @param {boolean}  options.maskMargins     Include target's margins in size (default false).
	 * @param {boolean}  options.destroyOnHide   Remove mask from DOM on hide (default false).
	 * @param {boolean}  options.hideOnClick     Hide mask when clicked (default false).
	 * @param {Function} options.onShow
	 * @param {Function} options.onHide
	 * @param {Function} options.onDestroy
	 * @param {Function} options.onClick
	 */
	constructor(target, options)
	{
		options = options || {};
		this.target          = target || document.body;
		this.cssClass        = options['class']        || 'mask';
		this.extraStyle      = options.style           || {};
		this.maskMargins     = options.maskMargins     || false;
		this.destroyOnHide   = options.destroyOnHide   || false;
		this.hideOnClick     = options.hideOnClick     || false;
		this.onShow          = options.onShow          || null;
		this.onHide          = options.onHide          || null;
		this.onDestroy       = options.onDestroy       || null;
		this.onClick         = options.onClick         || null;

		this._hidden     = true;
		this._element    = null;
		this._onResize   = this.position.bind(this);

		this._build();

		// Store on target
		this.target._mask = this;
	}

	/** @private Create the mask div and insert it. */
	_build()
	{
		var div = document.createElement('div');
		div.className = this.cssClass;
		div.style.display    = 'none';
		div.style.position   = 'absolute';
		div.style.top        = '0';
		div.style.left       = '0';

		Object.keys(this.extraStyle).forEach(function(prop)
		{
			div.style[prop] = this.extraStyle[prop];
		}, this);

		var self = this;
		div.addEventListener('click', function(event)
		{
			if (self.onClick) self.onClick.call(self, event);
			if (self.hideOnClick) self.hide();
		});

		this._element = div;

		if (this.target === document.body)
		{
			document.body.appendChild(div);
		}
		else
		{
			var cs = window.getComputedStyle(this.target);
			if (cs.position === 'static') this.target.style.position = 'relative';
			this.target.insertBefore(div, this.target.firstChild);
		}
	}

	/** Resize and reposition the mask over the target. */
	position()
	{
		var el     = this._element;
		var target = this.target;

		if (target === document.body)
		{
			var scrollW = Math.max(document.documentElement.scrollWidth,  document.body.scrollWidth,  window.innerWidth);
			var scrollH = Math.max(document.documentElement.scrollHeight, document.body.scrollHeight, window.innerHeight);
			el.style.width  = scrollW + 'px';
			el.style.height = scrollH + 'px';
			el.style.top    = '0';
			el.style.left   = '0';
		}
		else
		{
			var cs     = window.getComputedStyle(target);
			var width  = target.offsetWidth;
			var height = target.offsetHeight;

			if (this.maskMargins)
			{
				width  += (parseFloat(cs.marginLeft)  || 0) + (parseFloat(cs.marginRight)  || 0);
				height += (parseFloat(cs.marginTop)   || 0) + (parseFloat(cs.marginBottom) || 0);
			}

			el.style.width  = width  + 'px';
			el.style.height = height + 'px';
			el.style.top    = '0';
			el.style.left   = '0';
		}

		return this;
	}

	/** Show the mask. */
	show()
	{
		if (!this._hidden) return this;
		window.addEventListener('resize', this._onResize);
		this.position();
		this._element.style.display = 'block';
		this._hidden = false;
		if (this.onShow) this.onShow.call(this);
		return this;
	}

	/** Hide the mask. */
	hide()
	{
		if (this._hidden) return this;
		window.removeEventListener('resize', this._onResize);
		this._element.style.display = 'none';
		this._hidden = true;
		if (this.onHide) this.onHide.call(this);
		if (this.destroyOnHide) this.destroy();
		return this;
	}

	/** Toggle visibility. */
	toggle()
	{
		if (this._hidden) this.show();
		else              this.hide();
		return this;
	}

	/** Remove the mask element and fire onDestroy. */
	destroy()
	{
		this.hide();
		if (this._element && this._element.parentNode)
		{
			this._element.parentNode.removeChild(this._element);
		}
		if (this.onDestroy) this.onDestroy.call(this);
		delete this.target._mask;
	}
}

// ── Element.prototype helpers ─────────────────────────────────────────────

/**
 * Show a mask over this element.
 * @param {Object} [options]  Passed to Mask constructor.
 */
Element.prototype.mask = function(options)
{
	var existing = this._mask;
	if (existing && options)
	{
		existing.destroy();
		existing = null;
	}
	if (!existing) existing = new Mask(this, options);
	existing.show();
	return this;
};

/** Hide the mask over this element. */
Element.prototype.unmask = function()
{
	if (this._mask) this._mask.hide();
	return this;
};
