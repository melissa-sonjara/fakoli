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
 * Locale — simple key/value localization store with inheritance.
 *
 * ES equivalent of MooTools More Locale.
 *
 * Usage:
 *   Locale.define('en-US', 'Date', { months: [...], days: [...] });
 *   Locale.use('en-US');
 *   Locale.get('Date.months');   // → Array
 *   Locale.get('Date.months', 3); // → 'April'
 */
(function()
{

var _locales  = {};
var _current  = null;
var _listeners = [];

// ── LocaleSet ─────────────────────────────────────────────────────────────

class LocaleSet
{
	constructor(name)
	{
		this.name     = name || '';
		this._sets    = {};
		this._inherits = { locales: [], sets: {} };
	}

	/**
	 * Define key/value data for a set category.
	 * @param {string}        set    Category name (e.g. 'Date').
	 * @param {string|Object} key    Key string or object of key→value pairs.
	 * @param {*}             [value]
	 */
	define(set, key, value)
	{
		if (!this._sets[set]) this._sets[set] = {};
		if (key)
		{
			if (typeof key === 'object') Object.assign(this._sets[set], key);
			else this._sets[set][key] = value;
		}
		return this;
	}

	/**
	 * Retrieve a value by dotted path (e.g. 'Date.months').
	 * Walks inheritance chain when not found locally.
	 * @param {string} key
	 * @param {*}      [args]  Passed to function values.
	 * @param {string[]} [_base]  Internal visited list (cycle guard).
	 */
	get(key, args, _base)
	{
		// Walk dotted path into _sets
		var value = _getFromPath(this._sets, key);

		if (value != null)
		{
			if (typeof value === 'function') return value.apply(null, args != null ? [].concat(args) : []);
			if (typeof value === 'object')   return JSON.parse(JSON.stringify(value)); // shallow clone
			return value;
		}

		// Inheritance
		var dot  = key.indexOf('.');
		var set  = dot < 0 ? key : key.substring(0, dot);
		var names = (this._inherits.sets[set] || []).concat(this._inherits.locales);
		if (names.indexOf('en-US') < 0) names.push('en-US');

		if (!_base) _base = [];

		for (var i = 0; i < names.length; i++)
		{
			var name = names[i];
			if (_base.indexOf(name) >= 0) continue;
			_base.push(name);
			var loc = _locales[name];
			if (!loc) continue;
			value = loc.get(key, args, _base);
			if (value != null) return value;
		}

		return '';
	}

	/**
	 * Declare that this locale inherits from others for fallback.
	 * @param {string|string[]} names  Locale name(s) to inherit.
	 * @param {string}          [set]  Limit inheritance to this category.
	 */
	inherit(names, set)
	{
		names = [].concat(names);
		if (set && !this._inherits.sets[set]) this._inherits.sets[set] = [];
		var target = set ? this._inherits.sets[set] : this._inherits.locales;
		names.forEach(function(n) { if (target.indexOf(n) < 0) target.unshift(n); });
		return this;
	}
}

// ── Helper ────────────────────────────────────────────────────────────────

function _getFromPath(obj, path)
{
	if (typeof path === 'string') path = path.split('.');
	var cur = obj;
	for (var i = 0; i < path.length; i++)
	{
		if (cur == null || !Object.prototype.hasOwnProperty.call(cur, path[i])) return null;
		cur = cur[path[i]];
	}
	return cur;
}

// ── Locale (public singleton) ─────────────────────────────────────────────

var Locale = window.Locale =
{
	/**
	 * Define locale data.
	 * @param {string}        localeName  e.g. 'en-US'.
	 * @param {string}        [set]       Category name.
	 * @param {string|Object} [key]       Key or object.
	 * @param {*}             [value]
	 * @returns {LocaleSet}
	 */
	define: function(localeName, set, key, value)
	{
		if (!_locales[localeName]) _locales[localeName] = new LocaleSet(localeName);
		var loc = _locales[localeName];
		if (set) loc.define(set, key, value);
		if (!_current) _current = loc;
		return loc;
	},

	/**
	 * Set the active locale.
	 * @param {string|LocaleSet} locale
	 */
	use: function(locale)
	{
		if (typeof locale === 'string') locale = _locales[locale];
		if (locale)
		{
			_current = locale;
			_listeners.forEach(function(fn) { fn(locale); });
		}
		return this;
	},

	/** @returns {LocaleSet|null} */
	getCurrent: function()
	{
		return _current;
	},

	/**
	 * Get a value from the current locale.
	 * @param {string} key   Dotted path e.g. 'Date.months'.
	 * @param {*}      [args]
	 */
	get: function(key, args)
	{
		return _current ? _current.get(key, args) : '';
	},

	/**
	 * Declare inheritance for a locale.
	 * @param {string}        localeName
	 * @param {string|string[]} inherits
	 * @param {string}        [set]
	 */
	inherit: function(localeName, inherits, set)
	{
		var loc = _locales[localeName];
		if (loc) loc.inherit(inherits, set);
		return this;
	},

	/** @returns {string[]} Names of all defined locales. */
	list: function()
	{
		return Object.keys(_locales);
	},

	/**
	 * Register a callback to be fired whenever the active locale changes.
	 * Fires immediately with the current locale if one is set.
	 * @param {string}   event  Only 'change' is supported.
	 * @param {Function} fn
	 */
	addEvent: function(event, fn)
	{
		if (event === 'change')
		{
			_listeners.push(fn);
			if (_current) fn(_current);
		}
		return this;
	},

	fireEvent: function(event, arg)
	{
		if (event === 'change') _listeners.forEach(function(fn) { fn(arg); });
		return this;
	}
};

// ── Built-in en-US locale data ────────────────────────────────────────────

Locale.define('en-US', 'Date',
{
	months:       ['January','February','March','April','May','June','July','August','September','October','November','December'],
	months_abbr:  ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'],
	days:         ['Sunday','Monday','Tuesday','Wednesday','Thursday','Friday','Saturday'],
	days_abbr:    ['Sun','Mon','Tue','Wed','Thu','Fri','Sat'],

	dateOrder:    ['month','date','year'],
	shortDate:    '%m/%d/%Y',
	shortTime:    '%I:%M%p',
	AM:           'AM',
	PM:           'PM',
	firstDayOfWeek: 0,

	ordinal: function(dayOfMonth)
	{
		return (dayOfMonth > 3 && dayOfMonth < 21)
			? 'th'
			: ['th','st','nd','rd','th'][Math.min(dayOfMonth % 10, 4)];
	},

	lessThanMinuteAgo:  'less than a minute ago',
	minuteAgo:          'about a minute ago',
	minutesAgo:         '{delta} minutes ago',
	hourAgo:            'about an hour ago',
	hoursAgo:           'about {delta} hours ago',
	dayAgo:             '1 day ago',
	daysAgo:            '{delta} days ago',
	weekAgo:            '1 week ago',
	weeksAgo:           '{delta} weeks ago',
	monthAgo:           '1 month ago',
	monthsAgo:          '{delta} months ago',
	yearAgo:            '1 year ago',
	yearsAgo:           '{delta} years ago',

	lessThanMinuteUntil: 'less than a minute from now',
	minuteUntil:         'about a minute from now',
	minutesUntil:        '{delta} minutes from now',
	hourUntil:           'about an hour from now',
	hoursUntil:          'about {delta} hours from now',
	dayUntil:            '1 day from now',
	daysUntil:           '{delta} days from now',
	weekUntil:           '1 week from now',
	weeksUntil:          '{delta} weeks from now',
	monthUntil:          '1 month from now',
	monthsUntil:         '{delta} months from now',
	yearUntil:           '1 year from now',
	yearsUntil:          '{delta} years from now'
});

Locale.define('en-US', 'FormValidator',
{
	required:        'This field is required.',
	length:          'Please enter {length} characters (you entered {elLength} characters)',
	minLength:       'Please enter at least {minLength} characters (you entered {length} characters).',
	maxLength:       'Please enter no more than {maxLength} characters (you entered {length} characters).',
	integer:         'Please enter an integer in this field. Numbers with decimals (e.g. 1.25) are not permitted.',
	numeric:         'Please enter only numeric values in this field (i.e. "1" or "1.1" or "-1" or "-1.1").',
	digits:          'Please use numbers and punctuation only in this field (for example, a phone number with dashes or dots is permitted).',
	alpha:           'Please use only letters (a-z) within this field. No spaces or other characters are allowed.',
	alphanum:        'Please use only letters (a-z) or numbers (0-9) in this field. No spaces or other characters are allowed.',
	dateSuchAs:      'Please enter a valid date such as {date}',
	dateInFormatMDY: 'Please enter a valid date such as MM/DD/YYYY (i.e. "12/31/1999")',
	email:           'Please enter a valid email address. For example "fred@domain.com".',
	url:             'Please enter a valid URL such as http://www.example.com.',
	currencyDollar:  'Please enter a valid $ amount. For example $100.00 .',
	oneRequired:     'Please enter something for at least one of these inputs.',
	errorPrefix:     'Error: ',
	warningPrefix:   'Warning: ',
	noSpace:         'There can be no spaces in this input.',
	reqChkByNode:    'No items are selected.',
	requiredChk:     'This field is required.',
	reqChkByName:    'Please select a {label}.',
	match:           'This field needs to match the {matchName} field',
	startDate:       'the start date',
	endDate:         'the end date',
	currentDate:     'the current date',
	afterDate:       'The date should be the same or after {label}.',
	beforeDate:      'The date should be the same or before {label}.',
	startMonth:      'Please select a start month',
	sameMonth:       'These two dates must be in the same month - you must change one or the other.',
	creditcard:      'The credit card number entered is invalid. Please check the number and try again. {length} digits entered.'
});

Locale.use('en-US');

})();
