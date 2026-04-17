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
 * Date extensions — extends the native Date prototype with formatting,
 * parsing, arithmetic, and relative-time helpers.
 *
 * ES equivalent of MooTools More Date + Date.Extras.
 *
 * Depends on locale.js (Locale singleton) being loaded first so that
 * localized month/day names are available.
 */
(function()
{

// ── Private helpers ───────────────────────────────────────────────────────

function _pad(n, digits, padChar)
{
	var s = String(Math.abs(Math.round(n)));
	while (s.length < digits) s = (padChar || '0') + s;
	return s;
}

function _msg(key, args)
{
	return (typeof Locale !== 'undefined') ? Locale.get('Date.' + key, args) : '';
}

// Named format strings
var _formats =
{
	db:      '%Y-%m-%d %H:%M:%S',
	compact: '%Y%m%dT%H%M%S',
	'short': '%d %b %H:%M',
	'long':  '%B %d, %Y %H:%M',
	rfc822: function(d)
	{
		var da = ['Sun','Mon','Tue','Wed','Thu','Fri','Sat'];
		var mo = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
		return da[d.getDay()] + d.format(', %d ') + mo[d.getMonth()] + d.format(' %Y %H:%M:%S ') + d.getTimezone();
	},
	rfc2822: function(d)
	{
		var da = ['Sun','Mon','Tue','Wed','Thu','Fri','Sat'];
		var mo = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
		return da[d.getDay()] + d.format(', %d ') + mo[d.getMonth()] + d.format(' %Y %H:%M:%S ') + d.getGMTOffset();
	},
	iso8601: function(d)
	{
		return d.getUTCFullYear() + '-' +
			_pad(d.getUTCMonth() + 1, 2) + '-' +
			_pad(d.getUTCDate(), 2) + 'T' +
			_pad(d.getUTCHours(), 2) + ':' +
			_pad(d.getUTCMinutes(), 2) + ':' +
			_pad(d.getUTCSeconds(), 2) + '.' +
			_pad(d.getUTCMilliseconds(), 3) + 'Z';
	}
};

// ── Date.prototype extensions ─────────────────────────────────────────────

/**
 * Clone this date.
 * @returns {Date}
 */
Date.prototype.clone = function()
{
	return new Date(this.getTime());
};

/**
 * Clear time portion (midnight).
 * @returns {Date} this
 */
Date.prototype.clearTime = function()
{
	this.setHours(0, 0, 0, 0);
	return this;
};

/**
 * Increment the date by `times` units of `interval`.
 * @param {string} [interval]  'year'|'month'|'week'|'day'|'hour'|'minute'|'second'|'ms' (default 'day').
 * @param {number} [times]     Number of units (default 1).
 * @returns {Date} this
 */
Date.prototype.increment = function(interval, times)
{
	interval = interval || 'day';
	times    = times != null ? times : 1;
	switch (interval)
	{
		case 'year':
			return this.increment('month', times * 12);
		case 'month':
			var d = this.getDate();
			this.setDate(1);
			this.setMonth(this.getMonth() + times);
			this.setDate(Math.min(d, Date.daysInMonth(this.getMonth(), this.getFullYear())));
			return this;
		case 'week':
			return this.increment('day', times * 7);
		case 'day':
			this.setDate(this.getDate() + times);
			return this;
		case 'hour':
			this.setTime(this.getTime() + times * 3600000);
			return this;
		case 'minute':
			this.setTime(this.getTime() + times * 60000);
			return this;
		case 'second':
			this.setTime(this.getTime() + times * 1000);
			return this;
		case 'ms':
			this.setTime(this.getTime() + times);
			return this;
	}
	var unit = Date.units[interval];
	if (!unit) throw new Error(interval + ' is not a supported interval');
	this.setTime(this.getTime() + times * unit());
	return this;
};

/**
 * Decrement (negative increment).
 * @param {string} [interval]
 * @param {number} [times]
 * @returns {Date} this
 */
Date.prototype.decrement = function(interval, times)
{
	return this.increment(interval, -1 * (times != null ? times : 1));
};

/**
 * @returns {boolean} True in a leap year.
 */
Date.prototype.isLeapYear = function()
{
	return Date.isLeapYear(this.getFullYear());
};

/**
 * Difference in `resolution` units between this date and `date`.
 * @param {Date|string} date
 * @param {string}      [resolution]  Default 'day'.
 * @returns {number}
 */
Date.prototype.diff = function(date, resolution)
{
	if (typeof date === 'string') date = Date.parse(date);
	var unit = Date.units[resolution || 'day'];
	return Math.round((date - this) / (unit ? unit(3, 3) : 86400000));
};

/**
 * @returns {number} Last day-of-month number for this date.
 */
Date.prototype.getLastDayOfMonth = function()
{
	return Date.daysInMonth(this.getMonth(), this.getFullYear());
};

/**
 * @returns {number} Day of year (1-based).
 */
Date.prototype.getDayOfYear = function()
{
	return Math.floor((Date.UTC(this.getFullYear(), this.getMonth(), this.getDate()) -
		Date.UTC(this.getFullYear(), 0, 1)) / 86400000) + 1;
};

/**
 * @returns {number} ISO week number.
 */
Date.prototype.getWeek = function()
{
	var d    = new Date(Date.UTC(this.getFullYear(), this.getMonth(), this.getDate()));
	var day  = d.getUTCDay() || 7;
	d.setUTCDate(d.getUTCDate() + 4 - day);
	var year = new Date(Date.UTC(d.getUTCFullYear(), 0, 1));
	return Math.ceil(((d - year) / 86400000 + 1) / 7);
};

/**
 * @returns {string} GMT offset string, e.g. '+0500'.
 */
Date.prototype.getGMTOffset = function()
{
	var off = this.getTimezoneOffset();
	return (off > 0 ? '-' : '+') + _pad(Math.floor(Math.abs(off) / 60), 2) + _pad(Math.abs(off) % 60, 2);
};

/**
 * @returns {string} Abbreviated timezone identifier from toString().
 */
Date.prototype.getTimezone = function()
{
	return this.toString()
		.replace(/^.*? ([A-Z]{3}).[0-9]{4}.*$/, '$1')
		.replace(/^.*?\(([A-Z])[a-z]+ ([A-Z])[a-z]+ ([A-Z])[a-z]+\)$/, '$1$2$3');
};

/**
 * @returns {string} Ordinal suffix for the day-of-month.
 */
Date.prototype.getOrdinal = function()
{
	return _msg('ordinal', this.getDate()) || '';
};

/**
 * @returns {string} 'AM' | 'PM'.
 */
Date.prototype.getAMPM = function()
{
	return this.getHours() < 12 ? 'AM' : 'PM';
};

/**
 * Set AM/PM.
 * @param {string} ampm  'AM' | 'PM' (case-insensitive).
 * @returns {Date} this
 */
Date.prototype.setAMPM = function(ampm)
{
	ampm = ampm.toUpperCase();
	var hr = this.getHours();
	if (hr > 11 && ampm === 'AM') return this.decrement('hour', 12);
	if (hr < 12 && ampm === 'PM') return this.increment('hour', 12);
	return this;
};

/**
 * @returns {boolean} True when the date value is valid (not NaN).
 */
Date.prototype.isValid = function()
{
	return !isNaN(this.valueOf());
};

/**
 * Format the date using strftime-style patterns.
 *
 * Supported tokens: %a %A %b %B %c %d %e %H %I %j %k %l %L %m %M %o %p %s %S %T %U %w %x %X %y %Y %z %Z %%
 *
 * Named formats: 'db', 'compact', 'short', 'long', 'rfc822', 'rfc2822', 'iso8601'
 *
 * @param {string|Function} [format]  Default '%x %X'.
 * @returns {string}
 */
Date.prototype.format = function(format)
{
	if (!this.isValid()) return 'invalid date';
	if (!format) format = '%x %X';

	if (typeof format === 'string')
	{
		var lc = format.toLowerCase();
		if (_formats[lc]) format = _formats[lc];
	}
	if (typeof format === 'function') return format(this);

	var d = this;
	return format.replace(/%([a-z%])/gi, function($0, $1)
	{
		switch ($1)
		{
			case 'a': return (_msg('days_abbr')  || [])[d.getDay()]        || '';
			case 'A': return (_msg('days')       || [])[d.getDay()]        || '';
			case 'b': return (_msg('months_abbr') || [])[d.getMonth()]     || '';
			case 'B': return (_msg('months')      || [])[d.getMonth()]     || '';
			case 'c': return d.format('%a %b %d %H:%M:%S %Y');
			case 'd': return _pad(d.getDate(), 2);
			case 'e': return _pad(d.getDate(), 2, ' ');
			case 'H': return _pad(d.getHours(), 2);
			case 'I': return _pad((d.getHours() % 12) || 12, 2);
			case 'j': return _pad(d.getDayOfYear(), 3);
			case 'k': return _pad(d.getHours(), 2, ' ');
			case 'l': return _pad((d.getHours() % 12) || 12, 2, ' ');
			case 'L': return _pad(d.getMilliseconds(), 3);
			case 'm': return _pad(d.getMonth() + 1, 2);
			case 'M': return _pad(d.getMinutes(), 2);
			case 'o': return d.getOrdinal();
			case 'p': return _msg(d.getAMPM()) || d.getAMPM();
			case 's': return String(Math.round(d.getTime() / 1000));
			case 'S': return _pad(d.getSeconds(), 2);
			case 'T': return d.format('%H:%M:%S');
			case 'U': return _pad(d.getWeek(), 2);
			case 'w': return String(d.getDay());
			case 'x': return d.format(_msg('shortDate') || '%m/%d/%Y');
			case 'X': return d.format(_msg('shortTime') || '%I:%M%p');
			case 'y': return String(d.getFullYear()).slice(-2);
			case 'Y': return String(d.getFullYear());
			case 'z': return d.getGMTOffset();
			case 'Z': return d.getTimezone();
			case '%': return '%';
		}
		return $1;
	});
};

/** Alias. */
Date.prototype.strftime   = Date.prototype.format;
Date.prototype.toISOString = Date.prototype.toISOString || function() { return this.format('iso8601'); };

/**
 * Human-readable relative time string (e.g. "about 2 hours ago").
 * @returns {string}
 */
Date.prototype.timeDiffInWords = function()
{
	var now   = new Date();
	var diff  = Math.floor((now - this) / 1000); // seconds
	var future = diff < 0;
	diff = Math.abs(diff);

	var suffix = future ? 'Until' : 'Ago';
	var str;

	if (diff < 60)         str = _msg('lessThanMinute' + suffix) || (future ? 'less than a minute from now' : 'less than a minute ago');
	else if (diff < 120)   str = _msg('minute' + suffix) || (future ? 'about a minute from now' : 'about a minute ago');
	else if (diff < 3600)  str = (_msg('minutes' + suffix) || '{delta} minutes').replace('{delta}', Math.floor(diff / 60));
	else if (diff < 7200)  str = _msg('hour' + suffix) || (future ? 'about an hour from now' : 'about an hour ago');
	else if (diff < 86400) str = (_msg('hours' + suffix) || 'about {delta} hours').replace('{delta}', Math.floor(diff / 3600));
	else if (diff < 172800) str = _msg('day' + suffix) || (future ? '1 day from now' : '1 day ago');
	else if (diff < 604800) str = (_msg('days' + suffix) || '{delta} days').replace('{delta}', Math.floor(diff / 86400));
	else if (diff < 1209600) str = _msg('week' + suffix) || (future ? '1 week from now' : '1 week ago');
	else if (diff < 2592000) str = (_msg('weeks' + suffix) || '{delta} weeks').replace('{delta}', Math.floor(diff / 604800));
	else if (diff < 5184000) str = _msg('month' + suffix) || (future ? '1 month from now' : '1 month ago');
	else if (diff < 31536000) str = (_msg('months' + suffix) || '{delta} months').replace('{delta}', Math.floor(diff / 2592000));
	else if (diff < 63072000) str = _msg('year' + suffix) || (future ? '1 year from now' : '1 year ago');
	else str = (_msg('years' + suffix) || '{delta} years').replace('{delta}', Math.floor(diff / 31536000));

	return str;
};

// ── Static methods ─────────────────────────────────────────────────────────

/**
 * @param {number} month  0-based month index.
 * @param {number} year
 * @returns {number} Number of days in that month.
 */
Date.daysInMonth = function(month, year)
{
	return [31, Date.isLeapYear(year) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31][month];
};

/**
 * @param {number} year
 * @returns {boolean}
 */
Date.isLeapYear = function(year)
{
	return ((year % 4 === 0) && (year % 100 !== 0)) || (year % 400 === 0);
};

/** Time unit values in milliseconds (as functions for consistency with MooTools). */
Date.units =
{
	ms:     function() { return 1; },
	second: function() { return 1000; },
	minute: function() { return 60000; },
	hour:   function() { return 3600000; },
	day:    function() { return 86400000; },
	week:   function() { return 604800000; },
	month:  function(month, year)
	{
		var d = new Date();
		return Date.daysInMonth(month != null ? month : d.getMonth(), year != null ? year : d.getFullYear()) * 86400000;
	},
	year: function(year)
	{
		year = year || new Date().getFullYear();
		return Date.isLeapYear(year) ? 31622400000 : 31536000000;
	}
};

/**
 * Get a localized Date string.
 * @param {string} key
 * @param {*}      [args]
 */
Date.getMsg = function(key, args)
{
	return _msg(key, args);
};

/**
 * Add a named format.
 * @param {string}          name
 * @param {string|Function} format
 */
Date.defineFormat = function(name, format)
{
	_formats[name] = format;
	return Date;
};

// ── Date.parse extension ──────────────────────────────────────────────────

var _nativeParse    = Date.parse;
var _parsePatterns  = [];
var _startCentury   = 1900;
var _startYear      = 70;

/**
 * Extended Date.parse — tries registered patterns first, then falls back to
 * the native implementation.
 * @param {string|number|Date} from
 * @returns {Date|null}
 */
Date.parse = function(from)
{
	if (from instanceof Date)   return from;
	if (typeof from === 'number') return new Date(from);
	if (typeof from !== 'string') return null;

	var str = from.trim();
	if (!str) return null;

	var parsed = null;
	_parsePatterns.some(function(pattern)
	{
		var bits = pattern.re.exec(str);
		if (bits)
		{
			parsed = pattern.handler(bits);
			return true;
		}
		return false;
	});

	if (parsed && parsed.isValid()) return parsed;

	var ts = _nativeParse(str);
	if (!isNaN(ts)) return new Date(ts);

	var asInt = parseInt(str, 10);
	if (!isNaN(asInt)) return new Date(asInt);

	return null;
};

/**
 * Lookup a month name/abbreviation and return the 0-based index.
 * @param {string|number} month
 * @returns {number}
 */
Date.parseMonth = function(month, asIndex)
{
	var months = _msg('months') || [];
	var abbr   = _msg('months_abbr') || [];
	if (typeof month === 'number') return asIndex ? month : months[month];
	var re     = new RegExp('^' + month, 'i');
	var idx    = months.findIndex(function(m) { return re.test(m); });
	if (idx < 0) idx = abbr.findIndex(function(m) { return re.test(m); });
	return asIndex ? idx : (months[idx] || null);
};

/**
 * Lookup a day name and return the 0-based index.
 * @param {string|number} day
 * @returns {number}
 */
Date.parseDay = function(day, asIndex)
{
	var days = _msg('days') || [];
	var abbr = _msg('days_abbr') || [];
	if (typeof day === 'number') return asIndex ? day : days[day];
	var re   = new RegExp('^' + day, 'i');
	var idx  = days.findIndex(function(d) { return re.test(d); });
	if (idx < 0) idx = abbr.findIndex(function(d) { return re.test(d); });
	return asIndex ? idx : (days[idx] || null);
};

/**
 * Register a parser or format string to extend Date.parse().
 * @param {Object|string} pattern  {re, handler} or a format string.
 */
Date.defineParser = function(pattern)
{
	if (pattern && pattern.re && pattern.handler)
	{
		_parsePatterns.push(pattern);
	}
	else if (typeof pattern === 'string')
	{
		// Build a simple parser for common ISO / US date patterns
		_parsePatterns.push(_buildParser(pattern));
	}
	return Date;
};

Date.defineParsers = function()
{
	var args = Array.from(arguments).reduce(function(a, b) { return a.concat(b); }, []);
	args.forEach(Date.defineParser);
	return Date;
};

Date.define2DigitYearStart = function(year)
{
	_startYear    = year % 100;
	_startCentury = year - _startYear;
	return Date;
};

/** @private Build a simple regex-based parser for a strftime-like format. */
function _buildParser(format)
{
	var parsed = [];
	var keyMap =
	{
		d: '([0-2]?[0-9]|3[01])',
		H: '([01]?[0-9]|2[0-3])',
		I: '(0?[1-9]|1[0-2])',
		M: '([0-5]?\\d)',
		s: '(\\d+)',
		S: '([0-5]?\\d)',
		m: '(0?[1-9]|1[0-2])',
		Y: '(\\d{4})',
		y: '(\\d{2}|\\d{4})',
		p: '([aApP]\\.?[mM]\\.?)',
		z: '(Z|[+-]\\d{2}:?\\d{2}?)'
	};

	var reStr = format
		.replace(/%([a-zA-Z])/g, function($0, $1)
		{
			if (keyMap[$1])
			{
				parsed.push($1);
				return keyMap[$1];
			}
			return '(?:.+?)';
		})
		.replace(/[.+*?^${}()|[\]\\]/g, '\\$&')
		.replace(/\\[(]/g, '(')
		.replace(/\\[)]/g, ')');

	return {
		re: new RegExp('^' + reStr + '$', 'i'),
		handler: function(bits)
		{
			var map   = {};
			var parts = bits.slice(1);
			parsed.forEach(function(k, i) { map[k] = parts[i]; });

			var date = new Date();
			date.clearTime();

			if (map.Y) date.setFullYear(parseInt(map.Y, 10));
			if (map.y)
			{
				var yr = parseInt(map.y, 10);
				if (yr < 100) yr += _startCentury + (yr < _startYear ? 100 : 0);
				date.setFullYear(yr);
			}
			if (map.m) date.setMonth(parseInt(map.m, 10) - 1);
			if (map.d) date.setDate(parseInt(map.d, 10));
			if (map.H) date.setHours(parseInt(map.H, 10));
			if (map.I)
			{
				var hr = parseInt(map.I, 10);
				if (map.p && /p/i.test(map.p) && hr < 12) hr += 12;
				if (map.p && /a/i.test(map.p) && hr === 12) hr = 0;
				date.setHours(hr);
			}
			if (map.M) date.setMinutes(parseInt(map.M, 10));
			if (map.S) date.setSeconds(parseInt(map.S, 10));
			if (map.s) date.setMilliseconds(Math.round(parseFloat('0.' + map.s) * 1000));
			return date;
		}
	};
}

/**
 * Set the date to the given day of the week within the current week.
 * @param {number|string} day           Day index (0 = Sun) or day name.
 * @param {number}        [firstDayOfWeek]  First day of the week (default from
 *                                          locale; falls back to 1 = Monday).
 */
Date.prototype.setDay = function(day, firstDayOfWeek)
{
	if (firstDayOfWeek == null)
	{
		firstDayOfWeek = Date.getMsg('firstDayOfWeek');
		if (firstDayOfWeek === '') firstDayOfWeek = 1;
	}

	day        = (7 + Date.parseDay(day, true) - firstDayOfWeek) % 7;
	var current = (7 + this.getDay()            - firstDayOfWeek) % 7;

	return this.increment('day', day - current);
};

/**
 * Returns a compact string of the time difference between this date and `to`
 * (default: now), broken into years / days / hours / minutes / seconds.
 * @param {Date}   [to]         End date (default: now).
 * @param {string} [separator]  Separator between components (default ':').
 * @returns {string}  e.g. "1y:3d:2h:15m:30s"
 */
Date.prototype.timeDiff = function(to, separator)
{
	if (to == null) to = new Date;
	var delta = Math.abs(Math.floor((to - this) / 1000));

	var vals      = [];
	var durations = [60, 60, 24, 365, 0];
	var names     = ['s', 'm', 'h', 'd', 'y'];

	for (var item = 0; item < durations.length; item++)
	{
		if (item && !delta) break;
		var value    = delta;
		var duration = durations[item];
		if (duration)
		{
			value = delta % duration;
			delta = Math.floor(delta / duration);
		}
		vals.unshift(value + (names[item] || ''));
	}

	return vals.join(separator != null ? separator : ':');
};

// Register common parsing patterns
Date.defineParsers(
	'%Y-%m-%d',         // 1999-12-31
	'%Y-%m-%dT%H:%M:%S', // ISO 8601
	'%m/%d/%Y',         // 12/31/1999
	'%m/%d/%y',         // 12/31/99
	'%d %b %Y',         // 31 Dec 1999
	'%b %d %Y',         // Dec 31 1999
	'%H:%M',            // 11:05
	'%I:%M %p'          // 11:05 PM
);

})();
