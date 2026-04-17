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
 * FormRequest — intercepts an HTML form's submit event, posts it via fetch(),
 * and injects the response HTML into a target element.
 *
 * ES equivalent of MooTools More Form.Request + Form.Request.Append.
 *
 * Optional spinner support: if the target element has a _spinner property
 * (set by spinner.js) it will be shown/hidden automatically.
 */
class FormRequest
{
	/**
	 * @param {HTMLFormElement} form    The form to intercept.
	 * @param {HTMLElement}     target  Element whose innerHTML receives the response.
	 * @param {Object}          [options]
	 * @param {string}   options.url            Override form action URL.
	 * @param {string}   options.method         Override form method (default from form attribute or 'post').
	 * @param {Object}   options.extraData      Extra key/value pairs merged into the request.
	 * @param {boolean}  options.resetForm      Reset the form after successful submission (default true).
	 * @param {boolean}  options.evalScripts    Execute <script> tags in the response (default true).
	 * @param {boolean}  options.useSpinner     Show spinner on form element while loading (default false).
	 * @param {Function} options.onSend         Called before submission with (form, data).
	 * @param {Function} options.onSuccess      Called with (target, html) on success.
	 * @param {Function} options.onFailure      Called with (response) on failure.
	 * @param {Function} options.onComplete     Called with (target, html) on completion (success or failure).
	 */
	constructor(form, target, options)
	{
		// Guard against double-initialization
		if (form._formRequest) return form._formRequest;

		this.element = form;
		this.target  = typeof target === 'string' ? document.querySelector(target) : (target || null);
		this.options = Object.assign(
			{
				url:         null,
				method:      null,
				extraData:   {},
				resetForm:   true,
				evalScripts: true,
				useSpinner:  false,
				onSend:      null,
				onSuccess:   null,
				onFailure:   null,
				onComplete:  null
			},
			options || {}
		);

		this._abortController = null;
		this._clickedButton   = null;

		this._onSubmit      = this._handleSubmit.bind(this);
		this._onButtonClick = this._saveButton.bind(this);

		this._attach();
		form._formRequest = this;
	}

	/** Change the target element. */
	setTarget(target)
	{
		this.target = typeof target === 'string' ? document.querySelector(target) : target;
		return this;
	}

	/** Attach submit/click listeners. */
	_attach()
	{
		this.element.addEventListener('submit', this._onSubmit);
		this.element.addEventListener('click',  this._onButtonClick);
	}

	/** Detach listeners (disables the enhancement). */
	detach()
	{
		this.element.removeEventListener('submit', this._onSubmit);
		this.element.removeEventListener('click',  this._onButtonClick);
		return this;
	}

	/** Synonym for detach(). */
	disable() { return this.detach(); }

	/** Re-attach listeners. */
	enable()
	{
		this.detach();
		this._attach();
		return this;
	}

	/** @private Save the name/value of the clicked submit button. */
	_saveButton(event)
	{
		var btn = event.target.closest('button, input[type=submit]');
		if (btn && btn.name) this._clickedButton = { name: btn.name, value: btn.value || '' };
	}

	/** @private Handle the form submit event. */
	_handleSubmit(event)
	{
		event.preventDefault();
		this.send();
	}

	/** Perform the AJAX submission. */
	send()
	{
		if (this._abortController) this._abortController.abort();
		this._abortController = new AbortController();

		var form    = this.element;
		var opts    = this.options;
		var data    = new FormData(form);
		var method  = (opts.method || form.method || 'post').toUpperCase();
		var url     = opts.url    || form.action  || window.location.href;

		// Merge extraData
		var extra = opts.extraData || {};
		Object.keys(extra).forEach(function(k) { data.append(k, extra[k]); });

		// Include clicked submit button
		if (this._clickedButton)
		{
			data.append(this._clickedButton.name, this._clickedButton.value);
			this._clickedButton = null;
		}

		var target  = this.target;
		var self    = this;
		var signal  = this._abortController.signal;

		// Show spinner if configured
		if (opts.useSpinner && form.spin) form.spin();

		if (opts.onSend) opts.onSend.call(this, form, data);

		var fetchOptions = {
			method:  method,
			body:    method === 'GET' ? null : data,
			signal:  signal,
			headers: { 'X-Requested-With': 'XMLHttpRequest' }
		};

		var finalUrl = url;
		if (method === 'GET')
		{
			var params = new URLSearchParams(data);
			var sep    = url.indexOf('?') >= 0 ? '&' : '?';
			finalUrl   = url + sep + params.toString();
			delete fetchOptions.body;
		}

		fetch(finalUrl, fetchOptions)
			.then(function(response)
			{
				if (!response.ok) throw response;
				return response.text();
			})
			.then(function(html)
			{
				if (opts.useSpinner && form.unspin) form.unspin();

				if (target) target.innerHTML = html;

				if (opts.evalScripts && target)
				{
					Array.from(target.querySelectorAll('script')).forEach(function(oldScript)
					{
						var newScript = document.createElement('script');
						Array.from(oldScript.attributes).forEach(function(attr) { newScript.setAttribute(attr.name, attr.value); });
						newScript.textContent = oldScript.textContent;
						oldScript.parentNode.replaceChild(newScript, oldScript);
					});
				}

				if (opts.resetForm)
				{
					try { form.reset(); } catch(e) {}
				}

				if (opts.onSuccess)  opts.onSuccess.call(self, target, html);
				if (opts.onComplete) opts.onComplete.call(self, target, html);
			})
			.catch(function(err)
			{
				if (err && err.name === 'AbortError') return;
				if (opts.useSpinner && form.unspin) form.unspin();
				if (opts.onFailure)  opts.onFailure.call(self, err);
				if (opts.onComplete) opts.onComplete.call(self, null, null);
			});

		return this;
	}

	/** Cancel an in-flight request. */
	cancel()
	{
		if (this._abortController) this._abortController.abort();
		return this;
	}
}

// ── FormRequestAppend ─────────────────────────────────────────────────────

/**
 * FormRequestAppend — like FormRequest but appends the response to the target
 * rather than replacing it, optionally revealing it with FxReveal.
 *
 * ES equivalent of MooTools More Form.Request.Append.
 */
class FormRequestAppend extends FormRequest
{
	/**
	 * @param {HTMLFormElement} form
	 * @param {HTMLElement}     target
	 * @param {Object} [options]
	 * @param {string}   options.inject      Insert position: 'bottom' | 'top' (default 'bottom').
	 * @param {boolean}  options.useReveal   Reveal appended content with FxReveal (default true).
	 * @param {Object}   options.revealOptions  Options passed to FxReveal.
	 */
	constructor(form, target, options)
	{
		super(form, target, Object.assign({ inject: 'bottom', useReveal: true, revealOptions: {} }, options));
	}

	/** Override send() to append instead of replace. */
	send()
	{
		if (this._abortController) this._abortController.abort();
		this._abortController = new AbortController();

		var form   = this.element;
		var opts   = this.options;
		var target = this.target;
		var self   = this;
		var data   = new FormData(form);
		var method = (opts.method || form.method || 'post').toUpperCase();
		var url    = opts.url || form.action || window.location.href;

		Object.keys(opts.extraData || {}).forEach(function(k) { data.append(k, opts.extraData[k]); });
		if (this._clickedButton)
		{
			data.append(this._clickedButton.name, this._clickedButton.value);
			this._clickedButton = null;
		}

		var signal = this._abortController.signal;
		if (opts.useSpinner && form.spin) form.spin();
		if (opts.onSend) opts.onSend.call(this, form, data);

		fetch(url, { method: method, body: data, signal: signal, headers: { 'X-Requested-With': 'XMLHttpRequest' } })
			.then(function(r) { if (!r.ok) throw r; return r.text(); })
			.then(function(html)
			{
				if (opts.useSpinner && form.unspin) form.unspin();

				var fragment = document.createElement('div');
				fragment.innerHTML = html;

				// Append fragment to target
				if (target)
				{
					if (opts.inject === 'top') target.insertBefore(fragment, target.firstChild);
					else                       target.appendChild(fragment);
				}

				if (opts.resetForm) try { form.reset(); } catch(e) {}

				if (opts.useReveal && typeof FxReveal !== 'undefined')
				{
					var fx = new FxReveal(fragment, opts.revealOptions);
					fx.options.onComplete = function()
					{
						if (opts.onSuccess)  opts.onSuccess.call(self, target, html);
						if (opts.onComplete) opts.onComplete.call(self, target, html);
					};
					fragment.style.display = 'none';
					fx.reveal();
				}
				else
				{
					if (opts.onSuccess)  opts.onSuccess.call(self, target, html);
					if (opts.onComplete) opts.onComplete.call(self, target, html);
				}
			})
			.catch(function(err)
			{
				if (err && err.name === 'AbortError') return;
				if (opts.useSpinner && form.unspin) form.unspin();
				if (opts.onFailure) opts.onFailure.call(self, err);
			});

		return this;
	}
}

// ── Element.prototype helper ──────────────────────────────────────────────

/**
 * Submit this form, updating `update` element with the response.
 * @param {HTMLElement|string} update   Target element or selector.
 * @param {Object}             [options]
 */
Element.prototype.formUpdate = function(update, options)
{
	var fq = this._formRequest;
	if (!fq)
	{
		fq = new FormRequest(this, update, options);
	}
	else
	{
		if (update)  fq.setTarget(update);
		if (options) Object.assign(fq.options, options);
	}
	fq.send();
	return this;
};
