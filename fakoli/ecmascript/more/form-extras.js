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
 * Element.Forms — text-selection and caret helpers for input/textarea elements.
 * ES equivalent of MooTools More Element.Forms.
 *
 * Depends on: string-extras.js (String.prototype.tidy)
 */

/**
 * Apply String.tidy() to the element's value, normalising whitespace and
 * smart-quote characters in place.
 */
Element.prototype.tidy = function()
{
	this.value = this.value.tidy();
};

/**
 * Returns the text between character positions `start` and `end` in the
 * element's value.
 * @param {number} start
 * @param {number} end
 * @returns {string}
 */
Element.prototype.getTextInRange = function(start, end)
{
	return this.value.substring(start, end);
};

/**
 * Returns the currently selected text, or an empty string if nothing is
 * selected.
 * @returns {string}
 */
Element.prototype.getSelectedText = function()
{
	return this.getTextInRange(this.getSelectionStart(), this.getSelectionEnd());
};

/**
 * Returns the current selection as `{ start, end }` character indices.
 * @returns {{ start: number, end: number }}
 */
Element.prototype.getSelectedRange = function()
{
	return {
		start: this.selectionStart || 0,
		end:   this.selectionEnd   || 0
	};
};

/** Returns the start index of the current selection. */
Element.prototype.getSelectionStart = function()
{
	return this.getSelectedRange().start;
};

/** Returns the end index of the current selection. */
Element.prototype.getSelectionEnd = function()
{
	return this.getSelectedRange().end;
};

/**
 * Move the caret to `pos` without changing the value.
 * Pass `'end'` to move to the end of the value.
 * @param {number|'end'} pos
 * @returns {Element} this
 */
Element.prototype.setCaretPosition = function(pos)
{
	if (pos === 'end') pos = this.value.length;
	this.selectRange(pos, pos);
	return this;
};

/**
 * Returns the current caret (insertion point) position.
 * @returns {number}
 */
Element.prototype.getCaretPosition = function()
{
	return this.getSelectedRange().start;
};

/**
 * Focus the element and select the text between `start` and `end`.
 * @param {number} start
 * @param {number} end
 * @returns {Element} this
 */
Element.prototype.selectRange = function(start, end)
{
	this.focus();
	this.setSelectionRange(start, end);
	return this;
};

/**
 * Insert `value` at the current caret position, replacing any selected text.
 * Unless `select` is explicitly `false`, the inserted text is selected
 * afterwards.
 * @param {string}  value
 * @param {boolean} [select]  Pass `false` to leave the caret at the end.
 * @returns {Element} this
 */
Element.prototype.insertAtCursor = function(value, select)
{
	var pos  = this.getSelectedRange();
	var text = this.value;
	this.value = text.substring(0, pos.start) + value + text.substring(pos.end);
	if (select !== false) this.selectRange(pos.start, pos.start + value.length);
	else                  this.setCaretPosition(pos.start + value.length);
	return this;
};

/**
 * Wrap the current selection (or `options.defaultMiddle` if nothing is
 * selected) with `options.before` and `options.after`.
 * Unless `select` is explicitly `false`, the middle portion is selected
 * afterwards.
 * @param {{ before: string, defaultMiddle: string, after: string }} options
 * @param {boolean} [select]
 * @returns {Element} this
 */
Element.prototype.insertAroundCursor = function(options, select)
{
	options = Object.assign({ before: '', defaultMiddle: '', after: '' }, options);

	var value = this.getSelectedText() || options.defaultMiddle;
	var pos   = this.getSelectedRange();
	var text  = this.value;

	if (pos.start === pos.end)
	{
		this.value = text.substring(0, pos.start)
		           + options.before + value + options.after
		           + text.substring(pos.end);
		this.selectRange(
			pos.start + options.before.length,
			pos.start + options.before.length + value.length
		);
	}
	else
	{
		var current  = text.substring(pos.start, pos.end);
		this.value = text.substring(0, pos.start)
		           + options.before + current + options.after
		           + text.substring(pos.end);
		var selStart = pos.start + options.before.length;
		if (select !== false) this.selectRange(selStart, selStart + current.length);
		else                  this.setCaretPosition(selStart + current.length);
	}

	return this;
};
