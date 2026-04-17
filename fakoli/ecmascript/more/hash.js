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
 * Hash — a plain-object wrapper with a rich collection API.
 * ES equivalent of MooTools More Hash + Hash.Extras.
 */
class Hash
{
	/** @param {Object} [obj]  Initial key/value pairs. */
	constructor(obj)
	{
		this._data = Object.assign({}, obj || {});
	}

	/** Iterate over each key/value pair. */
	forEach(fn, bind)
	{
		var self = bind || this;
		var data = this._data;
		Object.keys(data).forEach(function(key)
		{
			fn.call(self, data[key], key);
		});
		return this;
	}

	/** Returns a plain object copy of the hash. */
	getClean()
	{
		return Object.assign({}, this._data);
	}

	/** Returns the number of entries. */
	getLength()
	{
		return Object.keys(this._data).length;
	}

	/** Returns the first key whose value strictly equals `value`, or null. */
	keyOf(value)
	{
		var data = this._data;
		return Object.keys(data).find(function(k) { return data[k] === value; }) || null;
	}

	/** Returns true if any value strictly equals `value`. */
	hasValue(value)
	{
		return this.keyOf(value) !== null;
	}

	/** Merges all properties from `obj` (or another Hash) into this hash. */
	extend(obj)
	{
		Object.assign(this._data, (obj instanceof Hash) ? obj._data : obj);
		return this;
	}

	/** Merges only keys that don't already exist in this hash. */
	combine(obj)
	{
		var data = this._data;
		var src  = (obj instanceof Hash) ? obj._data : obj;
		Object.keys(src).forEach(function(key)
		{
			if (!Object.prototype.hasOwnProperty.call(data, key))
			{
				data[key] = src[key];
			}
		});
		return this;
	}

	/** Removes the entry for `key`. */
	erase(key)
	{
		delete this._data[key];
		return this;
	}

	/** Returns the value for `key`. */
	get(key)
	{
		return this._data[key];
	}

	/** Sets `key` to `value`. */
	set(key, value)
	{
		this._data[key] = value;
		return this;
	}

	/** Removes all entries. */
	empty()
	{
		this._data = {};
		return this;
	}

	/** Sets `key` to `value` only if `key` is not already set. */
	include(key, value)
	{
		if (!Object.prototype.hasOwnProperty.call(this._data, key))
		{
			this._data[key] = value;
		}
		return this;
	}

	/** Returns a new Hash with values transformed by `fn(value, key)`. */
	map(fn, bind)
	{
		var self = bind || this;
		var data = this._data;
		var result = {};
		Object.keys(data).forEach(function(key)
		{
			result[key] = fn.call(self, data[key], key);
		});
		return new Hash(result);
	}

	/** Returns a new Hash containing only entries for which `fn(value, key)` is truthy. */
	filter(fn, bind)
	{
		var self = bind || this;
		var data = this._data;
		var result = {};
		Object.keys(data).forEach(function(key)
		{
			if (fn.call(self, data[key], key)) result[key] = data[key];
		});
		return new Hash(result);
	}

	/** Returns true if `fn(value, key)` returns true for every entry. */
	every(fn, bind)
	{
		var self = bind || this;
		var data = this._data;
		return Object.keys(data).every(function(key)
		{
			return fn.call(self, data[key], key);
		});
	}

	/** Returns true if `fn(value, key)` returns true for at least one entry. */
	some(fn, bind)
	{
		var self = bind || this;
		var data = this._data;
		return Object.keys(data).some(function(key)
		{
			return fn.call(self, data[key], key);
		});
	}

	/** Returns an array of all keys. */
	getKeys()
	{
		return Object.keys(this._data);
	}

	/** Returns an array of all values. */
	getValues()
	{
		return Object.values(this._data);
	}

	/** Serialises the hash to a URL query string. */
	toQueryString()
	{
		var params = new URLSearchParams();
		var data   = this._data;
		Object.keys(data).forEach(function(key)
		{
			params.set(key, data[key]);
		});
		return params.toString();
	}

	/**
	 * Retrieve a nested value using a dot-path string or array of keys.
	 * e.g. hash.getFromPath('a.b.c') or hash.getFromPath(['a','b','c'])
	 */
	getFromPath(path)
	{
		var keys = Array.isArray(path) ? path : String(path).split('.');
		var current = this._data;
		for (var i = 0; i < keys.length; i++)
		{
			if (current == null) return undefined;
			current = current[keys[i]];
		}
		return current;
	}

	/**
	 * Remove entries where `fn(value)` returns falsy (default: removes nullish values).
	 */
	cleanValues(fn)
	{
		var test = fn || function(v) { return v != null && v !== ''; };
		var data = this._data;
		Object.keys(data).forEach(function(key)
		{
			if (!test(data[key])) delete data[key];
		});
		return this;
	}

	/**
	 * Call the value stored at `key` as a function, passing `args`.
	 * @param {string} key
	 * @param {any[]} [args]
	 */
	run(key, args)
	{
		var fn = this._data[key];
		if (typeof fn === 'function') return fn.apply(this, args || []);
	}
}

/** Factory shorthand: returns a new Hash wrapping `obj`. */
function $H(obj)
{
	return new Hash(obj);
}

/**
 * HashCookie — a Hash whose contents are persisted to a browser cookie as JSON.
 * ES equivalent of MooTools More Hash.Cookie.
 */
class HashCookie extends Hash
{
	/**
	 * @param {string} name      Cookie name.
	 * @param {Object} [options]
	 * @param {number}  options.duration   Expiry in days (default: session cookie).
	 * @param {string}  options.path       Cookie path (default '/').
	 * @param {string}  options.domain     Cookie domain.
	 * @param {boolean} options.secure     Set Secure flag.
	 * @param {boolean} options.autoSave   Automatically save on every mutation (default true).
	 */
	constructor(name, options)
	{
		super();
		this._name    = name;
		this._options = Object.assign({ path: '/', autoSave: true }, options || {});
		this.load();
	}

	/** Serialise the hash to the cookie. Returns false if the value would exceed 4 KB. */
	save()
	{
		var value = JSON.stringify(this._data);
		if (!value || value.length > 4096) return false;

		var cookie = encodeURIComponent(this._name) + '=' + encodeURIComponent(value === '{}' ? '' : value);

		if (this._options.duration != null)
		{
			var exp = new Date();
			exp.setDate(exp.getDate() + this._options.duration);
			cookie += '; expires=' + exp.toUTCString();
		}
		if (this._options.path)    cookie += '; path='   + this._options.path;
		if (this._options.domain)  cookie += '; domain=' + this._options.domain;
		if (this._options.secure)  cookie += '; secure';
		if (this._options.sameSite) cookie += '; samesite=' + this._options.sameSite;

		if (value === '{}')
		{
			// Expire the cookie to clear it
			var past = new Date(0);
			document.cookie = encodeURIComponent(this._name) + '=; expires=' + past.toUTCString() + '; path=' + (this._options.path || '/');
		}
		else
		{
			document.cookie = cookie;
		}
		return true;
	}

	/** Load the hash from the cookie. */
	load()
	{
		var name = encodeURIComponent(this._name) + '=';
		var raw  = document.cookie.split('; ').find(function(c) { return c.startsWith(name); });
		var json = raw ? decodeURIComponent(raw.substring(name.length)) : null;
		try
		{
			this._data = JSON.parse(json) || {};
		}
		catch (e)
		{
			this._data = {};
		}
		return this;
	}

	// Override all mutating methods to auto-save after each call.

	set(key, value)      { super.set(key, value);       if (this._options.autoSave) this.save(); return this; }
	erase(key)           { super.erase(key);             if (this._options.autoSave) this.save(); return this; }
	empty()              { super.empty();                if (this._options.autoSave) this.save(); return this; }
	include(key, value)  { super.include(key, value);   if (this._options.autoSave) this.save(); return this; }
	extend(obj)          { super.extend(obj);            if (this._options.autoSave) this.save(); return this; }
	combine(obj)         { super.combine(obj);           if (this._options.autoSave) this.save(); return this; }
	cleanValues(fn)      { super.cleanValues(fn);        if (this._options.autoSave) this.save(); return this; }
}
