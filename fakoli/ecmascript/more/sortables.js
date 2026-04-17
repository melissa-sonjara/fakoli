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
 * Sortables — drag-and-drop list reordering with optional cross-list support.
 *
 * ES equivalent of MooTools More Sortables.
 *
 * Uses the HTML5 Drag and Drop API when available; falls back to Pointer Events
 * with manual DOM insertion for touch devices.
 */
class Sortables
{
	/**
	 * @param {HTMLElement|HTMLElement[]|NodeList|string} lists
	 *   One or more list container elements (or a CSS selector string).
	 * @param {Object} [options]
	 * @param {string|false}  options.handle         CSS selector for drag handle inside each item.
	 * @param {number}        options.opacity         Opacity of the dragged element (default 1 = show ghost).
	 * @param {boolean}       options.constrain       Restrict sorting to the item's own list (default false).
	 * @param {string[]}      options.unDraggableTags Tags that should not initiate a drag (default standard set).
	 * @param {Function|false} options.clone          Custom clone factory fn(event, element, list) or false.
	 * @param {Function}      options.onSort         Called with (element, clone) during sorting.
	 * @param {Function}      options.onStart        Called with (element, clone) when drag starts.
	 * @param {Function}      options.onComplete     Called with (element) when drag ends.
	 */
	constructor(lists, options)
	{
		this.options = Object.assign(
			{
				handle:          false,
				opacity:         1,
				constrain:       false,
				clone:           false,
				unDraggableTags: ['button', 'input', 'a', 'textarea', 'select', 'option'],
				onSort:          null,
				onStart:         null,
				onComplete:      null
			},
			options || {}
		);

		this.lists    = [];
		this.elements = [];
		this._idle    = true;

		// Accept selector string, single element, or array/NodeList
		var listEls;
		if (typeof lists === 'string') listEls = Array.from(document.querySelectorAll(lists));
		else if (lists instanceof Element) listEls = [lists];
		else listEls = Array.from(lists);

		this.addLists(listEls);
	}

	/** Add list containers and their children. */
	addLists(lists)
	{
		var self = this;
		Array.from(lists).forEach(function(list)
		{
			if (self.lists.indexOf(list) < 0) self.lists.push(list);
			self.addItems(Array.from(list.children));
		});
		return this;
	}

	/** Register individual sortable items. */
	addItems(items)
	{
		var self = this;
		Array.from(items).forEach(function(el)
		{
			if (self.elements.indexOf(el) >= 0) return;
			self.elements.push(el);

			var handle = self.options.handle
				? (el.querySelector(self.options.handle) || el)
				: el;

			var fn = function(event) { self._start(event, el); };
			el._sortablesStart = fn;
			handle.addEventListener('pointerdown', fn);
		});
		return this;
	}

	/** Unregister individual items. */
	removeItems(items)
	{
		var self = this;
		Array.from(items).forEach(function(el)
		{
			var idx = self.elements.indexOf(el);
			if (idx < 0) return;
			self.elements.splice(idx, 1);

			var handle = self.options.handle
				? (el.querySelector(self.options.handle) || el)
				: el;

			if (el._sortablesStart)
			{
				handle.removeEventListener('pointerdown', el._sortablesStart);
				delete el._sortablesStart;
			}
		});
		return this;
	}

	/** Remove list containers and their children. */
	removeLists(lists)
	{
		var self = this;
		Array.from(lists).forEach(function(list)
		{
			var idx = self.lists.indexOf(list);
			if (idx >= 0) self.lists.splice(idx, 1);
			self.removeItems(Array.from(list.children));
		});
		return this;
	}

	/** Detach all listeners. */
	detach()
	{
		this.removeLists(this.lists.slice());
		return this;
	}

	/** Re-attach listeners. */
	attach()
	{
		this.addLists(this.lists.slice());
		return this;
	}

	/**
	 * Return a serialized representation of the current order.
	 * @param {Function} [modifier]  fn(element) → identifier; default returns element.id.
	 * @param {number}   [index]     Return only the given list index; default returns all lists.
	 */
	serialize(modifier, index)
	{
		var serial = this.lists.map(function(list)
		{
			return Array.from(list.children).map(modifier || function(el) { return el.id; });
		});
		if (this.lists.length === 1) return serial[0];
		if (index != null && index >= 0 && index < serial.length) return serial[index];
		return serial;
	}

	// ── Drag internals ────────────────────────────────────────────────────

	_start(event, element)
	{
		if (!this._idle) return;
		if (event.button && event.button !== 0) return;

		var tag = (event.target.tagName || '').toLowerCase();
		if (!this.options.handle && this.options.unDraggableTags.indexOf(tag) >= 0) return;

		event.preventDefault();

		this._idle       = false;
		this._element    = element;
		this._savedOpacity = window.getComputedStyle(element).opacity;
		this._list       = element.parentNode;
		this._clone      = this._createClone(event, element);

		// Insert clone before element as placeholder
		element.parentNode.insertBefore(this._clone, element);
		element.style.opacity = this.options.opacity == null ? '0' : this.options.opacity;

		if (this.options.onStart) this.options.onStart.call(this, element, this._clone);

		// Pointer capture on clone
		this._onMove   = this._drag.bind(this);
		this._onEnd    = this._end.bind(this);

		document.addEventListener('pointermove',   this._onMove);
		document.addEventListener('pointerup',     this._onEnd);
		document.addEventListener('pointercancel', this._onEnd);

		this._drag(event);
	}

	_createClone(event, element)
	{
		var rect  = element.getBoundingClientRect();
		var clone = element.cloneNode(true);
		clone.style.position   = 'fixed';
		clone.style.left       = rect.left + 'px';
		clone.style.top        = rect.top  + 'px';
		clone.style.width      = rect.width  + 'px';
		clone.style.height     = rect.height + 'px';
		clone.style.margin     = '0';
		clone.style.opacity    = '0.7';
		clone.style.pointerEvents = 'none';
		clone.style.zIndex     = '9999';
		document.body.appendChild(clone);

		this._dragOffsetX = event.clientX - rect.left;
		this._dragOffsetY = event.clientY - rect.top;

		return clone;
	}

	_drag(event)
	{
		var x = event.clientX - this._dragOffsetX;
		var y = event.clientY - this._dragOffsetY;

		this._clone.style.left = x + 'px';
		this._clone.style.top  = y + 'px';

		// Find the element under the centre of the clone
		this._clone.style.display = 'none';
		var hit = document.elementFromPoint(event.clientX, event.clientY);
		this._clone.style.display = '';

		if (!hit) return;

		// Walk up to find a sortable element or list
		var target = hit;
		while (target && target !== document.body)
		{
			if (this.elements.indexOf(target) >= 0 && target !== this._element)
			{
				this._insertBefore(target);
				break;
			}
			if (this.lists.indexOf(target) >= 0)
			{
				// Dropped onto an empty list
				if (!this.options.constrain || target === this._list)
				{
					target.appendChild(this._element);
					this._list = target;
					if (this.options.onSort) this.options.onSort.call(this, this._element, this._clone);
				}
				break;
			}
			target = target.parentNode;
		}
	}

	_insertBefore(target)
	{
		if (!this.options.constrain || target.parentNode === this._list)
		{
			var rect        = target.getBoundingClientRect();
			var midY        = rect.top + rect.height / 2;
			var cloneRect   = this._clone.getBoundingClientRect();
			var cloneCentreY = cloneRect.top + cloneRect.height / 2;

			if (cloneCentreY < midY) target.parentNode.insertBefore(this._element, target);
			else                     target.parentNode.insertBefore(this._element, target.nextSibling);

			this._list = this._element.parentNode;
			if (this.options.onSort) this.options.onSort.call(this, this._element, this._clone);
		}
	}

	_end()
	{
		document.removeEventListener('pointermove',   this._onMove);
		document.removeEventListener('pointerup',     this._onEnd);
		document.removeEventListener('pointercancel', this._onEnd);

		this._element.style.opacity = this._savedOpacity;
		if (this._clone && this._clone.parentNode) this._clone.parentNode.removeChild(this._clone);
		this._clone = null;

		var el    = this._element;
		this._idle = true;
		if (this.options.onComplete) this.options.onComplete.call(this, el);
	}
}
