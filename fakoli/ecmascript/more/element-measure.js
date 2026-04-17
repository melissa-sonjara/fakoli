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
 * Element measurement utilities — measures dimensions of elements that
 * may be hidden, and computes sizes including padding/border/margin.
 *
 * ES equivalent of MooTools More Element.Measure.
 */

/**
 * Returns true if the element is visible (has layout dimensions).
 * @param {Element} el
 */
function isElementVisible(el)
{
	return !!(el && (el.offsetHeight || el.offsetWidth));
}

/**
 * Temporarily make a hidden element measurable by setting it to
 * display:block with visibility:hidden, call `fn`, then restore.
 * Returns a restore function.
 * @param {HTMLElement} el
 */
function exposeElement(el)
{
	var cs = window.getComputedStyle(el);
	if (cs.display !== 'none') return function() {};

	var savedCssText = el.style.cssText;
	el.style.display    = 'block';
	el.style.position   = 'absolute';
	el.style.visibility = 'hidden';

	return function()
	{
		el.style.cssText = savedCssText;
	};
}

/**
 * Execute `fn` in the context of `el`, temporarily exposing any hidden
 * ancestors so that layout measurements are valid.
 * @param {HTMLElement} el
 * @param {Function} fn  Called with `el` as `this`. Return value is passed through.
 */
function measureElement(el, fn)
{
	if (isElementVisible(el)) return fn.call(el);

	// Walk up and expose hidden ancestors
	var toRestore = [];
	var parent = el.parentElement;
	while (parent && !isElementVisible(parent) && parent !== document.body)
	{
		toRestore.push(exposeElement(parent));
		parent = parent.parentElement;
	}

	var restore = exposeElement(el);
	var result  = fn.call(el);
	restore();
	toRestore.forEach(function(r) { r(); });
	return result;
}

/**
 * Returns { width, height } of `el`, measuring through hidden state if needed.
 * @param {HTMLElement} el
 */
function getDimensions(el)
{
	return measureElement(el, function()
	{
		return { width: this.offsetWidth, height: this.offsetHeight };
	});
}

/**
 * Returns an object containing the element's content dimensions plus
 * computed edge sizes for the requested style categories.
 *
 * @param {HTMLElement} el
 * @param {Object} [options]
 * @param {string[]} options.styles  CSS property categories: 'padding', 'border', 'margin' (default ['padding','border'])
 * @param {Object}  options.planes  { width: ['left','right'], height: ['top','bottom'] }
 * @param {string}  options.mode    'both' | 'vertical' | 'horizontal' (default 'both')
 * @returns {Object}  { width, height, totalWidth, totalHeight, computedTop, computedBottom, ... }
 */
function getComputedSize(el, options)
{
	options = Object.assign(
		{
			styles: ['padding', 'border'],
			planes: { height: ['top', 'bottom'], width: ['left', 'right'] },
			mode:   'both'
		},
		options || {}
	);

	if (options.mode === 'vertical')   delete options.planes.width;
	if (options.mode === 'horizontal') delete options.planes.height;

	var cs     = window.getComputedStyle(el);
	var styles = {};
	var size   = {};

	// Collect numeric values for each requested style+edge combination
	Object.keys(options.planes).forEach(function(plane)
	{
		var edges = options.planes[plane];
		options.styles.forEach(function(style)
		{
			edges.forEach(function(edge)
			{
				var prop = style + '-' + edge + (style === 'border' ? '-width' : '');
				styles[prop] = parseFloat(cs.getPropertyValue(prop)) || 0;
			});
		});
	});

	// Build size object
	Object.keys(options.planes).forEach(function(plane)
	{
		var edges = options.planes[plane];
		var dimPx = parseFloat(cs.getPropertyValue(plane));
		if (isNaN(dimPx)) dimPx = plane === 'width' ? el.offsetWidth : el.offsetHeight;

		styles[plane] = dimPx;
		var capitalized = plane.charAt(0).toUpperCase() + plane.slice(1);
		size['total' + capitalized] = dimPx;

		edges.forEach(function(edge)
		{
			var edgeTotal = 0;
			options.styles.forEach(function(style)
			{
				var prop = style + '-' + edge + (style === 'border' ? '-width' : '');
				edgeTotal += (styles[prop] || 0);
			});
			var capEdge = edge.charAt(0).toUpperCase() + edge.slice(1);
			size['computed' + capEdge] = edgeTotal;
			size['total' + capitalized] += edgeTotal;
		});
	});

	return Object.assign(size, styles);
}

// ── Element.prototype extensions ──────────────────────────────────────────

/**
 * Execute `fn` with the element temporarily exposed for measurement.
 * The result of `fn` is returned.
 */
Element.prototype.measure = function(fn)
{
	return measureElement(this, fn);
};

/**
 * Temporarily expose a hidden element for measurement.
 * Returns a restore function (call it when done measuring).
 */
Element.prototype.expose = function()
{
	return exposeElement(this);
};

/** Returns { width, height } handling hidden elements. */
Element.prototype.getDimensions = function(options)
{
	return getDimensions(this);
};

/**
 * Returns computed size including edge contributions.
 * @param {Object} [options]  See getComputedSize() above.
 */
Element.prototype.getComputedSize = function(options)
{
	return getComputedSize(this, options);
};
