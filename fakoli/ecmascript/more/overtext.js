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
 * OverText — shows a label overlaid on an empty input field that disappears
 * when the user focuses or types a value.
 *
 * ES equivalent of MooTools More OverText.
 *
 * In modern projects you should prefer the native `placeholder` attribute, but
 * this class provides compatible behaviour for existing markup that uses
 * alt/title attributes or explicit textOverride options.
 *
 * Depends on element-position.js (Element.prototype.position) being available
 * to position the label over the input.
 */
class OverText
{
	/**
	 * @param {HTMLElement} element  The input/textarea element.
	 * @param {Object} [options]
	 * @param {string}   options.textOverride   Use this text instead of alt/title.
	 * @param {string}   options.element        Tag name for the overlay element (default 'label').
	 * @param {string}   options.labelClass     CSS class for the overlay (default 'overTxtLabel').
	 * @param {Object}   options.positionOptions Position options passed to element.position().
	 * @param {boolean}  options.poll           Poll for external value changes (default false).
	 * @param {number}   options.pollInterval   Polling interval in ms (default 250).
	 * @param {Function} options.onTextHide     Called with (textEl, inputEl) when text hides.
	 * @param {Function} options.onTextShow     Called with (textEl, inputEl) when text shows.
	 * @param {Function} options.onFocus        Called when input is focused.
	 */
	constructor(element, options)
	{
		// Guard against double-initialization
		if (element._overText) return element._overText;

		this.element = element;
		this.options = Object.assign(
			{
				textOverride:    null,
				element:         'label',
				labelClass:      'overTxtLabel',
				positionOptions: {
					position: { x: 'left', y: 'top' },
					edge:     { x: 'left', y: 'top' },
					offset:   { x: 4, y: 2 }
				},
				poll:         false,
				pollInterval: 250,
				onTextHide:   null,
				onTextShow:   null,
				onFocus:      null
			},
			options || {}
		);

		this.text       = null;
		this._poller    = null;
		this._pollPaused = false;

		this._focus      = this._onFocus.bind(this);
		this._assert     = this._onAssert.bind(this);
		this._reposition = this._onReposition.bind(this);

		this._attach();
		element._overText = this;
		OverText.instances.push(this);

		if (this.options.poll) this.startPolling();
	}

	/** @private Build and insert the overlay label. */
	_attach()
	{
		var el    = this.element;
		var opts  = this.options;
		var value = opts.textOverride || el.getAttribute('alt') || el.getAttribute('title');
		if (!value) return;

		var tag = document.createElement(opts.element);
		tag.className   = opts.labelClass;
		tag.style.cssText = 'line-height:normal;position:absolute;cursor:text;';
		tag.innerHTML   = value;
		this.text       = tag;

		if (opts.element === 'label')
		{
			if (!el.id)
			{
				el.id = 'input_' + Math.random().toString(36).slice(2);
			}
			tag.setAttribute('for', el.id);
		}

		// Insert after the input in the DOM
		el.parentNode.insertBefore(tag, el.nextSibling);

		tag.addEventListener('click', this._focus);

		this.enable();
	}

	/** Attach event listeners and position. */
	enable()
	{
		this.element.addEventListener('focus',  this._focus);
		this.element.addEventListener('blur',   this._assert);
		this.element.addEventListener('change', this._assert);
		window.addEventListener('resize', this._reposition);
		this.reposition();
		return this;
	}

	/** Detach event listeners. */
	disable()
	{
		this.element.removeEventListener('focus',  this._focus);
		this.element.removeEventListener('blur',   this._assert);
		this.element.removeEventListener('change', this._assert);
		window.removeEventListener('resize', this._reposition);
		this.hide(true, true);
		return this;
	}

	/** Remove the overlay from the DOM entirely. */
	destroy()
	{
		this.disable();
		this.stopPolling();
		if (this.text && this.text.parentNode) this.text.parentNode.removeChild(this.text);
		this.text = null;
		delete this.element._overText;
		var idx = OverText.instances.indexOf(this);
		if (idx >= 0) OverText.instances.splice(idx, 1);
		return this;
	}

	/** Start polling for external value changes. */
	startPolling()
	{
		this._pollPaused = false;
		if (!this._poller)
		{
			var self = this;
			this._poller = setInterval(function()
			{
				if (!self._pollPaused) self._onAssert(true);
			}, this.options.pollInterval);
		}
		return this;
	}

	/** Stop polling. */
	stopPolling()
	{
		this._pollPaused = true;
		if (this._poller)
		{
			clearInterval(this._poller);
			this._poller = null;
		}
		return this;
	}

	/** @returns {boolean} True when the input is empty. */
	test()
	{
		return !this.element.value;
	}

	/** Reposition and re-assert visibility. */
	reposition()
	{
		this._onAssert(true);
		if (!this.element.offsetParent) return this.stopPolling().hide(true, true);
		if (this.text && this.test() && this.element.offsetWidth > 0)
		{
			// Position label over the input
			var rect  = this.element.getBoundingClientRect();
			var pRect = this.element.offsetParent.getBoundingClientRect();
			var scrollX = window.scrollX || window.pageXOffset;
			var scrollY = window.scrollY || window.pageYOffset;
			var opts  = this.options.positionOptions;
			var offX  = opts.offset ? opts.offset.x || 0 : 0;
			var offY  = opts.offset ? opts.offset.y || 0 : 0;

			this.text.style.left = (rect.left - pRect.left + offX) + 'px';
			this.text.style.top  = (rect.top  - pRect.top  + offY) + 'px';
		}
		return this;
	}

	/** Hide the overlay label. */
	hide(suppressFocus, force)
	{
		if (!this.text) return this;
		if (this.text.style.display !== 'none' && (!this.element.disabled || force))
		{
			this.text.style.display = 'none';
			this._pollPaused = true;
			if (this.options.onTextHide) this.options.onTextHide.call(this, this.text, this.element);
			if (!suppressFocus)
			{
				try { this.element.focus(); } catch(e) {}
			}
		}
		return this;
	}

	/** Show the overlay label if the field is empty. */
	show()
	{
		if (!this.text) return this;
		if (this.text.style.display === 'none')
		{
			this.text.style.display = '';
			this.reposition();
			this._pollPaused = false;
			if (this.options.onTextShow) this.options.onTextShow.call(this, this.text, this.element);
		}
		return this;
	}

	// ── Event handlers ────────────────────────────────────────────────────

	_onFocus()
	{
		if (this.options.onFocus) this.options.onFocus.call(this);
		if (!this.text || this.text.style.display === 'none' || this.element.disabled) return;
		this.hide();
	}

	_onAssert(suppressFocus)
	{
		if (this.test()) this.show();
		else             this.hide(suppressFocus);
	}

	_onReposition()
	{
		this.reposition();
	}
}

// ── Static members ────────────────────────────────────────────────────────

OverText.instances = [];

OverText.each = function(fn)
{
	OverText.instances.forEach(function(ot, i)
	{
		if (ot.element && ot.text) fn.call(OverText, ot, i);
	});
};

OverText.update = function()
{
	OverText.each(function(ot) { ot.reposition(); });
};

OverText.hideAll = function()
{
	OverText.each(function(ot) { ot.hide(true, true); });
};

OverText.showAll = function()
{
	OverText.each(function(ot) { ot.show(); });
};
