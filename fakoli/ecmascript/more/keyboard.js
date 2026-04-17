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
 * Keyboard — hierarchical keyboard-shortcut management system.
 * ES equivalent of MooTools More Keyboard + Keyboard.Extras.
 *
 * Shortcut strings follow the pattern: [keydown:|keyup:]modifier+...+key
 * e.g. 'ctrl+s', 'shift+alt+enter', 'keyup:escape'
 */

var _keyboardParsed = {};
var _modifiers      = ['shift', 'control', 'alt', 'meta'];
var _modRe          = /^(?:shift|control|ctrl|alt|meta)$/;

class Keyboard
{
	/**
	 * @param {Object} [options]
	 * @param {string}   options.defaultEventType  'keydown' or 'keyup' (default 'keydown')
	 * @param {boolean}  options.active             Activate immediately (default false)
	 * @param {Object}   options.events             Map of shortcut strings to handlers
	 * @param {Keyboard} options.manager            Parent keyboard instance
	 */
	constructor(options)
	{
		options = options || {};
		this._defaultEventType = options.defaultEventType || 'keydown';
		this._active    = false;
		this._manager   = options.manager || null;
		this._instances = [];
		this._activeKB  = null;
		this.previous   = null;
		this._listeners = {}; // parsed-type → [fn, ...]

		this._shortcuts      = [];
		this._shortcutIndex  = {};

		// Register with the root manager unless this IS the root manager
		if (Keyboard.manager && !this._manager)
		{
			Keyboard.manager.manage(this);
		}

		if (options.events)
		{
			var self = this;
			Object.keys(options.events).forEach(function(type)
			{
				self.addEvent(type, options.events[type]);
			});
		}

		if (options.active) this.activate();
	}

	/**
	 * Parse a shortcut string into a canonical event-type key.
	 * 'ctrl+s' → 'keydown:control+s'
	 * 'keyup:escape' → 'keyup:escape'
	 */
	static parse(type, defaultEventType)
	{
		defaultEventType = defaultEventType || 'keydown';

		if (_keyboardParsed[type]) return _keyboardParsed[type];

		var eventType = defaultEventType;
		var rest = type.toLowerCase().replace(/^(keyup|keydown):/, function(m, et)
		{
			eventType = et;
			return '';
		});

		var canonical;
		if (rest === '+')
		{
			canonical = rest;
		}
		else
		{
			var key, mods = {};
			rest.split('+').forEach(function(part)
			{
				if (_modRe.test(part))
				{
					mods[part === 'ctrl' ? 'control' : part] = true;
				}
				else
				{
					key = part;
				}
			});

			var parts = _modifiers.filter(function(m) { return mods[m]; });
			if (key) parts.push(key);
			canonical = parts.join('+');
		}

		var result = eventType + ':' + canonical;
		_keyboardParsed[type] = result;
		return result;
	}

	/** Add a keyboard shortcut listener. `type` may include modifier syntax. */
	addEvent(type, fn)
	{
		var parsed = Keyboard.parse(type, this._defaultEventType);
		if (!this._listeners[parsed]) this._listeners[parsed] = [];
		this._listeners[parsed].push(fn);
		return this;
	}

	/** Remove a keyboard shortcut listener. */
	removeEvent(type, fn)
	{
		var parsed = Keyboard.parse(type, this._defaultEventType);
		var list   = this._listeners[parsed];
		if (!list) return this;
		var idx = list.indexOf(fn);
		if (idx !== -1) list.splice(idx, 1);
		return this;
	}

	/** Activate this keyboard context (routes events here). */
	activate(instance)
	{
		if (instance)
		{
			if (instance.isActive()) return this;
			if (this._activeKB && instance !== this._activeKB)
			{
				this.previous = this._activeKB;
				this._activeKB._fireEvent('deactivate');
			}
			this._activeKB = instance;
			instance._fireEvent('activate');
			if (Keyboard.manager) Keyboard.manager._fireEvent('changed');
		}
		else if (this._manager)
		{
			this._manager.activate(this);
		}
		return this;
	}

	/** Returns true if this keyboard is the active child of its manager. */
	isActive()
	{
		if (this._manager) return this._manager._activeKB === this;
		return Keyboard.manager === this;
	}

	/** Deactivate this keyboard context or a child instance. */
	deactivate(instance)
	{
		if (instance)
		{
			if (instance === this._activeKB)
			{
				this._activeKB = null;
				instance._fireEvent('deactivate');
				if (Keyboard.manager) Keyboard.manager._fireEvent('changed');
			}
		}
		else if (this._manager)
		{
			this._manager.deactivate(this);
		}
		return this;
	}

	/** Give up focus, falling back to the previous active keyboard. */
	relinquish()
	{
		if (this.isActive() && this._manager && this._manager.previous)
		{
			this._manager.activate(this._manager.previous);
		}
		else
		{
			this.deactivate();
		}
		return this;
	}

	/** Add a child keyboard to be managed by this instance. */
	manage(instance)
	{
		if (instance._manager) instance._manager.drop(instance);
		this._instances.push(instance);
		instance._manager = this;
		if (!this._activeKB) this.activate(instance);
		return this;
	}

	/** Remove a managed child keyboard. */
	drop(instance)
	{
		instance.relinquish();
		var idx = this._instances.indexOf(instance);
		if (idx !== -1) this._instances.splice(idx, 1);
		if (this._activeKB === instance)
		{
			if (this.previous && this._instances.indexOf(this.previous) !== -1)
			{
				this.activate(this.previous);
			}
			else
			{
				this._activeKB = this._instances[0] || null;
			}
		}
		return this;
	}

	/** @private Fire listeners for `type` on this instance. */
	_fireEvent(type, event)
	{
		var list = this._listeners[type];
		if (list) list.forEach(function(fn) { fn(event); });
	}

	/** @private Route a keyboard event through the hierarchy. */
	_handle(event, parsedType)
	{
		if (event._preventKeyboardPropagation) return;

		var bubbles = !!this._manager;
		if (bubbles && this._activeKB)
		{
			this._activeKB._handle(event, parsedType);
			if (event._preventKeyboardPropagation) return;
		}
		this._fireEvent(parsedType, event);
		if (!bubbles && this._activeKB)
		{
			this._activeKB._handle(event, parsedType);
		}
	}

	// ── Keyboard.Extras ──────────────────────────────────────────────────────

	/**
	 * Register a named shortcut.
	 * @param {string} name
	 * @param {Object} shortcut  { keys, description, handler }
	 */
	addShortcut(name, shortcut)
	{
		shortcut.getKeyboard = () => this;
		shortcut.name = name;
		this._shortcutIndex[name] = shortcut;
		this._shortcuts.push(shortcut);
		if (shortcut.keys) this.addEvent(shortcut.keys, shortcut.handler);
		return this;
	}

	/** Register multiple named shortcuts from an object map. */
	addShortcuts(obj)
	{
		var self = this;
		Object.keys(obj).forEach(function(name) { self.addShortcut(name, obj[name]); });
		return this;
	}

	/** Remove a named shortcut by name. */
	removeShortcut(name)
	{
		var shortcut = this._shortcutIndex[name];
		if (shortcut && shortcut.keys)
		{
			this.removeEvent(shortcut.keys, shortcut.handler);
			delete this._shortcutIndex[name];
			var idx = this._shortcuts.indexOf(shortcut);
			if (idx !== -1) this._shortcuts.splice(idx, 1);
		}
		return this;
	}

	/** Returns all registered shortcuts. */
	getShortcuts()
	{
		return this._shortcuts.slice();
	}

	/** Looks up a shortcut by name. */
	getShortcut(name)
	{
		return this._shortcutIndex[name] || null;
	}
}

/** Prevent keyboard event propagation through the hierarchy. */
Keyboard.stop = function(event)
{
	event._preventKeyboardPropagation = true;
};

/**
 * Rebind one or more shortcuts to new keys.
 * @param {string} newKeys
 * @param {Object|Object[]} shortcuts
 */
Keyboard.rebind = function(newKeys, shortcuts)
{
	(Array.isArray(shortcuts) ? shortcuts : [shortcuts]).forEach(function(shortcut)
	{
		var kb = shortcut.getKeyboard();
		kb.removeEvent(shortcut.keys, shortcut.handler);
		kb.addEvent(newKeys, shortcut.handler);
		shortcut.keys = newKeys;
		kb._fireEvent('rebound');
	});
};

/** Returns all shortcuts for all active keyboards in the hierarchy. */
Keyboard.getActiveShortcuts = function(keyboard)
{
	var result = [];
	Keyboard.each(keyboard, function(kb) { result = result.concat(kb.getShortcuts()); });
	return result;
};

/** Walk the active keyboard chain, calling `fn` for each. */
Keyboard.each = function(keyboard, fn)
{
	var current = keyboard || Keyboard.manager;
	while (current)
	{
		fn(current);
		current = current._activeKB;
	}
};

// ── Root manager setup ─────────────────────────────────────────────────────
// Create the singleton root manager (no parent manager).
Keyboard.manager = null; // set to null before construction so constructor doesn't recurse
Keyboard.manager = new Keyboard({ active: true });

// ── Global key handler ─────────────────────────────────────────────────────
(function()
{
	function globalKeyHandler(event)
	{
		var keys = _modifiers.filter(function(mod) { return event[mod + 'Key'] || event[mod === 'control' ? 'ctrlKey' : mod + 'Key']; });

		// Map native modifier properties
		var mods = [];
		if (event.shiftKey)   mods.push('shift');
		if (event.ctrlKey)    mods.push('control');
		if (event.altKey)     mods.push('alt');
		if (event.metaKey)    mods.push('meta');

		var key = (event.key || '').toLowerCase();
		if (!_modRe.test(key)) mods.push(key);

		var parsedType = event.type + ':' + mods.join('+');
		if (Keyboard.manager) Keyboard.manager._handle(event, parsedType);
	}

	document.addEventListener('keydown', globalKeyHandler);
	document.addEventListener('keyup',   globalKeyHandler);
})();
