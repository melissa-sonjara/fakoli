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
 * Scroller — auto-scrolls a container element when the mouse approaches
 * its edges.
 *
 * ES equivalent of MooTools More Scroller.
 */
class Scroller
{
	/**
	 * @param {Element|Window} element  The scrollable container.
	 * @param {Object} [options]
	 * @param {number|Object} options.area      Trigger zone in px (default 20).
	 *                                          Can be {top,right,bottom,left}.
	 * @param {number}  options.velocity         Scroll speed multiplier (default 1).
	 * @param {number}  options.fps              Polling frequency (default 50).
	 * @param {Function} options.onChange        Called with (scrollX, scrollY) each tick.
	 */
	constructor(element, options)
	{
		options = options || {};
		this.element  = element instanceof Window ? document.documentElement : element;
		this.isWindow = element instanceof Window || element === window;
		this.area     = options.area     != null ? options.area     : 20;
		this.velocity = options.velocity != null ? options.velocity : 1;
		this.fps      = options.fps      != null ? options.fps      : 50;
		this.onChange = options.onChange || null;

		this._timer    = null;
		this._mouse    = null;

		this._onEnter  = this._attach.bind(this);
		this._onLeave  = this._detach.bind(this);
		this._onMove   = this.getCoords.bind(this);
	}

	/** Begin monitoring mouse position within the element. */
	start()
	{
		var listener = this.isWindow ? document.body : this.element;
		listener.addEventListener('mouseover',  this._onEnter);
		listener.addEventListener('mouseleave', this._onLeave);
		return this;
	}

	/** Stop monitoring. */
	stop()
	{
		var listener = this.isWindow ? document.body : this.element;
		listener.removeEventListener('mouseover',  this._onEnter);
		listener.removeEventListener('mouseleave', this._onLeave);
		this._detach();
		return this;
	}

	/** @private Start tracking mousemove. */
	_attach()
	{
		var listener = this.isWindow ? document.body : this.element;
		listener.addEventListener('mousemove', this._onMove);
	}

	/** @private Stop tracking mousemove and cancel the scroll timer. */
	_detach()
	{
		var listener = this.isWindow ? document.body : this.element;
		listener.removeEventListener('mousemove', this._onMove);
		clearInterval(this._timer);
		this._timer = null;
	}

	/**
	 * Record the current mouse position from the event.
	 * @param {MouseEvent} event
	 */
	getCoords(event)
	{
		this._mouse = { x: event.clientX, y: event.clientY };
		if (!this._timer)
		{
			var interval = Math.round(1000 / this.fps);
			this._timer  = setInterval(this._scroll.bind(this), interval);
		}
	}

	/**
	 * Scroll the element toward the mouse if it is near an edge.
	 * Called at `fps` frequency while the mouse is inside the element.
	 */
	_scroll()
	{
		if (!this._mouse) return;

		var el     = this.element;
		var rect   = this.isWindow
			? { left: 0, top: 0, width: window.innerWidth, height: window.innerHeight }
			: el.getBoundingClientRect();

		var mouse  = this._mouse;
		var area   = this.area;
		var top    = (typeof area === 'object' ? area.top    : area) || 0;
		var bottom = (typeof area === 'object' ? area.bottom : area) || 0;
		var left   = (typeof area === 'object' ? area.left   : area) || 0;
		var right  = (typeof area === 'object' ? area.right  : area) || 0;

		var scrollX = this.isWindow ? window.scrollX : el.scrollLeft;
		var scrollY = this.isWindow ? window.scrollY : el.scrollTop;
		var maxScrollX = (this.isWindow ? document.documentElement.scrollWidth  - window.innerWidth  : el.scrollWidth  - el.clientWidth);
		var maxScrollY = (this.isWindow ? document.documentElement.scrollHeight - window.innerHeight : el.scrollHeight - el.clientHeight);

		var dx = 0, dy = 0;
		var relX = mouse.x - rect.left;
		var relY = mouse.y - rect.top;

		if (relY < top    && scrollY > 0)           dy = (relY - top)              * this.velocity;
		if (relY > rect.height - bottom && scrollY < maxScrollY)
		                                             dy = (relY - rect.height + bottom) * this.velocity;
		if (relX < left   && scrollX > 0)           dx = (relX - left)             * this.velocity;
		if (relX > rect.width  - right  && scrollX < maxScrollX)
		                                             dx = (relX - rect.width  + right) * this.velocity;

		dx = Math.round(dx);
		dy = Math.round(dy);

		if (dx || dy)
		{
			var newX = scrollX + dx;
			var newY = scrollY + dy;
			if (this.onChange)
			{
				this.onChange.call(this, newX, newY);
			}
			else
			{
				this.scroll(newX, newY);
			}
		}
	}

	/**
	 * Scroll the element to the given coordinates.
	 * @param {number} x
	 * @param {number} y
	 */
	scroll(x, y)
	{
		if (this.isWindow)
		{
			window.scrollTo(x, y);
		}
		else
		{
			this.element.scrollLeft = x;
			this.element.scrollTop  = y;
		}
	}
}
