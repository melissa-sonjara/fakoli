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
 * URI — wraps the native URL API with a MooTools-compatible interface.
 * Includes URI.Relative functionality for converting between absolute
 * and relative URLs.
 *
 * ES equivalent of MooTools More URI + URI.Relative.
 */
class URI
{
	/**
	 * @param {string|URL|URI} [uri]   URI string or object. Defaults to the current page.
	 * @param {string|URL}     [base]  Base for resolving relative URIs.
	 */
	constructor(uri, base)
	{
		var raw;
		if (!uri)
		{
			raw = window.location.href;
		}
		else if (uri instanceof URI)
		{
			raw = uri.toString();
		}
		else if (uri instanceof URL)
		{
			raw = uri.href;
		}
		else
		{
			raw = String(uri);
		}

		var baseStr = base ? String(base instanceof URI ? base.toString() : base) : undefined;

		try
		{
			this._url = new URL(raw, baseStr || window.location.href);
		}
		catch (e)
		{
			// Fallback for non-parseable strings
			this._url = new URL(window.location.href);
		}
	}

	/**
	 * Set a URI component by name.
	 * Recognised parts: scheme/protocol, host, hostname, port, directory/pathname,
	 * file, query/search, fragment/hash, data (sets query params from object).
	 */
	set(part, value)
	{
		var u = this._url;
		switch (part)
		{
			case 'scheme':
			case 'protocol':   u.protocol = value.replace(/:$/, '') + ':'; break;
			case 'host':       u.host     = value; break;
			case 'hostname':   u.hostname = value; break;
			case 'port':       u.port     = value; break;
			case 'directory':
			case 'pathname':   u.pathname = value; break;
			case 'file':
			{
				// Replace the filename portion of the path
				var dir = u.pathname.substring(0, u.pathname.lastIndexOf('/') + 1);
				u.pathname = dir + value;
				break;
			}
			case 'query':
			case 'search':     u.search   = value ? '?' + value.replace(/^\?/, '') : ''; break;
			case 'fragment':
			case 'hash':       u.hash     = value ? '#' + value.replace(/^#/, '') : ''; break;
			case 'data':       this.setData(value); break;
			default:           break;
		}
		return this;
	}

	/**
	 * Get a URI component by name.
	 * Recognised parts: scheme, host, hostname, port, directory, file,
	 * query, fragment, data (returns parsed query object).
	 */
	get(part)
	{
		var u = this._url;
		switch (part)
		{
			case 'scheme':
			case 'protocol':   return u.protocol.replace(/:$/, '');
			case 'host':       return u.host;
			case 'hostname':   return u.hostname;
			case 'port':       return u.port;
			case 'directory':
			case 'pathname':   return u.pathname.substring(0, u.pathname.lastIndexOf('/') + 1) || '/';
			case 'file':       return u.pathname.substring(u.pathname.lastIndexOf('/') + 1);
			case 'query':
			case 'search':     return u.search.replace(/^\?/, '');
			case 'fragment':
			case 'hash':       return u.hash.replace(/^#/, '');
			case 'data':       return this.getData();
			default:           return '';
		}
	}

	/**
	 * Get query parameter(s).
	 * @param {string} [key]  If given, returns that param's value. Otherwise returns all as object.
	 */
	getData(key)
	{
		var params = this._url.searchParams;
		if (key != null) return params.get(key);
		var obj = {};
		params.forEach(function(value, k) { obj[k] = value; });
		return obj;
	}

	/**
	 * Set query parameter(s).
	 * @param {Object|string} values  Object of params, or key string (with value as second arg).
	 * @param {boolean} [merge]       If true, merge with existing params.
	 */
	setData(values, merge)
	{
		if (typeof values === 'string')
		{
			this._url.searchParams.set(values, arguments[2] !== undefined ? arguments[2] : '');
			return this;
		}

		if (!merge) this._url.search = '';

		var params = this._url.searchParams;
		Object.keys(values).forEach(function(key)
		{
			params.set(key, values[key]);
		});
		return this;
	}

	/** Remove all query parameters. */
	clearData()
	{
		this._url.search = '';
		return this;
	}

	/** Navigate to this URI. */
	go()
	{
		document.location.href = this.toString();
	}

	/** Returns self (for compatibility). */
	toURI()
	{
		return this;
	}

	/**
	 * Convert this URI to an absolute URL relative to `base`.
	 * @param {string|URI|URL} base
	 */
	toAbsolute(base)
	{
		try
		{
			return new URI(new URL(this.toString(), String(base instanceof URI ? base.toString() : base)));
		}
		catch (e)
		{
			return new URI(this.toString());
		}
	}

	/**
	 * Convert this URI to a path relative to `base`.
	 * @param {string|URI|URL} base
	 */
	toRelative(base)
	{
		var baseUrl;
		try
		{
			baseUrl = new URL(String(base instanceof URI ? base.toString() : base));
		}
		catch (e)
		{
			return this.toString();
		}

		var thisUrl = this._url;

		// Must share origin to produce a relative path
		if (thisUrl.origin !== baseUrl.origin)
		{
			return this.toString();
		}

		var baseParts = baseUrl.pathname.split('/');
		var thisParts = thisUrl.pathname.split('/');

		// Remove filename from base directory
		baseParts.pop();

		// Find common prefix
		var offset = 0;
		while (offset < baseParts.length && offset < thisParts.length && baseParts[offset] === thisParts[offset])
		{
			offset++;
		}

		var path = '';
		for (var i = 0; i < baseParts.length - offset; i++) path += '../';
		for (var j = offset; j < thisParts.length; j++) path += thisParts[j] + (j < thisParts.length - 1 ? '/' : '');

		if (!path) path = './';

		return path
			+ (thisUrl.search || '')
			+ (thisUrl.hash   || '');
	}

	/** Returns the full URI string. */
	toString()
	{
		return this._url.href;
	}
}

// String.prototype helper
String.prototype.toURI = function(base)
{
	return new URI(String(this), base);
};
