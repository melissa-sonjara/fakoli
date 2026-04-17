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
 * Extensions to Array.prototype mirroring MooTools More Array.Extras.
 */

/** Returns the minimum numeric value in the array. */
Array.prototype.min = function()
{
	return Math.min.apply(Math, this);
};

/** Returns the maximum numeric value in the array. */
Array.prototype.max = function()
{
	return Math.max.apply(Math, this);
};

/** Returns the arithmetic mean of the array's numeric values. */
Array.prototype.average = function()
{
	return this.length ? this.reduce(function(sum, v) { return sum + v; }, 0) / this.length : 0;
};

/** Returns the sum of the array's numeric values. */
Array.prototype.sum = function()
{
	return this.reduce(function(sum, v) { return sum + v; }, 0);
};

/** Returns a new array with duplicate values removed (first occurrence kept). */
Array.prototype.unique = function()
{
	return this.filter(function(value, index, self)
	{
		return self.indexOf(value) === index;
	});
};

/**
 * Returns a new array with elements in randomised order (Fisher-Yates shuffle).
 */
Array.prototype.shuffle = function()
{
	var arr = this.slice();
	for (var i = arr.length - 1; i > 0; i--)
	{
		var j = Math.floor(Math.random() * (i + 1));
		var tmp = arr[i];
		arr[i] = arr[j];
		arr[j] = tmp;
	}
	return arr;
};

/**
 * Returns an array of values extracted from each element by the given key.
 * Equivalent to Array.prototype.map(x => x[key]).
 * @param {string} key
 */
Array.prototype.pluck = function(key)
{
	return this.map(function(item) { return item[key]; });
};

/**
 * Returns the first non-null, non-undefined value in the array.
 */
Array.prototype.pick = function()
{
	for (var i = 0; i < this.length; i++)
	{
		if (this[i] != null) return this[i];
	}
	return null;
};

/**
 * Polyfill for Array.prototype.flatten (native in modern browsers).
 * Recursively flattens nested arrays.
 */
if (!Array.prototype.flatten)
{
	Array.prototype.flatten = function(depth)
	{
		depth = depth === undefined ? Infinity : depth;
		return depth > 0
			? this.reduce(function(acc, val)
			{
				return acc.concat(Array.isArray(val) ? val.flatten(depth - 1) : val);
			}, [])
			: this.slice();
	};
}
