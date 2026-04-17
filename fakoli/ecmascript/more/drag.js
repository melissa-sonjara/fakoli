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
 * Drag — base drag class for moving / resizing elements with mouse/touch events.
 * ES equivalent of MooTools More Drag + Drag.Move.
 */

var _unDraggableTags = ['button', 'input', 'a', 'textarea', 'select', 'option'];

class Drag
{
	/**
	 * @param {HTMLElement} element   The element to be dragged.
	 * @param {Object}      [options]
	 * @param {number}   options.snap           Pixel threshold before drag starts (default 6).
	 * @param {string}   options.unit           CSS unit to append to values (default 'px').
	 * @param {number|Object} options.grid      Snap to grid (number or {x,y}).
	 * @param {boolean}  options.style          Apply values via element.style (default true).
	 * @param {Object}   options.limit          {x:[min,max], y:[min,max]} pixel limits.
	 * @param {Element}  options.handle         Drag handle element (defaults to element itself).
	 * @param {boolean}  options.invert         Invert movement direction.
	 * @param {Object}   options.modifiers      CSS property names {x:'left', y:'top'}.
	 * @param {boolean}  options.preventDefault Call preventDefault on pointer events.
	 * @param {boolean}  options.stopPropagation Call stopPropagation on pointer events.
	 * @param {Function} options.onBeforeStart
	 * @param {Function} options.onStart
	 * @param {Function} options.onSnap
	 * @param {Function} options.onDrag
	 * @param {Function} options.onCancel
	 * @param {Function} options.onComplete
	 */
	constructor(element, options)
	{
		options = options || {};
		this.element   = element;
		this.snap      = options.snap      != null ? options.snap      : 6;
		this.unit      = options.unit      || 'px';
		this.grid      = options.grid      || false;
		this.style     = options.style     !== false;
		this.limit     = options.limit     || false;
		this.invert    = options.invert    || false;
		this.modifiers = options.modifiers || { x: 'left', y: 'top' };
		this.preventDefault   = options.preventDefault   || false;
		this.stopPropagation  = options.stopPropagation  || false;

		this.onBeforeStart = options.onBeforeStart || null;
		this.onStart       = options.onStart       || null;
		this.onSnap        = options.onSnap        || null;
		this.onDrag        = options.onDrag        || null;
		this.onCancel      = options.onCancel      || null;
		this.onComplete    = options.onComplete    || null;

		// Resolve handle
		var handle = options.handle;
		if (handle)
		{
			this.handles = Array.isArray(handle) ? handle : [handle];
		}
		else
		{
			this.handles = [element];
		}

		this.mouse = { now: {}, pos: {}, start: null };
		this.value = { now: {} };
		this._limit = { x: [], y: [] };

		this._bound = {
			start:  this._start.bind(this),
			check:  this._check.bind(this),
			drag:   this._drag.bind(this),
			stop:   this._stop.bind(this),
			cancel: this._cancel.bind(this)
		};

		this.attach();
	}

	/** Attach drag event listeners to the handle element(s). */
	attach()
	{
		this.handles.forEach(function(h)
		{
			h.addEventListener('mousedown',  this._bound.start);
			h.addEventListener('touchstart', this._bound.start, { passive: false });
		}, this);
		return this;
	}

	/** Remove drag event listeners. */
	detach()
	{
		this.handles.forEach(function(h)
		{
			h.removeEventListener('mousedown',  this._bound.start);
			h.removeEventListener('touchstart', this._bound.start);
		}, this);
		return this;
	}

	/** @private Extract page coordinates from a mouse or touch event. */
	_pageCoords(event)
	{
		if (event.touches && event.touches.length)
		{
			return { x: event.touches[0].pageX, y: event.touches[0].pageY };
		}
		return { x: event.pageX, y: event.pageY };
	}

	/** @private Handle initial press. */
	_start(event)
	{
		// Ignore right-click
		if (event.button === 2) return;

		// Ignore presses on non-draggable tags unless the element itself is the target
		var tag = (event.target || event.srcElement).tagName.toLowerCase();
		if (_unDraggableTags.indexOf(tag) !== -1 && event.target !== this.element) return;

		if (this.preventDefault)  event.preventDefault();
		if (this.stopPropagation) event.stopPropagation();

		var page = this._pageCoords(event);
		this.mouse.start = page;

		if (this.onBeforeStart) this.onBeforeStart.call(this, this.element);

		// Resolve limits (allow function values)
		this._limit = { x: [], y: [] };
		if (this.limit)
		{
			for (var z in this.modifiers)
			{
				if (!this.modifiers[z] || !this.limit[z]) continue;
				for (var i = 0; i < 2; i++)
				{
					var lv = this.limit[z][i];
					if (lv != null) this._limit[z][i] = (typeof lv === 'function') ? lv() : lv;
				}
			}
		}

		// Record starting values
		for (var axis in this.modifiers)
		{
			if (!this.modifiers[axis]) continue;
			var prop = this.modifiers[axis];
			var val;
			if (this.style)
			{
				val = parseFloat(window.getComputedStyle(this.element).getPropertyValue(prop)) || 0;
			}
			else
			{
				val = this.element[prop] || 0;
			}
			if (this.invert) val *= -1;
			this.value.now[axis] = val;
			this.mouse.pos[axis] = page[axis] - val;
		}

		// Normalise grid option
		if (typeof this.grid === 'number')
		{
			this.grid = { x: this.grid, y: this.grid };
		}

		document.addEventListener('mousemove', this._bound.check);
		document.addEventListener('mouseup',   this._bound.cancel);
		document.addEventListener('touchmove', this._bound.check, { passive: false });
		document.addEventListener('touchend',  this._bound.cancel);
	}

	/** @private Check snap threshold. */
	_check(event)
	{
		if (this.preventDefault) event.preventDefault();
		var page = this._pageCoords(event);
		var dx   = page.x - this.mouse.start.x;
		var dy   = page.y - this.mouse.start.y;
		var dist = Math.round(Math.sqrt(dx * dx + dy * dy));

		if (dist > this.snap)
		{
			this._cancelListeners();

			document.addEventListener('mousemove', this._bound.drag);
			document.addEventListener('mouseup',   this._bound.stop);
			document.addEventListener('touchmove', this._bound.drag, { passive: false });
			document.addEventListener('touchend',  this._bound.stop);

			if (this.onStart) this.onStart.call(this, this.element, event);
			if (this.onSnap)  this.onSnap.call(this, this.element);
		}
	}

	/** @private Handle ongoing drag movement. */
	_drag(event)
	{
		if (this.preventDefault) event.preventDefault();
		var page = this._pageCoords(event);
		this.mouse.now = page;
		this._render();
		if (this.onDrag) this.onDrag.call(this, this.element, event);
	}

	/** @private Apply the current mouse position to element styles. */
	_render()
	{
		var grid = this.grid || {};
		for (var z in this.modifiers)
		{
			if (!this.modifiers[z]) continue;
			var val = this.mouse.now[z] - this.mouse.pos[z];
			if (this.invert) val *= -1;

			if (this.limit && this._limit[z])
			{
				if (this._limit[z][1] != null && val > this._limit[z][1]) val = this._limit[z][1];
				if (this._limit[z][0] != null && val < this._limit[z][0]) val = this._limit[z][0];
			}
			if (grid[z]) val -= ((val - (this._limit[z][0] || 0)) % grid[z]);

			this.value.now[z] = val;
			if (this.style)
			{
				this.element.style[this.modifiers[z]] = val + this.unit;
			}
			else
			{
				this.element[this.modifiers[z]] = val;
			}
		}
	}

	/** @private Remove check/cancel listeners. */
	_cancelListeners()
	{
		document.removeEventListener('mousemove', this._bound.check);
		document.removeEventListener('mouseup',   this._bound.cancel);
		document.removeEventListener('touchmove', this._bound.check);
		document.removeEventListener('touchend',  this._bound.cancel);
	}

	/** @private Cancel during snap check phase. */
	_cancel(event)
	{
		this._cancelListeners();
		if (event && this.onCancel) this.onCancel.call(this, this.element);
	}

	/** @private Handle drag end. */
	_stop(event)
	{
		document.removeEventListener('mousemove', this._bound.drag);
		document.removeEventListener('mouseup',   this._bound.stop);
		document.removeEventListener('touchmove', this._bound.drag);
		document.removeEventListener('touchend',  this._bound.stop);
		this.mouse.start = null;
		if (event && this.onComplete) this.onComplete.call(this, this.element, event);
	}
}

/**
 * Drag.Move — extends Drag with container constraints and droppable support.
 */
class DragMove extends Drag
{
	/**
	 * @param {HTMLElement} element
	 * @param {Object} [options]
	 * @param {Element}   options.container       Constrain movement to this element.
	 * @param {Element[]} options.droppables      Elements that can receive the dragged item.
	 * @param {boolean}   options.includeMargins  Include element margins in limit calc (default true).
	 * @param {boolean}   options.precalculate    Pre-compute droppable coords on drag start.
	 * @param {boolean}   options.checkDroppables Check droppables during drag.
	 * @param {Function}  options.onEnter
	 * @param {Function}  options.onLeave
	 * @param {Function}  options.onDrop
	 */
	constructor(element, options)
	{
		options = Object.assign(
			{
				modifiers:       { x: 'left', y: 'top' },
				includeMargins:  true,
				checkDroppables: true,
				precalculate:    false
			},
			options || {}
		);

		// Wrap user onDrag to also check droppables
		var userOnDrag = options.onDrag;
		var self;
		options.onDrag = function(el, event)
		{
			if (options.checkDroppables) self._checkDroppables();
			if (userOnDrag) userOnDrag.call(this, el, event);
		};

		super(element, options);
		self = this;

		this.includeMargins  = options.includeMargins;
		this.precalculate    = options.precalculate;
		this.checkDroppables = options.checkDroppables;
		this.onEnter = options.onEnter || null;
		this.onLeave = options.onLeave || null;
		this.onDrop  = options.onDrop  || null;

		this.droppables = Array.isArray(options.droppables) ? options.droppables : (options.droppables ? [options.droppables] : []);
		this._overed    = null;

		this.setContainer(options.container);

		// Make element absolute if needed
		var cs = window.getComputedStyle(element);
		if (cs.position === 'static') element.style.position = 'absolute';
	}

	/** Set or clear the drag container. */
	setContainer(container)
	{
		this._container = container || null;
	}

	/** @private Override _start to set limits from container. */
	_start(event)
	{
		if (this._container) this.limit = this._calculateLimit();

		if (this.precalculate)
		{
			this._positions = this.droppables.map(function(el)
			{
				return el.getBoundingClientRect();
			});
		}

		super._start(event);
		if (this.checkDroppables) this._checkDroppables();
	}

	/** @private Compute movement limits from the container element. */
	_calculateLimit()
	{
		var el        = this.element;
		var container = this._container;
		var cRect     = container.getBoundingClientRect();
		var eRect     = el.getBoundingClientRect();
		var cs        = window.getComputedStyle(el);
		var ccs       = window.getComputedStyle(container);

		var marginL  = parseFloat(cs.marginLeft)         || 0;
		var marginR  = parseFloat(cs.marginRight)        || 0;
		var marginT  = parseFloat(cs.marginTop)          || 0;
		var marginB  = parseFloat(cs.marginBottom)       || 0;
		var borderR  = parseFloat(ccs.borderRightWidth)  || 0;
		var borderB  = parseFloat(ccs.borderBottomWidth) || 0;

		var scrollX = window.scrollX || window.pageXOffset;
		var scrollY = window.scrollY || window.pageYOffset;

		var left = cRect.left + scrollX + (this.includeMargins ? marginL : 0);
		var top  = cRect.top  + scrollY + (this.includeMargins ? marginT : 0);
		var right  = cRect.right  + scrollX - borderR - eRect.width  - (this.includeMargins ? 0 : marginR);
		var bottom = cRect.bottom + scrollY - borderB - eRect.height - (this.includeMargins ? 0 : marginB);

		return { x: [left, right], y: [top, bottom] };
	}

	/** @private Get position of a droppable (adjusting for fixed positioning). */
	_getDroppableCoords(el)
	{
		var rect    = el.getBoundingClientRect();
		var scrollX = window.scrollX || window.pageXOffset;
		var scrollY = window.scrollY || window.pageYOffset;
		var fixed   = window.getComputedStyle(el).position === 'fixed';
		return {
			left:   rect.left   + (fixed ? 0 : scrollX),
			right:  rect.right  + (fixed ? 0 : scrollX),
			top:    rect.top    + (fixed ? 0 : scrollY),
			bottom: rect.bottom + (fixed ? 0 : scrollY)
		};
	}

	/** @private Check which droppable (if any) the element is over. */
	_checkDroppables()
	{
		var mouse = this.mouse.now;
		if (!mouse || (mouse.x == null)) return;

		var overed = null;
		for (var i = 0; i < this.droppables.length; i++)
		{
			var coords = this._positions ? this._positions[i] : this._getDroppableCoords(this.droppables[i]);
			if (mouse.x > coords.left && mouse.x < coords.right &&
			    mouse.y > coords.top  && mouse.y < coords.bottom)
			{
				overed = this.droppables[i];
			}
		}

		if (this._overed !== overed)
		{
			if (this._overed && this.onLeave) this.onLeave.call(this, this.element, this._overed);
			if (overed       && this.onEnter) this.onEnter.call(this, this.element, overed);
			this._overed = overed;
		}
	}

	/** @private On drop, fire the onDrop event. */
	_stop(event)
	{
		super._stop(event);
		if (this._overed && this.onDrop) this.onDrop.call(this, this.element, this._overed, event);
		this._overed = null;
	}
}

// ── Element.prototype helper ───────────────────────────────────────────────

/**
 * Make the element resizable by dragging its bottom-right corner.
 * @param {Object} [options]  Passed to Drag. modifiers default to width/height.
 */
/**
 * Make the element draggable by attaching a DragMove instance.
 * A reference to the DragMove is stored on the element as _dragger.
 * @param {Object} [options]  Passed to DragMove.
 * @returns {DragMove}
 */
Element.prototype.makeDraggable = function(options)
{
	var drag = new DragMove(this, options);
	this._dragger = drag;
	return drag;
};

Element.prototype.makeResizable = function(options)
{
	var drag = new Drag(this, Object.assign({ modifiers: { x: 'width', y: 'height' } }, options || {}));
	this._resizer = drag;
	var el = this;
	var userOnDrag = drag.onDrag;
	drag.onDrag = function()
	{
		el.dispatchEvent(new CustomEvent('resize', { detail: drag }));
		if (userOnDrag) userOnDrag.apply(this, arguments);
	};
	return drag;
};
