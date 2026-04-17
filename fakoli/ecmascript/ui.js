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

// Requires mootools-backfill.js for $el, addClass/removeClass, uniqueID, window.addEvent, and Document.id().

// Fetch HTML fragment, replacing Request.HTML
// onSuccess(html, scripts) where scripts is extracted inline script text
function fetchHTML(url, onSuccess)
{
	fetch(url)
		.then(function(r) { return r.text(); })
		.then(function(raw)
		{
			var tmp = document.createElement('div');
			tmp.innerHTML = raw;
			var scripts = Array.from(tmp.querySelectorAll('script')).map(function(s)
			{
				var t = s.textContent;
				s.remove();
				return t;
			}).join('\n');
			onSuccess(tmp.innerHTML, scripts);
		});
}

// Fetch plain text, replacing Request (with optional timeout)
function fetchText(url, onSuccess, onFailure, onTimeout, timeoutMs)
{
	var controller = new AbortController();
	var timer = timeoutMs ? setTimeout(function()
	{
		controller.abort();
		if (onTimeout) onTimeout();
	}, timeoutMs) : null;

	fetch(url, { signal: controller.signal })
		.then(function(r) { if (timer) clearTimeout(timer); return r.text(); })
		.then(function(t) { if (onSuccess) onSuccess(t); })
		.catch(function(err)
		{
			if (timer) clearTimeout(timer);
			if (err.name !== 'AbortError' && onFailure) onFailure();
		});
}

// Execute extracted inline scripts, replacing Browser.exec()
function execScripts(text)
{
	if (!text) return text;
	if (window.execScript)
	{
		window.execScript(text);
	} 
	else 
	{
		var script = document.createElement('script');
		script.setAttribute('type', 'text/javascript');
		script.text = text;
		document.head.appendChild(script);
		document.head.removeChild(script);
	}
	return text;
}

// Drag-and-drop, replacing Drag.Move
function makeDraggable(element, options)
{
	options = options || {};
	var handle = options.handle || element;

	handle.addEventListener('mousedown', function(e)
	{
		if (options.preventDefault) e.preventDefault();
		var startX = e.clientX, startY = e.clientY;
		var origLeft = parseInt(element.style.left) || 0;
		var origTop  = parseInt(element.style.top)  || 0;

		function onMove(e)
		{
			if (options.onDrag) options.onDrag(element, e);
			element.style.left = (origLeft + e.clientX - startX) + 'px';
			element.style.top  = (origTop  + e.clientY - startY) + 'px';
		}
		function onUp()
		{
			document.removeEventListener('mousemove', onMove);
			document.removeEventListener('mouseup', onUp);
		}
		document.addEventListener('mousemove', onMove);
		document.addEventListener('mouseup', onUp);
	});
}


// ================================================================
// Annotate document body with browser info
// ================================================================

document.addEventListener('DOMContentLoaded', function()
{
	var ua = navigator.userAgent;
	var name = 'unknown', version = '';
	if      (/Edg\//.test(ua))     { name = 'edge';    version = (ua.match(/Edg\/([\d.]+)/)    || [])[1] || ''; }
	else if (/Chrome\//.test(ua))  { name = 'chrome';  version = (ua.match(/Chrome\/([\d.]+)/) || [])[1] || ''; }
	else if (/Firefox\//.test(ua)) { name = 'firefox'; version = (ua.match(/Firefox\/([\d.]+)/)|| [])[1] || ''; }
	else if (/Safari\//.test(ua))  { name = 'safari';  version = (ua.match(/Version\/([\d.]+)/)|| [])[1] || ''; }
	document.body.classList.add(name);
	document.body.classList.add((name + version).replace('.', '_'));
});


// ================================================================
// window utilities
// ================================================================

window.size = function()
{
	return { width: window.innerWidth, height: window.innerHeight };
};

window.center = function()
{
	var hWnd = arguments[0] || { width: 0, height: 0 };
	return {
		x: ((this.size().width  - hWnd.width)  / 2) + window.pageXOffset,
		y: ((this.size().height - hWnd.height) / 2) + window.pageYOffset
	};
};

window.scrollToElement = function(element, offset)
{
	if (typeof offset === 'undefined') offset = 0;
	element = $el(element);
	var rect = element.getBoundingClientRect();
	window.scrollTo({ left: window.pageXOffset, top: rect.top + window.pageYOffset + offset, behavior: 'smooth' });
};

window.alignHeights = function(selector)
{
	if (!selector) selector = '.align_height';
	document.querySelectorAll(selector).forEach(function(container)
	{
		var max = 0;
		Array.from(container.children).forEach(function(child) { if (child.offsetHeight > max) max = child.offsetHeight; });
		Array.from(container.children).forEach(function(child) { child.style.height = max + 'px'; });
	});
};

window.scaleToFit = function(selector)
{
	if (!selector) selector = '.scale_to_fit';
	document.querySelectorAll(selector).forEach(function(element)
	{
		var parent = element.parentElement;
		var parentWidth = parent.offsetWidth;
		var width = element.offsetWidth;
		if (width === 0) return;
		var scale = parentWidth / width;
		if (element.tagName === 'IMG')
		{
			element.style.width  = parentWidth + 'px';
			element.style.height = (element.offsetHeight * scale) + 'px';
		}
		else
		{
			element.style.transformOrigin = 'top left';
			element.style.transform = 'scale(' + scale + ')';
		}
	});
};

document.addEventListener('DOMContentLoaded', function() { window.scaleToFit(); });
window.addEventListener('resize', function() { window.scaleToFit(); });

window.getZIndex = function(element)
{
	if (!(element instanceof Element)) return 0;
	var z = parseInt(window.getComputedStyle(element).getPropertyValue('z-index'));
	if (isNaN(z)) return window.getZIndex(element.parentNode);
	return z;
};

window.raiseCurtain = function(onComplete) { window.curtain.raise(onComplete); };
window.lowerCurtain = function(onComplete) { window.curtain.lower(onComplete); };


// ================================================================
// Curtain
// Replaces: MooTools Class, IframeShim (dropped — IE6/7 only need),
//           element.addClass/removeClass, element.setStyles
// ================================================================

/**
 * Manages a full-page curtain overlay used to block interaction while a
 * modal dialog is displayed, lowering and raising it with a CSS transition.
 */
class Curtain
{
	constructor()
	{
		this.curtain = document.getElementById('curtain');
		if (!this.curtain)
		{
			this.curtain = document.createElement('div');
			this.curtain.id = 'curtain';
			(document.body || document.documentElement).appendChild(this.curtain);
		}
	}

	lower(onComplete)
	{
		var opacity  = parseFloat(getStyle(this.curtain, 'opacity'));
		var display  = getStyle(this.curtain, 'display');
		if (opacity > 0 && display === 'block') { if (onComplete) onComplete(); return; }

		var ws = window.size();
		var w, h;
		if (document.body.classList.contains('full-height')) { w = '100%'; h = '100%'; }
		else { w = ws.width + 'px'; h = ws.height + 'px'; }

		setStyles(this.curtain, { top: '0', left: '0', width: w, height: h, position: 'fixed' });
		document.body.classList.add('curtain_lowered');
		void document.body.offsetWidth;
		if (onComplete) setTimeout(onComplete, 500);
	}

	raise(onComplete)
	{
		document.body.classList.remove('curtain_lowered');
		void document.body.offsetWidth;
		var curtain = this.curtain;
		setTimeout(function()
		{
			curtain.style.width = '';
			curtain.style.height = '';
			if (onComplete) onComplete();
		}, 500);
	}

	loadingCursor() { this.curtain.style.cursor = 'progress'; }
	normalCursor()  { this.curtain.style.cursor = 'auto'; }
}

document.addEventListener('DOMContentLoaded', function()
{
	window.curtain = new Curtain();
});


// ================================================================
// Dialog helpers
// ================================================================

function modalPopup(title, url, width, height, returnPopup, draggable, clazz)
{
	var popup = new ModalDialog('modalPopup_' + uniqueID(), { title: title, width: width, height: height, draggable: draggable, 'class': clazz, maximized: (width === '100%' && height === '100%') });
	popup.show(null, url);
	if (returnPopup) return popup;
}

function hideModalPopup(popup)
{
	if (popup) popup.hide();
}

function messagePopup(title, message, width, height, returnPopup, draggable, clazz)
{
	var popup = new ModalDialog('modalPopup_' + uniqueID(), { title: title, width: width, height: height, draggable: draggable, 'class': clazz, maximized: (width === '100%' && height === '100%') });
	popup.options.body.innerHTML = message;
	popup.show();
	if (returnPopup) return popup;
}

function floatingPopup(id, title, url, width, height, returnPopup, draggable, clazz)
{
	var popup = new FloatingDialog(id, { title: title, width: width, height: height, draggable: draggable, 'class': clazz, maximized: (width === '100%' && height === '100%') });
	popup.show(null, url);
	if (returnPopup) return popup;
}


// ================================================================
// AbstractDialog
// Replaces: new Class({Implements: [Options, Events]}), document.id(),
//           element.setStyles, element.set/get, element.inject/adopt,
//           new Element(), element.addEvent, new DOMEvent(e).stop()
// ================================================================

/**
 * Base class for dialog implementations, handling creation, content loading
 * from a remote URL, draggable handles, and show/hide lifecycle events.
 */
class AbstractDialog extends EventEmitter
{
	constructor(element, options)
	{
		super();
		this.options = Object.assign({
			draggable:  false,
			handle:     null,
			closeLink:  null,
			body:       null,
			width:      '500px',
			height:     'auto',
			title:      null,
			top:        null,
			left:       null,
			position:   null,
			onHide:     function() {},
			'class':    null,
			maximized:  false
		}, options);

		this.element    = $el(element);
		this.remoteURL  = null;

		if (!this.element)
		{
			this.element = this.createDialog(element);
		}
		else
		{
			this.dialog_header = this.element.querySelector('.dialog_header');
		}

		if (this.options['class']) this.element.classList.add(this.options['class']);

		if (this.element)
		{
			this.element.style.display = 'none';
			this.element.setAttribute('role', 'dialog');
		}

		this.options.body = this.options.body
			? $el(this.options.body)
			: document.getElementById(this.element.id + 'Body');

		if (this.options.closeLink)
		{
			var self = this;
			$el(this.options.closeLink).addEventListener('click', function(e)
			{
				e.preventDefault(); e.stopPropagation();
				self.hide();
				if (self.disposeOnExit) self.options.body.textContent = '';
			});
		}

		if (this.options.title) this.setTitle(this.options.title);
		this.element.style.width  = this.options.width;
		this.element.style.height = this.options.height;
		this.element.dialog = this;
	}

	resize(width, height)
	{
		this.element.style.width  = width;
		this.element.style.height = height;
		return this;
	}

	setTitle(title)
	{
		this.options.title = title;
		var el = document.getElementById(this.element.id + 'Title');
		if (el) el.textContent = title;
	}

	createDialog(id)
	{
		var dialog = document.createElement('div');
		dialog.className = 'dialog';
		dialog.id = id;
		setStyles(dialog, { display: 'none', width: this.options.width, height: this.options.height });

		this.dialog_header = document.createElement('div');
		this.dialog_header.className = 'dialog_header';
		this.dialog_header.id = id + 'Header';

		var padding = document.createElement('div');
		padding.style.padding = '4px';
		padding.innerHTML = "<div style='float: right'>&nbsp;<a id='close" + id + "' class='dialog_close' href=''>" + AbstractDialog.closeButtonHTML + "</a></div>" +
		                    "<span style='font-weight: bold' id='" + id + "Title'>" + (this.options.title || '') + "</span>";

		var body = document.createElement('div');
		body.id = id + 'Body';
		body.className = 'dialog_body';

		this.options.closeLink = 'close' + id;
		this.options.body = body;

		if (this.options.draggable)
		{
			this.options.handle = this.dialog_header;
			this.dialog_header.style.cursor = 'move';
		}

		this.dialog_header.appendChild(padding);
		dialog.appendChild(this.dialog_header);
		dialog.appendChild(body);
		(document.body || document.documentElement).appendChild(dialog);
		return dialog;
	}

	deleteDialog()
	{
		if (this.element) this.element.remove();
	}
}

AbstractDialog.closeButtonHTML = 'Close &times;';

AbstractDialog.onClose = function(dialog)
{
	if (dialog.remoteURL)
	{
		dialog.element.querySelectorAll('textarea.richtext').forEach(function(rte)
		{
			tinymce.remove('#' + rte.id);
		});
	}
};

AbstractDialog.findDialog = function(element)
{
	while (element)
	{
		if (element.dialog) return element.dialog;
		element = element.parentElement;
	}
	return null;
};


// ================================================================
// ModalDialog
// Replaces: Extends: AbstractDialog, element.fade(), Drag.Move,
//           Request.HTML, window.addEvent/removeEvent
// ================================================================

/**
 * A centered, curtain-backed modal dialog that fades in over a page curtain
 * and supports maximized mode and automatic recentering on resize.
 */
class ModalDialog extends AbstractDialog
{
	constructor(element, options)
	{
		super(element, options);
		this.element.style.opacity = '0';
		this.element.style.display = 'none';
	}

	center()
	{
		if (this.options.body) this.options.body.style.height = 'auto';

		if (this.options.maximized)
		{
			this.element.classList.add('maximized');
			setStyles(this.element, { position: 'fixed', top: '0', left: '0', width: '100%', height: '100%', 'z-index': 10000 });
			return;
		}

		this.element.classList.remove('maximized');
		var ws = window.size();

		if (this.element.offsetWidth > ws.width)
		{
			this.element.style.width = (ws.width * 0.9) + 'px';
		}
		if (this.element.offsetHeight > ws.height && this.options.body)
		{
			this.options.body.style.height    = (ws.height * 0.9) + 'px';
			this.options.body.style.overflowY = 'auto';
		}

		var x = (ws.width  - this.element.offsetWidth)  / 2;
		var y = (ws.height - this.element.offsetHeight) / 2;
		setStyles(this.element, { position: 'fixed', top: y, left: x, 'z-index': 10000 });
		this.fireEvent('resize');
	}

	show(onComplete, fragmentURL)
	{
		var reload = this.element.style.display !== 'none';
		this.remoteURL = fragmentURL;
		if (!reload) ModalDialog.activeDialogs.push(this);

		if (this.options.draggable)
		{
			makeDraggable(this.element, {
				preventDefault: true,
				handle: this.options.handle || undefined,
				onDrag: function(el, e) { e.preventDefault(); e.stopPropagation(); }
			});
		}

		var self = this;

		if (fragmentURL && this.options.body)
		{
			window.curtain.loadingCursor();
			this.options.body.setAttribute('data-url', fragmentURL);
			addReloadHandler(this.options.body);
			this.disposeOnExit = true;
			if (!reload) this.options.body.textContent = 'Loading...';

			fetchHTML(fragmentURL, function(html, scripts)
			{
				self.options.body.textContent = '';
				self.options.body.innerHTML = html;
				execScripts(scripts);
				addReloadHandlers(self.options.body);
				fadeIn(self.element, 400);
				self.center();
				window.curtain.normalCursor();
				if (onComplete) onComplete(self);
			});
		}
		else
		{
			fadeIn(this.element, 400);
		}

		window.lowerCurtain(function()
		{
			self.element.style.display = 'block';
			self.center();
			self.addResizeHook();
			if (onComplete && !fragmentURL) onComplete(self);
		});
	}

	addResizeHook()
	{
		var self = this;
		this.resizeHook = function() { self.center(); };
		window.addEventListener('resize', this.resizeHook);
	}

	removeResizeHook()
	{
		if (this.resizeHook)
		{
			window.removeEventListener('resize', this.resizeHook);
			this.resizeHook = null;
		}
	}

	hide(whenDone)
	{
		this.fireEvent('hide', this);
		if (AbstractDialog.onClose) AbstractDialog.onClose(this);
		ModalDialog.activeDialogs.pop();
		if (ModalDialog.activeDialogs.length === 0) window.raiseCurtain();
		this.element.style.display = 'none';
		if (this.remoteURL) this.element.remove();
		this.removeResizeHook();
		if (whenDone) whenDone();
	}

	dispose() { this.hide(this.deleteDialog.bind(this)); }

	reload()
	{
		if (this.options.body)
		{
			var self = this;
			this.options.body.reload(function() { self.center(); });
		}
	}
}

ModalDialog.activeDialogs = [];

ModalDialog.getActiveDialog = function()
{
	if (!ModalDialog.activeDialogs.length) return null;
	return ModalDialog.activeDialogs[ModalDialog.activeDialogs.length - 1];
};

ModalDialog.recenterActiveDialog = function()
{
	var d = ModalDialog.getActiveDialog();
	if (d) d.center();
};

ModalDialog.closeAllDialogs = function()
{
	while (ModalDialog.activeDialogs.length) ModalDialog.getActiveDialog().hide();
};


// ================================================================
// FloatingDialog
// ================================================================

/**
 * A dialog that floats at a fixed or computed position without a curtain,
 * suitable for non-blocking secondary panels and inline editors.
 */
class FloatingDialog extends AbstractDialog
{
	constructor(element, options)
	{
		super(element, options);
	}

	position()
	{
		var windowHeight = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
		var x, y;

		if (this.top && this.left)
		{
			x = this.left;
			y = this.top;
		}
		else
		{
			x = (document.body.clientWidth - this.element.offsetWidth) / 2;
			y = (windowHeight - this.element.offsetHeight) / 2;
		}

		var pos = this.options.position || (this.options.draggable ? 'absolute' : 'fixed');

		if (this.options.height !== 'auto' && this.element.offsetHeight > 0)
		{
			var height = this.element.offsetHeight - this.dialog_header.offsetHeight;
			this.options.body.style.maxHeight = height + 'px';
			this.options.body.style.overflow  = 'auto';
		}

		setStyles(this.element, { position: pos, top: y, left: x, 'z-index': 10000 });
		this.fireEvent('resize');
	}

	show(onComplete, fragmentURL)
	{
		if (this.options.draggable)
		{
			makeDraggable(this.element, {
				preventDefault: true,
				handle: this.options.handle || undefined,
				onDrag: function(el, e) { e.preventDefault(); e.stopPropagation(); }
			});
		}

		this.position();
		this.remoteURL = fragmentURL;
		var self = this;

		if (fragmentURL && this.options.body)
		{
			this.options.body.setAttribute('data-url', fragmentURL);
			addReloadHandler(this.options.body);
			this.disposeOnExit = true;
			this.options.body.textContent = 'Loading...';

			fetchHTML(fragmentURL, function(html, scripts)
			{
				self.options.body.textContent = '';
				self.options.body.innerHTML = html;
				execScripts(scripts);
				self.position();
			});
		}

		this.element.style.opacity = '0';
		this.element.style.display = 'block';
		fadeIn(this.element, 400, onComplete);
	}

	hide()
	{
		this.fireEvent('hide', this);
		if (AbstractDialog.onClose) AbstractDialog.onClose(this);
		var self = this;
		fadeOut(this.element, 400, function() { self.element.style.display = 'none'; });
	}

	dispose() { this.hide(this.deleteDialog.bind(this)); }
}


// ================================================================
// Interstitial
// Replaces: new Class({Implements: Options}), Asset.image(),
//           element.inject, element.setStyles, window.addEvent
// ================================================================

/**
 * Displays a centered loading spinner overlay with a message while an
 * operation is in progress, automatically recentering on window resize.
 */
class Interstitial
{
	constructor(message, options)
	{
		this.options = Object.assign({
			spinner:  '/fakoli/images/loader.gif',
			cssClass: 'interstitial',
			id:       'interstitial',
			width:    400,
			height:   66
		}, options);
		this.message     = message;
		this.resizeHook  = null;
		this.interstitial = this.createInterstitial();
	}

	createInterstitial()
	{
		var div = document.createElement('div');
		div.className = this.options.cssClass;
		div.id        = this.options.id;
		div.setAttribute('role', 'alert');
		setStyles(div, { width: this.options.width, height: this.options.height, display: 'none', padding: '10px' });

		var img = document.createElement('img');
		img.src = this.options.spinner;
		img.setAttribute('align', 'left');
		img.style.marginRight = '20px';
		div.appendChild(img);

		var text = document.createElement('span');
		text.innerHTML = this.message;
		div.appendChild(text);

		(document.body || document.documentElement).appendChild(div);
		return div;
	}

	center()
	{
		var size = window.size();
		var rect = this.interstitial.getBoundingClientRect();
		setStyles(this.interstitial, {
			position: 'fixed',
			top:  (size.height - rect.height) / 2,
			left: (size.width  - rect.width)  / 2,
			'z-index': 10005
		});
	}

	addResizeHook()
	{
		var self = this;
		this.resizeHook = function() { self.center(); };
		window.addEventListener('resize', this.resizeHook);
	}

	removeResizeHook()
	{
		if (this.resizeHook) { window.removeEventListener('resize', this.resizeHook); this.resizeHook = null; }
	}

	show()
	{
		var self = this;
		window.lowerCurtain(function()
		{
			self.interstitial.style.display = 'block';
			self.center();
			self.addResizeHook();
			Interstitial.current = self;
		});
	}

	hide()
	{
		this.removeResizeHook();
		window.raiseCurtain();
		this.interstitial.style.display = 'none';
		Interstitial.current = null;
	}
}

Interstitial.setDefaultSpinner = function(spinner)
{
	Interstitial.defaultSpinner = spinner;
	new Image().src = spinner; // preload
};

Interstitial.setDefaultSpinner('/fakoli/images/loader.gif');

function interstitial(message, image)
{
	if (!image) image = Interstitial.defaultSpinner;
	var i = new Interstitial(message, { spinner: image });
	i.show();
	return i;
}

function hideInterstitial()
{
	if (Interstitial.current) Interstitial.current.hide();
}


// ================================================================
// Notification
// Replaces: new Class({Implements: [Options, Events]}),
//           element.fade(), fn.delay()
// ================================================================

/**
 * Shows a transient notification message that auto-dismisses after a
 * configurable delay, with optional click handler and blocking mode.
 */
class Notification extends EventEmitter
{
	constructor(message, options)
	{
		super();
		this.options = Object.assign({
			cssClass:    'notification',
			width:       400,
			height:      'auto',
			wait:        3000,
			onHide:      function() {},
			onClick:     null,
			blocking:    false,
			buttonClass: 'button',
			buttonText:  'OK'
		}, options);
		this.message = message;
		this.notification = this.createNotification();
		this.center();
		this.show();
		if (!this.options.blocking)
		{
			var self = this;
			setTimeout(function() { self.hide(); }, this.options.wait);
			setTimeout(function() { self.fireEvent('hide', self); }, this.options.wait + 1000);
		}
	}

	createNotification()
	{
		var div = document.createElement('div');
		div.className = this.options.cssClass;
		div.setAttribute('role', 'alert');
		setStyles(div, {
			'max-width': this.options.width,
			height:      this.options.height,
			display:     'block',
			opacity:     '0'
		});
		div.innerHTML = this.message;
		(document.body || document.documentElement).appendChild(div);

		if (this.options.blocking)
		{
			var self = this;
			var button = document.createElement('a');
			button.className = this.options.buttonClass;
			button.setAttribute('role', 'button');
			button.innerHTML = this.options.buttonText;
			button.addEventListener('click', function(e)
			{
				e.preventDefault(); e.stopPropagation();
				self.hide();
				self.fireEvent('click');
			});
			div.appendChild(button);
		}
		return div;
	}

	center()
	{
		var size = window.size();
		var rect = this.notification.getBoundingClientRect();
		var x = (size.width - rect.width) / 2;
		var y = (size.height - rect.height) / 2;
		if (rect.width > size.width * 0.8)
		{
			this.notification.style.width = (size.width * 0.8) + 'px';
			x = size.width * 0.1;
		}
		setStyles(this.notification, { position: 'fixed', top: y, left: x, 'z-index': 15000 });
	}

	show() { fadeIn(this.notification, 400); }
	hide() { fadeOut(this.notification, 400); }
}

function notification(message, options)
{
	new Notification(message, options);
}


// ================================================================
// ProgressiveSearch
// Replaces: new Class({Implements: Options}), Request,
//           element.addEvent, element.setStyles, element.getCoordinates
// ================================================================

/**
 * Attaches a live-search dropdown to an input field, fetching results from
 * a server URL as the user types and optionally offering a browse dialog.
 */
class ProgressiveSearch
{
	constructor(element, options)
	{
		this.options = Object.assign({
			search:        null,
			minimumLength: 4,
			height:        '150px',
			width:         null,
			cssClass:      '',
			parameter:     '',
			defaultSearch: null,
			browse:        false,
			browseText:    'Browse',
			browseCssClass:'button'
		}, options);
		this.allowHide = true;
		this.sequence  = 0;
		this.element   = $el(element);
		this.element.search = this;

		this.list = document.createElement('div');
		this.list.id = this.element.id + '_progressive_search';
		this.list.className = 'progressive_search';
		setStyles(this.list, { display: 'none', position: 'absolute', 'max-height': this.options.height, 'overflow-y': 'auto' });

		this.container = document.createElement('div');
		this.list.appendChild(this.container);

		var self = this;
		this.container.select = function(value) { self.element.value = value; self.list.style.display = 'none'; return self; };
		this.container.search = function(value) { self.search(value); return self; };
		this.container.hide   = function() { self.hideList(true); };

		if (this.options.cssClass) this.list.classList.add(this.options.cssClass);
		(document.body || document.documentElement).appendChild(this.list);

		this.element.addEventListener('keyup',  function() { self.onKeyPress(); });
		this.element.addEventListener('blur',   function() { self.hideList(); });
		this.element.addEventListener('focus',  function() { self.showDefaultList(); });
		this.element.setAttribute('autocomplete', 'off');
		this.list.addEventListener('mouseover', function() { self.allowHide = false; });
		this.list.addEventListener('mouseout',  function() { self.allowHide = true; });

		if (this.options.browse)
		{
			this.browseButton = document.createElement('a');
			this.browseButton.href = '#';
			this.browseButton.className = this.options.browseCssClass;
			this.browseButton.innerHTML = this.options.browseText;
			this.browseButton.style.marginLeft = '5px';
			this.browseButton.addEventListener('click', function(e)
			{
				e.preventDefault(); e.stopPropagation();
				self.element.focus();
				self.browse();
			});
			this.element.insertAdjacentElement('afterend', this.browseButton);
		}
	}

	onKeyPress()
	{
		var val = this.element.value;
		if (val === '') this.showDefaultList();
		if (val.length < this.options.minimumLength) { this.list.style.display = 'none'; return; }
		this.handleSearch(val);
	}

	handleSearch(val)
	{
		this.sequence++;
		var name = this.options.parameter || this.element.id;
		var self = this;
		fetchText(
			appendQueryString(this.options.search, name + '=' + encodeURIComponent(val) + '&sequence=' + this.sequence),
			function(html) { self.showList(html); }
		);
	}

	browse()
	{
		var self = this;
		if (this.element.value) { this.handleSearch(this.element.value); return; }
		fetchText(appendQueryString(this.options.search, 'browse=1'), function(html) { self.showList(html); });
	}

	search(val) { this.element.value = val; this.onKeyPress(); }

	showList(html)
	{
		var match = /data-sequence=['"](\d+)['"]/i.exec(html);
		if (match && match[1] != this.sequence) return;

		this.container.innerHTML = html;
		var rect = this.element.getBoundingClientRect();
		setStyles(this.list, {
			top:        rect.bottom + window.pageYOffset,
			left:       rect.left   + window.pageXOffset,
			width:      this.options.width || rect.width,
			'max-height': this.options.height,
			display:    'block'
		});
		var zIndex = calculateZIndex(this.element);
		if (zIndex !== 0) this.list.style.zIndex = zIndex + 1;
	}

	hideList(override)
	{
		if (!this.allowHide && !override) return;
		this.list.style.display = 'none';
	}

	reset() { this.hideList(true); this.element.value = ''; }

	showDefaultList()
	{
		var self = this;
		if (this.element.value === '' && this.options.defaultSearch)
		{
			fetchText(this.options.defaultSearch, function(html) { self.showList(html); });
		}
	}
}


// ================================================================
// PaginatingList
// Replaces: new Class({Implements: Options}), element.getChildren(),
//           element.getElements(), element.empty(), element.setStyle,
//           new DOMEvent(evt).stop(), fn.bind()
// ================================================================

/**
 * Paginates the items of an unordered list with Prev/Next controls,
 * with optional FacetManager integration for filtered pagination.
 */
class PaginatingList
{
	constructor(list, paginator, options)
	{
		this.options = Object.assign({ per_page: 10, display_mode: 'list-item' }, options);
		this.current_page = 1;
		this.list = $el(list);
		if (!this.list) return;
		this.paginator = $el(paginator);
		this.pages = Math.ceil(this.list.querySelectorAll('li').length / this.options.per_page);

		if (this.list.facetManager)
		{
			var self = this;
			this.list.facetManager.addEvent('filterChanged', function() { self.filterChanged(); });
			this.list.facetManager.addEvent('filterCleared', function() { self.filterCleared(); });
			this.preprocessFacets();
		}

		this.createPagination();
		this.toPage(this.current_page);
	}

	createPagination()
	{
		if (!this.paginator) return;
		this.paginator.innerHTML = '';
		var self = this;

		this.paginator.appendChild(this._paginationNode('&#60;&#60;&#32;&#80;&#114;&#101;&#118;', function(e)
		{
			e.preventDefault(); self.toPrevPage();
		}));

		var li = document.createElement('li');
		li.className = 'pager';
		var a = document.createElement('a');
		a.href = '#'; a.className = 'goto-page';
		a.innerHTML = 'Page ' + (this.current_page || 1) + ' of ' + this.pages;
		li.appendChild(a);
		this.paginator.appendChild(li);

		this.paginator.appendChild(this._paginationNode('&#78;&#101;&#120;&#116;&#32;&#62;&#62;', function(e)
		{
			e.preventDefault(); self.toNextPage();
		}));

		if (this.pages <= 1 && this.paginator.parentElement)
		{
			this.paginator.parentElement.style.display = 'none';
		}
	}

	_paginationNode(text, handler)
	{
		var span = document.createElement('span');
		span.innerHTML = text;
		var a = document.createElement('a');
		a.href = '#'; a.className = 'paginate';
		a.addEventListener('click', handler);
		a.appendChild(span);
		var li = document.createElement('li');
		li.appendChild(a);
		return li;
	}

	countRows()
	{
		return this.list.children.length - this.list.querySelectorAll('.filtered').length;
	}

	updatePageCount()
	{
		this.pages = Math.ceil(this.countRows() / this.options.per_page);
		this.toPage(1);
		this.updatePage();
	}

	updatePage()
	{
		if (!this.paginator) return;
		var self = this;
		this.paginator.querySelectorAll('a.goto-page').forEach(function(p)
		{
			p.innerHTML = 'Page ' + ((self.pages > 0) ? (self.current_page || 1) : '0') + ' of ' + self.pages;
		});
	}

	toPrevPage() { this.toPage(this.current_page - 1); }
	toNextPage() { this.toPage(this.current_page + 1); }

	toPage(page_num)
	{
		if (!this.paginator) return;
		page_num = parseInt(page_num);
		if (page_num > this.pages || page_num < 1) return;
		this.current_page = page_num;
		var lo = this.options.per_page * (page_num - 1);
		var hi = this.options.per_page * page_num;
		var kids = Array.from(this.list.querySelectorAll(':scope > *:not(.filtered)'));
		for (var i = 0; i < kids.length; i++)
		{
			kids[i].style.display = (i >= lo && i < hi) ? this.options.display_mode : 'none';
		}
		this.updatePage();
	}

	preprocessFacets()
	{
		var self = this;
		this.list.querySelectorAll('li').forEach(function(elt) { self.list.facetManager.preprocess(elt); });
		this.list.facetManager.preprocessComplete();
	}

	filterChanged()
	{
		var self = this;
		this.list.querySelectorAll('li').forEach(function(elt)
		{
			elt.classList.remove('filtered', 'filtermatch');
			if (self.list.facetManager.filter(elt))
			{
				elt.classList.add('filtermatch');
				elt.style.display = '';
			}
			else
			{
				elt.classList.add('filtered');
				elt.style.display = 'none';
			}
		});
		this.updatePageCount();
	}

	filterCleared()
	{
		this.list.querySelectorAll('li').forEach(function(elt)
		{
			elt.classList.remove('filtered', 'filtermatch');
			elt.style.display = '';
		});
		this.updatePageCount();
	}
}


// ================================================================
// Splitter
// Replaces: new Class({Implements: [Options, Events]}), element.wraps(),
//           element.getWidth/Height, element.getCoordinates,
//           document.addEvents, document.removeEvent
// ================================================================

/**
 * Divides a container into two resizable panes separated by a draggable
 * splitter bar, supporting both horizontal and vertical orientations.
 */
class Splitter extends EventEmitter
{
	constructor(element, options)
	{
		super();
		this.container = $el(element);
		this.options = Object.assign({ orientation: 'vertical', split: [50, 50], minimumSize: 10 }, options);
		this.resizing = false;
		this.panes = Array.from(this.container.children);

		if (this.panes.length !== 2) { alert('Splitting Headache!'); return; }

		this.layout = document.createElement('div');
		setStyles(this.layout, { width: '100%', height: '100%' });

		this.splitter = document.createElement('div');
		this.splitter.className = this.getSplitterClass();
		this.splitter.style.float = 'left';

		// Wrap panes[0] with layout (replacing element.wraps())
		this.container.insertBefore(this.layout, this.panes[0]);
		this.layout.appendChild(this.panes[0]);
		this.layout.appendChild(this.splitter);
		this.layout.appendChild(this.panes[1]);
		this.container.appendChild(this.layout);

		this.panes.forEach(function(elt) { setStyles(elt, { 'overflow-y': 'auto', float: 'left' }); });

		var self = this;
		this.splitter.addEventListener('mousedown', function(e) { self.startResize(e); });
		this.calculateLayout();
	}

	getSplitterClass() { return 'splitter_' + this.options.orientation; }

	setOrientation(orientation)
	{
		this.splitter.classList.remove(this.getSplitterClass());
		this.options.orientation = orientation;
		this.splitter.classList.add(this.getSplitterClass());
		this.calculateLayout();
	}

	calculateLayout()
	{
		if (this.resizing) return;
		this.resizing = true;
		var vert = this.options.orientation === 'vertical';
		var prop = vert ? 'height' : 'width';
		if (vert)
		{
			var w = this.layout.offsetWidth;
			this.panes.forEach(function(elt) { elt.style.width = w + 'px'; });
		}
		else
		{
			var h = this.layout.offsetHeight;
			this.panes.forEach(function(elt) { elt.style.height = h + 'px'; });
		}
		var splitterSize = 9;
		var total = this.options.split[0] + this.options.split[1];
		var r0 = this.options.split[0] / total, r1 = this.options.split[1] / total;
		var size = Math.max((vert ? this.layout.offsetHeight : this.layout.offsetWidth) - splitterSize, 0);
		this.panes[0].style[prop] = Math.floor(r0 * size) + 'px';
		this.panes[1].style[prop] = Math.floor(r1 * size) + 'px';
		this.fireEvent('resize');
		this.resizing = false;
	}

	startResize(e)
	{
		var self = this;
		var vert = this.options.orientation === 'vertical';
		var minF = vert ? 'top' : 'left', maxF = vert ? 'bottom' : 'right';

		var performDrag = function(e)
		{
			var rect   = self.container.getBoundingClientRect();
			var coords = {
				top:    rect.top    + window.pageYOffset,
				left:   rect.left   + window.pageXOffset,
				bottom: rect.bottom + window.pageYOffset,
				right:  rect.right  + window.pageXOffset
			};
			var pos    = vert ? e.clientY + document.documentElement.scrollTop
			                  : e.clientX + document.documentElement.scrollLeft;
			var range  = coords[maxF] - coords[minF];
			var thumb  = (pos - coords[minF]) / range * 100;
			thumb = Math.min(Math.max(thumb, self.options.minimumSize), 100 - self.options.minimumSize);
			self.options.split[0] = thumb;
			self.options.split[1] = 100 - thumb;
			self.calculateLayout();
		};
		var endDrag = function()
		{
			document.removeEventListener('mousemove', performDrag);
			document.removeEventListener('mouseup', endDrag);
		};
		document.addEventListener('mousemove', performDrag);
		document.addEventListener('mouseup', endDrag);
	}
}


// ================================================================
// CrossFader
// Replaces: new Class({Implements: Options}), element.fade/set('tween'),
//           fn.periodical(), element.getSize(), element.position(),
//           window.addEvent('resize')
// ================================================================

/**
 * Cycles through child elements with a cross-fade transition, supporting
 * optional navigation dots or prev/next links and configurable timing.
 */
class CrossFader
{
	constructor(container, options)
	{
		this.options = Object.assign({
			duration:                  5000,
			transition:                1000,
			navigation:                false,
			navigationType:            'byItem',
			navigationPosition:        'bottomLeft',
			navigationEdge:            'bottomLeft',
			navigationContainerClass:  'crossfader_nav',
			navigationClass:           'crossfader_nav_item',
			navigationCurrentClass:    'crossfader_current',
			navigationHighlightClass:  'crossfader_highlight',
			navigationPreviousClass:   'crossfader_previous',
			navigationNextClass:       'crossfader_next',
			prevLinkPosition:          'centerLeft',
			prevLinkEdge:              'centerLeft',
			nextLinkPosition:          'centerRight',
			nextLinkEdge:              'centerRight',
			navigationShowNumbers:     false,
			firstElementPosition:      'absolute'
		}, options);

		this.container       = $el(container);
		this.idx             = 0;
		this.navigationLinks = [];
		this.timer           = null;
		this.paused          = false;

		if (!this.container) return;
		this.elements = Array.from(this.container.children);
		if (!this.elements.length) return;

		setStyles(this.elements[0], { display: 'block', position: this.options.firstElementPosition, visibility: 'visible', opacity: '1' });
		if (this.elements.length === 1) return;

		this.createNavigation();
		this.start();
	}

	createNavigation()
	{
		if (!this.options.navigation) return;
		var self = this;

		this.navigationContainer = document.createElement('div');
		this.navigationContainer.className = 'crossfader_nav';
		this.navigationContainer.style.position = 'absolute';

		if (this.options.navigationType === 'PrevNext')
		{
			var leftArrow  = this._navLink(this.options.navigationClass + ' ' + this.options.navigationPreviousClass, '&nbsp;');
			var rightArrow = this._navLink(this.options.navigationClass + ' ' + this.options.navigationNextClass,     '&nbsp;');
			leftArrow.addEventListener('click',  function(e) { e.preventDefault(); self.goToPrevious(); });
			rightArrow.addEventListener('click', function(e) { e.preventDefault(); self.goToNext(); });
			this.navigationContainer.appendChild(leftArrow);
			this.navigationContainer.appendChild(rightArrow);
			this.navigationLinks.push(leftArrow, rightArrow);

			document.body.appendChild(this.navigationContainer);
			var reposition = function()
			{
				positionRelative(self.navigationContainer, { relativeTo: self.container, position: 'topLeft', edge: 'topLeft' });
				setStyles(self.navigationContainer, { width: self.container.offsetWidth, height: self.container.offsetHeight });
				positionRelative(leftArrow,  { relativeTo: self.navigationContainer, position: self.options.prevLinkPosition, edge: self.options.prevLinkEdge });
				positionRelative(rightArrow, { relativeTo: self.navigationContainer, position: self.options.nextLinkPosition, edge: self.options.nextLinkEdge });
			};
			reposition();
			window.addEventListener('resize', reposition);

		}
		else if (this.options.navigationType === 'byItem')
		{
			this.elements.forEach(function(elt, idx)
			{
				var blob = self._navLink(self.options.navigationClass, self.options.navigationShowNumbers ? idx + 1 : '&nbsp;');
				blob.addEventListener('mouseenter', function() { blob.classList.add(self.options.navigationHighlightClass); });
				blob.addEventListener('mouseleave', function() { blob.classList.remove(self.options.navigationHighlightClass); });
				blob.addEventListener('click', function(e) { e.preventDefault(); self.goTo(idx); });
				self.navigationContainer.appendChild(blob);
				self.navigationLinks.push(blob);
			});
			document.body.appendChild(this.navigationContainer);
			var reposition = function()
			{
				positionRelative(self.navigationContainer, { relativeTo: self.container, position: self.options.navigationPosition, edge: self.options.navigationEdge });
			};
			reposition();
			window.addEventListener('resize', reposition);
		}
	}

	_navLink(className, html)
	{
		var a = document.createElement('a');
		a.href = '#'; a.className = className; a.innerHTML = html;
		return a;
	}

	start()
	{
		if (this.options.duration === 0) return;
		var self = this;
		this.timer = setInterval(function() { self.next(); }, this.options.duration);
	}

	next()
	{
		if (this.paused) return;
		if (this.idx >= 0)
		{
			fadeOut(this.elements[this.idx], this.options.transition);
			if (this.options.navigation && this.options.navigationType === 'byItem')
				this.navigationLinks[this.idx].classList.remove(this.options.navigationCurrentClass);
		}
		this.idx = (this.idx + 1) % this.elements.length;
		fadeIn(this.elements[this.idx], this.options.transition);
		if (this.options.navigation && this.options.navigationType === 'byItem')
			this.navigationLinks[this.idx].classList.add(this.options.navigationCurrentClass);
	}

	goTo(idx)
	{
		this.paused = true;
		if (this.idx === idx) return;
		if (this.idx >= 0 && this.options.navigation && this.options.navigationType === 'byItem')
			this.navigationLinks[this.idx].classList.remove(this.options.navigationCurrentClass);
		fadeOut(this.elements[this.idx], this.options.transition);
		this.idx = idx;
		fadeIn(this.elements[this.idx], this.options.transition);
		if (this.options.navigation && this.options.navigationType === 'byItem')
			this.navigationLinks[this.idx].classList.add(this.options.navigationCurrentClass);
	}

	goToPrevious() { this.goTo((this.idx - 1 + this.elements.length) % this.elements.length); }
	goToNext()     { this.goTo((this.idx + 1) % this.elements.length); }
}


// ================================================================
// FocusWatcher
// Replaces: new Class({Implements: [Options, Events]}),
//           Array.combine(), $$(), element.addEvents()
// ================================================================

/**
 * Tracks focus and blur across all interactive elements on the page,
 * firing a focusChanged event whenever the focused element changes.
 */
class FocusWatcher extends EventEmitter
{
	constructor(options)
	{
		super();
		this.options = Object.assign({
			elementTypes: ['textarea', 'input', 'select', 'button', 'a'],
			onFocusChanged: function() {}
		}, options);
		this.focus = null;
		this.blur  = null;

		var self = this;
		var seen = new Set();
		this.options.elementTypes.forEach(function(type)
		{
			document.querySelectorAll(type).forEach(function(elt)
			{
				if (seen.has(elt)) return;
				seen.add(elt);
				elt.addEventListener('focus', function() { self.focus = elt; self.fireEvent('focusChanged'); });
				elt.addEventListener('blur',  function() { self.blur  = elt; });
			});
		});
	}
}


// ================================================================
// ScrollWatcher
// Replaces: new Class(), scroller.addEvent/removeEvent('scroll'),
//           element.getScroll(), element.getScrollSize(), element.getHeight()
// ================================================================

/**
 * Watches a scrollable element (or window) and applies CSS classes to
 * target elements when they scroll above or below specified thresholds.
 */
class ScrollWatcher
{
	constructor(scroller)
	{
		this.scroller = scroller || window;
		this._handlers = [];
	}

	watch(position, element, above, below)
	{
		element = $el(element);
		if (!element) return this;
		var self = this;
		var handler = function() { self.onScroll(position, element, above, below); };
		this._handlers.push(handler);
		this.scroller.addEventListener('scroll', handler);
		return this;
	}

	clear()
	{
		var self = this;
		this._handlers.forEach(function(h) { self.scroller.removeEventListener('scroll', h); });
		this._handlers = [];
	}

	onScroll(position, element, above, below)
	{
		var isWindow  = this.scroller === window;
		var scrollY   = isWindow ? window.pageYOffset    : this.scroller.scrollTop;
		var scrollMax = isWindow ? document.documentElement.scrollHeight : this.scroller.scrollHeight;

		element.setAttribute('data-offset', scrollY);

		var threshold = position >= 0 ? position : scrollMax + position - element.offsetHeight;

		if (scrollY > threshold)
		{
			if (typeof below === 'function') below(element);
			else { element.classList.remove(above); element.classList.add(below); }
		}
		else
		{
			if (typeof above === 'function') above(element);
			else { element.classList.remove(below); element.classList.add(above); }
		}
	}
}

document.addEventListener('DOMContentLoaded', function()
{
	document.focusWatcher  = new FocusWatcher();
	document.scrollWatcher = new ScrollWatcher();
});


// ================================================================
// fitText
// Replaces: Element.implement('fitText'), $$('.fitText').fitText()
// Note: was an element method in MooTools; now a standalone function.
// ================================================================

function fitText(el)
{
	var parent   = el.parentElement;
	var maxWidth = parent.offsetWidth, maxHeight = parent.offsetHeight;
	var sizeX    = el.offsetWidth,     sizeY     = el.offsetHeight;
	if (sizeY <= maxHeight && sizeX <= maxWidth) return;
	var fontSize = parseInt(getStyle(el, 'font-size'));
	while ((sizeX > maxWidth || sizeY > maxHeight) && fontSize > 4)
	{
		fontSize -= 0.5;
		el.style.fontSize = fontSize + 'px';
		sizeX = el.offsetWidth; sizeY = el.offsetHeight;
	}
}

document.addEventListener('DOMContentLoaded', function()
{
	document.querySelectorAll('.fitText').forEach(fitText);
});


// ================================================================
// CountIndicator
// Replaces: new Class({Implements: Options}), element.position(),
//           element.set/get, element.adopt
// ================================================================

/**
 * Renders a small badge overlay showing a numeric count on a container
 * element, positioned relative to the container's corner.
 */
class CountIndicator
{
	constructor(container, options)
	{
		this.options = Object.assign({
			cssClass: 'count_indicator',
			position: 'bottomRight',
			edge:     'bottomRight',
			showZero: false,
			maximum:  99,
			fixed:    false,
			offset:   { x: 0, y: 0 }
		}, options);
		this.container = $el(container);
		this.refresh();
	}

	refresh()
	{
		var self = this;
		this.container.querySelectorAll('[data-count]').forEach(function(element)
		{
			if (!element.countDisplay)
			{
				var div = document.createElement('div');
				div.className = self.options.cssClass;
				setStyles(div, { display: 'block', width: 'auto', position: self.options.fixed ? 'fixed' : 'absolute' });
				document.body.appendChild(div);
				element.countDisplay = div;
			}
			var count = parseInt(element.getAttribute('data-count'));
			element.countDisplay.textContent = (count > self.options.maximum) ? self.options.maximum + '+' : count;
			element.countDisplay.style.display = (count === 0 && !self.options.showZero) ? 'none' : '';
			positionRelative(element.countDisplay, { relativeTo: element, position: self.options.position, edge: self.options.edge, offset: self.options.offset });
			if (self.options.fixed) element.countDisplay.style.position = 'fixed';
		});
	}
}


// ================================================================
// ToggleManager
// Replaces: new Class({Implements: Options}), element.addEvent,
//           new DOMEvent(event).target, element.get('target'),
//           element.hasClass, elt.reveal(), elt.dissolve()
// ================================================================

/**
 * Manages expand/collapse toggles within a container, applying open/closed
 * CSS classes and calling configurable show/hide functions on each toggle.
 */
class ToggleManager
{
	constructor(options)
	{
		this.options = Object.assign({
			container: null,
			show:      function(elt) { reveal(elt); },
			hide:      function(elt) { dissolve(elt); },
			selector:  '.toggle',
			cssOpen:   'open',
			cssClosed: 'closed',
			event:     'click'
		}, options);

		this.container = this.options.container ? $el(this.options.container) : document.body;
		if (!this.container) return;

		var self = this;
		this.container.querySelectorAll(this.options.selector).forEach(function(toggle)
		{
			toggle.addEventListener(self.options.event, function(event)
			{
				var elt    = event.target;
				var target = elt.getAttribute('target') || elt.getAttribute('data-target');
				if (!target) return;
				target = $el(target);
				if (elt.classList.contains(self.options.cssOpen))
				{
					self.options.hide(target);
					elt.classList.replace(self.options.cssOpen, self.options.cssClosed);
				}
				else
				{
					self.options.show(target);
					elt.classList.replace(self.options.cssClosed, self.options.cssOpen);
				}
				return false;
			});
		});
	}
}


// ================================================================
// ConnectivityChecker
// Replaces: new Class({Implements: [Options, Events]}),
//           fn.periodical(), new Request(), this.fireEvent()
// ================================================================

/**
 * Periodically pings the server to detect connectivity loss, firing online
 * and offline events so the UI can react to network state changes.
 */
class ConnectivityChecker extends EventEmitter
{
	constructor(options)
	{
		super();
		this.onlineFlag = true;
		this.timerID    = null;
		this.options = Object.assign({
			timeout: 2000,
			period:  5000,
			url:     '/action/component/ping'
		}, options);
		this.start();
	}

	start()
	{
		if (this.timerID) return;
		var self = this;
		this.checkConnectivity();
		this.timerID = setInterval(function() { self.checkConnectivity(); }, this.options.period);
	}

	stop()
	{
		if (this.timerID) { clearInterval(this.timerID); this.timerID = null; }
	}

	checkConnectivity()
	{
		var self = this;
		fetchText(
			this.options.url,
			function(response) { self.updateConnectivity(response === 'OK'); },
			function()         { self.updateConnectivity(false); },
			function()         { self.updateConnectivity(false); },
			this.options.timeout
		);
	}

	updateConnectivity(online)
	{
		if (online !== this.onlineFlag)
		{
			this.fireEvent(online ? 'online' : 'offline');
			this.onlineFlag = online;
		}
	}
}
