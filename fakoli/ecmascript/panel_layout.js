// Requires ui.js for EventEmitter, $el, fetchHTML, execScripts, popup.

var PanelLayout = (function()
{
	/**
	 * Singleton managing a multi-panel layout: tracks slots, docks/closes panels,
	 * handles tearoff popups, and coordinates splitter layout recalculation.
	 */
	class PanelLayoutSingleton extends EventEmitter
	{
		constructor()
		{
			super();
			this.container   = null;
			this.slots       = {};
			this.panels      = {};
			this.panelList   = [];
			this.splitters   = [];
			this.disconnected = null;
			this.options = {
				stretch:       false,
				replace:       false,
				tearoffWidth:  800,
				tearoffHeight: 600,
				tearoff:       "tearoff",
				defaultPanel:  "",
				onLoad:        function() {}
			};

			window.addEventListener('resize', function() { this.stretch(); }.bind(this));
			this.disconnected = document.createElement('div');
			this.disconnected.style.display = 'none';
		}

		setup(container, options)
		{
			this.container = $el(container);
			Object.assign(this.options, options || {});
		}

		addSlot(slot)
		{
			slot = $el(slot);
			this.slots[slot.id]  = slot;
			this.panels[slot.id] = {};
		}

		addSlots(expr)
		{
			var self = this;
			document.querySelectorAll(expr).forEach(function(slot) { self.addSlot(slot); });
		}

		addSplitter(splitter)  { this.splitters.push(splitter); }

		addSplitters(expr)
		{
			this.splitters = Array.from(document.querySelectorAll(expr));
		}

		disconnect()
		{
			var self = this;
			this.panelList.forEach(function(panel)
			{
				self.disconnected.appendChild(panel.getContent());
			});
			this.panels = {};
		}

		reParent()
		{
			var self = this;
			this.panelList.forEach(function(panel)
			{
				var slot = self.slots[panel.slotID];
				if (!slot) { panel.close(); return; }
				self.dock(panel, slot);
			});
			this.loadDefaultPanels();
		}

		dock(panel, slot)
		{
			slot = $el(slot);
			slot.appendChild(panel.getContent());

			this.panelList.push(panel);

			if (!this.panels[slot.id]) this.panels[slot.id] = {};
			this.panels[slot.id][panel.id] = panel;

			if (this.options.stretch) panel.stretch();
		}

		dockAndLoad(id, panelURL, slot, options)
		{
			slot = $el(slot);
			slot.classList.add("background-spinner");

			if (this.options.replace)
			{
				var self = this;
				Object.keys(this.panels[slot.id] || {}).forEach(function(doomedid)
				{
					var panel = self.panels[slot.id][doomedid];
					panel.close();
					delete self.panels[slot.id][doomedid];
					self.panelList = self.panelList.filter(function(p) { return p !== panel; });
				});
			}

			var newPanel = new Panel(id, options);
			var self = this;
			newPanel.load(panelURL, slot.id, function()
			{
				slot.classList.remove("background-spinner");
				self.fireEvent('load');
			});

			this.dock(newPanel, slot);
			return newPanel;
		}

		stretch()
		{
			this.panelList.forEach(function(panel) { panel.stretch(); });
		}

		findPanel(panelID)
		{
			var found = this.panelList.filter(function(p) { return p.id == panelID; });
			return (found.length == 0) ? null : found[0];
		}

		closePanel(panelID)
		{
			var panel = this.findPanel(panelID);
			if (panel == null) return;

			delete this.panels[panel.slotID][panel.id];
			panel.close();
			this.panelList = this.panelList.filter(function(p) { return p !== panel; });

			this.loadDefaultPanels();
		}

		tearoff(panelID)
		{
			var panel = this.findPanel(panelID);
			if (panel == null) return;
			var url = "/" + this.options.tearoff + "?uri=" + encodeURIComponent(panel.url);
			if (this.options.defaultPanel) url += "&defaultPanel=" + encodeURIComponent(this.options.defaultPanel);
			popup(url, "_blank", this.options.tearoffWidth, this.options.tearoffHeight);
		}

		tearoffURL(url)
		{
			var dest = "/" + this.options.tearoff + "?uri=" + encodeURIComponent(url);
			if (this.options.defaultPanel) dest += "&defaultPanel=" + encodeURIComponent(this.options.defaultPanel);
			popup(dest, "_blank", this.options.tearoffWidth, this.options.tearoffHeight);
		}

		calculateLayout()
		{
			var self = this;
			if (this.splitters.length > 0)
			{
				this.splitters.forEach(function(s) { s.calculateLayout(); });
			}
			else
			{
				Object.values(this.slots).forEach(function(slot)
				{
					setStyles(slot, {
						width:  self.container.offsetWidth  + 'px',
						height: self.container.offsetHeight + 'px'
					});
				});
			}
		}

		loadDefaultPanels()
		{
			if (!this.options.defaultPanel) return;
			var self = this;
			Object.keys(this.slots).forEach(function(slotID)
			{
				var slot = self.slots[slotID];
				if (Object.keys(self.panels[slotID] || {}).length == 0)
				{
					self.dockAndLoad(slotID + "_empty_panel", self.options.defaultPanel, slot, { stretch: self.options.stretch });
				}
			});
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new PanelLayoutSingleton();
	};
})();


/**
 * Represents a single content panel that loads its HTML from a URL,
 * supports stretching to fill its slot, and fires load/close/stretched events.
 */
class Panel extends EventEmitter
{
	constructor(id, options)
	{
		super();
		this.id     = id;
		this.url    = "";
		this.div    = null;
		this.header = null;
		this.body   = null;
		this.slotID = "";
		this.options = Object.assign({
			stretch:    false,
			onstretched: function() {},
			onclose:    function() {},
			onload:     function() {}
		}, options || {});
	}

	load(panelURL, slotID, loadcallback)
	{
		if (loadcallback) this.addEvent('load', loadcallback);
		this.url = panelURL;
		if (slotID) this.slotID = slotID;

		panelURL = this.addPanelToURL(panelURL);

		this.div      = document.createElement('div');
		this.div.id   = this.id;
		this.div.panel = this;

		var self = this;
		fetchHTML(panelURL, function(html, scripts)
		{
			self.div.innerHTML = html;
			Array.from(self.div.querySelectorAll('*')).forEach(function(elt) { elt.panel = self; });

			self.header = self.div.querySelectorAll('.panel_header')[0] || null;
			self.body   = self.div.querySelectorAll('.panel_body')[0]   || null;
			self.stretch();

			execScripts(scripts);
			self.fireEvent('load');
		});
	}

	getContent() { return this.div; }

	stretch()
	{
		if (!this.options.stretch) return;

		var parent = this.div ? this.div.parentElement : null;
		if (!parent) return;

		var parentHeight = Math.max(0, parent.offsetHeight - 2);
		var parentWidth  = Math.max(0, parent.offsetWidth  - 2);

		setStyles(this.div, { width: parentWidth + 'px', height: parentHeight + 'px' });

		if (this.body)
		{
			var headerHeight = this.header ? this.header.offsetHeight : 0;
			setStyles(this.body, { width: parentWidth + 'px', height: (parentHeight - headerHeight) + 'px' });
		}

		this.fireEvent('stretched');
	}

	close()
	{
		this.fireEvent('close');
		if (this.div && this.div.parentNode) this.div.parentNode.removeChild(this.div);
	}

	addPanelToURL(panelURL)
	{
		panelURL += (panelURL.indexOf("?") >= 0) ? "&" : "?";
		panelURL += "panel=" + this.id;
		if (this.slotID) panelURL += "&slot=" + this.slotID;
		return panelURL;
	}

	update(panelURL)
	{
		if (!panelURL)
		{
			panelURL = this.url;
		}
		else
		{
			this.url = panelURL;
		}

		panelURL = this.addPanelToURL(panelURL);

		var self = this;
		fetchHTML(panelURL, function(html, scripts)
		{
			self.fireEvent('close');
			self.div.innerHTML = html;
			Array.from(self.div.querySelectorAll('*')).forEach(function(elt) { elt.panel = self; });

			self.header = self.div.querySelectorAll('.panel_header')[0] || null;
			self.body   = self.div.querySelectorAll('.panel_body')[0]   || null;
			self.stretch();

			execScripts(scripts);
			self.fireEvent('load');
		});
	}
}
