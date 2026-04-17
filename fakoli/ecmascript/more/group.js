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
 * EventGroup — monitors a collection of EventTarget objects and fires a
 * callback once every member has fired the given event type at least once.
 *
 * ES equivalent of MooTools More Group.
 */
class EventGroup
{
	/**
	 * @param {...EventTarget|EventTarget[]} targets  One or more targets to monitor.
	 */
	constructor()
	{
		this.instances = Array.from(arguments).flat();
	}

	/**
	 * Register a listener that fires after every instance has emitted `type`
	 * at least once (per round — resets after firing).
	 *
	 * For DOM elements the native addEventListener is used.
	 * For plain objects with an addEvent() method (legacy pattern) that is used.
	 *
	 * @param {string} type
	 * @param {Function} fn  Called with (instances, lastFired, args).
	 */
	addEvent(type, fn)
	{
		var instances = this.instances;
		var len  = instances.length;
		var togo = len;
		var args = new Array(len);
		var self = this;

		instances.forEach(function(instance, i)
		{
			var handler = function()
			{
				if (!args[i]) togo--;
				args[i] = Array.from(arguments);
				if (!togo)
				{
					fn.call(self, instances, instance, args);
					togo = len;
					args = new Array(len);
				}
			};

			if (typeof instance.addEventListener === 'function')
			{
				instance.addEventListener(type, handler);
			}
			else if (typeof instance.addEvent === 'function')
			{
				instance.addEvent(type, handler);
			}
		});
	}
}

/**
 * EventPseudos — pseudo-event syntax support for addEventListener.
 *
 * Supported pseudo suffixes (appended with a colon):
 *   element.addEventListener('click:once', fn)
 *   element.addEventListener('input:pause(300)', fn)
 *   element.addEventListener('scroll:throttle(250)', fn)
 *
 * Usage:
 *   EventPseudos.addPseudoEvent(el, 'click:once', handler);
 *   EventPseudos.removePseudoEvent(el, 'click:once', handler);
 */
var EventPseudos = (function()
{
	var _pseudos = {};

	// Built-in: once — fires handler once then removes it
	_pseudos['once'] = function(baseType, fn, value, target)
	{
		var wrapper = function()
		{
			fn.apply(this, arguments);
			target.removeEventListener(baseType, wrapper);
		};
		return wrapper;
	};

	// Built-in: throttle(ms) — rate-limits the handler
	_pseudos['throttle'] = function(baseType, fn, value)
	{
		var delay = parseInt(value, 10) || 250;
		var last  = 0;
		return function()
		{
			var now = Date.now();
			if (now - last >= delay)
			{
				last = now;
				fn.apply(this, arguments);
			}
		};
	};

	// Built-in: pause(ms) — debounces the handler
	_pseudos['pause'] = function(baseType, fn, value)
	{
		var delay  = parseInt(value, 10) || 250;
		var timer  = null;
		var self   = this;
		return function()
		{
			var ctx  = this;
			var args = arguments;
			clearTimeout(timer);
			timer = setTimeout(function() { fn.apply(ctx, args); }, delay);
		};
	};

	function _parse(type)
	{
		var match = type.match(/^([^:]+):(\w+)(?:\(([^)]*)\))?$/);
		if (!match) return null;
		return { baseType: match[1], pseudo: match[2], value: match[3] || null };
	}

	// WeakMap: element → Map<originalFn → wrappedFn>
	var _wrappers = new WeakMap();

	return {
		/**
		 * Register a new pseudo-event handler.
		 * @param {string} key   Pseudo name (e.g. 'once')
		 * @param {Function} handler  factory(baseType, fn, value, target) → wrappedFn
		 */
		definePseudo: function(key, handler)
		{
			_pseudos[key] = handler;
		},

		/** Look up a pseudo handler by name. */
		lookupPseudo: function(key)
		{
			return _pseudos[key] || null;
		},

		/**
		 * Add an event listener that honours pseudo syntax.
		 * Falls back to plain addEventListener if no pseudo is found.
		 */
		addPseudoEvent: function(target, type, fn)
		{
			var parsed = _parse(type);
			if (!parsed || !_pseudos[parsed.pseudo])
			{
				target.addEventListener(type, fn);
				return;
			}

			var factory = _pseudos[parsed.pseudo];
			var wrapper = factory(parsed.baseType, fn, parsed.value, target);

			var map = _wrappers.get(target);
			if (!map)
			{
				map = new Map();
				_wrappers.set(target, map);
			}
			// key on [type + fn] so we can remove later
			map.set(type + '::' + fn.toString().slice(0, 60), wrapper);

			target.addEventListener(parsed.baseType, wrapper);
		},

		/** Remove a pseudo event listener that was added with addPseudoEvent. */
		removePseudoEvent: function(target, type, fn)
		{
			var parsed = _parse(type);
			if (!parsed || !_pseudos[parsed.pseudo])
			{
				target.removeEventListener(type, fn);
				return;
			}

			var map = _wrappers.get(target);
			if (!map) return;

			var key     = type + '::' + fn.toString().slice(0, 60);
			var wrapper = map.get(key);
			if (wrapper)
			{
				target.removeEventListener(parsed.baseType, wrapper);
				map.delete(key);
			}
		}
	};
})();
