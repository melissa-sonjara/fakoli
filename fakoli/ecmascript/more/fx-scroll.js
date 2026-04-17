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
 * FxScroll — smoothly scroll any scrollable element or the window.
 *
 * ES equivalent of MooTools More Fx.Scroll + Fx.SmoothScroll.
 *
 * Uses requestAnimationFrame with a quadratic easing curve; no dependency on
 * the MooTools Fx engine.
 */
class FxScroll
{
	/**
	 * @param {HTMLElement|Window} element  Scrollable element (default document.body/window).
	 * @param {Object} [options]
	 * @param {number}   options.duration     Scroll duration in ms (default 500).
	 * @param {Object}   options.offset       {x, y} pixel offset added to the target position.
	 * @param {boolean}  options.wheelStops   Cancel scroll on mouse-wheel (default true).
	 * @param {Function} options.onStart
	 * @param {Function} options.onComplete
	 * @param {Function} options.onCancel
	 */
	constructor(element, options)
	{
		this.element = element || document.body;
		this.options = Object.assign(
			{
				duration:   500,
				offset:     { x: 0, y: 0 },
				wheelStops: true,
				onStart:    null,
				onComplete: null,
				onCancel:   null
			},
			options || {}
		);

		this._raf       = null;
		this._cancelled = false;

		if (this.options.wheelStops)
		{
			var self = this;
			this.element.addEventListener('wheel', function()
			{
				self.cancel();
			}, { passive: true });
		}
	}

	/** @private True when scrolling the root document. */
	_isBody()
	{
		var el = this.element;
		return el === document.body || el === document.documentElement || el === window;
	}

	/** @private Current scroll position of the managed element. */
	_getScroll()
	{
		if (this._isBody()) return { x: window.scrollX || window.pageXOffset, y: window.scrollY || window.pageYOffset };
		return { x: this.element.scrollLeft, y: this.element.scrollTop };
	}

	/** @private Total scrollable size of the element. */
	_getScrollSize()
	{
		if (this._isBody())
		{
			return {
				x: Math.max(document.documentElement.scrollWidth,  document.body.scrollWidth),
				y: Math.max(document.documentElement.scrollHeight, document.body.scrollHeight)
			};
		}
		return { x: this.element.scrollWidth, y: this.element.scrollHeight };
	}

	/** @private Viewport size of the element. */
	_getSize()
	{
		if (this._isBody()) return { x: window.innerWidth, y: window.innerHeight };
		return { x: this.element.clientWidth, y: this.element.clientHeight };
	}

	/** @private Apply a scroll position. */
	_scrollTo(x, y)
	{
		if (this._isBody()) window.scrollTo(x, y);
		else
		{
			this.element.scrollLeft = x;
			this.element.scrollTop  = y;
		}
	}

	/** @private Quadratic ease-in-out. */
	_ease(t)
	{
		return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
	}

	/**
	 * Animate scroll to the target position.
	 * @param {number} x  Target scrollLeft (or scrollX).
	 * @param {number} y  Target scrollTop (or scrollY).
	 * @returns {FxScroll} this
	 */
	start(x, y)
	{
		if (this._raf !== null) cancelAnimationFrame(this._raf);
		this._cancelled = false;

		var scroll   = this._getScroll();
		var fromX    = scroll.x;
		var fromY    = scroll.y;
		var toX      = x + (this.options.offset.x || 0);
		var toY      = y + (this.options.offset.y || 0);
		var duration = this.options.duration;
		var start    = null;
		var self     = this;

		if (this.options.onStart) this.options.onStart.call(this);

		var step = function(timestamp)
		{
			if (self._cancelled) return;
			if (!start) start = timestamp;
			var elapsed = timestamp - start;
			var progress = Math.min(elapsed / duration, 1);
			var eased    = self._ease(progress);

			self._scrollTo(
				fromX + (toX - fromX) * eased,
				fromY + (toY - fromY) * eased
			);

			if (progress < 1)
			{
				self._raf = requestAnimationFrame(step);
			}
			else
			{
				self._raf = null;
				if (self.options.onComplete) self.options.onComplete.call(self);
			}
		};

		this._raf = requestAnimationFrame(step);
		return this;
	}

	/** Cancel an in-progress animation. */
	cancel()
	{
		if (this._raf !== null)
		{
			cancelAnimationFrame(this._raf);
			this._raf       = null;
			this._cancelled = true;
			if (this.options.onCancel) this.options.onCancel.call(this);
		}
		return this;
	}

	/** @private Resolve an x/y pair, replacing nullish values with current scroll. */
	_calculateScroll(x, y)
	{
		var scroll     = this._getScroll();
		var scrollSize = this._getScrollSize();
		var size       = this._getSize();

		var rx = (x == null) ? scroll.x : (typeof x === 'number' ? x : scrollSize.x - size.x);
		var ry = (y == null) ? scroll.y : (typeof y === 'number' ? y : scrollSize.y - size.y);
		return { x: rx, y: ry };
	}

	/** Scroll to the top of the element. */
	toTop()
	{
		var pos = this._calculateScroll(null, 0);
		return this.start(pos.x, pos.y);
	}

	/** Scroll to the bottom. */
	toBottom()
	{
		var pos = this._calculateScroll(null, 'bottom');
		return this.start(pos.x, pos.y);
	}

	/** Scroll to the left edge. */
	toLeft()
	{
		var pos = this._calculateScroll(0, null);
		return this.start(pos.x, pos.y);
	}

	/** Scroll to the right edge. */
	toRight()
	{
		var pos = this._calculateScroll('right', null);
		return this.start(pos.x, pos.y);
	}

	/**
	 * Scroll so that `el` is at the top-left of the container.
	 * @param {HTMLElement} el
	 * @param {string[]}    [axes]  Axes to scroll (default ['x','y']).
	 */
	toElement(el, axes)
	{
		axes = axes || ['x', 'y'];
		var scroll  = this._getScroll();
		var rect    = el.getBoundingClientRect();
		var contRect = this._isBody()
			? { left: 0, top: 0 }
			: this.element.getBoundingClientRect();

		var tx = axes.indexOf('x') >= 0 ? rect.left - contRect.left + scroll.x : scroll.x;
		var ty = axes.indexOf('y') >= 0 ? rect.top  - contRect.top  + scroll.y : scroll.y;
		return this.start(tx, ty);
	}

	/**
	 * Scroll just enough to make `el` fully visible (edge-to-edge).
	 * @param {HTMLElement} el
	 * @param {string[]}    [axes]
	 * @param {Object}      [offset]  {x, y} additional pixel offset.
	 */
	toElementEdge(el, axes, offset)
	{
		axes   = axes   || ['x', 'y'];
		offset = offset || { x: 0, y: 0 };

		var scroll    = this._getScroll();
		var size      = this._getSize();
		var rect      = el.getBoundingClientRect();
		var contRect  = this._isBody() ? { left: 0, top: 0 } : this.element.getBoundingClientRect();

		var elLeft   = rect.left - contRect.left + scroll.x;
		var elTop    = rect.top  - contRect.top  + scroll.y;
		var elRight  = elLeft + rect.width;
		var elBottom = elTop  + rect.height;

		var tx = scroll.x;
		var ty = scroll.y;

		if (axes.indexOf('x') >= 0)
		{
			if (elRight  > scroll.x + size.x) tx = elRight  - size.x;
			if (elLeft   < scroll.x)           tx = elLeft;
			tx += (offset.x || 0);
		}
		if (axes.indexOf('y') >= 0)
		{
			if (elBottom > scroll.y + size.y) ty = elBottom - size.y;
			if (elTop    < scroll.y)           ty = elTop;
			ty += (offset.y || 0);
		}

		if (tx !== scroll.x || ty !== scroll.y) this.start(tx, ty);
		return this;
	}

	/**
	 * Scroll so that `el` is centred in the container.
	 * @param {HTMLElement} el
	 * @param {string[]}    [axes]
	 * @param {Object}      [offset]
	 */
	toElementCenter(el, axes, offset)
	{
		axes   = axes   || ['x', 'y'];
		offset = offset || { x: 0, y: 0 };

		var scroll   = this._getScroll();
		var size     = this._getSize();
		var rect     = el.getBoundingClientRect();
		var contRect = this._isBody() ? { left: 0, top: 0 } : this.element.getBoundingClientRect();

		var elLeft = rect.left - contRect.left + scroll.x;
		var elTop  = rect.top  - contRect.top  + scroll.y;

		var tx = scroll.x;
		var ty = scroll.y;

		if (axes.indexOf('x') >= 0) tx = elLeft - (size.x - rect.width)  / 2 + (offset.x || 0);
		if (axes.indexOf('y') >= 0) ty = elTop  - (size.y - rect.height) / 2 + (offset.y || 0);

		if (tx !== scroll.x || ty !== scroll.y) this.start(tx, ty);
		return this;
	}
}

// ── FxSmoothScroll ────────────────────────────────────────────────────────

/**
 * FxSmoothScroll — attach smooth scrolling to all same-page anchor links.
 *
 * ES equivalent of MooTools More Fx.SmoothScroll.
 */
class FxSmoothScroll extends FxScroll
{
	/**
	 * @param {Object}      [options]        Passed to FxScroll.
	 * @param {string[]}    [options.links]  CSS selector for links (default 'a[href]').
	 * @param {string[]}    [options.axes]   Default ['x', 'y'].
	 * @param {Document}    [context]        Document to search (default current document).
	 */
	constructor(options, context)
	{
		options = options || {};
		var doc = context || document;

		super(doc.body, options);

		this.doc    = doc;
		this._axes  = options.axes || ['x', 'y'];
		this.anchor = null;

		this._attach(options.links);
	}

	/** @private Bind click handlers to matching anchor links. */
	_attach(selector)
	{
		var links   = Array.from(this.doc.querySelectorAll(selector || 'a[href]'));
		var baseUrl = window.location.href.split('#')[0] + '#';
		var self    = this;

		links.forEach(function(link)
		{
			var href = link.getAttribute('href') || '';
			if (href.indexOf('#') !== 0 && href.indexOf(baseUrl) !== 0) return;
			var anchor = href.charAt(0) === '#' ? href.slice(1) : href.slice(baseUrl.length);
			if (!anchor) return;
			self.useLink(link, anchor);
		});
	}

	/**
	 * Attach smooth-scroll behaviour to a specific link.
	 * @param {HTMLAnchorElement} link
	 * @param {string}            anchor  Target anchor id.
	 */
	useLink(link, anchor)
	{
		var self = this;
		link.addEventListener('click', function(event)
		{
			var target = self.doc.getElementById(anchor) || self.doc.querySelector('[name="' + anchor + '"]');
			if (!target) return;
			event.preventDefault();
			self.anchor = anchor;
			self.toElement(target, self._axes);
		});
		return this;
	}
}
