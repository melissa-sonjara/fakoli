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
 * ES equivalents of MooTools More class utilities:
 * Chain.Wait, Class.Binds, Class.Occlude, Class.Refactor, Class.Singleton,
 * and Elements.From.
 */

/**
 * Returns a Promise that resolves after the given duration (ms).
 * ES equivalent of Chain.Wait — use with async/await for timed sequencing.
 */
function chainWait(duration)
{
	return new Promise(function(resolve)
	{
		setTimeout(resolve, duration == null ? 500 : duration);
	});
}

/**
 * Prevents a behaviour from being applied twice to the same DOM element.
 * Stores the instance on the element via a WeakMap keyed by `key`.
 * If an instance already exists, returns it. Otherwise calls factory()
 * to create one, stores it, and returns it.
 *
 * ES equivalent of Class.Occlude.
 */
var _occludeStore = new WeakMap();

function occlude(element, key, factory)
{
	var map = _occludeStore.get(element);
	if (!map)
	{
		map = {};
		_occludeStore.set(element, map);
	}
	if (map[key] !== undefined)
	{
		return map[key];
	}
	var instance = factory();
	map[key] = instance;
	return instance;
}

/**
 * Wraps a class constructor so that it always returns the same instance
 * (singleton pattern).
 *
 * ES equivalent of Class.Singleton.
 */
function makeSingleton(ClassDef)
{
	var instance = null;
	return function()
	{
		if (!instance)
		{
			instance = new ClassDef(...arguments);
		}
		return instance;
	};
}

/**
 * Extends the prototype of `target` with new methods from `extensions`,
 * while allowing overridden methods to call their predecessor via
 * `this.previous(...)`.
 *
 * ES equivalent of Class.Refactor.
 */
function refactor(target, extensions)
{
	Object.keys(extensions).forEach(function(name)
	{
		var item = extensions[name];
		var origin = target.prototype[name] || function() {};

		if (typeof item === 'function')
		{
			target.prototype[name] = function()
			{
				var old = this.previous;
				this.previous = origin.bind(this);
				var result = item.apply(this, arguments);
				this.previous = old;
				return result;
			};
		}
		else
		{
			target.prototype[name] = item;
		}
	});
	return target;
}

/**
 * Binds the named methods of `instance` to that instance so they can be
 * safely detached and used as callbacks.
 *
 * ES equivalent of Class.Binds.
 */
function autoBindMethods(instance, methodNames)
{
	(Array.isArray(methodNames) ? methodNames : [methodNames]).forEach(function(name)
	{
		if (typeof instance[name] === 'function')
		{
			instance[name] = instance[name].bind(instance);
		}
	});
	return instance;
}

/**
 * Parses an HTML string and returns an array of the resulting DOM nodes.
 * Equivalent of MooTools Elements.from().
 */
function elementsFrom(html)
{
	var div = document.createElement('div');
	div.innerHTML = html;
	return Array.from(div.childNodes);
}
