var Chart = new Class(
{
	Implements: [Options, Events],
	id:			"",
	container:	Class.Empty,
	paper: 		Class.Empty,
	palette:	Class.Empty,
	saveIcon:	Class.Empty,
	
	initialize: function(id)
	{
		this.id = id;
		this.container = document.id(id);
		this.container.chart = this;
	},
	
	createChart: function()
	{
		var ratio = this.options.height / this.options.width;
		var size = this.container.getSize();
		if (size.y == 0)
		{
			size.y = size.x * ratio;
			this.container.set('height', size.y);
			window.addEvent('resize', function(e)
			{
				var size = this.container.getSize();
				size.y = size.x * ratio;
				this.container.set('height', size.y);
				this.paper.setSize(size.x, size.y);
			}.bind(this));
		}

		this.paper = Raphael(this.container.id, size.x, size.y);
		this.paper.setViewBox(0, 0, this.options.width, this.options.height, true);
		this.palette = Palette.palettes[this.options.palette];
	},
	
	draw: function()
	{
		this.drawChart();		
		
		if (this.testSVG())
		{
			this.saveIcon = new Element("img", {src: "/components/svg_charts/images/save_icon.png", 
												alt: "Save Chart as PNG",
												title: "Save Chart as PNG",
												id:  this.id + "_saveIcon"});
			this.saveIcon.setStyles({'cursor': 'pointer', 'opacity': 0});
			
			this.container.adopt(this.saveIcon);
			this.saveIcon.position({relativeTo: this.container, position: 'topRight', edge: 'topRight'});
			this.saveIcon.addEvent('click', function() { this.saveImage(); }.bind(this));
			this.saveIcon.addEvent('mouseover', function(){ this.saveIcon.set('src', "/components/svg_charts/images/save_icon_hover.png"); }.bind(this));
			this.saveIcon.addEvent('mouseout', function(){ this.saveIcon.set('src', "/components/svg_charts/images/save_icon.png"); }.bind(this));
			this.container.addEvent('mouseover', function() { this.saveIcon.position({relativeTo: this.container, position: 'topRight', edge: 'topRight'}); this.saveIcon.fade('in');}.bind(this));
			this.container.addEvent('mouseout', function() { this.saveIcon.fade('out');}.bind(this));
		}
	},
	
	drawLegend: function()
	{
		if (this.options.legend)
		{
			var x = this.options.legendX;
			var y = this.options.legendY;
			var s = this.options.legendSwatchSize || 20;
			var h = this.options.legendLineHeight || 30;
			
			this.labels.each(function(text, index)
			{
				var rect = this.paper.rect(x, y, s, s, 3);
				rect.attr({fill:this.palette.swatches[index], stroke: this.palette.strokeColor, "stroke-width": this.options.strokeWidth});
				
				var text = this.paper.text(x + h, y + 10, text);
				text.attr({"text-anchor": "start", fill: this.palette.strokeColor, stroke: "none" , opacity: 1, "font-size": this.options.labelSize});
					
				rect.mouseover(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				rect.mousemove(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				rect.mouseout(function(e) { this.fireEvent('legendOut', [e, index]); }.bind(this));
				
				text.mouseover(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				text.mousemove(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				text.mouseout(function(e) { this.fireEvent('legendOut', [e, index]); }.bind(this));

				y+= h;
			}.bind(this));
		}
	},
	
	testSVG: function() 
	{
	    return !!document.createElementNS && !! document.createElementNS('http://www.w3.org/2000/svg', 'svg').createSVGRect;
	},
	
	saveImage: function()
	{	
		if (!this.canvas)
		{
			this.canvas = new Element("canvas", {id: this.id + "_canvas"});
			this.canvas.setStyles({width: this.container.getWidth(), height: this.container.getHeight(), display: 'none'});
			document.body.adopt(this.canvas);
		}
		
		var s = this.container.getElement('svg').clone();
		var tmp = new Element('div');
		tmp.adopt(s);
		svg = tmp.get('html');
		tmp.destroy();
		
	    canvg(this.canvas.id, svg, {renderCallback: this.saveImageCallback.bind(this), ignoreMouse: true, ignoreAnimation: true});
	},
	
	saveImageCallback: function()
	{
		if (!this.form)
		{
			this.form = new Element("form", {method: 'post', action: '/action/svg_charts/save_image', display: 'none'});
			var input = new Element("input", {type: 'hidden', name: 'img', value: ''});
			var filename = new Element("input", {type: 'hidden', name: 'filename', value: this.id});
			
			this.form.adopt(input);
			this.form.adopt(filename);
			document.body.adopt(this.form);
		}
		
		var img = document.getElementById(this.id + "_canvas").toDataURL("image/png");
	    this.form["img"].value = img;
	    this.form.submit();
	}

});