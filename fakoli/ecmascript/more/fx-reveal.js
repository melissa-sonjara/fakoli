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
 * FxReveal — show/hide elements with a CSS transition on height and opacity.
 *
 * ES equivalent of MooTools More Fx.Reveal.
 *
 * Depends on element-measure.js (measureElement / getComputedSize) being
 * available for measuring hidden elements before revealing them.
 */
class FxReveal
{
	/**
	 * @param {HTMLElement} element
	 * @param {Object} [options]
	 * @param {string}   options.mode            'vertical' | 'horizontal' | 'both' (default 'vertical').
	 * @param {string}   options.display         CSS display value to restore on reveal (default 'block').
	 * @param {boolean}  options.opacity         Animate opacity as well (default true).
	 * @param {number}   options.duration        Transition duration in ms (default 300).
	 * @param {string}   options.easing          CSS transition easing (default 'ease-in-out').
	 * @param {Function} options.onShow
	 * @param {Function} options.onHide
	 * @param {Function} options.onComplete
	 */
	constructor(element, options)
	{
		this.element = element;
		this.options = Object.assign(
			{
				mode:     'vertical',
				display:  null,        // auto-detected from element tag
				opacity:  true,
				duration: 300,
				easing:   'ease-in-out',
				onShow:     null,
				onHide:     null,
				onComplete: null
			},
			options || {}
		);

		this._showing = false;
		this._hiding  = false;
	}

	/** Set options after construction. Returns this. */
	setOptions(options)
	{
		Object.assign(this.options, options || {});
		return this;
	}

	/** @private Resolve the display value to use when showing the element. */
	_displayValue()
	{
		if (this.options.display) return this.options.display;
		var tag = this.element.tagName.toLowerCase();
		return (tag === 'tr') ? 'table-row' : 'block';
	}

	/**
	 * Animate the element to zero size/opacity, then hide it.
	 * @returns {FxReveal} this
	 */
	dissolve()
	{
		if (this._showing || this._hiding) return this;
		var el      = this.element;
		var opts    = this.options;
		var cs      = window.getComputedStyle(el);
		var display = cs.display;

		if (display === 'none')
		{
			// Already hidden
			if (opts.onHide)     opts.onHide.call(this, el);
			if (opts.onComplete) opts.onComplete.call(this, el);
			return this;
		}

		this._hiding = true;

		// Capture dimensions before transitioning
		var measured = getComputedSize(el, { mode: opts.mode });
		var savedCss = el.style.cssText;

		// Build start state
		el.style.overflow   = 'hidden';
		el.style.display    = display;

		// Collect the CSS properties we'll transition to zero
		var props = [];
		if (opts.mode !== 'horizontal')
		{
			props.push('height', 'padding-top', 'padding-bottom', 'border-top-width', 'border-bottom-width', 'margin-top', 'margin-bottom');
		}
		if (opts.mode !== 'vertical')
		{
			props.push('width', 'padding-left', 'padding-right', 'border-left-width', 'border-right-width', 'margin-left', 'margin-right');
		}
		if (opts.opacity) props.push('opacity');

		// Set explicit current values so browser can animate from them
		if (opts.mode !== 'horizontal')
		{
			el.style.height           = el.offsetHeight + 'px';
			el.style.paddingTop       = cs.paddingTop;
			el.style.paddingBottom    = cs.paddingBottom;
			el.style.borderTopWidth   = cs.borderTopWidth;
			el.style.borderBottomWidth = cs.borderBottomWidth;
			el.style.marginTop        = cs.marginTop;
			el.style.marginBottom     = cs.marginBottom;
		}
		if (opts.mode !== 'vertical')
		{
			el.style.width           = el.offsetWidth + 'px';
			el.style.paddingLeft     = cs.paddingLeft;
			el.style.paddingRight    = cs.paddingRight;
			el.style.borderLeftWidth = cs.borderLeftWidth;
			el.style.borderRightWidth = cs.borderRightWidth;
			el.style.marginLeft      = cs.marginLeft;
			el.style.marginRight     = cs.marginRight;
		}
		if (opts.opacity) el.style.opacity = '1';

		el.style.transition = props.join(' 0ms, ').replace(/\s0ms/g, ' ' + opts.duration + 'ms') + ' ' + opts.duration + 'ms ' + opts.easing;

		var self = this;

		var finish = function()
		{
			el.removeEventListener('transitionend', finish);
			el.style.cssText    = savedCss;
			el.style.display    = 'none';
			self._hiding        = false;
			if (opts.onHide)     opts.onHide.call(self, el);
			if (opts.onComplete) opts.onComplete.call(self, el);
		};

		el.addEventListener('transitionend', finish);

		// Trigger transition in next frame
		requestAnimationFrame(function()
		{
			if (opts.mode !== 'horizontal')
			{
				el.style.height            = '0';
				el.style.paddingTop        = '0';
				el.style.paddingBottom     = '0';
				el.style.borderTopWidth    = '0';
				el.style.borderBottomWidth = '0';
				el.style.marginTop         = '0';
				el.style.marginBottom      = '0';
			}
			if (opts.mode !== 'vertical')
			{
				el.style.width            = '0';
				el.style.paddingLeft      = '0';
				el.style.paddingRight     = '0';
				el.style.borderLeftWidth  = '0';
				el.style.borderRightWidth = '0';
				el.style.marginLeft       = '0';
				el.style.marginRight      = '0';
			}
			if (opts.opacity) el.style.opacity = '0';
		});

		return this;
	}

	/**
	 * Show the element by animating it in from zero size/opacity.
	 * @returns {FxReveal} this
	 */
	reveal()
	{
		if (this._showing || this._hiding) return this;
		var el   = this.element;
		var opts = this.options;
		var cs   = window.getComputedStyle(el);

		if (cs.display !== 'none')
		{
			// Already visible
			if (opts.onShow)     opts.onShow.call(this, el);
			if (opts.onComplete) opts.onComplete.call(this, el);
			return this;
		}

		this._showing = true;

		var displayVal = this._displayValue();
		var savedCss   = el.style.cssText;

		// Measure target dimensions while temporarily visible
		el.style.position   = 'absolute';
		el.style.visibility = 'hidden';
		el.style.display    = displayVal;

		var targetH = el.offsetHeight;
		var targetW = el.offsetWidth;

		el.style.position   = '';
		el.style.visibility = '';
		el.style.display    = 'none';

		// Start from zero
		el.style.overflow = 'hidden';
		if (opts.mode !== 'horizontal') el.style.height = '0';
		if (opts.mode !== 'vertical')   el.style.width  = '0';
		if (opts.opacity)               el.style.opacity = '0';
		el.style.display = displayVal;

		var props = [];
		if (opts.mode !== 'horizontal') props.push('height');
		if (opts.mode !== 'vertical')   props.push('width');
		if (opts.opacity)               props.push('opacity');

		el.style.transition = props.map(function(p)
		{
			return p + ' ' + opts.duration + 'ms ' + opts.easing;
		}).join(', ');

		var self = this;

		var finish = function()
		{
			el.removeEventListener('transitionend', finish);
			el.style.cssText  = savedCss;
			el.style.display  = displayVal;
			self._showing     = false;
			if (opts.onShow)     opts.onShow.call(self, el);
			if (opts.onComplete) opts.onComplete.call(self, el);
		};

		el.addEventListener('transitionend', finish);

		requestAnimationFrame(function()
		{
			if (opts.mode !== 'horizontal') el.style.height  = targetH + 'px';
			if (opts.mode !== 'vertical')   el.style.width   = targetW + 'px';
			if (opts.opacity)               el.style.opacity = '1';
		});

		return this;
	}

	/** Toggle between revealed and dissolved. */
	toggle()
	{
		var cs = window.getComputedStyle(this.element);
		if (cs.display === 'none') this.reveal();
		else                       this.dissolve();
		return this;
	}

	/** Cancel any in-progress animation, restoring the saved style. */
	cancel()
	{
		this._showing = false;
		this._hiding  = false;
		return this;
	}
}

// ── Element.prototype extensions ──────────────────────────────────────────

/** Lazy-create a FxReveal for this element; return it. */
Element.prototype._getFxReveal = function()
{
	if (!this._fxReveal) this._fxReveal = new FxReveal(this);
	return this._fxReveal;
};

/**
 * Reveal (animate in) this element.
 * @param {Object} [options]
 */
Element.prototype.reveal = function(options)
{
	this._getFxReveal().setOptions(options).reveal();
	return this;
};

/**
 * Dissolve (animate out) this element.
 * @param {Object} [options]
 */
Element.prototype.dissolve = function(options)
{
	this._getFxReveal().setOptions(options).dissolve();
	return this;
};

/**
 * Dissolve this element, then remove it from the DOM.
 * @param {Object}  [options]
 * @param {boolean} [destroy=false]  If true, destroy rather than merely detach.
 */
Element.prototype.nix = function(options, destroy)
{
	var fx   = this._getFxReveal();
	var self = this;
	var orig = fx.options.onComplete;
	fx.setOptions(Object.assign({}, options, {
		onComplete: function()
		{
			if (orig) orig.call(fx, self);
			if (destroy) self.parentNode && self.parentNode.removeChild(self);
			else         self.parentNode && self.parentNode.removeChild(self);
		}
	})).dissolve();
	return this;
};

/**
 * Flash: reveal, wait `duration` ms, then dissolve.
 * @param {number} [duration=2000]
 * @param {Object} [options]
 */
Element.prototype.wink = function(duration, options)
{
	var self = this;
	var fx   = this._getFxReveal();
	var orig = fx.options.onShow;
	fx.setOptions(Object.assign({}, options, {
		onShow: function()
		{
			if (orig) orig.call(fx, self);
			setTimeout(function() { self.dissolve(options); }, duration || 2000);
		}
	})).reveal();
	return this;
};
