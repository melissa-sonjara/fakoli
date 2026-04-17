class HistogramSeries
{
	constructor(type, title, values, options)
	{
		this.type = type;
		this.title = title;
		this.values = values;
		this.columns = [];
		this.renderer = null;
		this.chart = null;
		this.group = null;
		this._events = {};

		this.max = Math.max.apply(null, this.values);
		this.min = Math.min.apply(null, this.values);

		this.options = Object.assign(
		{
			shadow: false,
			emboss: false,
			styles: {},
			strokeWidth: 1,
			symbolSize: 4,
			onClick: null,
			onMouseOver: null,
			onMouseOut: null,
			onItemDrawn: null,
			colorMode: 'series',
			colorFunction: null,
			toolTips: [],
			labels: [],
			labelSize: 24,
			labelColor: '#000',
			labelClass: 'value-label',
			indicateTooltips: false,
			areaFill: false,
			areaFillOpacity: 0,
			showValues: false,
			color: 0,
			applyOffset: true,
			joinPrevious: false,
			joinBlanks: false
		}, options || {});
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

	getRenderer(chart, index)
	{
		if (!this.renderer)
		{
			this.renderer = chart.getSeriesRenderer(this, index);
		}

		return this.renderer;
	}

	getColor(i)
	{
		if (this.options.colorMode == 'function' && typeof(this.options.colorFunction) == 'function')
		{
			return this.options.colorFunction(this, i);
		}
		if (this.options.colorMode == 'fixed') return this.chart.palette.getColor(this.options.color);
		return this.chart.palette.getColor((this.options.colorMode == 'series') ? this.index : i);
	}

	connectToChart(chart, index)
	{
		this.chart = chart;
		this.index = index;
	}

	draw(chart, index)
	{
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.draw();
	}

	drawFill(chart, index)
	{
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.drawFill();
	}

	drawDots(chart, index)
	{
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.drawDots();
	}

	drawLabels(chart, index)
	{
		if (this.options.labels.length == 0) return;
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.drawDots();
	}

	morph(series)
	{
		if (!this.renderer) return;
		this.renderer.morph(series);
	}

	hasTooltip(idx)
	{
		if (!this.options.toolTips) return false;

		if (idx > this.options.toolTips.length) return false;

		var text = this.options.toolTips[idx];
		if (text == '') return false;

		return true;
	}

	showToolTip(evt, idx)
	{
		if (idx > this.options.toolTips.length) return;

		var text = this.options.toolTips[idx];
		if (text == '') return;

		showTextToolTip(this.chart.container, evt, this.chart.id + "_tooltip", this.options.toolTips[idx]);
	}

	hideToolTip()
	{
		hideToolTip(this.chart.id + "_tooltip");
	}

	collectLegends(chart, index, legends)
	{
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.collectLegends(legends);
	}
}

class VerticalBlockSeriesRenderer
{
	constructor(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
		this.dropShadow = null;
	}

	addShadow(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.chart.paper.filter(Snap.filter.shadow(1, 1, 5, 0.1));
		}
		shape.attr({filter: this.dropShadow});
	}

	getColor(i)
	{
		return this.series.getColor(i);
	}

	draw()
	{
		this.series.values.forEach(function(val, i)
		{
			var fillSwatch = this.getColor(i);

			var columnWidth = this.chart.blockWidth;

			var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth);
			if (this.series.options.applyOffset)
			{
				columnOffset += this.chart.blockSeriesDrawn * columnWidth;
			}
			var columnLeft = this.chart.columnWidth * i + columnOffset;

			var x = this.chart.options.chartLeft + columnLeft;
			var columnHeight = this.chart.options.chartHeight * val / this.chart.range();
			var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight - this.chart.xAxisOffset;

			var column = this.chart.paper.rect(x, y, columnWidth, columnHeight);
			column.attr({fill: fillSwatch, 'stroke-width': this.series.options.strokeWidth, 'stroke': this.chart.palette.strokeColor});

			if (this.series.options.emboss)
			{
				column.emboss();
			}

			if (this.series.options.shadow)
			{
				this.addShadow(column);
			}

			if (this.series.options.showValues)
			{
				var label;

				if (val === null)
				{
					label = this.chart.options.emptyValueLabel;
				}
				else
				{
					if (!val) val = 0;
					label = val + this.chart.options.units;
				}

				this.chart.paper.text(x + columnWidth / 2, y - 8, label)
					.attr({"text-anchor": "middle", 'font-size': this.chart.options.labelSize});
			}

			column.mouseover(function(e) { this.series.fireEvent('mouseOver', [e, i]); this.series.showToolTip(e, i);}.bind(this));
			column.mouseout(function(e) { this.series.fireEvent('mouseOut', [e, i]);  this.series.hideToolTip();}.bind(this));
			column.click(function() { this.series.fireEvent('click', i); }.bind(this));

			this.series.fireEvent('itemDrawn', [this.series, column, i]);

			this.series.columns.push(column);

		}.bind(this));

		this.chart.blockSeriesDrawn++;
	}

	drawDots()
	{
	}

	drawFill()
	{
	}

	drawLabels()
	{
	}

	// Morph this series to match the values of the supplied series
	morph(series)
	{
		series.values.forEach(function(val, i)
		{
			var fillSwatch = this.getColor(i);
			var columnHeight = this.chart.options.chartHeight * val / this.chart.range() + this.chart.xAxisOffset;
			var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight;

			this.series.columns[i].animate({'y' :y, 'height': columnHeight, fill: fillSwatch}, 1000, mina.easeinout);
		}.bind(this));
	}

	collectLegends(legends)
	{
		legends.push({title: this.series.title, color: this.getColor(this.index)});
	}
}


class HorizontalBlockSeriesRenderer
{
	constructor(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
		this.dropShadow = null;
	}

	addShadow(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.chart.paper.filter(Snap.filter.shadow(1, 1, 5, 0.1));
		}
		shape.attr({filter: this.dropShadow});
	}

	getColor(i)
	{
		return this.series.getColor(i);
	}

	draw()
	{
		this.series.values.forEach(function(val, i)
		{
			var fillSwatch = this.getColor(i);

			var columnWidth = this.chart.blockWidth;

			var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth);
			if (this.series.options.applyOffset)
			{
				columnOffset += this.chart.blockSeriesDrawn * columnWidth;
			}

			var columnTop = this.chart.columnWidth * i + columnOffset;

			var x = this.chart.options.chartLeft;
			var columnHeight = this.chart.options.chartWidth * val / this.chart.range();
			var y = this.chart.options.chartTop + columnTop;

			this.chart.options.chartHeight - columnHeight - this.chart.xAxisOffset;

			var column = this.chart.paper.rect(x, y, columnHeight, columnWidth);
			column.attr({fill: fillSwatch, 'stroke-width': this.series.options.strokeWidth, 'stroke': this.chart.palette.strokeColor});

			if (this.series.options.emboss)
			{
				column.emboss();
			}

			if (this.series.options.shadow)
			{
				this.addShadow(column);
			}

			if (this.series.options.showValues)
			{
				var label;

				if (val === null)
				{
					label = this.chart.options.emptyValueLabel;
				}
				else
				{
					if (!val) val = 0;
					label = val + this.chart.options.units;
				}

				this.chart.paper.text(x + columnHeight + 5, y + columnWidth / 2, label).attr({'text-anchor': 'start', 'font-size': this.chart.options.labelSize, 'alignment-baseline': 'middle'});
			}

			column.mouseover(function(e) { this.series.fireEvent('mouseOver', [e, i]);  this.series.showToolTip(e, i); }.bind(this));
			column.mouseout(function(e) { this.series.fireEvent('mouseOut', [e, i]);  this.series.hideToolTip();}.bind(this));
			column.click(function() { this.series.fireEvent('click', i); }.bind(this));

			this.series.fireEvent('itemDrawn', [this.series, column, i]);

			this.series.columns.push(column);

		}.bind(this));

		this.chart.blockSeriesDrawn++;
	}

	drawDots()
	{
	}

	drawFill()
	{
	}

	drawLabels()
	{
	}

	// Morph this series to match the values of the supplied series
	morph(series)
	{
		series.values.forEach(function(val, i)
		{
			var fillSwatch = this.getColor(i);
			var columnHeight = this.chart.options.chartWidth * val / this.chart.range();

			this.series.columns[i].animate({'width': columnHeight, fill: fillSwatch}, 1000, mina.easeinout);
		}.bind(this));
	}

	collectLegends(legends)
	{
		legends.push({title: this.series.title, color: this.getColor(this.index)});
	}
}

class LineSeriesRenderer
{
	constructor(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
		this.path = null;
		this.dots = [];
		this.labels = [];
		this.coords = [];
		this.fill = null;
		this.dropShadow = null;
	}

	getColor(i)
	{
		return this.series.getColor(i);
	}

	draw()
	{
		this.drawLine();
		if (this.series.group === null || this.series.group === undefined)
		{
			this.drawDots();
			this.drawLabels();
		}
	}

	drawLine()
	{
		var lineColor = this.getColor(this.index);
		var fillColor = this.chart.options.chartBackground;
		var p = this.calculatePath(this.series, false);

		this.path = this.chart.paper.path(p).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor, fill: 'none'});

		this.series.fireEvent('itemDrawn', [this.series, this.path, this.index]);
	}

	drawDots()
	{
		var lineColor = this.getColor(this.index);
		var fillColor = this.chart.options.chartBackground;
		this.coords.forEach(function(c, i) {
			if (c !== null)
			{
				var dot = this.chart.paper.circle(c.x, c.y, this.series.options.symbolSize).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor, fill: (this.series.options.indicateTooltips && this.series.hasTooltip(i) ) ? lineColor : fillColor, 'cursor': 'pointer'});
				dot.mouseover(function(e) {dot.animate({'r': this.series.options.symbolSize * 2}, 250, mina.easein); this.series.fireEvent('mouseOver', [e, i]); this.series.showToolTip(e, i);}.bind(this));
				dot.mouseout(function(e) {dot.animate({'r': this.series.options.symbolSize}, 250, mina.easeout);  this.series.fireEvent('mouseOut', [e, i]); this.series.hideToolTip();}.bind(this));
				dot.click(function() { this.series.fireEvent('click', i); }.bind(this));
				this.dots.push(dot);
			}
			else
			{
				this.dots.push(null);
			}
		}.bind(this));
	}

	drawFill()
	{
		var lineColor = this.getColor(this.index);

		if (this.series.options.areaFill)
		{
			var f = this.calculatePath(this.series, true);
			this.fill = this.chart.paper.path(f).attr({"stroke-width": 0, stroke: lineColor, fill: lineColor, 'fill-opacity': this.series.options.areaFillOpacity});
		}
	}

	drawLabels()
	{
		this.coords.forEach(function(c, i)
		{
			if (c !== null)
			{
				var label = this.chart.paper.text(c.x, c.y - 20, this.series.options.labels[i])
												.attr({"text-anchor": "middle",
													   "font-size": this.series.options.labelSize,
													   "fill": this.series.options.labelColor,
													   "class": this.series.options.labelClass});
			}
		}.bind(this));
	}

	calculatePath(series, closed)
	{
		var p = "";
		var cmd = "M";
		this.coords = [];
		var startX = -999;
		var lastX = -999;

		var values = [...series.values];
		if (typeof(series.group) != 'undefined' && series.group !== null && series.options.joinPrevious)
		{
			for(var i = 0; i < values.length; ++i)
			{
				if (values[i] !== null)
				{
					if (i > 0)
					{
						values[i-1] = series.group.getValue(i-1);
					}
					break;
				}
			}
		}

		var first = true;

		values.forEach(function(val, i)
		{
			if (val !== null)
			{
				var columnOffset = this.chart.getColumnOffset();
				var columnCenter = this.chart.columnWidth * i + columnOffset;

				var x = this.chart.options.chartLeft + columnCenter;
				var columnHeight = this.chart.options.chartHeight * val / this.chart.range() + this.chart.xAxisOffset;
				var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight;

				if (!first || !this.series.options.joinPrevious)
				{
					this.coords.push({'x': x, 'y': y});
				}
				else
				{
					this.coords.push(null);
				}

				first = false;
				p += cmd + x + "," + y;
				cmd = "L";

				if (startX == -999) startX = x;
				lastX = x;
			}
			else
			{
				this.coords.push(null);
				if (!series.options.joinBlanks) cmd = "M";
			}

			if (i == series.values.length - 1 && closed)
			{
				var y = this.chart.options.chartTop + this.chart.options.chartHeight - this.chart.xAxisOffset;
				p += "L" + lastX + "," + y + "L" + startX + "," + y + "Z";
			}

		}.bind(this));

		return p;
	}

	// Morph this series to match the values of the supplied series
	morph(series)
	{
		var p = this.calculatePath(series);
		var lineColor = this.getColor(series.index);

		var fillColor = this.chart.options.chartBackground;
		this.path.animate({'path': p, 'stroke': lineColor}, 1000, mina.easeinout);

		if (this.series.options.areaFill)
		{
			if (!this.fill)
			{
				this.drawFill();
			}
			else
			{
				var f = this.calculatePath(series, true);
				this.fill.animate({'path': f, stroke: lineColor, fill: lineColor}, 1000, mina.easeinout);
			}
		}

		this.dots.forEach(function(dot, i)
		{
			if (this.coords[i] !== null)
			{
				dot.animate({'cy': this.coords[i].y, 'stroke': lineColor, fill: (this.series.options.indicateTooltips && this.series.hasTooltip(i) ) ? lineColor : fillColor}, 1000, mina.easeinout);
			}
		}.bind(this));
	}

	collectLegends(legends)
	{
		legends.push({title: this.series.title, color: this.getColor(this.index)});
	}
}

class VerticalHistogramAxisRenderer
{
	constructor(chart)
	{
		this.chart = chart;
		this.ticks = [];
	}

	calculateColumnWidth(count)
	{
		return this.chart.options.chartWidth / count;
	}

	showToolTip(evt, idx)
	{
		if (idx > this.chart.labelTooltips.length) return;

		var text = this.chart.labelTooltips[idx];
		if (text == '') return;

		showTextToolTip(this.chart.container, evt, this.chart.id + "_tooltip", text);
	}

	hideToolTip()
	{
		hideToolTip(this.chart.id + "_tooltip");
	}

	drawLabels()
	{
		this.chart.labels.forEach(function(text, index)
		{
			var chart = this.chart;
			var x = chart.options.chartLeft + chart.columnWidth * index + (chart.getColumnOffset());
			var y = chart.options.chartTop + chart.options.chartHeight + chart.options.labelOffset;

			var decorations = chart.labelDecorations[index];
			var attrs = {stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": chart.options.labelAnchor};
			Object.assign(attrs, decorations);

			var label;
			if (chart.options.labelAngle)
			{
				attrs.transform = "rotate(" + chart.options.labelAngle + "," + x + "," + y + ")";
				label = chart.paper.text(x, y, text);
				label.attr(attrs);
			}
			else
			{
				label = chart.paper.multitext(x, y, text, chart.columnWidth, attrs);
			}

			var i = "Tooltip";

			label.mouseover(function(e) { chart.fireEvent('mouseOver', [e, i]); this.showToolTip(e, index);}.bind(this));
			label.mouseout(function(e) { chart.fireEvent('mouseOut', [e, i]);  this.hideToolTip();}.bind(this));
			label.click(function() { chart.fireEvent('click', i); }.bind(this));
		}.bind(this));
	}

	drawTicks()
	{
		var chart = this.chart;

		var increment = chart.range() / chart.options.ticks;

		if (increment == 0) return;

		var tick = chart.min;
		var idx = 0;
		var y = chart.options.chartTop + chart.options.chartHeight;
		var ystep = chart.options.chartHeight / chart.options.ticks;

		for(tick = chart.min; tick <= chart.max; tick += increment)
		{
			var label;
			if (chart.yAxisLabels.length > 0)
			{
				label = chart.yAxisLabels[idx];
			}
			else
			{
				label = number_format(tick, 0) + chart.options.units;
			}
			var text = chart.paper.text(chart.options.chartLeft - 10, y, label);
			text.attr({stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": "end"});
			this.ticks.push(text);
			y -= ystep;
			++idx;
		}
	}

	drawGrid(x, y, w, h, wv, hv, color)
	{
	    color = color || "#000";
	    var path = '',
	        rowHeight = h / hv,
	        columnWidth = w / wv,
	        chart = this.chart;
	    for (var i = 1; i < hv; i++)
	    {
	        path = chart.path(path, ["M", (Math.round(x) + .5), (Math.round(y + i * rowHeight) + .5)], ["H", (Math.round(x + w) + .5)]);
	    }
	    for (i = 1; i < wv; i++)
	    {
	    	if (chart.labels[i] == "") continue;
	        path = chart.path(path, ["M", (Math.round(x + i * columnWidth) + .5), (Math.round(y) + .5)], ["V", (Math.round(y + h) + .5)]);
	    }
	    return this.chart.paper.path({d: path, stroke: color, 'class': 'grid'});
	}
}

class HorizontalHistogramAxisRenderer
{
	constructor(chart)
	{
		this.chart = chart;
		this.ticks = [];
	}

	calculateColumnWidth(count)
	{
		return this.chart.options.chartHeight / count;
	}

	showToolTip(evt, idx)
	{
		//if (idx > this.chart.labelTooltips.length) return;

		var text = this.chart.labelTooltips[idx];
		if (text == '') text = "N/A";

		showTextToolTip(this.chart.container, evt, this.chart.id + "_tooltip", text);
	}

	hideToolTip()
	{
		hideToolTip(this.chart.id + "_tooltip");
	}

	drawLabels()
	{
		this.chart.labels.forEach(function(text, index)
		{
			var chart = this.chart;
			var decorations = this.chart.labelDecorations[index];
			var x = chart.options.chartLeft - 5;
			var y = chart.options.chartTop + chart.columnWidth * index + (chart.getColumnOffset());
			//+ this.options.chartHeight + 20 + (text.count("\n") * this.options.labelSize / 2);

			var label = chart.paper.text(x, y, text);

			var attrs = {stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": "end"};
			Object.assign(attrs, decorations);

			label.attr(attrs);
			label.mouseover(function(e) { chart.fireEvent('mouseOver', [e, index]); this.showToolTip(e, index);}.bind(this));
			label.mouseout(function(e) { chart.fireEvent('mouseOut', [e, index]);  this.hideToolTip();}.bind(this));
			label.click(function() { chart.fireEvent('click', index); }.bind(this));
		}.bind(this));
	}

	drawTicks()
	{
		var chart = this.chart;

		var increment = chart.range() / chart.options.ticks;

		if (increment == 0) return;

		var tick = chart.min;
		var idx = 0;
		var x = chart.options.chartLeft;
		var xstep = chart.options.chartWidth / chart.options.ticks;

		for(tick = chart.min; tick <= chart.max; tick += increment)
		{
			var label;
			if (chart.yAxisLabels.length > 0)
			{
				label = chart.yAxisLabels[idx];
			}
			else
			{
				label = number_format(tick, 0) + chart.options.units;
			}
			var text = chart.paper.text(x, chart.options.chartTop - 10, label);
			text.attr({stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": "middle"});
			this.ticks.push(text);
			x += xstep;
			++idx;
		}
	}

	drawGrid(x, y, w, h, hv, wv, color)
	{
	    color = color || "#000";
	    var path = '',
	        rowHeight = h / hv,
	        columnWidth = w / wv,
	        chart = this.chart;
	    for (var i = 1; i < hv; i++)
	    {
	        path = chart.path(path, ["M", Math.round(x) + .5, Math.round(y + i * rowHeight) + .5], ["H", Math.round(x + w) + .5]);
	    }
	    for (i = 1; i < wv; i++)
	    {
	        path = chart.path(path, ["M", Math.round(x + i * columnWidth) + .5, Math.round(y) + .5], ["V", Math.round(y + h) + .5]);
	    }
	    return this.chart.paper.path({d: path, stroke: color, 'class': 'grid'});
	}
}

class Histogram extends Chart
{
	constructor(id, options, labels)
	{
		super(id);

		this.series = [];
		this.labels = labels;
		this.labelTooltips = [];
		this.labelDecorations = [];
		this.yAxisLabels = [];
		this.max = 0;
		this.min = 0;
		this.xAxisOffset = 0;
		this.linearOnly = true;
		this.dropShadow = null;

		this.options = Object.assign(
		{
			palette: 'standard',
			width: 600,
			height: 400,
			orientation: 'vertical',
			chartLeft: 50,
			chartTop: 50,
			chartWidth: 500,
			chartHeight: 300,
			chartBackground: "#ddd",
			labelSize: 12,
			labelAngle: 0,
			labelAnchor: 'middle',
			labelOffset: 20,
			strokeWidth: 2,
			animate: true,
			shadow: false,
			showGrid: true,
			gridStrokeWidth: 1,
			ticks: 10,
			units: "",
			columnMargin: 0.2,
			titleSize: 20,
			title: '',
			max: 0,
			min: 0,
			enableDownload: true,
			emptyValueLabel: ""
		}, options || {});

		this.createAxisRenderer();
	}

	getSeriesRenderer(series, index)
	{
		var renderer = null;

		switch(series.type)
		{
		case 'block':

			renderer = (this.options.orientation == 'horizontal') ?
					new HorizontalBlockSeriesRenderer(this, series, index) :
					new VerticalBlockSeriesRenderer(this, series, index);
			break;

		case 'line':
		default:

			renderer = new LineSeriesRenderer(this, series, index);
			break;
		}

		return renderer;
	}

	addShadow(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.paper.filter(Snap.filter.shadow(1, 1, 7, 0.2));
		}
		shape.attr({filter: this.dropShadow});
	}

	addSeries(series)
	{
		this.series.push(series);
		series.connectToChart(this, this.series.length - 1);
	}

	updateSeries(index, values)
	{
		this.series[index].values = values;
		this.series[index].morph(this.series[index]);
	}

	updateTooltips(index, tips)
	{
		this.series[index].options.tooltips = tips;
	}

	range()
	{
		return this.max - this.min;
	}

	drawChart()
	{
		this.createChart();

		this.setupHistogram();

		this.blockWidth = (this.columnWidth / this.blockSeriesCount) * (1 - this.options.columnMargin);
		this.drawHistogram();
	}

	createAxisRenderer()
	{
		switch(this.options.orientation)
		{
		case 'horizontal':
			this.axisRenderer = new HorizontalHistogramAxisRenderer(this);
			break;

		case 'vertical':
		default:

			this.axisRenderer = new VerticalHistogramAxisRenderer(this);
		}
	}

	drawHistogram()
	{
		this.drawBlocks();
		this.drawLabels();
		this.drawTicks();
		this.drawLegend();
		this.drawTitle();
	}

	setupHistogram()
	{
		if (this.labels.length == 0)
		{
			var count = this.series[0].length;
			for(var idx = 1; idx <= count; ++idx)
			{
				this.labels.push(idx);
			}
		}

		this.max = this.options.max;
		this.min = this.options.min;

		this.series.forEach(function(s)
		{
			if (s.max > this.max) this.max = s.max;
			if (s.min < this.min) this.min = s.min;
			if (s.type != 'line') this.linearOnly = false;
		}.bind(this));

		this.max = Math.ceil(this.max / this.options.ticks) * this.options.ticks;

		if (this.min < 0)
		{
			this.xAxisOffset = -(this.options.chartHeight * this.min / this.range());
		}

		this.blockSeriesCount = 0;
		this.blockSeriesDrawn = 0;

		this.series.forEach(function(s)
		{
			if (s.type == 'block')
			{
				this.blockSeriesCount++;
			}
		}.bind(this));

		var count = this.labels.length;
		if (this.linearOnly) count -= 1;

		this.columnWidth = this.axisRenderer.calculateColumnWidth(count);

		if (this.options.showGrid)
		{
			var grid = this.paper.rect(this.options.chartLeft, this.options.chartTop, this.options.chartWidth, this.options.chartHeight);
			grid.attr({"stroke-width": this.options.strokeWidth, stroke: this.palette.strokeColor, fill: this.options.chartBackground});

			if (this.options.shadow)
			{
				this.addShadow(grid);
			}

			this.fireEvent('drawGrid', this);
			this.axisRenderer.drawGrid(this.options.chartLeft, this.options.chartTop, this.options.chartWidth, this.options.chartHeight, count, this.options.ticks, this.palette.strokeColor);
			this.fireEvent('drawGridComplete', this);
		}
	}

	getColumnOffset()
	{
		return this.linearOnly ? 0 : this.columnWidth / 2;
	}

	drawBlocks()
	{
		var idx = 0;
		this.series.forEach(function(s)
		{
			s.drawFill(this, idx++);
		}.bind(this));

		idx = 0;
		this.series.forEach(function(s)
		{
			s.draw(this, idx++);
		}.bind(this));
	}

	drawLabels()
	{
		this.axisRenderer.drawLabels();
	}

	drawTicks()
	{
		this.axisRenderer.drawTicks();
	}

	drawGrid(x, y, w, h, wv, hv, color)
	{
		var chart = this.chart;

	    color = color || "#000";
	    var path = chart.path(["M", Math.round(x) + .5, Math.round(y) + .5],
	    					  ["L", Math.round(x + w) + .5, Math.round(y) + .5,
	    					   Math.round(x + w) + .5, Math.round(y + h) + .5,
	    					   Math.round(x) + .5, Math.round(y + h) + .5,
	    					   Math.round(x) + .5, Math.round(y) + .5]);
	    var rowHeight = h / hv,
	        columnWidth = w / wv;
	    for (var i = 1; i < hv; i++)
	    {
	        path = chart.path(path, ["M", Math.round(x) + .5, Math.round(y + i * rowHeight) + .5], ["H", Math.round(x + w) + .5]);
	    }
	    for (i = 1; i < wv; i++)
	    {
	        path = chart.path(path, ["M", Math.round(x + i * columnWidth) + .5, Math.round(y) + .5], ["V", Math.round(y + h) + .5]);
	    }
	    return this.paper.path({d: path, stroke: color, 'class': 'grid'});
	}

	drawTitle()
	{
		if (this.options.title == '') return;
		this.title = this.paper.multitext(this.options.width / 2, this.options.chartTop - 60, this.options.title, this.options.width,
										  {stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.titleSize, 'text-anchor': 'middle', 'class': 'title'});
	}

	drawLegend()
	{
		if (this.options.legend)
		{
			var x = this.options.legendX;
			var y = this.options.legendY;
			var s = this.options.legendSwatchSize || 20;
			var h = this.options.legendLineHeight || 30;

			var legends = [];
			this.series.forEach(function(series, index)
			{
				series.collectLegends(this, index, legends);
			}.bind(this));

			legends.forEach(function(legend, index)
			{
				var rect = this.paper.rect(x, y, s, s, 3);
				rect.attr({fill: legend.color, stroke: this.palette.strokeColor, "stroke-width": this.options.strokeWidth});

				var text = this.paper.text(x + h, y + h / 2, legend.title);

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
	}
}

class GroupedHistogramSeries extends HistogramSeries
{
	constructor(type, title, options)
	{
		super(type, title, [], options);
		this.title = title;
		this.children = [];
		this.min = Infinity;
		this.max = -Infinity;
	}

	addSeries(series)
	{
		series.group = this;
		this.children.push(series);
		this.children.forEach(function(child)
		{
			if (child.max > this.max) this.max = child.max;
			if (child.min < this.min) this.min = child.min;
		}.bind(this));

		return this;
	}

	connectToChart(chart, index)
	{
		this.chart = chart;
		this.index = index;
		this.children.forEach(function(child, i)
		{
			child.connectToChart(chart, index + i);
		});
	}

	getRange()
	{
		var max = -Infinity;
		var min = Infinity;

		var hasRange = false;

		this.children.forEach(function(child)
		{
			if (child.values.length == 0) return;

			child.values.forEach(function(value)
			{
				if (value == null) return;

				if (value > max) max = value;
				if (value < min) min = value;

				hasRange = true;
			});
		});

		return hasRange ? {'max': max, 'min': min} : false;
	}

	getValue(idx)
	{
		var value = null;
		this.children.forEach(function(child)
		{
			if (value !== null) return;
			value = child.values[idx];
		});

		return value;
	}

	draw(chart, index)
	{
		this.children.forEach(function(child)
		{
			child.draw(chart, index);
		});

		this.drawDots(chart, index);
	}

	drawFill(chart, index)
	{
		this.children.forEach(function(child)
		{
			child.drawFill(chart, index);
		});
	}

	drawDots(chart, index)
	{
		this.children.forEach(function(child)
		{
			child.drawDots(chart, index);
		});
	}

	morph(series)
	{
		this.children.forEach(function(child, idx)
		{
			child.morph(series.children[idx]);
		});
	}

	hasTooltip(idx)
	{
		var found = false;

		this.children.forEach(function(child)
		{
			if (child.hasTooltip(idx)) found = true;
		});

		return found;
	}

	showToolTip(evt, idx)
	{
		this.children.forEach(function(child)
		{
			child.showTooltip(evt, idx);
		});
		if (idx > this.options.toolTips.length) return;
	}

	hideToolTip()
	{
		hideToolTip(this.chart.id + "_tooltip");
	}

	collectLegends(chart, index, legends)
	{
		this.children.forEach(function(child, idx)
		{
			child.collectLegends(chart, idx, legends);
		});
	}
}

class StackedHistogramSeries extends HistogramSeries
{
	constructor(type, title, options)
	{
		super(type, title, [], options);
		this.title = title;
		this.children = [];
		this.totals = [];
		this.max = 0;
		this.min = 0;
	}

	addSeries(series)
	{
		series.group = this;
		this.children.push(series);

		this.calculateTotals();
		this.min = Math.min.apply(null, this.totals);
		this.max = Math.max.apply(null, this.totals);

		return this;
	}

	connectToChart(chart, index)
	{
		this.chart = chart;
		this.index = index;
		this.children.forEach(function(child, i)
		{
			child.connectToChart(chart, index + i);
		});
	}

	calculateTotals()
	{
		this.totals = [];

		this.children[0].values.forEach(function() { this.totals.push(0); }.bind(this));

		this.children.forEach(function(child)
		{
			child.values.forEach(function(value, idx)
			{
				this.totals[idx] += value;
			}.bind(this));
		}.bind(this));
	}

	getValue(idx)
	{
		// Return total value
		return this.totals[idx];
	}

	getRenderer(chart, index)
	{
		if (!this.renderer)
		{
			switch(this.type)
			{
			case 'block':

				this.renderer = (chart.options.orientation == 'horizontal') ?
						new StackedHorizontalBlockSeriesRenderer(chart, this, index) :
						new StackedVerticalBlockSeriesRenderer(chart, this, index);
				break;

			case 'line':

				this.renderer = new StackedLineSeriesRenderer(chart, this, index);
				break;
			}
		}

		return this.renderer;
	}

	hasTooltip(idx)
	{
		var found = false;

		this.children.forEach(function(child)
		{
			if (child.hasTooltip(idx)) found = true;
		});

		return found;
	}

	showToolTip(evt, idx)
	{
		this.children.forEach(function(child)
		{
			child.showTooltip(evt, idx);
		});
		if (idx > this.options.toolTips.length) return;
	}

	hideToolTip()
	{
		hideToolTip(this.chart.id + "_tooltip");
	}

	collectLegends(chart, index, legends)
	{
		this.children.forEach(function(child, idx)
		{
			child.collectLegends(chart, idx, legends);
		});
	}
}


class StackedVerticalBlockSeriesRenderer
{
	constructor(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
		this.dropShadow = null;
	}

	addShadow(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.chart.paper.filter(Snap.filter.shadow(1, 1, 5, 0.1));
		}
		shape.attr({filter: this.dropShadow});
	}

	getColor(i)
	{
		if (this.series.children[i].options.colorMode == 'fixed') return this.chart.palette.getColor(this.series.children[i].options.color);
		return this.chart.palette.getColor(this.index + i);
	}

	draw()
	{
		var runningTotals = [];
		this.series.children[0].values.forEach(function() { runningTotals.push(0);});

		this.series.children.forEach(function(child, idx)
		{
			child.values.forEach(function(val, i)
			{
				var runningTotal = runningTotals[i];
				if (!runningTotal) runningTotal = 0;

				var fillSwatch = this.getColor(idx);

				var columnWidth = this.chart.blockWidth;

				var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth);
				if (this.series.options.applyOffset)
				{
					columnOffset += this.chart.blockSeriesDrawn * columnWidth;
				}
				var columnLeft = this.chart.columnWidth * i + columnOffset;

				var x = this.chart.options.chartLeft + columnLeft;
				var columnHeight = this.chart.options.chartHeight * val / this.chart.range();
				var columnBottom = this.chart.options.chartHeight * runningTotal / this.chart.range();

				var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight - columnBottom - this.chart.xAxisOffset;

				var column = this.chart.paper.rect(x, y, columnWidth, columnHeight);
				column.attr({fill: fillSwatch, 'stroke-width': this.series.options.strokeWidth, 'stroke': this.chart.palette.strokeColor});

				runningTotals[i] = val + runningTotal;

				if (this.series.options.emboss)
				{
					column.emboss();
				}

				if (this.series.options.shadow)
				{
					this.addShadow(column);
				}

				if (this.series.options.showValues)
				{
					if (!val) val = 0;

					this.chart.paper.text(x + columnWidth / 2, y - 8, val + this.chart.options.units);
				}

				column.mouseover(function(e) { child.fireEvent('mouseOver', [e, i]); child.showToolTip(e, i);}.bind(this));
				column.mouseout(function(e) { child.fireEvent('mouseOut', [e, i]);  child.hideToolTip();}.bind(this));
				column.click(function() { child.fireEvent('click', i); }.bind(this));

				child.columns.push(column);

			}.bind(this));
		}.bind(this));

		this.chart.blockSeriesDrawn++;
	}

	drawDots()
	{
	}

	drawFill()
	{
	}

	// Morph this series to match the values of the supplied series
	morph(series)
	{
		/*series.values.forEach(function(val, i)
		{
			var columnHeight = this.chart.options.chartHeight * val / this.chart.range() + this.chart.xAxisOffset;
			var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight;

			this.series.columns[i].animate({'y' :y, 'height': columnHeight}, 1000, mina.easeinout);
		}.bind(this));*/
	}
}


class StackedHorizontalBlockSeriesRenderer
{
	constructor(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
		this.dropShadow = null;
	}

	addShadow(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.chart.paper.filter(Snap.filter.shadow(1, 1, 5, 0.1));
		}
		shape.attr({filter: this.dropShadow});
	}

	getColor(i)
	{
		if (this.series.children[i].options.colorMode == 'fixed') return this.chart.palette.getColor(this.series.children[i].options.color);
		return this.chart.palette.getColor(this.index + i);
	}

	draw()
	{
		var runningTotals = [];
		this.series.children[0].values.forEach(function() { runningTotals.push(0);});

		this.series.children.forEach(function(child, idx)
		{
			child.values.forEach(function(val, i)
			{
				var runningTotal = runningTotals[i];
				if (!runningTotal) runningTotal = 0;
				var fillSwatch = this.getColor(idx);

				var columnWidth = this.chart.blockWidth;

				var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth);
				if (this.series.options.applyOffset)
				{
					columnOffset += this.chart.blockSeriesDrawn * columnWidth;
				}

				var columnTop = this.chart.columnWidth * i + columnOffset;

				var columnLeft = this.chart.options.chartWidth * runningTotal / this.chart.range();

				var x = this.chart.options.chartLeft + columnLeft;
				var columnHeight = this.chart.options.chartWidth * val / this.chart.range();
				var y = this.chart.options.chartTop + columnTop;

				var column = this.chart.paper.rect(x, y, columnHeight, columnWidth);
				column.attr({fill: fillSwatch, 'stroke-width': this.series.options.strokeWidth, 'stroke': this.chart.palette.strokeColor});

				if (this.series.options.emboss)
				{
					column.emboss();
				}

				if (this.series.options.shadow)
				{
					this.addShadow(column);
				}

				if (this.series.options.showValues)
				{
					if (!val) val = 0;

					this.chart.paper.text(x + columnHeight + 5, y + columnWidth / 2, val + this.chart.options.units)
						.attr({'text-anchor': 'start', 'font-size': this.chart.options.labelSize});
				}

				column.mouseover(function(e) { child.fireEvent('mouseOver', [e, i]);  child.showToolTip(e, i); }.bind(this));
				column.mouseout(function(e) { child.fireEvent('mouseOut', [e, i]);  child.hideToolTip();}.bind(this));
				column.click(function() { child.fireEvent('click', i); }.bind(this));

				runningTotals[i] = val + runningTotal;

				child.columns.push(column);

			}.bind(this));
		}.bind(this));

		this.chart.blockSeriesDrawn++;
	}

	drawDots()
	{
	}

	drawFill()
	{
	}

	// Morph this series to match the values of the supplied series
	morph(series)
	{
		/*series.values.forEach(function(val, i)
		{
			var columnHeight = this.chart.options.chartWidth * val / this.chart.range();

			this.series.columns[i].animate({'width': columnHeight}, 1000, mina.easeinout);
		}.bind(this));*/
	}
}
