var ColorPicker = new Class(
{
	Implements: [Options],
	
	valueField: null,
	button:    	null,
	panel:		null,
	options:
	{
		defaultColor: "yellow",
		standardColors: ["white", "black", "yellow", "yellowgreen", "green", "red", "orange", "royalblue", "cyan", "magenta"],
		shades: ["#000", "#ffff00", "#228B22", "#00008b", "#8B0000", "#8B008B", "#ff69b4", "#008b8b", "#6b8e23", "#A0522D"],
		width: "auto",
		height: "auto",
		trigger: "click"
	},
	
	
	initialize: function(valueField, button, options)
	{
		this.valueField = document.id(valueField);
		this.button = document.id(button);

		this.setOptions(options);
		
		if (!this.valueField.value) this.valueField.value = this.options.defaultColor;

		this.buildPanel();
	
		this.button.addEvent(this.options.trigger, function(e) { new Event(e).stop(); this.showPanel(); }.bind(this));		
	},
	
	buildPanel: function()
	{
		var doc = document.id(document.body ? document.body : document.documentElement);
				
		this.panel = new Element('div', {'class': 'color_picker'});
		this.panel.setStyles({'width': this.options.width, 
							  'height': this.options.height,
							  'position': 'absolute',
							  'display': 'none'});
		this.panel.inject(doc);
		
		this.noneDiv = new Element('div');
		this.noneDiv.setStyle('margin', '4px');
		this.none = new Element('table');
		this.none.setStyle('display', 'inline');
		this.noneBody = new Element('tbody');
		this.addColorRow(this.noneBody, ["transparent"]);
		this.noneLabel = new Element("span", {'text': "No Highlight"});
		
		this.noneBody.inject(this.none);
		this.noneLabel.inject(this.noneDiv);
		this.none.inject(this.noneDiv);
		this.noneDiv.inject(this.panel);
		
		this.standard = new Element('table');
		this.standardBody = new Element('tbody');
		this.standardBody.inject(this.standard);
		this.addColorRow(this.standardBody, this.options.standardColors);
		
		this.standard.inject(this.panel);
		
		this.palette = new Element('table');
		this.paletteBody = new Element('tbody');
		this.paletteBody.inject(this.palette);
		
		for(var brightness = 80; brightness >= 0; brightness -= 20)
		{
			var colors = [];
			this.options.shades.each(function(shade)
			{
				var color = new Color(shade);
				var color = color.mix("#fff", brightness);
				colors.push(color);
			});
			
			this.addColorRow(this.paletteBody, colors);
		}
		
		this.palette.inject(this.panel);
		
		this.swatch = new Element('div', {'class': 'color_swatch'});
		this.swatch.set('html', "&nbsp");
		this.button.set('html', '');
		this.button.adopt(this.swatch);
		
		this.updateSwatch();
		doc.addEvent('click', function(e) { this.panel.hide(); }.bind(this));
	},
	
	addColorRow: function(body, colors)
	{
		var tr = new Element('tr');
		var me = this;
		
		colors.each(function(color)
		{
			var td = new Element('td');
			td.set('html', '&nbsp;');
			td.setStyle('background-color', color);
			td.addEvents({'mouseenter': function(e) { td.addClass('hover'); },
						  'mouseleave': function(e) { td.removeClass('hover'); },
						  'click': function(e) { new Event(e).stop(); me.selectColor(td.getStyle('background-color')); }
						  });			
			tr.adopt(td);
		});
		
		body.adopt(tr);
	},
	
	showPanel: function()
	{
		var coords = this.button.getCoordinates();
		this.panel.setStyles({'top': coords.bottom, 
							  'left': coords.left,
							  'display': 'block'});
		this.panel.fade('in');
	},
	
	updateSwatch: function()
	{
		this.swatch.setStyle('background-color', this.valueField.value);
	},
	
	selectColor: function(color)
	{
		this.valueField.value = color;
		this.updateSwatch();
		this.panel.fade('out');
	}
});