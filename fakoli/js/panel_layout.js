var PanelLayout = (function()
{
	var PanelLayoutSingleton = new Class(
	{
		Implements: [Options],
		container: null,
		slots: {},
		panels: new Hash({}),
		panelList: [],
		
		options: 
		{
			stretch: false,
			replace: false,
			tearoffWidth: 800,
			tearoffHeight: 600
		},
		
		initialize: function()
		{
			window.addEvent('resize', function() { this.stretch();}.bind(this));
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
		
		/**
		 * Docks a panel into a slot
		 */
		dock: function(panel, slot)
		{
			slot = $(slot);
			
			slot.adopt(panel.getContent());
	
			this.panelList.push(panel);
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
			newPanel.load(panelURL, slot.id, function() { slot.removeClass("background-spinner");});
			
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

			panel.close();
			this.panelList.erase(panel);
		},
		
		tearoff: function(panelID)
		{
			var panel = this.findPanel(panelID);
			if (panel == null) return;
			
			popup("/tearoff?uri=" + escape(panel.url), "_blank", this.options.tearoffWidth, this.options.tearoffHeight);
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

		if (panelURL.indexOf("?") >= 0)
		{
			panelURL += "&";
		}
		else
		{
			panelURL += "?";
		}
		
		panelURL += "panel=" + this.id;
		
		if (slotID)
		{
			panelURL += "&slot=" + slotID;
		}
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
				
				this.header = $$('.panel_header')[0];
				this.body = $$('.panel_body')[0];
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
		var parentHeight = parent.getHeight();
		var parentWidth = parent.getWidth();
		
		this.div.setStyles({width: parentWidth - 2, height: parentHeight - 2});
		
		if (this.body)
		{
			var headerHeight = this.header ? this.header.getHeight() : 0;
			
			this.body.setStyles({width: parentWidth - 2, height: parentHeight - headerHeight - 2});
		}
		
		this.fireEvent('stretched');
	},
	
	close: function()
	{
		this.fireEvent('close');
		this.div.dispose();
	}
});