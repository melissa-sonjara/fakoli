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
 * Number formatting utilities mirroring MooTools More Number.Format.
 * Locale defaults match the en-US settings from Locale.en-US.Number.
 */

var NumberLocale = {
	decimal:          '.',
	group:            ',',
	currency:         '$',
	currencyPosition: 'left',
	groupSize:        3,
	decimals:         2
};

/**
 * Format a number with thousands separators and a fixed decimal count.
 * @param {Object} [options]
 * @param {number}  options.decimals   Decimal places (default: locale.decimals)
 * @param {string}  options.decimal    Decimal separator (default: locale.decimal)
 * @param {string}  options.group      Thousands separator (default: locale.group)
 * @param {number}  options.groupSize  Group size (default: locale.groupSize)
 */
Number.prototype.format = function(options)
{
	options = Object.assign({}, NumberLocale, options || {});
	var decimals  = options.decimals  != null ? options.decimals  : NumberLocale.decimals;
	var dec       = options.decimal   || NumberLocale.decimal;
	var grp       = options.group     || NumberLocale.group;
	var grpSize   = options.groupSize || NumberLocale.groupSize;

	var fixed = Math.abs(this).toFixed(decimals);
	var parts = fixed.split('.');
	var integer = parts[0];
	var fraction = parts[1] || '';

	if (grp)
	{
		var regex = new RegExp('(\\d+)(\\d{' + grpSize + '})');
		while (regex.test(integer))
		{
			integer = integer.replace(regex, '$1' + grp + '$2');
		}
	}

	var result = fraction ? (integer + dec + fraction) : integer;
	return (this < 0 ? '-' : '') + result;
};

/**
 * Format the number as a currency string using locale settings.
 * @param {Object} [options]  Same as format(), plus currency/currencyPosition overrides.
 */
Number.prototype.formatCurrency = function(options)
{
	options = Object.assign({}, NumberLocale, options || {});
	var symbol   = options.currency         || NumberLocale.currency;
	var position = options.currencyPosition || NumberLocale.currencyPosition;
	var formatted = this.format(options);

	return position === 'right' ? formatted + symbol : symbol + formatted;
};

/**
 * Format the number as a percentage string.
 * @param {Object} [options]  decimals, decimal, group.
 */
Number.prototype.formatPercentage = function(options)
{
	options = Object.assign({ decimals: 2 }, options || {});
	return this.format(options) + '%';
};
