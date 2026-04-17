/*
---
name: Picker.Attach
description: Adds attach and detach methods to the Picker, to attach it to element events
authors: Arian Stolwijk
requires: [Picker, Core/Element.Event]
provides: Picker.Attach
...
*/


class PickerAttach extends Picker
{
	constructor(attachTo, options)
	{
		var mergedOptions = Object.assign({
			/*
			onAttached: function(event){},

			toggleElements: null, // deprecated
			toggle: null, // When set it deactivate toggling by clicking on the input */
			togglesOnly: true, // set to false to always make calendar popup on input element, if true, it depends on the toggles elements set.
			showOnInit: false, // overrides the Picker option
			blockKeydown: true
		}, options);

		super(mergedOptions);

		this.attachedEvents = [];
		this.attachedElements = [];
		this.toggles = [];
		this.inputs = [];

		var self = this;
		var documentEvent = function(event)
		{
			if (self.attachedElements.indexOf(event.target) >= 0) return;
			self.close();
		};
		this._documentEvent = documentEvent;
		document.addEventListener('click', documentEvent);

		var preventPickerClick = function(event)
		{
			event.stopPropagation();
			return false;
		};
		this.picker.addEventListener('click', preventPickerClick);
		this._preventPickerClick = preventPickerClick;

		// Support for deprecated toggleElements
		if (this.options.toggleElements)
		{
			this.options.toggle = Array.from(document.querySelectorAll(this.options.toggleElements));
		}

		this.attach(attachTo, this.options.toggle);
	}

	attach(attachTo, toggle)
	{
		if (typeof attachTo === 'string') attachTo = document.getElementById(attachTo);
		if (typeof toggle === 'string') toggle = document.getElementById(toggle);

		var elements = attachTo ? (Array.isArray(attachTo) ? attachTo : [attachTo]) : [];
		var toggles = toggle ? (Array.isArray(toggle) ? toggle : [toggle]) : [];
		// allElements = elements combined with toggles (no duplicates)
		var allElements = elements.slice();
		toggles.forEach(function(t)
		{
			if (allElements.indexOf(t) < 0) allElements.push(t);
		});
		var self = this;

		var closeEvent = function(event)
		{
			var stopInput = self.options.blockKeydown
					&& event.type === 'keydown'
					&& (['Tab', 'Escape'].indexOf(event.key) < 0),
				isCloseKey = event.type === 'keydown'
					&& (['Tab', 'Escape'].indexOf(event.key) >= 0),
				isA = event.target.tagName.toLowerCase() === 'a';

			if (stopInput || isA) event.preventDefault();
			if (isCloseKey || isA) self.close();
		};

		var getOpenEvent = function(element)
		{
			return function(event)
			{
				var tag = event.target.tagName.toLowerCase();
				if (tag === 'input' && event.type === 'click' && document.activeElement !== element || (self.opened && self.input === element)) return;
				if (tag === 'a') event.preventDefault();
				self.position(element);
				self.open();
				self.fireEvent('attached', [event, element]);
			};
		};

		var getToggleEvent = function(open, close)
		{
			return function(event)
			{
				if (self.opened) close(event);
				else open(event);
			};
		};

		allElements.forEach(function(element)
		{
			// The events are already attached!
			if (self.attachedElements.indexOf(element) >= 0) return;

			var events = {};
			var tag = element.tagName.toLowerCase();
			var openEvent = getOpenEvent(element);
			// closeEvent does not have a dependency on element
			var toggleEvent = getToggleEvent(openEvent, closeEvent);

			if (tag === 'input')
			{
				// Fix in order to use togglers only
				if (!self.options.togglesOnly || !toggles.length)
				{
					events = {
						focus: openEvent,
						click: openEvent,
						keydown: closeEvent
					};
				}
				self.inputs.push(element);
			}
			else
			{
				if (toggles.indexOf(element) >= 0)
				{
					self.toggles.push(element);
					events.click = toggleEvent;
				}
				else
				{
					events.click = openEvent;
				}
			}

			Object.keys(events).forEach(function(eventName)
			{
				element.addEventListener(eventName, events[eventName]);
			});
			self.attachedElements.push(element);
			self.attachedEvents.push(events);
		});
		return this;
	}

	detach(attachTo, toggle)
	{
		if (typeof attachTo === 'string') attachTo = document.getElementById(attachTo);
		if (typeof toggle === 'string') toggle = document.getElementById(toggle);

		var elements = attachTo ? (Array.isArray(attachTo) ? attachTo : [attachTo]) : [];
		var toggles = toggle ? (Array.isArray(toggle) ? toggle : [toggle]) : [];
		// allElements = elements combined with toggles (no duplicates)
		var allElements = elements.slice();
		toggles.forEach(function(t)
		{
			if (allElements.indexOf(t) < 0) allElements.push(t);
		});
		var self = this;

		if (!allElements.length) allElements = self.attachedElements.slice();

		allElements.forEach(function(element)
		{
			var i = self.attachedElements.indexOf(element);
			if (i < 0) return;

			var events = self.attachedEvents[i];
			Object.keys(events).forEach(function(eventName)
			{
				element.removeEventListener(eventName, events[eventName]);
			});
			delete self.attachedEvents[i];
			delete self.attachedElements[i];

			var toggleIndex = self.toggles.indexOf(element);
			if (toggleIndex !== -1) delete self.toggles[toggleIndex];

			var inputIndex = self.inputs.indexOf(element);
			if (inputIndex !== -1) delete self.inputs[inputIndex];
		});
		return this;
	}

	destroy()
	{
		if (this._documentEvent)
		{
			document.removeEventListener('click', this._documentEvent);
		}
		this.detach();
		return super.destroy();
	}
}

// Expose as Picker.Attach
Picker.Attach = PickerAttach;
