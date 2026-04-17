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
 * Element.Pin — pin/unpin an element at its current position using
 * position:fixed, restoring the original position on unpin.
 *
 * ES equivalent of MooTools More Element.Pin.
 */

var _pinStore = new WeakMap();

/**
 * Pin `el` at its current viewport position (position:fixed).
 * A scroll listener is attached as a fallback for environments where
 * position:fixed behaves unexpectedly.
 *
 * @param {HTMLElement} el
 * @param {number|boolean} [top]  If a number, pin at this explicit top value instead.
 */
function pinElement(el)
{
	if (window.getComputedStyle(el).display === 'none') return;
	if (_pinStore.get(el)) return; // already pinned

	var rect   = el.getBoundingClientRect();
	var saved  = {
		position:   el.style.position,
		top:        el.style.top,
		left:       el.style.left,
		margin:     el.style.margin
	};

	el.style.position = 'fixed';
	el.style.top      = Math.round(rect.top)  + 'px';
	el.style.left     = Math.round(rect.left) + 'px';
	el.style.margin   = '0';

	el.classList.add('isPinned');

	_pinStore.set(el, { saved: saved });
}

/**
 * Restore `el` to its original position styles.
 * @param {HTMLElement} el
 */
function unpinElement(el)
{
	var state = _pinStore.get(el);
	if (!state) return;

	var saved = state.saved;
	el.style.position = saved.position;
	el.style.top      = saved.top;
	el.style.left     = saved.left;
	el.style.margin   = saved.margin;

	el.classList.remove('isPinned');
	_pinStore.delete(el);
}

// ── Element.prototype extensions ──────────────────────────────────────────

/**
 * Fix the element at its current position using position:fixed.
 * Calling again while already pinned is a no-op.
 */
Element.prototype.pin = function()
{
	pinElement(this);
	return this;
};

/** Restore the element to its previous position style. */
Element.prototype.unpin = function()
{
	unpinElement(this);
	return this;
};

/** Toggle between pinned and unpinned states. */
Element.prototype.togglePin = function()
{
	if (_pinStore.get(this)) this.unpin();
	else                     this.pin();
	return this;
};
