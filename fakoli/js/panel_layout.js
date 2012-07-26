var PanelLayout = (function()
{
	var PanelLayoutSingleton = new Class(
	{
		Implements: [Options, Events],
		container: null,
		slots: new Hash({}),
		panels: new Hash({}),
		panelList: [],
		splitters: [],
		disconnected: null,
		
		options: 
		{
			stretch: false,
			replace: false,
			tearoffWidth: 800,
			tearoffHeight: 600,
			tearoff: "tearoff",
			defaultPanel: "",
			onLoad: Class.Empty
		},
		
		initialize: function()
		{
			window.addEvent('resize', function() { this.stretch();}.bind(this));
			this.disconnected = new Element('div');
			this.disconnected.setStyle('display', 'none');			
		},
			
		setup: function(container, options)
		{
			this.container = $(container);
			
			this.setOptions(options);
		},
		
		/**
		 * Registers a slot into the layout
		 */
		addSlot: function(slot)
		{
			slot = $(slot);
			this.slots[slot.id] = slot;
			this.panels[slot.id] = new Hash({});
		},
		
		addSlots: function(expr)
		{
			$$(expr).each(function(slot) { this.addSlot(slot); }.bind(this));
		},
		
		addSplitter: function(splitter)
		{
			this.splitters.push(splitter);
		},
		
		addSplitters: function(expr)
		{
			this.splitters = $$(expr);
		},
		
		disconnect: function()
		{
			this.panelList.each(function(panel)			
			{
				this.disconnected.adopt(panel.getContent());
			}.bind(this));
			this.panels = new Hash({});
		},
		
		reParent: function()
		{
			this.panelList.each(function(panel)
			{
				var slot = this.slots[panel.slotID];
				if (!slot) panel.close();
				this.dock(panel, slot);
			}.bind(this));
			
			this.loadDefaultPanels();
		},
		
		/**
		 * Docks a panel into a slot
		 */
		dock: function(panel, slot)
		{
			slot = $(slot);
			
			slot.adopt(panel.getContent());
	
			this.panelList.push(panel);
			
			if (!this.panels[slot.id]) this.panels[slot.id] = new Hash({});
			
			this.panels[slot.id][panel.id] = panel;
			
			if (this.options.stretch)
			{
				panel.stretch();
			}		
		},
		
		/**
		 * Loads a panel from the specified URL 
		 * and docks it at the given slot.
		 */
		dockAndLoad: function(id, panelURL, slot, options)
		{
			slot = $(slot);
			slot.addClass("background-spinner");
			if (this.options.replace)
			{
				$each(this.panels[slot.id], function(panel, doomedid)
				{	
					panel.close();
					this.panels[slot.id].erase(doomedid);
					this.panelList.erase(panel);// = this.panelList.filter(function(panel) { return panel.id != doomedid; });
				}.bind(this));
			}
				
			var newPanel = new Panel(id, options);
			newPanel.load(panelURL, slot.id, function() { slot.removeClass("background-spinner"); this.fireEvent('load');}.bind(this));
			
			this.dock(newPanel, slot);
			
			return newPanel;
		},
		
		stretch: function()
		{
			this.panelList.each(function(panel) { panel.stretch(); });
		},
		
		findPanel: function(panelID)
		{
			var panel = this.panelList.filter(function(p) {return p.id == panelID; });
			return (panel.length == 0) ? null : panel[0];
		},
		
		closePanel: function(panelID)
		{
			var panel = this.findPanel(panelID);
			if (panel == null) return;

			this.panels[panel.slotID].erase(panel.id);
			
			panel.close();
			this.panelList.erase(panel);
			
			this.loadDefaultPanels();
		},
		
		tearoff: function(panelID)
		{
			var panel = this.findPanel(panelID);
			if (panel == null) return;
			
			var url = "/" + this.options.tearoff + "?uri=" + escape(panel.url);
			if (this.options.defaultPanel) url += "&defaultPanel=" + escape(this.options.defaultPanel);
			
			popup(url, "_blank", this.options.tearoffWidth, this.options.tearoffHeight);
		},
		
		tearoffURL: function(url)
		{
			var url = "/" + this.options.tearoff + "?uri=" + escape(url);
			if (this.options.defaultPanel) url += "&defaultPanel=" + escape(this.options.defaultPanel);
			
			popup(url, "_blank", this.options.tearoffWidth, this.options.tearoffHeight);
		},
		
		calculateLayout: function()
		{
			if (this.splitters.length > 0)
			{
				this.splitters.each(function(s) { s.calculateLayout(); });
			}
			else
			{
				$each(this.slots, function(slot, slotID)
				{
					slot.setStyles({'width': this.container.getWidth(), 'height': this.container.getHeight()});
				}.bind(this));
			}
		},
		
		loadDefaultPanels: function()
		{
			if (!this.options.defaultPanel) return;
			
			$each(this.slots, function(slot, slotID)
			{
				if (this.panels[slotID].getLength() == 0)
				{
					this.dockAndLoad(slotID + "_empty_panel", this.options.defaultPanel, slot, {stretch: this.options.stretch});
				}
			}.bind(this));
		}
	});
	
	var instance;
	return function()
	{
		return instance ? instance : instance = new PanelLayoutSingleton();
	};
	
})();

var Panel = new Class(
{
	Implements: [Options, Events],
	
	id:	"",
	url: "",
	div: null,
	header: null,
	body: null,
	slotID: "",
	options: 
	{ 
		stretch: false, 
		onstretched: Class.Empty,
		onclose: Class.Empty,
		onload: Class.Empty
	},
	
	initialize: function(id, options)
	{
		this.id = id;
		this.setOptions(options);
	},
	
	load: function(panelURL, slotID, loadcallback)
	{		
		if (loadcallback) this.addEvent('load', loadcallback);
		this.url = panelURL;
		if (slotID) this.slotID = slotID;
		
		panelURL = this.addPanelToURL(panelURL);

		this.div = new Element('div', {id: this.id});
		this.div.panel = this;
		
		var request = new Request.HTML(
		{
			evalScripts: false,
			evalResponse: false,
			method: 'get', 
			url: panelURL, 
			onSuccess: function(tree, elements, html, script) 
			{ 
				elements.each(function(elt) { elt.panel = this;}.bind(this));
				this.div.set('text', '');
				this.div.adopt(tree);
				
				this.header = this.div.getElements('.panel_header')[0];
				this.body = this.div.getElements('.panel_body')[0];
				this.stretch();

				$exec(script);
				
				this.fireEvent('load');
			}.bind(this)
		});
		request.send();
	},
	
	getContent: function()
	{
		return this.div;
	},
	
	stretch: function()
	{
		if (!this.options.stretch) return;
		
		var parent = this.div.getParent();
		
		if (!parent) return;
		
		var parentHeight = parent.getHeight() - 2;
		var parentWidth = parent.getWidth() - 2;
		
		if (parentHeight < 0) parentHeight = 0;
		if (parentWidth < 0) parentWidth = 0;
		
		this.div.setStyles({width: parentWidth, height: parentHeight});
		
		if (this.body)
		{
			var headerHeight = this.header ? this.header.getHeight() : 0;
			
			this.body.setStyles({width: parentWidth, height: parentHeight - headerHeight});
		}
		
		this.fireEvent('stretched');
	},
	
	close: function()
	{
		this.fireEvent('close');
		this.div.dispose();
	},
	
	addPanelToURL: function(panelURL)
	{
		if (panelURL.indexOf("?") >= 0)
		{
			panelURL += "&";
		}
		else
		{
			panelURL += "?";
		}
		
		panelURL += "panel=" + this.id;
		
		if (this.slotID)
		{
			panelURL += "&slot=" + this.slotID;
		}
		
		return panelURL;
	},
		
	update: function (panelURL)
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
				
		var request = new Request.HTML(
		{
			evalScripts: false,
			evalResponse: false,
			method: 'get', 
			url: panelURL, 
			onSuccess: function(tree, elements, html, script) 
			{ 
				this.fireEvent('close');
				elements.each(function(elt) { elt.panel = this;}.bind(this));
				this.div.set('text', '');
				this.div.adopt(tree);
				
				this.header = this.div.getElements('.panel_header')[0];
				this.body = this.div.getElements('.panel_body')[0];
				this.stretch();

				$exec(script);
				
				this.fireEvent('load');
			}.bind(this)
		});
		request.send();
	}
});