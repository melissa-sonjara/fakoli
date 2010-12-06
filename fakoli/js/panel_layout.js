var PanelLayout = new Class(
{	
	Implements: [Options],
	container: null,
	slots: {},
	panels: new Hash({}),
	panelList: [],
	
	options: 
	{
		stretch: false,
		replace: false
	},
	
	initialize: function(container, options)
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
		if (this.options.replace)
		{
			$each(this.panels[slot.id], function(panel, doomedid)
			{	
				panel.close();
				this.panels[slot.id].erase(doomedid);
				this.panelList.erase(panel);// = this.panelList.filter(function(panel) { return panel.id != doomedid; });
			}.bind(this));
		}
			
		panel = new Panel(id, options);
		panel.load(panelURL);
		
		this.dock(panel, slot);
	},
	
	stretch: function()
	{
		this.panelList.each(function(panel) { panel.stretch(); });
	}
});

var Panel = new Class(
{
	Implements: [Options, Events],
	
	id:	"",
	url: "",
	div: null,
	options: 
	{ 
		stretch: false, 
		onstretched: Class.Empty,
		onclose: Class.Empty
	},
	
	initialize: function(id, options)
	{
		this.id = id;
		this.setOptions(options);
	},
	
	load: function(panelURL)
	{
		this.url = panelURL;
		this.div = new Element('div', {id: this.id});
		this.div.panel = this;
		this.div.set('text', "Loading...");
		
		var request = new Request.HTML(
		{
			evalScripts: false,
			evalResponse: false,
			method: 'get', 
			url: panelURL, 
			onSuccess: function(tree, elements, html, script) 
			{ 
				this.div.set('text', '');
				this.div.adopt(tree);
				$exec(script);
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
		this.div.setStyles({width: parent.getWidth() - 2, height: parent.getHeight() - 2});
		this.fireEvent('stretched');
	},
	
	close: function()
	{
		this.fireEvent('close');
		this.div.dispose();
	}
});