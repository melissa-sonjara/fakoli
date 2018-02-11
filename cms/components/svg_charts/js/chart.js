var Chart = new Class(
{
	Implements: [Options, Events],
	id:			"",
	container:	Class.Empty,
	paper: 		Class.Empty,
	palette:	Class.Empty,
	saveIcon:	Class.Empty,
	
	options:
	{
		caption: '',
		captionTop: 400,
		captionLeft: 10,
		captionWidth: 580,
		captionAttributes: {},
		exportStyles: []
	},

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
		this.drawCaption();
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
	
	drawCaption: function()
	{
		if (this.options.caption == '') return;
		this.caption = this.paper.multitext(this.options.captionX, this.options.captionY, this.options.caption, 
											this.options.captionWidth, this.options.captionAttributes);
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
	
	svgDocType: '<?xml version="1.0" encoding="utf-8" standalone="no"?><!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">',
	   
	inlineStyles: 
	[
		"* { font-family: 'Arial', 'Helvetica', 'sans-serif'}"
	],
		
	massageSVG: function(data)
	{
		var tag = data.match(/<svg.*?>/m);
		var fixed = tag[0];
		
		if (!fixed.includes("version"))
		{
			fixed = fixed.replace("<svg", "<svg version='1.1'");
		}

		if (!fixed.includes("xmlns="))
		{
			fixed = fixed.replace("<svg", '<svg xmlns="http://www.w3.org/2000/svg"');
		}
		
		if (!fixed.includes("xmlns:xlink"))
		{
			fixed = fixed.replace("<svg", '<svg xmlns:xlink="http://www.w3.org/1999/xlink"');
		}

		data = data.replace(tag[0], fixed);
		
		var inlineStyles = this.inlineStyles.concat(this.options.exportStyles);
		
		data = data.replace("</defs>", "</defs><style type='text/css'>" + inlineStyles.join("\r\n") + "</style>");

		data = this.svgDocType + data;
		return data;
	},
	
	generateSVGDataURL: function()
	{
		var s = this.container.getElement('svg').clone();
		var tmp = new Element('div');
		tmp.adopt(s);
		var data = tmp.get('html');
		tmp.destroy();
		
		data = this.massageSVG(data);
		
		var svg = 'data:image/svg+xml;base64,' + btoa(data);
		return svg;
	},

	generateSVGDataURLFixedSize: function(w, h)
	{
		var s = this.container.getElement('svg').clone();
		var tmp = new Element('div');
		tmp.adopt(s);
		var data = tmp.get('html');
		tmp.destroy();
		
		var tag = data.match(/<svg.*?>/m);
		var fixed = tag[0].replace('width="100%"', 'width="' + w + '"');
		fixed = fixed.replace('height="100%"', 'height="' + h + '"');
		data = data.replace(tag[0], fixed);

		data = this.massageSVG(data);
		var svg = 'data:image/svg+xml;base64,' + btoa(data);
		return svg;
	},
	
	saveSVG: function()
	{
		var svg = this.generateSVGDataURL();
		if (this.form) this.form.dispose();
		
		this.form = new Element("form", {method: 'post', action: '/action/svg_charts/save_svg', display: 'none'});
		var input = new Element("input", {type: 'hidden', name: 'img', value: ""});
		var filename = new Element("input", {type: 'hidden', name: 'filename', value: this.id});
				
		this.form.adopt(input);
		this.form.adopt(filename);
		document.body.adopt(this.form);
		
		this.form["img"].value = svg;
		this.form.submit();
	},

	saveImage: function()
	{	
		var w = this.container.getWidth();
		var h = this.container.getHeight();

		w = Math.floor(w+.5);
		h = Math.floor(h+.5);
		
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
		
		var svg = this.generateSVGDataURLFixedSize(w, h);
		//var svg = this.generateSVGDataURL();
		
		var DOMURL = window.URL || window.webkitURL || window;

		var img = new Image(w, h);

		img.onload = function() 
		{
		    ctx.drawImage(img, 0, 0);
		  
			if (this.form) this.form.dispose();
		
			this.form = new Element("form", {method: 'post', action: '/action/svg_charts/save_image', display: 'none'});
			var input = new Element("input", {type: 'hidden', name: 'img', value: ''});
			var filename = new Element("input", {type: 'hidden', name: 'filename', value: this.id});
				
			this.form.adopt(input);
			this.form.adopt(filename);
			document.body.adopt(this.form);
			
			var output = document.getElementById(this.id + "_canvas").toDataURL("image/png");
		    this.form["img"].value = output;
		    this.form.submit();
		}.bind(this);

		img.onerror = function() 
		{ 
			alert("PNG export failed");
		};
		
		img.src = svg;
	},
	
	path: function()
	{
		var d = [];
		var step = 0;
		for(step = 0; step < arguments.length; ++step)
		{
			if (arguments[step] == '') continue;
			step = (typeOf(arguments[step]) == "array") ? arguments[step] : [arguments[step]];
			params = step.slice(1);
			d.push(step[0] + params.join(','));
		}
		return d.join(' ');
	}

});