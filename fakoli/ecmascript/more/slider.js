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
 * Slider — horizontal or vertical range slider control.
 *
 * ES equivalent of MooTools More Slider.
 *
 * Uses Pointer Events for cross-device dragging. The caller is responsible for
 * positioning the knob element inside the track element with CSS
 * (position:relative on track, position:absolute on knob is a common pattern).
 */
class Slider
{
	/**
	 * @param {HTMLElement} element   Track (rail) element.
	 * @param {HTMLElement} knob      Draggable thumb element.
	 * @param {Object}      [options]
	 * @param {string}   options.mode         'horizontal' | 'vertical' (default 'horizontal').
	 * @param {number}   options.steps        Number of discrete steps (default 100).
	 * @param {number[]} options.range        [min, max] values (default [0, steps]).
	 * @param {number}   options.initialStep  Starting value (default min).
	 * @param {number}   options.offset       Pixel offset for knob positioning (default 0).
	 * @param {boolean}  options.snap         Snap to integer step positions (default false).
	 * @param {boolean}  options.wheel        Enable mouse-wheel control (default false).
	 * @param {Function} options.onChange     Called with step value when value changes.
	 * @param {Function} options.onComplete   Called with step value (as string) on drag end.
	 * @param {Function} options.onMove       Called whenever knob moves.
	 * @param {Function} options.onTick       Called with pixel position; default sets knob pos.
	 */
	constructor(element, knob, options)
	{
		this.element = element;
		this.knob    = knob;
		this.options = Object.assign(
			{
				mode:        'horizontal',
				steps:       100,
				range:       null,
				initialStep: null,
				offset:      0,
				snap:        false,
				wheel:       false,
				onChange:    null,
				onComplete:  null,
				onMove:      null,
				onTick:      null
			},
			options || {}
		);

		this._isDragging     = false;
		this._previousChange = null;
		this._previousEnd    = null;
		this._pointerActive  = false;

		this._onPointerDown  = this._pointerDown.bind(this);
		this._onPointerMove  = this._pointerMove.bind(this);
		this._onPointerUp    = this._pointerUp.bind(this);
		this._onTrackClick   = this._trackClick.bind(this);
		this._onWheel        = this._wheel.bind(this);

		// Configure axis
		if (this.options.mode === 'vertical')
		{
			this._axis     = 'y';
			this._property = 'top';
			this._sizeKey  = 'offsetHeight';
		}
		else
		{
			this._axis     = 'x';
			this._property = 'left';
			this._sizeKey  = 'offsetWidth';
		}

		this._computeDimensions();
		this._setRange(this.options.range);

		if (window.getComputedStyle(knob).position === 'static') knob.style.position = 'relative';
		knob.style[this._property] = -this.options.offset + 'px';

		var initialStep = this.options.initialStep != null ? this.options.initialStep : this.min;
		this.set(initialStep, true);

		this.attach();
	}

	/** @private Compute track and knob dimensions. */
	_computeDimensions()
	{
		this._half  = this.knob[this._sizeKey] / 2;
		this._full  = this.element[this._sizeKey] - this.knob[this._sizeKey] + this.options.offset * 2;
	}

	/** @private Set min/max/range/stepWidth. */
	_setRange(range)
	{
		this.min   = range ? range[0] : 0;
		this.max   = range ? range[1] : this.options.steps;
		this.range = this.max - this.min;
		this.steps = this.options.steps || this._full;
		this._stepSize  = Math.abs(this.range) / this.steps;
		this._stepWidth = this._stepSize * this._full / Math.abs(this.range);
	}

	/** Attach event listeners. Returns this. */
	attach()
	{
		this.knob.addEventListener('pointerdown',  this._onPointerDown);
		this.element.addEventListener('click',     this._onTrackClick);
		if (this.options.wheel)
		{
			this.element.addEventListener('wheel', this._onWheel, { passive: false });
		}
		return this;
	}

	/** Detach event listeners. Returns this. */
	detach()
	{
		this.knob.removeEventListener('pointerdown',    this._onPointerDown);
		this.element.removeEventListener('click',       this._onTrackClick);
		this.element.removeEventListener('wheel',       this._onWheel);
		document.removeEventListener('pointermove',     this._onPointerMove);
		document.removeEventListener('pointerup',       this._onPointerUp);
		document.removeEventListener('pointercancel',   this._onPointerUp);
		return this;
	}

	/** Recalculate dimensions after a layout change. Returns this. */
	autosize()
	{
		this._computeDimensions();
		this._setRange(this.options.range);
		this.setKnobPosition(this.toPosition(this.step));
		return this;
	}

	/** @private Clamp and round step to valid range. */
	_clamp(step)
	{
		return Math.min(this.max, Math.max(this.min, step));
	}

	/** @private Round step to the modulus of stepSize. */
	_roundStep(step)
	{
		if (!this._stepSize) return step;
		var decimals = ((this._stepSize + '').split('.')[1] || '').length;
		return parseFloat(step.toFixed(decimals));
	}

	/**
	 * Set the slider to a specific value.
	 * @param {number}  step
	 * @param {boolean} [silently]  Suppress events.
	 */
	set(step, silently)
	{
		step = this._roundStep(this._clamp(step));
		this.step = step;
		var pos = this.toPosition(step);
		if (silently)
		{
			this._checkStep();
			this.setKnobPosition(pos);
		}
		else
		{
			this._checkStep();
			this._tick(pos);
			this._move();
			this._end();
		}
		return this;
	}

	/** Position the knob element at `position` pixels. */
	setKnobPosition(position)
	{
		if (this.options.snap) position = this.toPosition(this.step);
		this.knob.style[this._property] = position + 'px';
		return this;
	}

	/** Convert a pixel position to a step value. */
	toStep(position)
	{
		return (position + this.options.offset) * this._stepSize / this._full * this.steps;
	}

	/** Convert a step value to a pixel position. */
	toPosition(step)
	{
		return (this._full * Math.abs(this.min - step)) / (this.steps * this._stepSize) - this.options.offset || 0;
	}

	/** @private Fire tick (knob position) callback. */
	_tick(position)
	{
		if (this.options.onTick) this.options.onTick.call(this, position);
		else this.setKnobPosition(position);
	}

	/** @private Fire move callback. */
	_move()
	{
		if (this.options.onMove) this.options.onMove.call(this);
	}

	/** @private Fire change callback if step changed. */
	_checkStep()
	{
		var step = this.step;
		if (this._previousChange !== step)
		{
			this._previousChange = step;
			if (this.options.onChange) this.options.onChange.call(this, step);
		}
		return this;
	}

	/** @private Fire complete callback if step changed from last end. */
	_end()
	{
		var step = this.step;
		if (this._previousEnd !== step)
		{
			this._previousEnd = step;
			if (this.options.onComplete) this.options.onComplete.call(this, step + '');
		}
		return this;
	}

	/** @private Pixel position of the element's relevant edge. */
	_elementOffset()
	{
		var rect = this.element.getBoundingClientRect();
		return this._axis === 'x'
			? rect.left + (window.scrollX || window.pageXOffset)
			: rect.top  + (window.scrollY || window.pageYOffset);
	}

	// ── Pointer handlers ──

	_pointerDown(event)
	{
		if (event.button && event.button !== 0) return;
		event.preventDefault();
		this._isDragging = true;
		this.knob.setPointerCapture(event.pointerId);
		document.addEventListener('pointermove',   this._onPointerMove);
		document.addEventListener('pointerup',     this._onPointerUp);
		document.addEventListener('pointercancel', this._onPointerUp);
		this._updateFromEvent(event);
	}

	_pointerMove(event)
	{
		if (!this._isDragging) return;
		this._updateFromEvent(event);
	}

	_pointerUp(event)
	{
		if (!this._isDragging) return;
		this._isDragging = false;
		document.removeEventListener('pointermove',   this._onPointerMove);
		document.removeEventListener('pointerup',     this._onPointerUp);
		document.removeEventListener('pointercancel', this._onPointerUp);
		this._updateFromEvent(event);
		this._end();
	}

	_trackClick(event)
	{
		if (this._isDragging || event.target === this.knob) return;
		var clientPos = this._axis === 'x' ? event.clientX : event.clientY;
		var rect      = this.element.getBoundingClientRect();
		var origin    = this._axis === 'x' ? rect.left : rect.top;
		var position  = clientPos - origin - this._half;
		position = Math.max(-this.options.offset, Math.min(this._full - this.options.offset, position));
		var dir  = this.range < 0 ? -1 : 1;
		this.step = this._roundStep(this._clamp(this.min + dir * this.toStep(position)));
		this._checkStep();
		this._tick(position);
		this._move();
		this._end();
	}

	_updateFromEvent(event)
	{
		var clientPos = this._axis === 'x' ? event.clientX : event.clientY;
		var rect      = this.element.getBoundingClientRect();
		var origin    = this._axis === 'x' ? rect.left : rect.top;
		var position  = clientPos - origin - this._half;
		position = Math.max(-this.options.offset, Math.min(this._full - this.options.offset, position));
		var dir  = this.range < 0 ? -1 : 1;
		this.step = this._roundStep(this._clamp(this.min + dir * this.toStep(position)));
		this._checkStep();
		this._tick(position);
		this._move();
	}

	_wheel(event)
	{
		event.preventDefault();
		var delta  = event.deltaY || -event.wheelDelta;
		var up     = delta > 0;
		var adjust = (this.options.mode === 'horizontal') ? (up ? -1 : 1) : (up ? 1 : -1);
		this.set(this.step + adjust * this._stepSize);
	}
}
