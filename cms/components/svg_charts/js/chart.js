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
	
	svgid: function()
	{
		return this.id + "_svg";
	},
	
	getFont: function()
	{
		var font = this.options.fontFamily;
		if (!font) font = "Arial";
		return font;
	},
	
	createChart: function()
	{		
		this.container.set('html', '<svg id="' + this.svgid() + '" xmlns:xlink="http://www.w3.org/1999/xlink" width="100%" height="100%"></svg>');
		
		this.paper = Snap("#" + this.svgid());
		this.paper.attr({viewBox: "0 0 " +this.options.width + " " + this.options.height});
		this.palette = Palette.palettes[this.options.palette];
		this.container.chart = this;
	},
	
	draw: function()
	{
		this.fireEvent('drawChart', this);
		this.drawChart();		
		this.fireEvent('drawChartComplete', this);
		
		if (this.testSVG() && this.options.enableDownload)
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
		this.fireEvent('drawLegend', this);
		
		if (this.options.legend)
		{
			this.fireEvent('drawLegend', this);
			
			var x = this.options.legendX;
			var y = this.options.legendY;
			var s = this.options.legendSwatchSize || 20;
			var h = this.options.legendLineHeight || 30;
			
			var font = this.getFont();
			
			this.labels.each(function(text, index)
			{
				var cl = this.options.selectable ? "selectable" : "";
				
				var rect = this.paper.rect(x, y, s, s, 3);
				rect.attr({fill:this.palette.swatches[index], stroke: this.palette.strokeColor, "stroke-width": this.options.strokeWidth, "class": cl});
				
				var text = this.paper.text(x + h, y + 10, text);
				text.attr({"text-anchor": "start", fill: this.palette.strokeColor, stroke: "none" , opacity: 1, "font-size": this.options.labelSize, "font-family": font, "class": cl});
					
				rect.mouseover(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				rect.mousemove(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				rect.mouseout(function(e) { this.fireEvent('legendOut', [e, index]); }.bind(this));
				rect.click(function(e) { this.fireEvent('legendClick', [e, index]); }.bind(this));
				
				text.mouseover(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				text.mousemove(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				text.mouseout(function(e) { this.fireEvent('legendOut', [e, index]); }.bind(this));
				text.click(function(e) { this.fireEvent('legendClick', [e, index]); }.bind(this));
				
				y+= h;
			}.bind(this));
			
			this.fireEvent('drawLegendComplete', this);
		}
	},
	
	testSVG: function() 
	{
	    return !!document.createElementNS && !! document.createElementNS('http://www.w3.org/2000/svg', 'svg').createSVGRect;
	},

	generateSVGDataURL: function()
	{
		var s = this.container.getElement('svg').clone();
		var tmp = new Element('div');
		tmp.adopt(s);
		var data = tmp.get('html');
		data = data.replace('xmlns:xlink="http://www.w3.org/1999/xlink"', 'xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"');
		data = data.replace("</defs>", "</defs><style>* { font-family: 'Arial'}</style>");
		tmp.destroy();
		var svg = 'data:image/svg+xml;base64,' + btoa(data);
		return svg;
	},
	
	saveSVG: function()
	{
		var svg = this.generateSVGDataURL();
		if (!this.form)
 		{
			this.form = new Element("form", {method: 'post', action: '/action/svg_charts/save_image', display: 'none'});
			var input = new Element("input", {type: 'hidden', name: 'img', value: ''});
			var filename = new Element("input", {type: 'hidden', name: 'filename', value: this.id});
				
			this.form.adopt(input);
			this.form.adopt(filename);
			document.body.adopt(this.form);
		}
			
		var output = document.getElementById(this.id + "_canvas").toDataURL("image/png");
		this.form["img"].value = output;
		this.form.submit();
	},

	saveImage: function()
	{	
		var w = this.container.getWidth();
		var h = this.container.getHeight();
		
		if (!this.canvas)
		{
			this.canvas = new Element("canvas", {id: this.id + "_canvas"});
			this.canvas.setStyles({width: w, height: h, display: 'none'});
			this.canvas.width = w;
			this.canvas.height = h;
			this.container.adopt(this.canvas);
		}
		
		var showBg = false;

		var ctx = this.canvas.getContext("2d");
		
		if (this.options.canvasBackground)
		{
			ctx.fillStyle = this.options.canvasBackground;
			ctx.fillRect(0, 0, w, h);
			showBg = true;
		}
		
		var svg = this.generateSVGDataURL();
		
		var DOMURL = window.URL || window.webkitURL || window;

		var img = new Image();

		img.onload = function() {
		  ctx.drawImage(img, 0, 0);
		  
		  if (!this.form)
  		  {
				this.form = new Element("form", {method: 'post', action: '/action/svg_charts/save_image', display: 'none'});
				var input = new Element("input", {type: 'hidden', name: 'img', value: ''});
				var filename = new Element("input", {type: 'hidden', name: 'filename', value: this.id});
				
				this.form.adopt(input);
				this.form.adopt(filename);
				document.body.adopt(this.form);
			}
			
			var output = document.getElementById(this.id + "_canvas").toDataURL("image/png");
		    this.form["img"].value = output;
		    this.form.submit();
		}.bind(this);

		//this.container.adopt(img);
		img.src = svg;

//	    canvg(this.canvas.id, svg, {renderCallback: this.saveImageCallback.bind(this), 
//	    							ignoreMouse: true, 
//	    							ignoreAnimation: true, 
//	    							ignoreDimensions: true,
//	    							offsetX: 0,
//	    							offsetY: 0,
//	    							ignoreClear: showBg});
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
	},
	
	path: function()
	{
		var d = [];
		for(var step in arguments)
		{
			if (arguments[step] == '') continue;
			step = (typeOf(arguments[step]) == "array") ? arguments[step] : [arguments[step]];
			params = step.slice(1);
			d.push(step[0] + params.join(','));
		}
		return d.join(' ');
	}

});