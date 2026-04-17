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
 * Extensions to String.prototype mirroring MooTools More String.Extras
 * and String.QueryString.
 */

(function()
{

// Accent/special-character substitution table (from MooTools More String.Extras)
var special = {
	'a':  /[àáâãäåăą]/g,
	'A':  /[ÀÁÂÃÄÅĂĄ]/g,
	'c':  /[ćčç]/g,
	'C':  /[ĆČÇ]/g,
	'd':  /[ďđ]/g,
	'D':  /[ĎÐ]/g,
	'e':  /[èéêëěę]/g,
	'E':  /[ÈÉÊËĚĘ]/g,
	'g':  /[ğ]/g,
	'G':  /[Ğ]/g,
	'i':  /[ìíîï]/g,
	'I':  /[ÌÍÎÏ]/g,
	'l':  /[ĺľł]/g,
	'L':  /[ĹĽŁ]/g,
	'n':  /[ñňń]/g,
	'N':  /[ÑŇŃ]/g,
	'o':  /[òóôõöøő]/g,
	'O':  /[ÒÓÔÕÖØ]/g,
	'r':  /[řŕ]/g,
	'R':  /[ŘŔ]/g,
	's':  /[ššş]/g,
	'S':  /[ŠŞŚ]/g,
	't':  /[ťţ]/g,
	'T':  /[ŤŢ]/g,
	'u':  /[ùúûůüµ]/g,
	'U':  /[ÙÚÛŮÜ]/g,
	'y':  /[ÿý]/g,
	'Y':  /[ŸÝ]/g,
	'z':  /[žźż]/g,
	'Z':  /[ŽŹŻ]/g,
	'th': /[þ]/g,
	'TH': /[Þ]/g,
	'dh': /[ð]/g,
	'DH': /[Ð]/g,
	'ss': /[ß]/g,
	'oe': /[œ]/g,
	'OE': /[Œ]/g,
	'ae': /[æ]/g,
	'AE': /[Æ]/g
};

// Character tidying table (fancy typographic characters → ASCII)
var tidy = {
	' ':     /[\xa0\u2002\u2003\u2009]/g,
	'*':     /[\xb7]/g,
	"'":     /[\u2018\u2019]/g,
	'"':     /[\u201c\u201d]/g,
	'...':   /[\u2026]/g,
	'-':     /[\u2013]/g,
	'&raquo;': /[\uFFFD]/g
};

// Time-unit multipliers for ms()
var timeConversions = { ms: 1, s: 1000, m: 6e4, h: 36e5 };
var findUnits = /(\d*\.?\d+)([msh]+)/;

function walk(string, replacements)
{
	var result = string;
	for (var key in replacements)
	{
		result = result.replace(replacements[key], key);
	}
	return result;
}

function getRegexForTag(tag, contents)
{
	tag = tag || (contents ? '' : '\\w+');
	var regstr = contents
		? '<' + tag + '(?!\\w)[^>]*>([\\s\\S]*?)<\\/' + tag + '(?!\\w)>'
		: '<\\/?' + tag + '\\/?>|<' + tag + '[\\s|\\/][^>]*>';
	return new RegExp(regstr, 'gi');
}

/**
 * Replace accented / special characters with ASCII equivalents.
 */
String.prototype.standardize = function()
{
	return walk(String(this), special);
};

/**
 * Repeat this string `times` times.
 * Guards against the native implementation for older environments.
 */
if (!String.prototype.repeat)
{
	String.prototype.repeat = function(times)
	{
		return new Array(times + 1).join(String(this));
	};
}

/**
 * Pad this string to `length` with `str` (default ' ') in the given direction.
 * @param {number} length
 * @param {string} [str=' ']
 * @param {'right'|'left'|'both'} [direction='right']
 */
String.prototype.pad = function(length, str, direction)
{
	var s = String(this);
	if (s.length >= length) return s;
	var pad = (str == null ? ' ' : String(str))
		.repeat(length - s.length)
		.substr(0, length - s.length);
	if (!direction || direction === 'right') return s + pad;
	if (direction === 'left') return pad + s;
	// both
	var half = Math.floor(pad.length / 2);
	return pad.substr(0, half) + s + pad.substr(0, pad.length - half);
};

/**
 * Return an array of all HTML tags (optionally matching a specific tag name).
 * @param {string} [tag]
 * @param {boolean} [contents]  If true, returns the inner content of each tag.
 */
String.prototype.getTags = function(tag, contents)
{
	return String(this).match(getRegexForTag(tag, contents)) || [];
};

/**
 * Strip HTML tags from the string.
 * @param {string} [tag]      If given, only that tag is stripped.
 * @param {boolean} [contents]  If true, strips the tag AND its contents.
 */
String.prototype.stripTags = function(tag, contents)
{
	return String(this).replace(getRegexForTag(tag, contents), '');
};

/**
 * Replace typographic / non-ASCII punctuation with ASCII equivalents.
 */
String.prototype.tidy = function()
{
	return walk(String(this), tidy);
};

/**
 * Truncate the string to at most `max` characters, appending `trail`
 * (default '…'). If `atChar` is given, the cut is made at the last
 * occurrence of that character within the truncated portion.
 */
String.prototype.truncate = function(max, trail, atChar)
{
	var s = String(this);
	if (trail === undefined && arguments.length === 1) trail = '\u2026';
	if (s.length > max)
	{
		s = s.substring(0, max);
		if (atChar)
		{
			var idx = s.lastIndexOf(atChar);
			if (idx !== -1) s = s.substr(0, idx);
		}
		if (trail) s += trail;
	}
	return s;
};

/**
 * Parse a time string like '500ms', '2s', '1m', '1h' into milliseconds.
 * Falls back to Number(this) for plain numeric strings.
 */
String.prototype.ms = function()
{
	var units = findUnits.exec(String(this));
	if (units == null) return Number(this);
	return Number(units[1]) * (timeConversions[units[2]] || 1);
};

/**
 * Parse a query string (with or without leading '?') into a plain object.
 * @param {boolean} [decodeKeys=true]
 * @param {boolean} [decodeValues=true]
 */
String.prototype.parseQueryString = function(decodeKeys, decodeValues)
{
	decodeKeys   = decodeKeys   !== false;
	decodeValues = decodeValues !== false;

	var qs = String(this).replace(/^\?/, '');
	var result = {};
	if (!qs) return result;

	qs.split('&').forEach(function(pair)
	{
		if (!pair) return;
		var parts = pair.split('=');
		var key   = decodeKeys   ? decodeURIComponent(parts[0]) : parts[0];
		var value = parts.length > 1
			? (decodeValues ? decodeURIComponent(parts.slice(1).join('=')) : parts.slice(1).join('='))
			: '';
		result[key] = value;
	});
	return result;
};

/**
 * Remove empty (or otherwise unwanted) parameters from a query string.
 * @param {Function} [method]  Return true to keep a key/value pair.
 *                              Defaults to keeping non-empty values.
 */
String.prototype.cleanQueryString = function(method)
{
	var test = method || function(key, value) { return value !== ''; };
	var qs   = String(this).replace(/^\?/, '');
	if (!qs) return '';

	var kept = qs.split('&').filter(function(pair)
	{
		if (!pair) return false;
		var parts = pair.split('=');
		var key   = decodeURIComponent(parts[0]);
		var value = parts.length > 1 ? decodeURIComponent(parts.slice(1).join('=')) : '';
		return test(key, value);
	});

	return kept.join('&');
};

})();
