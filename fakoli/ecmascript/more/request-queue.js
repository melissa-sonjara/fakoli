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
 * RequestQueue — runs fetch requests one at a time (or with configurable
 * concurrency), optionally stopping on failure.
 *
 * A "request" is a plain object: { url, options, onSuccess, onFailure }
 * where `options` is a standard fetch() init object.
 *
 * ES equivalent of MooTools More Request.Queue.
 */
class RequestQueue
{
	/**
	 * @param {Object} [options]
	 * @param {boolean}  options.stopOnFailure  Stop the queue on any failure (default true).
	 * @param {boolean}  options.autoAdvance    Auto-run next request after each completes (default true).
	 * @param {number}   options.concurrent     Max simultaneous requests (default 1).
	 * @param {Function} options.onRequest      Called when a request starts.
	 * @param {Function} options.onSuccess      Called on success.
	 * @param {Function} options.onFailure      Called on failure.
	 * @param {Function} options.onEnd          Called when the queue is empty.
	 */
	constructor(options)
	{
		options = options || {};
		this.stopOnFailure = options.stopOnFailure !== false;
		this.autoAdvance   = options.autoAdvance   !== false;
		this.concurrent    = options.concurrent    != null ? options.concurrent : 1;
		this.onRequest     = options.onRequest     || null;
		this.onSuccess     = options.onSuccess     || null;
		this.onFailure     = options.onFailure     || null;
		this.onEnd         = options.onEnd         || null;

		this._requests = {};   // name → request definition
		this._queue    = [];   // [{name, fn}]
		this._running  = {};   // name → AbortController
		this._error    = false;
	}

	/**
	 * Register a named request definition.
	 * @param {string} name
	 * @param {Object} request  { url, options, onSuccess, onFailure }
	 */
	addRequest(name, request)
	{
		this._requests[name] = request;
		return this;
	}

	/**
	 * Enqueue and send a named request.
	 * @param {string} name
	 * @param {Object} [overrides]  Merged into the request definition for this call.
	 */
	send(name, overrides)
	{
		var self = this;
		var fn = function()
		{
			self._execute(name, overrides);
		};
		fn.requestName = name;

		if (Object.keys(this._running).length >= this.concurrent || (this._error && this.stopOnFailure))
		{
			this._queue.push(fn);
		}
		else
		{
			fn();
		}
		return this;
	}

	/** Run the next queued request (optionally filtered by name). */
	runNext(name)
	{
		if (!this._queue.length) return this;
		if (!name)
		{
			var next = this._queue.shift();
			if (next) next();
		}
		else
		{
			for (var i = 0; i < this._queue.length; i++)
			{
				if (this._queue[i].requestName === name)
				{
					var found = this._queue.splice(i, 1)[0];
					found();
					break;
				}
			}
		}
		return this;
	}

	/** Returns true if the named request is queued or running. */
	hasRequest(name)
	{
		return (name in this._running) || this._queue.some(function(q) { return q.requestName === name; });
	}

	/**
	 * Clear all queued (not yet started) requests.
	 * @param {string} [name]  If given, only remove that named request from the queue.
	 */
	clear(name)
	{
		if (!name)
		{
			this._queue = [];
		}
		else
		{
			this._queue = this._queue.filter(function(q) { return q.requestName !== name; });
		}
		return this;
	}

	/** Abort a running request by name. */
	cancel(name)
	{
		var ctrl = this._running[name];
		if (ctrl) ctrl.abort();
		return this;
	}

	/** Resume the queue after a stopOnFailure pause. */
	resume()
	{
		this._error = false;
		var slots = this.concurrent - Object.keys(this._running).length;
		for (var i = 0; i < slots; i++) this.runNext();
		return this;
	}

	/** @private Execute a request by name. */
	_execute(name, overrides)
	{
		var def    = Object.assign({}, this._requests[name] || {}, overrides || {});
		var ctrl   = new AbortController();
		var self   = this;
		this._running[name] = ctrl;

		if (this.onRequest) this.onRequest.call(this, name, def);

		var fetchOptions = Object.assign({ signal: ctrl.signal }, def.options || {});

		fetch(def.url, fetchOptions)
			.then(function(resp)
			{
				if (!resp.ok) throw new Error('HTTP ' + resp.status);
				return resp.text();
			})
			.then(function(text)
			{
				delete self._running[name];
				if (self.onSuccess) self.onSuccess.call(self, name, text);
				if (def.onSuccess) def.onSuccess.call(self, text);
				if (self.autoAdvance) self.runNext();
				if (!self._queue.length && !Object.keys(self._running).length)
				{
					if (self.onEnd) self.onEnd.call(self);
				}
			})
			.catch(function(err)
			{
				delete self._running[name];
				if (err.name === 'AbortError') return; // cancelled — don't treat as failure
				self._error = true;
				if (self.onFailure) self.onFailure.call(self, name, err);
				if (def.onFailure) def.onFailure.call(self, err);
				if (!self.stopOnFailure && self.autoAdvance) self.runNext();
			});
	}
}
