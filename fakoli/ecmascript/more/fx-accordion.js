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
 * Accordion — expandable/collapsible sections driven by toggler elements.
 *
 * ES equivalent of MooTools More Fx.Accordion.
 *
 * Uses CSS transitions on height and opacity for animation. No MooTools Fx
 * engine required.
 */
class Accordion
{
	/**
	 * @param {HTMLElement[]|NodeList} togglers   Clickable headers.
	 * @param {HTMLElement[]|NodeList} elements   Content panels (parallel to togglers).
	 * @param {Object} [options]
	 * @param {number}   options.display          Index of initially open section (default 0).
	 * @param {boolean}  options.initialDisplayFx Animate the initial display (default false).
	 * @param {boolean}  options.alwaysHide       Allow all sections to be closed (default false).
	 * @param {boolean}  options.keepOpen         Allow multiple sections open simultaneously (default false).
	 * @param {boolean}  options.height           Animate height (default true).
	 * @param {boolean}  options.width            Animate width (default false).
	 * @param {boolean}  options.opacity          Animate opacity (default true).
	 * @param {number}   options.duration         Transition duration in ms (default 300).
	 * @param {string}   options.easing           CSS easing function (default 'ease-in-out').
	 * @param {string}   options.trigger          Event that opens sections (default 'click').
	 * @param {boolean}  options.resetHeight      Set height to 'auto' after opening (default false).
	 * @param {Function} options.onActive         Called with (toggler, section) when a section opens.
	 * @param {Function} options.onBackground     Called with (toggler, section) when a section closes.
	 */
	constructor(togglers, elements, options)
	{
		this.togglers = document.querySelectorAll(togglers);
		this.elements = document.querySelectorAll(elements);
		this.options  = Object.assign(
			{
				display:          0,
				initialDisplayFx: false,
				alwaysHide:       false,
				keepOpen:         false,
				height:           true,
				width:            false,
				opacity:          true,
				duration:         300,
				easing:           'ease-in-out',
				trigger:          'click',
				resetHeight:      false,
				onActive:         null,
				onBackground:     null
			},
			options || {}
		);

		this.current    = -1;
		this._listeners = [];  // {toggler, fn} pairs for cleanup

		this._init();
	}

	/** @private Set up all sections and display the initial one. */
	_init()
	{
		var self = this;
		this.togglers.forEach(function(toggler, i)
		{
			self._attach(toggler, i);
		});

		// Initially hide all sections
		this.elements.forEach(function(el, i)
		{
			el.style.overflow = 'hidden';
			if (self.options.height)  el.style.height  = '0';
			if (self.options.width)   el.style.width   = '0';
			if (self.options.opacity) el.style.opacity = '0';
		});

		var show = this.options.display;
		if (show != null && show >= 0 && show < this.elements.length)
		{
			this.display(show, this.options.initialDisplayFx);
		}
	}

	/** @private Attach click handler to a toggler. */
	_attach(toggler, index)
	{
		var self = this;
		var fn   = function() { self.display(index); };
		toggler.addEventListener(this.options.trigger, fn);
		toggler._accordionDisplay = fn;
		this._listeners.push({ toggler: toggler, fn: fn });
	}

	/**
	 * Add a new section.
	 * @param {HTMLElement} toggler
	 * @param {HTMLElement} element
	 */
	addSection(toggler, element)
	{
		var idx = this.togglers.indexOf(toggler);
		if (idx < 0)
		{
			this.togglers.push(toggler);
			this.elements.push(element);
			idx = this.togglers.length - 1;
			this._attach(toggler, idx);

			element.style.overflow = 'hidden';
			if (this.options.height)  element.style.height  = '0';
			if (this.options.width)   element.style.width   = '0';
			if (this.options.opacity) element.style.opacity = '0';
		}
		return this;
	}

	/**
	 * Remove a section.
	 * @param {HTMLElement} toggler
	 * @param {number}      [displayIndex]  Index to display after removal.
	 */
	removeSection(toggler, displayIndex)
	{
		var idx = this.togglers.indexOf(toggler);
		if (idx < 0) return this;

		this.detach(toggler);
		this.togglers.splice(idx, 1);
		this.elements.splice(idx, 1);

		if (this.current === idx)
		{
			var next = displayIndex != null ? displayIndex : (idx > 0 ? idx - 1 : 0);
			if (this.togglers.length > 0) this.display(next);
			else this.current = -1;
		}
		return this;
	}

	/**
	 * Detach event listeners from one or all togglers.
	 * @param {HTMLElement} [toggler]  If omitted, detach all.
	 */
	detach(toggler)
	{
		var self    = this;
		var trigger = this.options.trigger;

		if (!toggler)
		{
			this._listeners.forEach(function(entry)
			{
				entry.toggler.removeEventListener(trigger, entry.fn);
			});
			this._listeners = [];
		}
		else
		{
			var fn = toggler._accordionDisplay;
			if (fn)
			{
				toggler.removeEventListener(trigger, fn);
				delete toggler._accordionDisplay;
			}
			this._listeners = this._listeners.filter(function(e) { return e.toggler !== toggler; });
		}
		return this;
	}

	/**
	 * Open (display) the section at `index`, closing others (unless keepOpen).
	 * @param {number|HTMLElement} index
	 * @param {boolean}            [useFx=true]  Whether to animate.
	 */
	display(index, useFx)
	{
		if (typeof index !== 'number') index = this.elements.indexOf(index);
		if (index < 0 || index >= this.elements.length) return this;
		if (useFx == null) useFx = true;

		var opts      = this.options;
		var elements  = this.elements;
		var togglers  = this.togglers;
		var keepOpen  = opts.keepOpen;
		var alwaysHide = opts.alwaysHide;
		var self      = this;

		var isCurrent = (index === this.current);
		var isOpen    = elements[index].offsetHeight > 0 || elements[index].offsetWidth > 0;

		// Handle alwaysHide / keepOpen
		var willHide = !keepOpen && (isCurrent || (alwaysHide && isOpen));

		elements.forEach(function(el, i)
		{
			var hide;
			if (keepOpen)
			{
				// Only toggle the clicked one
				if (i !== index) return;
				var elIsOpen = el.offsetHeight > 0 || el.offsetWidth > 0;
				hide = alwaysHide ? elIsOpen : false;
			}
			else
			{
				hide = (i !== index) || willHide;
			}

			if (hide)
			{
				if (opts.onBackground) opts.onBackground.call(self, togglers[i], el);
			}
			else
			{
				if (opts.onActive) opts.onActive.call(self, togglers[i], el);
			}

			if (useFx) self._animate(el, hide);
			else        self._set(el, hide);
		});

		this.current = willHide ? -1 : index;
		return this;
	}

	/** @private Instantly set an element to open or closed. */
	_set(el, hide)
	{
		el.style.transition = '';
		if (this.options.height)  el.style.height  = hide ? '0' : el.scrollHeight + 'px';
		if (this.options.width)   el.style.width   = hide ? '0' : el.scrollWidth  + 'px';
		if (this.options.opacity) el.style.opacity = hide ? '0' : '1';
		if (!hide && this.options.resetHeight) el.style.height = 'auto';
	}

	/** @private Animate an element to open or closed. */
	_animate(el, hide)
	{
		var opts     = this.options;
		var props    = [];
		if (opts.height)  props.push('height');
		if (opts.width)   props.push('width');
		if (opts.opacity) props.push('opacity');

		var dur    = opts.duration;
		var easing = opts.easing;

		el.style.transition = props.map(function(p)
		{
			return p + ' ' + dur + 'ms ' + easing;
		}).join(', ');

		var self = this;

		if (!hide)
		{
			// Opening: must set explicit height first, then animate to it
			if (opts.height) el.style.height = el.scrollHeight + 'px';
			if (opts.width)  el.style.width  = el.scrollWidth  + 'px';

			if (opts.resetHeight)
			{
				var onEnd = function()
				{
					el.removeEventListener('transitionend', onEnd);
					el.style.height = 'auto';
				};
				el.addEventListener('transitionend', onEnd);
			}
		}

		requestAnimationFrame(function()
		{
			if (opts.height)  el.style.height  = hide ? '0' : el.scrollHeight + 'px';
			if (opts.width)   el.style.width   = hide ? '0' : el.scrollWidth  + 'px';
			if (opts.opacity) el.style.opacity = hide ? '0' : '1';
		});
	}
}
