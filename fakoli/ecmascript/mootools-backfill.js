/**
 * Provides a thin backward compatibility layer for Fakoli Javascript code that was implemented using Mootools. 
 * This file should be included before any of the other Javascript files in the project, and should be removed 
 * once all of the code has been migrated to use native ECMAScript features.
 */

// ================================================================
// Utility helpers replacing MooTools sugar
// ================================================================

// Like MooTools document.id() — accepts id string or element
function $el(element)
{
	if (!element) return null;
	if (typeof element === 'string') return document.getElementById(element);
	return element;
}

// Keep compatibility with MooTools Element, allowing $el() to be used on the document itself
Document.prototype.id = $el;

Element.prototype.addClass = function(className)
{
	this.classList.add(className);
};

Element.prototype.removeClass = function(className)
{
	this.classList.remove(className);
};

Element.prototype.hasClass = function(className)
{
	return this.classList.contains(className);
};

Element.prototype.toggleClass = function(className)
{
	this.classList.toggle(className);
};

Element.prototype.getParent = function()
{
	return this.parentElement;
};

// Unique ID generator replacing String.uniqueID()
var _uniqueIDCounter = 0;
function uniqueID()
{
	return 'uid_' + (++_uniqueIDCounter) + '_' + Math.floor(Math.random() * 10000);
}

String.prototype.uniqueID = uniqueID;

// Compatibility helper for window.addEvent, supporting 'domready' as well as normal events
window.addEvent = function(event, handler)
{
	if (event == 'domready') 
	{
		if (document.readyState != "loading")
		{
			handler();
		}
		document.addEventListener('DOMContentLoaded', handler);
	}
	else
	{
		window.addEventListener(event, handler);
	}
};


// Backwards compatibility for MooTools DOMEvent wrapper class
class DOMEvent
{
	constructor(event)
	{
		this.event = event;
	}

	stop()
	{
		this.preventDefault().stopPropagation();
	}

	preventDefault() { this.event.preventDefault(); return this; }
	stopPropagation() { this.event.stopPropagation(); return this; }
	
}

// Simple event emitter base class, replacing MooTools Implements: Events
/**
 * Lightweight event emitter base class providing addEvent, removeEvent,
 * and fireEvent, used as a base for UI components throughout the framework.
 */
class EventEmitter
{
	constructor() { this._listeners = {}; }

	addEvent(event, fn)
	{
		(this._listeners[event] = this._listeners[event] || []).push(fn);
		return this;
	}

	// Method alias to allow EventEmitters to be used interchangeably with native DOM objects
	addEventListener(event, fn) { this.addEvent(event, fn); }

	removeEvent(event, fn)
	{
		if (this._listeners[event])
			this._listeners[event] = this._listeners[event].filter(function(f) { return f !== fn; });
		return this;
	}

	fireEvent(event)
	{
		var args = Array.prototype.slice.call(arguments, 1);
		(this._listeners[event] || []).forEach(function(fn) { fn.apply(null, args); });
		return this;
	}
}

// Set multiple styles at once, replacing element.setStyles()
function setStyles(el, styles)
{
	for (var prop in styles)
	{
		var val = styles[prop];
		var cssProp = prop.replace(/-([a-z])/g, function(m, c) { return c.toUpperCase(); });
		if (val === null || val === undefined)
		{
			el.style[cssProp] = '';
		}
		else if (typeof val === 'number' && prop !== 'opacity' && prop !== 'z-index')
		{
			el.style[cssProp] = val + 'px';
		}
		else
		{
			el.style[cssProp] = val;
		}
	}
}

Element.prototype.setStyles = function(styles)
{
    setStyles(this, styles);
    return this;
};

// Get computed style value, replacing element.getStyle()
function getStyle(el, prop)
{
	return window.getComputedStyle(el).getPropertyValue(prop);
}

Element.prototype.getStyle = function(prop)
{
    return getStyle(this, prop);
};

// Get element dimensions, replacing element.getSize() -> {x, y}
function getSize(el)
{
	return { x: el.offsetWidth, y: el.offsetHeight };
}

Element.prototype.getSize = function()
{
    return getSize(this);
};

// Get element coordinates relative to document, replacing element.getCoordinates()
function getCoordinates(el)
{
	var rect = el.getBoundingClientRect();
	return {
		top:    rect.top    + window.pageYOffset,
		left:   rect.left   + window.pageXOffset,
		bottom: rect.bottom + window.pageYOffset,
		right:  rect.right  + window.pageXOffset,
		width:  rect.width,
		height: rect.height
	};
}

Element.prototype.getCoordinates = function()
{
    return getCoordinates(this);
};

// Fade element in via CSS transition, replacing element.fade('in') / Fx.Tween
function fadeIn(el, duration, onComplete)
{
	duration = duration || 400;
	el.style.transition = 'opacity ' + duration + 'ms';
	el.style.opacity = 0;
	el.style.display = 'block';
	requestAnimationFrame(function()
	{
		requestAnimationFrame(function()
		{
			el.style.opacity = 1;
			if (onComplete) setTimeout(onComplete, duration);
		});
	});
}

// Fade element out via CSS transition, replacing element.fade('out') / Fx.Tween
function fadeOut(el, duration, onComplete)
{
	duration = duration || 400;
	el.style.transition = 'opacity ' + duration + 'ms';
	el.style.opacity = 0;
	setTimeout(function()
	{
		el.style.display = 'none';
		if (onComplete) onComplete();
	}, duration);
}

Element.prototype.fade = function(direction, duration, onComplete)
{
    if (direction === 'in') return fadeIn(this, duration, onComplete);
    if (direction === 'out') return fadeOut(this, duration, onComplete);
};

// Show with fade, replacing element.reveal()
function reveal(el)
{
	el.style.transition = 'opacity 300ms';
	el.style.display = 'block';
	requestAnimationFrame(function() { el.style.opacity = 1; });
}

Element.prototype.reveal = function()
{
    return reveal(this);
}

// Hide with fade, replacing element.dissolve()
function dissolve(el)
{
	el.style.transition = 'opacity 300ms';
	el.style.opacity = 0;
	setTimeout(function() { el.style.display = 'none'; }, 300);
}

Element.prototype.dissolve = function()
{
    return dissolve(this);
};

// Append query string, used by ProgressiveSearch
function appendQueryString(url, params)
{
	return url + (url.indexOf('?') >= 0 ? '&' : '?') + params;
}

// Walk up DOM calculating effective z-index, replacing window.getZIndex()
function calculateZIndex(element)
{
	while (element)
	{
		var z = parseInt(window.getComputedStyle(element).getPropertyValue('z-index'));
		if (!isNaN(z) && z !== 0) return z;
		element = element.parentElement;
	}
	return 0;
}

Window.prototype.getZIndex = function(element)
{
    return calculateZIndex(element);
};

// ================================================================
// Panel reload helpers
// Replaces: addReloadHandler, addReloadHandlers,
//           Element.implement('reloadPanel'), Element.implement('loadPanel'),
//           Element.implement('getPanel')
// Note: MooTools added reloadPanel/loadPanel/getPanel as element methods.
//       They are now standalone functions. Call sites using element.reloadPanel()
//       should be updated to reloadPanel(element).
// ================================================================

function addReloadHandler(elt)
{
	elt.reload = function(onComplete)
	{
		var url = elt.getAttribute('data-url');
		fetchHTML(url, function(html, scripts)
		{
			elt.innerHTML = html;
			execScripts(scripts);
			if (onComplete) onComplete();
		});
	};
	elt.load = function(url, onComplete)
	{
		elt.setAttribute('data-url', url);
		elt.reload(onComplete);
	};
}

function addReloadHandlers(container)
{
	container.querySelectorAll('[data-url]').forEach(addReloadHandler);
}

document.addEventListener('DOMContentLoaded', function()
{
	addReloadHandlers(document.body);
});

function reloadPanel(elt, onComplete)
{
	var element = $el(elt);
	while (element)
	{
		if (element.reload) { element.reload(onComplete); return; }
		element = element.parentElement;
	}
}

function loadPanel(elt, url, onComplete)
{
	var element = $el(elt);
	while (element)
	{
		if (element.matches('[data-url]') && element.load) { element.load(url, onComplete); return; }
		element = element.parentElement;
	}
	go(url); // fall back to navigation
}

function getPanel(elt)
{
	var element = $el(elt);
	while (element)
	{
		if (element.matches('[data-url]') && element.load) return element;
		element = element.parentElement;
	}
	return null;
}

// Element method extensions provided for backwards compatibility

Element.prototype.reloadPanel = function(onComplete)
{
	reloadPanel(this, onComplete);
};

Element.prototype.loadPanel = function(url, onComplete)
{
	loadPanel(this, url, onComplete);
};

Element.prototype.getPanel = function() 
{ 
	return getPanel(this); 
};