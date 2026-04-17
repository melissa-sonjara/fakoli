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
 * FxSlide — slide an element in and out of view by animating its clipping
 * dimension (height or width) via a wrapper div.
 *
 * ES equivalent of MooTools More Fx.Slide.
 */
class FxSlide
{
	/**
	 * @param {HTMLElement} element
	 * @param {Object} [options]
	 * @param {string}   options.mode         'vertical' | 'horizontal' (default 'vertical').
	 * @param {boolean}  options.resetHeight  Reset wrapper height to '' after fully opening (default false).
	 * @param {number}   options.duration     Transition duration ms (default 300).
	 * @param {string}   options.easing       CSS easing (default 'ease-in-out').
	 * @param {Function} options.onStart
	 * @param {Function} options.onComplete
	 */
	constructor(element, options)
	{
		this.element = element;
		this.options = Object.assign(
			{
				mode:        'vertical',
				resetHeight: false,
				duration:    300,
				easing:      'ease-in-out',
				onStart:     null,
				onComplete:  null
			},
			options || {}
		);

		this.open = true;
		this._running = false;

		this._buildWrapper();
	}

	/** @private Wrap the element in a clipping div if not already wrapped. */
	_buildWrapper()
	{
		var el     = this.element;
		var parent = el.parentNode;

		// Retrieve or create a wrapper
		if (el._fxSlideWrapper)
		{
			this.wrapper = el._fxSlideWrapper;
			return;
		}

		var cs     = window.getComputedStyle(el);
		var wrapper = document.createElement('div');

		// Copy margin + positioning from the element to the wrapper
		wrapper.style.margin   = cs.margin;
		wrapper.style.position = cs.position === 'static' ? '' : cs.position;
		wrapper.style.overflow = 'hidden';

		parent.insertBefore(wrapper, el);
		wrapper.appendChild(el);

		// Reset element margin (wrapper carries it now)
		el.style.margin = '0';
		if (cs.overflow === 'visible') el.style.overflow = 'hidden';

		el._fxSlideWrapper = wrapper;
		this.wrapper       = wrapper;
	}

	/** @private Set the layout mode properties. */
	_setMode(mode)
	{
		if (mode === 'horizontal')
		{
			this._margin = 'marginLeft';
			this._layout = 'width';
			this._offset = this.element.offsetWidth;
		}
		else
		{
			this._margin = 'marginTop';
			this._layout = 'height';
			this._offset = this.element.offsetHeight;
		}
	}

	/** @private Animate from current values to target. */
	_animate(fromMargin, fromLayout, toMargin, toLayout, callback)
	{
		if (this._running) return;
		this._running = true;

		var el      = this.element;
		var wrapper = this.wrapper;
		var opts    = this.options;
		var layout  = this._layout;
		var margin  = this._margin;
		var dur     = opts.duration;
		var easing  = opts.easing;

		// Set explicit start values
		el.style[margin]       = fromMargin + 'px';
		wrapper.style[layout]  = fromLayout + 'px';

		if (opts.onStart) opts.onStart.call(this);

		var self = this;

		var finish = function()
		{
			wrapper.removeEventListener('transitionend', finish);
			self._running = false;
			self.open = (wrapper[layout === 'height' ? 'offsetHeight' : 'offsetWidth'] !== 0);
			if (self.open && opts.resetHeight) wrapper.style.height = '';
			if (opts.onComplete) opts.onComplete.call(self);
			if (callback) callback();
		};

		wrapper.addEventListener('transitionend', finish);

		requestAnimationFrame(function()
		{
			el.style.transition      = margin  + ' ' + dur + 'ms ' + easing;
			wrapper.style.transition = layout  + ' ' + dur + 'ms ' + easing;
			el.style[margin]         = toMargin + 'px';
			wrapper.style[layout]    = toLayout + 'px';
		});
	}

	/**
	 * Slide the element in (make visible).
	 * @param {string} [mode]  'vertical' | 'horizontal' (overrides constructor default).
	 */
	slideIn(mode)
	{
		this._setMode(mode || this.options.mode);
		var layout = this._layout;
		var margin = this._margin;
		var currentLayout = parseInt(this.wrapper.style[layout]) || 0;
		var currentMargin = parseInt(this.element.style[margin]) || -this._offset;
		this._animate(currentMargin, currentLayout, 0, this._offset);
		return this;
	}

	/**
	 * Slide the element out (hide it).
	 * @param {string} [mode]
	 */
	slideOut(mode)
	{
		this._setMode(mode || this.options.mode);
		var layout = this._layout;
		var margin = this._margin;
		var currentLayout = parseInt(this.wrapper.style[layout]) || this._offset;
		var currentMargin = parseInt(this.element.style[margin]) || 0;
		this._animate(currentMargin, currentLayout, -this._offset, 0);
		return this;
	}

	/**
	 * Toggle between visible and hidden.
	 * @param {string} [mode]
	 */
	toggle(mode)
	{
		if (this.open) this.slideOut(mode);
		else           this.slideIn(mode);
		return this;
	}

	/**
	 * Immediately hide (no animation).
	 * @param {string} [mode]
	 */
	hide(mode)
	{
		this._setMode(mode || this.options.mode);
		this.element.style[this._margin]     = -this._offset + 'px';
		this.wrapper.style[this._layout]     = '0px';
		this.element.style.transition        = '';
		this.wrapper.style.transition        = '';
		this.open = false;
		return this;
	}

	/**
	 * Immediately show (no animation).
	 * @param {string} [mode]
	 */
	show(mode)
	{
		this._setMode(mode || this.options.mode);
		this.element.style[this._margin]     = '0px';
		this.wrapper.style[this._layout]     = this._offset + 'px';
		this.element.style.transition        = '';
		this.wrapper.style.transition        = '';
		this.open = true;
		return this;
	}

	/** Set options after construction. Returns this. */
	setOptions(options)
	{
		Object.assign(this.options, options || {});
		return this;
	}

	/** Cancel any running animation. */
	cancel()
	{
		this._running = false;
		var layout = this._layout || 'height';
		var margin = this._margin || 'marginTop';
		this.element.style.transition = '';
		this.wrapper.style.transition = '';
		return this;
	}
}

// ── Element.prototype extensions ──────────────────────────────────────────

/** Lazy-create an FxSlide for this element. */
Element.prototype._getFxSlide = function()
{
	if (!this._fxSlide) this._fxSlide = new FxSlide(this, { link: 'cancel' });
	return this._fxSlide;
};

/**
 * Slide this element in/out/toggle/hide/show.
 * @param {string} [how]   'in' | 'out' | 'toggle' | 'hide' | 'show' (default 'toggle').
 * @param {string} [mode]  'vertical' | 'horizontal'.
 */
Element.prototype.slide = function(how, mode)
{
	how = how || 'toggle';
	var slide = this._getFxSlide();
	switch (how)
	{
		case 'hide':   slide.hide(mode);   break;
		case 'show':   slide.show(mode);   break;
		case 'in':     slide.slideIn(mode); break;
		case 'out':    slide.slideOut(mode); break;
		case 'toggle': slide.toggle(mode); break;
		default:       slide.slideIn(mode);
	}
	return this;
};
