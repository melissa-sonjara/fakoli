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
 * JSONP — cross-domain requests via dynamic script-tag injection.
 *
 * Note: Prefer fetch() with CORS when possible. JSONP is a legacy
 * pattern for servers that do not support CORS.
 *
 * ES equivalent of MooTools More Request.JSONP.
 */

var _jsonpCounter = 0;
var _jsonpMap     = {};

class JSONP
{
	/**
	 * @param {Object} [options]
	 * @param {string}   options.url          Request URL.
	 * @param {Object}   options.data         Query parameters to append.
	 * @param {string}   options.callbackKey  Query param name for callback (default 'callback').
	 * @param {number}   options.timeout      Timeout in ms (0 = disabled).
	 * @param {Function} options.onRequest    Called when the request starts.
	 * @param {Function} options.onSuccess    Called with the response data.
	 * @param {Function} options.onCancel     Called when cancelled.
	 * @param {Function} options.onTimeout    Called on timeout.
	 * @param {Function} options.onError      Called on error.
	 */
	constructor(options)
	{
		options = options || {};
		this.url         = options.url         || '';
		this.data        = options.data        || {};
		this.callbackKey = options.callbackKey || 'callback';
		this.timeout     = options.timeout     || 0;
		this.onRequest   = options.onRequest   || null;
		this.onSuccess   = options.onSuccess   || null;
		this.onCancel    = options.onCancel    || null;
		this.onTimeout   = options.onTimeout   || null;
		this.onError     = options.onError     || null;

		this._running    = false;
		this._script     = null;
		this._timer      = null;
		this._index      = null;
	}

	/**
	 * Send the JSONP request.
	 * @param {Object} [overrides]  Override any constructor options for this call.
	 */
	send(overrides)
	{
		if (this._running) return this;

		var opts = Object.assign({}, this, overrides || {});
		this._running = true;
		this._index   = _jsonpCounter++;
		var key       = 'cb_' + this._index;
		var self      = this;

		// Build query string
		var qs = new URLSearchParams(typeof opts.data === 'string' ? opts.data : opts.data || {}).toString();
		var sep = opts.url.indexOf('?') >= 0 ? '&' : '?';
		var callbackName = 'Fakoli_jsonp.' + key;

		var src = opts.url + sep + opts.callbackKey + '=' + encodeURIComponent(callbackName)
			+ (qs ? '&' + qs : '');

		// Register callback
		_jsonpMap[key] = function()
		{
			delete _jsonpMap[key];
			self._success(Array.from(arguments));
		};

		// Expose the map on window under a stable global name
		if (!window.Fakoli_jsonp) window.Fakoli_jsonp = _jsonpMap;

		// Inject script
		var script = document.createElement('script');
		script.type  = 'text/javascript';
		script.async = true;
		script.src   = src;
		script.addEventListener('error', function()
		{
			self.clear();
			if (self.onError) self.onError.call(self, src);
		});

		document.head.appendChild(script);
		this._script = script;

		if (opts.onRequest) opts.onRequest.call(this, src, script);

		if (this.timeout > 0)
		{
			this._timer = setTimeout(function() { self._handleTimeout(); }, this.timeout);
		}

		return this;
	}

	/** @private Handle a successful response. */
	_success(args)
	{
		if (!this._running) return;
		clearTimeout(this._timer);
		this.clear();
		if (this.onSuccess) this.onSuccess.apply(this, args);
	}

	/** @private Handle a timeout. */
	_handleTimeout()
	{
		if (this._running)
		{
			this.clear();
			if (this.onTimeout) this.onTimeout.call(this);
		}
	}

	/** Cancel the in-flight request. */
	cancel()
	{
		if (this._running)
		{
			this.clear();
			if (this.onCancel) this.onCancel.call(this);
		}
		return this;
	}

	/** Returns true if a request is in flight. */
	isRunning()
	{
		return this._running;
	}

	/** Clean up the script tag and reset state. */
	clear()
	{
		this._running = false;
		clearTimeout(this._timer);
		if (this._script && this._script.parentNode)
		{
			this._script.parentNode.removeChild(this._script);
		}
		this._script = null;
		return this;
	}
}

/**
 * RequestPeriodical — polls a URL at increasing intervals, backing off
 * when no new data is returned.
 *
 * ES equivalent of MooTools More Request.Periodical.
 */
class RequestPeriodical
{
	/**
	 * @param {string} url
	 * @param {Object} [options]
	 * @param {number}   options.initialDelay  Initial delay in ms (default 5000).
	 * @param {number}   options.delay         Normal polling interval in ms (default 5000).
	 * @param {number}   options.limit         Maximum back-off delay in ms (default 60000).
	 * @param {Function} options.onRequest     Called before each fetch.
	 * @param {Function} options.onSuccess     Called with response text on success.
	 * @param {Function} options.onFailure     Called on fetch error.
	 * @param {Function} options.onStop        Called when stopped.
	 */
	constructor(url, options)
	{
		options = options || {};
		this.url          = url;
		this.initialDelay = options.initialDelay != null ? options.initialDelay : 5000;
		this.delay        = options.delay        != null ? options.delay        : 5000;
		this.limit        = options.limit        != null ? options.limit        : 60000;
		this.onRequest    = options.onRequest    || null;
		this.onSuccess    = options.onSuccess    || null;
		this.onFailure    = options.onFailure    || null;
		this.onStop       = options.onStop       || null;

		this._timer     = null;
		this._lastDelay = this.initialDelay;
		this._running   = false;
	}

	/** Start polling. */
	startTimer(delay)
	{
		this._lastDelay = delay != null ? delay : this.initialDelay;
		this._schedule();
		return this;
	}

	/** Stop polling. */
	stopTimer()
	{
		clearTimeout(this._timer);
		this._timer   = null;
		this._running = false;
		if (this.onStop) this.onStop.call(this);
		return this;
	}

	/** @private Schedule the next fetch. */
	_schedule()
	{
		var self = this;
		this._timer = setTimeout(function() { self._fetch(); }, this._lastDelay);
	}

	/** @private Execute a fetch and schedule the next one. */
	_fetch()
	{
		if (this._running) return;
		this._running = true;
		var self = this;
		if (this.onRequest) this.onRequest.call(this);

		fetch(this.url)
			.then(function(resp) { return resp.text(); })
			.then(function(text)
			{
				self._running   = false;
				self._lastDelay = self.delay; // got data → reset to normal interval
				if (self.onSuccess) self.onSuccess.call(self, text);
				self._schedule();
			})
			.catch(function(err)
			{
				self._running   = false;
				// No data → back off (up to limit)
				self._lastDelay = Math.min(self._lastDelay + self.delay, self.limit);
				if (self.onFailure) self.onFailure.call(self, err);
				self._schedule();
			});
	}
}
