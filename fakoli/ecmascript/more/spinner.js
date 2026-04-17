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
 * Spinner — semi-transparent loading overlay with an animated spinner graphic.
 *
 * ES equivalent of MooTools More Spinner.
 *
 * Depends on mask.js (Mask class) being available.
 * The mask element gets the CSS class 'spinner'; a child div with class
 * 'spinner-content' holds an optional message and the spinner image div
 * ('spinner-img').  Minimal CSS required:
 *
 *   .spinner { background: rgba(255,255,255,0.7); }
 *   .spinner-img { width: 32px; height: 32px; animation: spin 0.8s linear infinite; }
 *   @keyframes spin { to { transform: rotate(360deg); } }
 */
class Spinner extends Mask
{
	/**
	 * @param {HTMLElement} [target]   Element to cover (default document.body).
	 * @param {Object}      [options]
	 * @param {string}   options.class           CSS class for the overlay (default 'spinner').
	 * @param {string}   options.message         Optional loading message text.
	 * @param {string}   options.contentClass    CSS class for the content div (default 'spinner-content').
	 * @param {string}   options.messageClass    CSS class for the message element (default 'spinner-msg').
	 * @param {string}   options.imgClass        CSS class for the spinner image div (default 'spinner-img').
	 * @param {boolean}  options.fxOpacity       Fade in/out with opacity transition (default true).
	 * @param {number}   options.fxDuration      Fade duration in ms (default 200).
	 * @param {Function} options.onShow
	 * @param {Function} options.onHide
	 */
	constructor(target, options)
	{
		options = Object.assign(
			{
				'class':      'spinner',
				message:      null,
				contentClass: 'spinner-content',
				messageClass: 'spinner-msg',
				imgClass:     'spinner-img',
				fxOpacity:    true,
				fxDuration:   200
			},
			options || {}
		);

		super(target, options);

		this._buildContent();

		// Store on target
		this.target._spinner = this;
	}

	/** @private Inject content + optional message + spinner image into the mask element. */
	_buildContent()
	{
		this._content = document.createElement('div');
		this._content.className = this.options.contentClass || 'spinner-content';
		this._content.style.cssText = 'position:absolute;';

		if (this.options.message)
		{
			var msg = document.createElement('p');
			msg.className = this.options.messageClass || 'spinner-msg';
			msg.textContent = this.options.message;
			this._content.appendChild(msg);
		}

		var img = document.createElement('div');
		img.className = this.options.imgClass || 'spinner-img';
		this._content.appendChild(img);
		this._img = img;

		this._element.appendChild(this._content);
		this._positionContent();
	}

	/** @private Centre the content div over the mask element. */
	_positionContent()
	{
		// Centre using transform; works whether mask covers body or an element
		this._content.style.top  = '50%';
		this._content.style.left = '50%';
		this._content.style.transform = 'translate(-50%, -50%)';
	}

	/** Show the spinner, optionally fading in. */
	show()
	{
		if (!this._hidden) return this;

		this.target.setAttribute('aria-busy', 'true');

		if (this.options.fxOpacity)
		{
			this.position();
			this._element.style.opacity = '0';
			this._element.style.display = 'block';
			this._hidden = false;
			this._positionContent();

			var el       = this._element;
			var duration = this.options.fxDuration;
			// rAF to allow display:block to take effect before transitioning
			requestAnimationFrame(function()
			{
				el.style.transition = 'opacity ' + duration + 'ms';
				el.style.opacity    = '1';
			});

			window.addEventListener('resize', this._onResize);
			if (this.onShow) this.onShow.call(this);
		}
		else
		{
			super.show();
			this._positionContent();
		}

		return this;
	}

	/** Hide the spinner, optionally fading out. */
	hide()
	{
		if (this._hidden) return this;

		this.target.setAttribute('aria-busy', 'false');

		if (this.options.fxOpacity)
		{
			var self     = this;
			var el       = this._element;
			var duration = this.options.fxDuration;

			el.style.transition = 'opacity ' + duration + 'ms';
			el.style.opacity    = '0';

			var finish = function()
			{
				el.removeEventListener('transitionend', finish);
				el.style.display    = 'none';
				el.style.transition = '';
				self._hidden = true;
				window.removeEventListener('resize', self._onResize);
				if (self.onHide) self.onHide.call(self);
				if (self.destroyOnHide) self.destroy();
			};

			el.addEventListener('transitionend', finish);
		}
		else
		{
			super.hide();
		}

		return this;
	}

	/** Remove the spinner from the DOM. */
	destroy()
	{
		super.destroy();
		delete this.target._spinner;
	}
}

// ── Element.prototype helpers ──────────────────────────────────────────────

/**
 * Show a spinner over this element.
 * @param {Object} [options]  Passed to Spinner constructor.
 */
Element.prototype.spin = function(options)
{
	var existing = this._spinner;
	if (existing && options)
	{
		existing.destroy();
		existing = null;
	}
	if (!existing) existing = new Spinner(this, options);
	existing.show();
	return this;
};

/** Hide the spinner over this element. */
Element.prototype.unspin = function()
{
	if (this._spinner) this._spinner.hide();
	return this;
};
