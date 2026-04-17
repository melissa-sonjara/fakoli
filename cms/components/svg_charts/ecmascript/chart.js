class Chart
{
	constructor(id)
	{
		this.id = id;
		this.container = document.getElementById(id);
		this.container.chart = this;

		this.paper = null;
		this.palette = null;
		this.saveIcon = null;

		this.options = Object.assign(
		{
			caption: '',
			captionTop: 400,
			captionLeft: 10,
			captionWidth: 580,
			captionAttributes: {},
			exportStyles: [],
			imageOutputScale: 2
		}, this.options || {});

		this._events = {};
	}

	addEvent(type, fn)
	{
		if (!this._events[type]) this._events[type] = [];
		this._events[type].push(fn);
		return this;
	}

	fireEvent(type, args)
	{
		if (!this._events[type]) return;
		var arr = Array.isArray(args) ? args : (args !== undefined ? [args] : []);
		this._events[type].forEach(function(fn) { fn.apply(this, arr); }.bind(this));
	}

	svgid()
	{
		return this.id + "_svg";
	}

	getFont()
	{
		var font = this.options.fontFamily;
		if (!font) font = "Arial";
		return font;
	}

	createChart()
	{
		this.container.innerHTML = '<svg id="' + this.svgid() + '" xmlns:xlink="http://www.w3.org/1999/xlink" width="100%" height="100%"></svg>';

		this.paper = Snap("#" + this.svgid());
		this.paper.attr({viewBox: "0 0 " +this.options.width + " " + this.options.height});
		this.palette = Palette.palettes[this.options.palette];
		this.container.chart = this;
	}

	draw()
	{
		this.fireEvent('drawChart', this);
		this.drawChart();
		this.drawCaption();
		this.fireEvent('drawChartComplete', this);

		if (this.testSVG() && this.options.enableDownload)
		{
			this.saveIcon = document.createElement("img");
			this.saveIcon.src = "/components/svg_charts/images/save_icon.png";
			this.saveIcon.alt = "Save Chart as PNG";
			this.saveIcon.title = "Save Chart as PNG";
			this.saveIcon.id = this.id + "_saveIcon";
			Object.assign(this.saveIcon.style, {cursor: 'pointer', opacity: 0});

			this.container.appendChild(this.saveIcon);

			this.saveIcon.addEventListener('click', function() { this.saveImage(); }.bind(this));
			this.saveIcon.addEventListener('mouseover', function(){ this.saveIcon.src = "/components/svg_charts/images/save_icon_hover.png"; }.bind(this));
			this.saveIcon.addEventListener('mouseout', function(){ this.saveIcon.src = "/components/svg_charts/images/save_icon.png"; }.bind(this));
			this.container.addEventListener('mouseover', function()
			{
				this.saveIcon.style.opacity = 1;
			}.bind(this));
			this.container.addEventListener('mouseout', function()
			{
				this.saveIcon.style.opacity = 0;
			}.bind(this));
		}
	}

	drawCaption()
	{
		if (this.options.caption == '') return;
		this.caption = this.paper.multitext(this.options.captionX, this.options.captionY, this.options.caption,
											this.options.captionWidth, this.options.captionAttributes);
	}

	drawLegend()
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

			this.labels.forEach(function(text, index)
			{
				var cl = this.options.selectable ? "selectable" : "";

				var rect = this.paper.rect(x, y, s, s, 3);
				rect.attr({fill:this.palette.swatches[index], stroke: this.palette.strokeColor, "stroke-width": this.options.strokeWidth, "class": cl});

				var labelText = this.paper.text(x + h, y + 10, text);
				labelText.attr({"text-anchor": "start", fill: this.palette.strokeColor, stroke: "none" , opacity: 1, "font-size": this.options.labelSize, "font-family": font, "class": cl});

				rect.mouseover(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				rect.mousemove(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				rect.mouseout(function(e) { this.fireEvent('legendOut', [e, index]); }.bind(this));
				rect.click(function(e) { this.fireEvent('legendClick', [e, index]); }.bind(this));

				labelText.mouseover(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				labelText.mousemove(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				labelText.mouseout(function(e) { this.fireEvent('legendOut', [e, index]); }.bind(this));
				labelText.click(function(e) { this.fireEvent('legendClick', [e, index]); }.bind(this));

				y+= h;
			}.bind(this));

			this.fireEvent('drawLegendComplete', this);
		}
	}

	testSVG()
	{
	    return !!document.createElementNS && !! document.createElementNS('http://www.w3.org/2000/svg', 'svg').createSVGRect;
	}

	massageSVG(data)
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
	}

	generateSVGDataURL()
	{
		var s = this.container.querySelector('svg').cloneNode(true);
		var tmp = document.createElement('div');
		tmp.appendChild(s);
		var data = tmp.innerHTML;
		tmp.remove();

		data = this.massageSVG(data);

		var svg = 'data:image/svg+xml;base64,' + btoa(unescape(encodeURIComponent(data)));
		return svg;
	}

	generateSVGDataURLFixedSize(w, h)
	{
		var s = this.container.querySelector('svg').cloneNode(true);
		var tmp = document.createElement('div');
		tmp.appendChild(s);
		var data = tmp.innerHTML;
		tmp.remove();

		var tag = data.match(/<svg.*?>/m);
		var fixed = tag[0].replace('width="100%"', 'width="' + w + '"');
		fixed = fixed.replace('height="100%"', 'height="' + h + '"');
		data = data.replace(tag[0], fixed);

		data = this.massageSVG(data);
		var svg = 'data:image/svg+xml;base64,' + btoa(unescape(encodeURIComponent(data)));
		return svg;
	}

	saveSVG()
	{
		var svg = this.generateSVGDataURL();
		if (this.form) this.form.remove();

		this.form = document.createElement("form");
		this.form.method = 'post';
		this.form.action = '/action/svg_charts/save_svg';
		this.form.style.display = 'none';
		var input = document.createElement("input");
		input.type = 'hidden';
		input.name = 'img';
		input.value = "";
		var filename = document.createElement("input");
		filename.type = 'hidden';
		filename.name = 'filename';
		filename.value = this.id;

		this.form.appendChild(input);
		this.form.appendChild(filename);
		document.body.appendChild(this.form);

		this.form["img"].value = svg;
		this.form.submit();
	}

	saveImage()
	{
		var w = this.container.offsetWidth;
		var h = this.container.offsetHeight;

		w = Math.floor(w+.5) * this.options.imageOutputScale;
		h = Math.floor(h+.5) * this.options.imageOutputScale;

		if (!this.canvas)
		{
			this.canvas = document.createElement("canvas");
			this.canvas.id = this.id + "_canvas";
			Object.assign(this.canvas.style, {width: w, height: h, display: 'none'});
			this.canvas.width = w;
			this.canvas.height = h;
			this.container.appendChild(this.canvas);
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

		var DOMURL = window.URL || window.webkitURL || window;

		var img = new Image(w, h);

		img.onload = function()
		{
		    ctx.drawImage(img, 0, 0);

			if (this.form) this.form.remove();

			this.form = document.createElement("form");
			this.form.method = 'post';
			this.form.action = '/action/svg_charts/save_image';
			this.form.style.display = 'none';
			var input = document.createElement("input");
			input.type = 'hidden';
			input.name = 'img';
			input.value = '';
			var filename = document.createElement("input");
			filename.type = 'hidden';
			filename.name = 'filename';
			filename.value = this.id;

			this.form.appendChild(input);
			this.form.appendChild(filename);
			document.body.appendChild(this.form);

			var output = document.getElementById(this.id + "_canvas").toDataURL("image/png");
		    this.form["img"].value = output;
		    this.form.submit();
		}.bind(this);

		img.onerror = function()
		{
			alert("PNG export failed");
		};

		img.src = svg;
	}

	path()
	{
		var d = [];
		var i = 0;
		for(i = 0; i < arguments.length; ++i)
		{
			if (arguments[i] == '') continue;
			var step = Array.isArray(arguments[i]) ? arguments[i] : [arguments[i]];
			var params = step.slice(1);
			d.push(step[0] + params.join(','));
		}
		return d.join(' ');
	}
}

Chart.prototype.svgDocType = '<?xml version="1.0" encoding="utf-8" standalone="no"?><!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">';

Chart.prototype.inlineStyles =
[
	"* { font-family: 'Arial', 'Helvetica', 'sans-serif'}"
];
