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
 * Element positioning utilities and display-state helpers.
 * ES equivalent of MooTools More Element.Position + Element.Shortcuts.
 *
 * Depends on: element-measure.js (measureElement, getComputedSize)
 */

// ── Positioning ────────────────────────────────────────────────────────────

/**
 * Parse a position shorthand string into {x, y}.
 *   'center'   → {x:'center', y:'center'}
 *   'top left' → {x:'left', y:'top'}
 * @param {string|Object} option
 * @returns {Object|*}
 */
function _parsePosition(option)
{
	if (!option || typeof option !== 'string') return option;
	var s = option.toLowerCase();
	return {
		x: /right/.test(s) ? 'right' : (/left/.test(s) ? 'left' : 'center'),
		y: /(top|upper)/.test(s) ? 'top' : (/bottom/.test(s) ? 'bottom' : 'center')
	};
}

/**
 * Compute the { left, top } CSS values to place `element` (position:absolute)
 * so that its specified edge aligns with the specified position of `relativeTo`.
 *
 * Because CSS `left`/`top` are relative to the element's offset parent, not the
 * document, this function subtracts the offset parent's document position and
 * adds its scroll — matching the MooTools Element.Position.setOffsetOption logic.
 *
 * @param {HTMLElement} element
 * @param {Object}  [options]
 * @param {Element}  options.relativeTo      Reference element (default document.body).
 * @param {Object}   options.position        {x:'left'|'center'|'right', y:'top'|'center'|'bottom'}
 * @param {Object}   options.edge            Which edge of `element` aligns to the position point.
 *                                           Defaults to {x:'center',y:'center'} when position is
 *                                           center/center, otherwise {x:'left',y:'top'}.
 * @param {Object}   options.offset          {x, y} pixel offset applied before edge adjustment.
 * @param {Object}   options.minimum         {x, y} minimum pixel values for left/top.
 * @param {Object}   options.maximum         {x, y} maximum pixel values for left/top.
 * @param {boolean}  options.relFixedPosition  Add window scroll when relativeTo is fixed-position.
 * @param {boolean}  options.ignoreScroll    Subtract relativeTo's own scroll from the result.
 * @param {boolean}  options.ignoreMargins   Adjust left/top by element's own margins.
 * @param {boolean}  options.allowNegative   Allow negative left/top values.
 * @returns {{ left: number, top: number }}
 */
function positionElement(element, options)
{
	options = Object.assign(
		{
			relativeTo: document.body,
			position:   { x: 'center', y: 'center' },
			offset:     { x: 0, y: 0 }
		},
		options || {}
	);

	if (typeof options.position === 'string')
		options.position = _parsePosition(options.position);
	if (typeof options.edge === 'string')
		options.edge = _parsePosition(options.edge);

	var edge = options.edge || (
		(options.position.x === 'center' && options.position.y === 'center')
			? { x: 'center', y: 'center' }
			: { x: 'left',   y: 'top'   }
	);

	var relativeTo = (options.relativeTo instanceof Element)
		? options.relativeTo
		: document.body;

	var scrollX = window.scrollX || window.pageXOffset;
	var scrollY = window.scrollY || window.pageYOffset;

	// --- Adjust offset for the element's CSS offset parent ---
	// CSS left/top are relative to the nearest positioned ancestor (offsetParent).
	// We compute positions in document-absolute coordinates, so we subtract the
	// offset parent's document position and add its scroll to get the correct
	// CSS value — equivalent to MooTools Element.Position.setOffsetOption.
	var offsetX = options.offset.x || 0;
	var offsetY = options.offset.y || 0;
	var parentPositioned = false;

	var offsetParent = measureElement(element, function()
	{
		return this.offsetParent || document.body;
	});

	if (offsetParent && offsetParent !== document.body)
	{
		var parentScroll = { x: offsetParent.scrollLeft || 0, y: offsetParent.scrollTop || 0 };

		var parentOffset = measureElement(offsetParent, function()
		{
			var rect = this.getBoundingClientRect();
			return { x: rect.left + scrollX, y: rect.top + scrollY };
		});

		offsetX -= parentOffset.x - parentScroll.x;
		offsetY -= parentOffset.y - parentScroll.y;
		parentPositioned = (offsetParent !== relativeTo);
	}

	// --- Compute document-absolute reference point from relativeTo ---
	var calc, refW, refH;
	if (relativeTo === document.body)
	{
		calc = { x: scrollX, y: scrollY };
		refW = window.innerWidth;
		refH = window.innerHeight;
	}
	else
	{
		var relRect = relativeTo.getBoundingClientRect();
		calc = { x: relRect.left + scrollX, y: relRect.top + scrollY };
		refW = relativeTo.offsetWidth;
		refH = relativeTo.offsetHeight;
	}

	var x, y;
	switch (options.position.x || 'center')
	{
		case 'left':  x = calc.x + offsetX;           break;
		case 'right': x = calc.x + offsetX + refW;    break;
		default:      x = calc.x + offsetX + refW / 2; break;
	}
	switch (options.position.y || 'center')
	{
		case 'top':    y = calc.y + offsetY;           break;
		case 'bottom': y = calc.y + offsetY + refH;   break;
		default:       y = calc.y + offsetY + refH / 2; break;
	}

	// --- Edge alignment: shift by element's outer dimensions ---
	// Uses getComputedSize (from element-measure.js) with all three style groups
	// so that totalWidth/totalHeight include margins, matching MooTools behaviour.
	var dimensions = getComputedSize(element, {
		styles: ['padding', 'border', 'margin'],
		planes: { width: ['left', 'right'], height: ['top', 'bottom'] }
	});

	switch (edge.x)
	{
		case 'left':                                            break;
		case 'right':  x -= dimensions.totalWidth;             break;
		default:       x -= Math.round(dimensions.totalWidth  / 2); break; // center
	}
	switch (edge.y)
	{
		case 'top':                                             break;
		case 'bottom': y -= dimensions.totalHeight;            break;
		default:       y -= Math.round(dimensions.totalHeight / 2); break; // center
	}

	// --- Clamp to non-negative (unless parentPositioned or allowNegative) ---
	var left = (x >= 0 || parentPositioned || options.allowNegative) ? x : 0;
	var top  = (y >= 0 || parentPositioned || options.allowNegative) ? y : 0;

	// --- Min / max bounds ---
	if (options.minimum)
	{
		if (options.minimum.x != null && left < options.minimum.x) left = options.minimum.x;
		if (options.minimum.y != null && top  < options.minimum.y) top  = options.minimum.y;
	}
	if (options.maximum)
	{
		if (options.maximum.x != null && left > options.maximum.x) left = options.maximum.x;
		if (options.maximum.y != null && top  > options.maximum.y) top  = options.maximum.y;
	}

	// --- relFixedPosition: compensate for a fixed-position relativeTo ---
	if (options.relFixedPosition ||
		(relativeTo !== document.body &&
		 window.getComputedStyle(relativeTo).position === 'fixed'))
	{
		left += scrollX;
		top  += scrollY;
	}

	// --- ignoreScroll: subtract relativeTo's own scroll offset ---
	if (options.ignoreScroll)
	{
		left -= (relativeTo !== document.body) ? relativeTo.scrollLeft : scrollX;
		top  -= (relativeTo !== document.body) ? relativeTo.scrollTop  : scrollY;
	}

	// --- ignoreMargins: adjust by element's own margin on the aligned edge ---
	if (options.ignoreMargins)
	{
		var mRight  = dimensions['margin-right']  || 0;
		var mLeft   = dimensions['margin-left']   || 0;
		var mTop    = dimensions['margin-top']    || 0;
		var mBottom = dimensions['margin-bottom'] || 0;

		left += (edge.x === 'right')  ? mRight
		      : (edge.x !== 'center') ? -mLeft
		      : -mLeft + (mRight + mLeft) / 2;

		top  += (edge.y === 'bottom') ? mBottom
		      : (edge.y !== 'center') ? -mTop
		      : -mTop + (mBottom + mTop) / 2;
	}

	return { left: Math.ceil(left), top: Math.ceil(top) };
}

function positionRelative(element, options)
{
	var pos = positionElement(element, options);
	element.style.left = pos.left + 'px';
	element.style.top  = pos.top  + 'px';
	return element;
}

// ── Element.prototype extensions ───────────────────────────────────────────

/**
 * Position this element relative to another.
 * Sets position:absolute and applies the computed left/top, unless
 * `options.returnPos` is true in which case the {left,top} object is returned
 * instead of applying styles.
 * If `options` contains numeric x/y, they are applied directly as left/top.
 */
Element.prototype.position = function(options)
{
	if (options && (options.x != null || options.y != null))
	{
		if (options.x != null) this.style.left = options.x + 'px';
		if (options.y != null) this.style.top  = options.y + 'px';
		return this;
	}

	this.style.position = 'absolute';
	var pos = positionElement(this, options);

	if (options && options.returnPos) return pos;

	this.style.left = pos.left + 'px';
	this.style.top  = pos.top  + 'px';
	return this;
};

/** Returns the computed {left, top} without applying styles. */
Element.prototype.calculatePosition = function(options)
{
	return positionElement(this, options);
};

/**
 * Returns the element's document-absolute position {x, y}.
 * If `relativeTo` is supplied, the coordinates are relative to that element.
 */
Element.prototype.getPosition = function(relativeTo)
{
	var rect    = this.getBoundingClientRect();
	var scrollX = window.scrollX || window.pageXOffset;
	var scrollY = window.scrollY || window.pageYOffset;
	var absX    = rect.left + scrollX;
	var absY    = rect.top  + scrollY;

	if (relativeTo && relativeTo instanceof Element)
	{
		var rRect = relativeTo.getBoundingClientRect();
		return { x: absX - (rRect.left + scrollX), y: absY - (rRect.top + scrollY) };
	}
	return { x: absX, y: absY };
};

/**
 * Returns {top, right, bottom, left, width, height} relative to the document
 * (or to `relativeTo` if supplied).
 */
Element.prototype.getCoordinates = function(relativeTo)
{
	var rect    = this.getBoundingClientRect();
	var scrollX = window.scrollX || window.pageXOffset;
	var scrollY = window.scrollY || window.pageYOffset;

	var top    = rect.top    + scrollY;
	var left   = rect.left   + scrollX;
	var width  = rect.width;
	var height = rect.height;

	if (relativeTo && relativeTo instanceof Element)
	{
		var rPos = relativeTo.getPosition();
		top  -= rPos.y;
		left -= rPos.x;
	}

	return {
		top:    top,
		left:   left,
		right:  left + width,
		bottom: top  + height,
		width:  width,
		height: height
	};
};

// ── Display-state helpers (Element.Shortcuts equivalent) ───────────────────

/** Returns true if the element is not display:none. */
Element.prototype.isDisplayed = function()
{
	return window.getComputedStyle(this).display !== 'none';
};

/**
 * Returns true if the element has visible dimensions.
 * An element with offsetWidth and offsetHeight of 0 is not visible.
 */
Element.prototype.isVisible = function()
{
	var w = this.offsetWidth, h = this.offsetHeight;
	if (w === 0 && h === 0) return false;
	if (w > 0   && h > 0)   return true;
	return this.style.display !== 'none';
};

/** Toggle between shown and hidden. */
Element.prototype.toggle = function()
{
	if (this.isDisplayed()) this.hide();
	else                    this.show();
	return this;
};

/** Hide the element, storing its original display value for later restoration. */
Element.prototype.hide = function()
{
	var d = window.getComputedStyle(this).display;
	if (d === 'none') return this;
	this.dataset._originalDisplay = d || '';
	this.style.display = 'none';
	return this;
};

/**
 * Show the element, restoring its original display value if available.
 * @param {string} [display]  Explicit display value (e.g. 'flex').
 */
Element.prototype.show = function(display)
{
	if (!display && this.isDisplayed()) return this;
	var d = display || this.dataset._originalDisplay || 'block';
	this.style.display = (d === 'none') ? 'block' : d;
	return this;
};

/**
 * Remove one CSS class and add another atomically.
 * @param {string} remove
 * @param {string} add
 */
Element.prototype.swapClass = function(remove, add)
{
	this.classList.remove(remove);
	this.classList.add(add);
	return this;
};

/** Clear the current text selection. */
document.clearSelection = function()
{
	var sel = window.getSelection ? window.getSelection() : null;
	if (sel && sel.removeAllRanges) sel.removeAllRanges();
};
