/*
---
name: Picker
description: Creates a Picker, which can be used for anything
authors: Arian Stolwijk
requires: [Core/Element.Dimensions, Core/Fx.Tween, Core/Fx.Transitions]
provides: Picker
...
*/


class Picker
{
	constructor(options)
	{
		this.options = Object.assign({
			/*
			onShow: function(){},
			onOpen: function(){},
			onHide: function(){},
			onClose: function(){},*/

			pickerClass: 'datepicker',
			inject: null,
			animationDuration: 400,
			useFadeInOut: true,
			positionOffset: {x: 0, y: 0},
			pickerPosition: 'bottom',
			draggable: true,
			showOnInit: true,
			columns: 1,
			footer: false
		}, options);

		this._eventListeners = {};

		this.constructPicker();
		if (this.options.showOnInit) this.show();
	}

	// Simple event emitter methods (replaces MooTools Events mixin)
	addEvent(name, fn)
	{
		if (!this._eventListeners[name]) this._eventListeners[name] = [];
		this._eventListeners[name].push(fn);
		return this;
	}

	removeEvent(name, fn)
	{
		if (!this._eventListeners[name]) return this;
		this._eventListeners[name] = this._eventListeners[name].filter(function(f)
		{
			return f !== fn;
		});
		return this;
	}

	removeEvents(name)
	{
		if (name)
		{
			delete this._eventListeners[name];
		}
		else
		{
			this._eventListeners = {};
		}
		return this;
	}

	fireEvent(name, args)
	{
		var listeners = this._eventListeners[name];
		if (!listeners) return this;
		var argsArray = args != null ? (Array.isArray(args) ? args : [args]) : [];
		listeners.forEach(function(fn)
		{
			fn.apply(this, argsArray);
		}.bind(this));
		return this;
	}

	constructPicker()
	{
		var options = this.options;

		// Build the picker container
		var picker = this.picker = document.createElement('div');
		picker.className = options.pickerClass;
		picker.style.left = '0';
		picker.style.top = '0';
		picker.style.display = 'none';
		picker.style.opacity = '0';
		(options.inject || document.body).appendChild(picker);

		picker.classList.add('column_' + options.columns);

		// Set up fade animation state
		if (options.useFadeInOut)
		{
			picker._tweenDuration = options.animationDuration;
		}

		// Build the header
		var header = this.header = document.createElement('div');
		header.className = 'header';
		picker.appendChild(header);

		var title = this.title = document.createElement('div');
		title.className = 'title';
		header.appendChild(title);

		var titleID = this.titleID = 'pickertitle-' + Math.random().toString(36).substring(2, 10);
		var titleText = this.titleText = document.createElement('div');
		titleText.setAttribute('role', 'heading');
		titleText.className = 'titleText';
		titleText.id = titleID;
		titleText.setAttribute('aria-live', 'assertive');
		titleText.setAttribute('aria-atomic', 'true');
		title.appendChild(titleText);

		var closeButton = this.closeButton = document.createElement('div');
		closeButton.className = 'closeButton';
		closeButton.textContent = 'x';
		closeButton.setAttribute('role', 'button');
		closeButton.addEventListener('click', this.close.bind(this, false));
		header.appendChild(closeButton);

		// Build the body of the picker
		var body = this.body = document.createElement('div');
		body.className = 'body';
		picker.appendChild(body);

		if (options.footer)
		{
			this.footer = document.createElement('div');
			this.footer.className = 'footer';
			picker.appendChild(this.footer);
			picker.classList.add('footer');
		}

		// oldContents and newContents are used to slide from the old content to a new one.
		var slider = this.slider = document.createElement('div');
		slider.className = 'slider';
		slider.style.position = 'absolute';
		slider.style.top = '0';
		slider.style.left = '0';
		slider._tweenDuration = options.animationDuration;
		body.appendChild(slider);

		var newContents = this.newContents = document.createElement('div');
		newContents.style.position = 'absolute';
		newContents.style.top = '0';
		newContents.style.left = '0';
		slider.appendChild(newContents);

		var oldContents = this.oldContents = document.createElement('div');
		oldContents.style.position = 'absolute';
		oldContents.style.top = '0';
		slider.appendChild(oldContents);

		this.originalColumns = options.columns;
		this.setColumns(options.columns);

		// IFrameShim is not used in the ES6 version
		this.shim = null;

		// Dragging — only set cursor if draggable option is set
		if (options.draggable)
		{
			picker.style.cursor = 'move';
		}
	}

	open(noFx)
	{
		if (this.opened == true) return this;
		this.opened = true;
		var self = this;
		this.picker.style.display = 'block';
		this.picker.setAttribute('aria-hidden', 'false');
		this.fireEvent('open');
		if (this.options.useFadeInOut && !noFx)
		{
			this._fadeTo(this.picker, 1, this.options.animationDuration, function()
			{
				self.fireEvent('show');
			});
		}
		else
		{
			this.picker.style.opacity = '1';
			this.fireEvent('show');
		}
		return this;
	}

	show()
	{
		return this.open(true);
	}

	close(noFx)
	{
		if (this.opened == false) return this;
		this.opened = false;
		this.fireEvent('close');
		var self = this;
		var picker = this.picker;
		var hide = function()
		{
			picker.style.display = 'none';
			picker.setAttribute('aria-hidden', 'true');
			self.fireEvent('hide');
		};
		if (this.options.useFadeInOut && !noFx)
		{
			this._fadeTo(picker, 0, this.options.animationDuration, hide);
		}
		else
		{
			picker.style.opacity = '0';
			hide();
		}
		return this;
	}

	hide()
	{
		return this.close(true);
	}

	toggle()
	{
		return this[this.opened == true ? 'close' : 'open']();
	}

	destroy()
	{
		this.picker.remove();
	}

	// Simple CSS opacity tween helper
	_fadeTo(el, targetOpacity, duration, callback)
	{
		var start = null;
		var startOpacity = parseFloat(el.style.opacity) || 0;
		var delta = targetOpacity - startOpacity;

		// Cancel any in-progress animation on this element
		if (el._fadeRAF)
		{
			cancelAnimationFrame(el._fadeRAF);
			el._fadeRAF = null;
		}

		var step = function(timestamp)
		{
			if (!start) start = timestamp;
			var progress = Math.min((timestamp - start) / duration, 1);
			el.style.opacity = String(startOpacity + delta * progress);
			if (progress < 1)
			{
				el._fadeRAF = requestAnimationFrame(step);
			}
			else
			{
				el._fadeRAF = null;
				if (callback) callback();
			}
		};

		el._fadeRAF = requestAnimationFrame(step);
	}

	// Simple left-property tween helper for slider
	_tweenLeft(el, fromLeft, toLeft, duration, callback)
	{
		var start = null;
		var delta = toLeft - fromLeft;

		if (el._tweenRAF)
		{
			cancelAnimationFrame(el._tweenRAF);
			el._tweenRAF = null;
		}

		var step = function(timestamp)
		{
			if (!start) start = timestamp;
			var progress = Math.min((timestamp - start) / duration, 1);
			el.style.left = String(fromLeft + delta * progress) + 'px';
			if (progress < 1)
			{
				el._tweenRAF = requestAnimationFrame(step);
			}
			else
			{
				el._tweenRAF = null;
				if (callback) callback();
			}
		};

		el._tweenRAF = requestAnimationFrame(step);
	}

	position(x, y)
	{
		var offset = this.options.positionOffset;
		var scrollX = window.scrollX || window.pageXOffset;
		var scrollY = window.scrollY || window.pageYOffset;
		var sizeX = document.documentElement.clientWidth;
		var sizeY = document.documentElement.clientHeight;
		var pickersize = {
			x: this.picker.offsetWidth,
			y: this.picker.offsetHeight
		};
		var where;

		if (x instanceof Element)
		{
			var element = x;
			where = y || this.options.pickerPosition;

			var elementCoords = element.getBoundingClientRect();
			var absLeft = elementCoords.left + scrollX;
			var absRight = elementCoords.right + scrollX;
			var absTop = elementCoords.top + scrollY;
			var absBottom = elementCoords.bottom + scrollY;

			x = (where == 'left') ? absLeft - pickersize.x
				: (where == 'bottom' || where == 'top') ? absLeft
				: absRight;
			y = (where == 'bottom') ? absBottom
				: (where == 'top') ? absTop - pickersize.y
				: absTop;
		}

		x += offset.x * ((where && where == 'left') ? -1 : 1);
		y += offset.y * ((where && where == 'top') ? -1 : 1);

		if ((x + pickersize.x) > (sizeX + scrollX)) x = (sizeX + scrollX) - pickersize.x;
		if ((y + pickersize.y) > (sizeY + scrollY)) y = (sizeY + scrollY) - pickersize.y;
		if (x < 0) x = 0;
		if (y < 0) y = 0;

		this.picker.style.left = x + 'px';
		this.picker.style.top = y + 'px';
		return this;
	}

	setBodySize()
	{
		var bodysize = this.bodysize = {
			x: this.body.offsetWidth,
			y: this.body.offsetHeight
		};

		this.slider.style.width = (2 * bodysize.x) + 'px';
		this.slider.style.height = bodysize.y + 'px';

		this.oldContents.style.left = bodysize.x + 'px';
		this.oldContents.style.width = bodysize.x + 'px';
		this.oldContents.style.height = bodysize.y + 'px';

		this.newContents.style.width = bodysize.x + 'px';
		this.newContents.style.height = bodysize.y + 'px';
	}

	setColumnContent(column, content)
	{
		var columnElement = this.columns[column];
		if (!columnElement) return this;

		var type = typeof content;
		if (type === 'string' || type === 'number')
		{
			columnElement.textContent = content;
		}
		else
		{
			columnElement.innerHTML = '';
			if (content instanceof Element)
			{
				columnElement.appendChild(content);
			}
			else if (content instanceof NodeList || Array.isArray(content))
			{
				Array.from(content).forEach(function(node)
				{
					columnElement.appendChild(node);
				});
			}
		}

		return this;
	}

	setColumnsContent(content, fx)
	{
		var old = this.columns;
		this.columns = this.newColumns;
		this.newColumns = old;

		content.forEach(function(_content, i)
		{
			this.setColumnContent(i, _content);
		}.bind(this));
		return this.setContent(null, fx);
	}

	setColumns(columns)
	{
		var _columns = this.columns = [];
		var _newColumns = this.newColumns = [];
		for (var i = columns; i--;)
		{
			var col = document.createElement('div');
			col.className = 'column column_' + (columns - i);
			_columns.push(col);

			var newCol = document.createElement('div');
			newCol.className = 'column column_' + (columns - i);
			_newColumns.push(newCol);
		}

		var oldClass = 'column_' + this.options.columns;
		var newClass = 'column_' + columns;
		this.picker.classList.remove(oldClass);
		this.picker.classList.add(newClass);

		this.options.columns = columns;
		return this;
	}

	setContent(content, fx)
	{
		if (content) return this.setColumnsContent([content], fx);

		// swap contents so we can fill the newContents again and animate
		var old = this.oldContents;
		this.oldContents = this.newContents;
		this.newContents = old;
		this.newContents.innerHTML = '';

		this.columns.forEach(function(col)
		{
			this.newContents.appendChild(col);
		}.bind(this));

		this.setBodySize();

		if (fx)
		{
			this.fx(fx);
		}
		else
		{
			this.slider.style.left = '0';
			this.oldContents.style.left = '0';
			this.oldContents.style.opacity = '0';
			this.newContents.style.left = '0';
			this.newContents.style.opacity = '1';
		}
		return this;
	}

	fx(fx)
	{
		var oldContents = this.oldContents;
		var newContents = this.newContents;
		var slider = this.slider;
		var bodysize = this.bodysize;
		var duration = this.options.animationDuration;

		if (fx == 'right')
		{
			oldContents.style.left = '0';
			oldContents.style.opacity = '1';
			newContents.style.left = bodysize.x + 'px';
			newContents.style.opacity = '1';
			this._tweenLeft(slider, 0, -bodysize.x, duration);
		}
		else if (fx == 'left')
		{
			oldContents.style.left = bodysize.x + 'px';
			oldContents.style.opacity = '1';
			newContents.style.left = '0';
			newContents.style.opacity = '1';
			this._tweenLeft(slider, -bodysize.x, 0, duration);
		}
		else if (fx == 'fade')
		{
			slider.style.left = '0';
			oldContents.style.left = '0';
			this._fadeTo(oldContents, 0, duration / 2, function()
			{
				oldContents.style.left = bodysize.x + 'px';
			});
			newContents.style.left = '0';
			newContents.style.opacity = '0';
			this._fadeTo(newContents, 1, duration);
		}
	}

	toElement()
	{
		return this.picker;
	}

	setTitle(content, fn)
	{
		if (!fn) fn = function(x) { return x; };
		this.titleText.innerHTML = '';
		var items = Array.isArray(content) ? content : [content];
		items.forEach(function(item, i)
		{
			if (item instanceof Element)
			{
				this.titleText.appendChild(item);
			}
			else
			{
				var div = document.createElement('div');
				div.className = 'column column_' + (i + 1);
				div.textContent = fn(item, this.options);
				this.titleText.appendChild(div);
			}
		}.bind(this));
		return this;
	}

	setTitleEvent(fn)
	{
		this.titleText.removeEventListener('click', this._titleEventFn);
		this._titleEventFn = fn || null;
		if (fn) this.titleText.addEventListener('click', fn);
		this.titleText.style.cursor = fn ? 'pointer' : '';
		return this;
	}
}
