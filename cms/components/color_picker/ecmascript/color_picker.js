class ColorPicker
{
	constructor(valueField, button, options)
	{
		this.options = Object.assign(
			{
				defaultColor:   "yellow",
				standardColors: ["white", "black", "yellow", "yellowgreen", "green", "red", "orange", "royalblue", "cyan", "magenta"],
				shades:         ["#000", "#ffff00", "#228B22", "#00008b", "#8B0000", "#8B008B", "#ff69b4", "#008b8b", "#6b8e23", "#A0522D"],
				width:          "auto",
				height:         "auto",
				trigger:        "click"
			},
			options || {}
		);

		this.valueField = (typeof valueField === 'string') ? document.getElementById(valueField) : valueField;
		this.button     = (typeof button     === 'string') ? document.getElementById(button)     : button;
		this.panel      = null;

		if (!this.valueField.value) this.valueField.value = this.options.defaultColor;

		this.buildPanel();

		var self = this;
		this.button.addEventListener(this.options.trigger, function(e)
		{
			e.preventDefault();
			e.stopPropagation();
			self.showPanel();
		});
	}

	buildPanel()
	{
		var doc = document.body || document.documentElement;

		this.panel = document.createElement('div');
		this.panel.className = 'color_picker';
		Object.assign(this.panel.style,
		{
			width:    this.options.width,
			height:   this.options.height,
			position: 'absolute',
			display:  'none',
			opacity:  '0'
		});
		doc.appendChild(this.panel);

		// "No Highlight" / transparent row
		this.noneDiv = document.createElement('div');
		this.noneDiv.style.margin = '4px';

		this.noneLabel = document.createElement('span');
		this.noneLabel.textContent = "No Highlight";

		this.none = document.createElement('table');
		this.none.style.display = 'inline';

		this.noneBody = document.createElement('tbody');
		this.addColorRow(this.noneBody, ["transparent"]);

		this.none.appendChild(this.noneBody);
		this.noneDiv.appendChild(this.noneLabel);
		this.noneDiv.appendChild(this.none);
		this.panel.appendChild(this.noneDiv);

		// Standard colors row
		this.standard = document.createElement('table');
		this.standardBody = document.createElement('tbody');
		this.standard.appendChild(this.standardBody);
		this.addColorRow(this.standardBody, this.options.standardColors);
		this.panel.appendChild(this.standard);

		// Shade palette rows
		this.palette = document.createElement('table');
		this.paletteBody = document.createElement('tbody');
		this.palette.appendChild(this.paletteBody);

		for (var brightness = 80; brightness >= 0; brightness -= 20)
		{
			var colors = [];
			this.options.shades.forEach(function(shade)
			{
				var color = new Color(shade).mix("#fff", brightness);
				colors.push(color);
			});
			this.addColorRow(this.paletteBody, colors);
		}

		this.palette.appendChild(this.paletteBody);
		this.panel.appendChild(this.palette);

		// Swatch in button
		this.swatch = document.createElement('div');
		this.swatch.className = 'color_swatch';
		this.swatch.innerHTML = '&nbsp;';
		this.button.innerHTML = '';
		this.button.appendChild(this.swatch);

		this.updateSwatch();

		var self = this;
		doc.addEventListener('click', function() { self._hidePanel(); });
	}

	addColorRow(body, colors)
	{
		var tr = document.createElement('tr');
		var self = this;

		colors.forEach(function(color)
		{
			var td = document.createElement('td');
			td.innerHTML = '&nbsp;';
			td.style.backgroundColor = color;

			td.addEventListener('mouseenter', function() { td.classList.add('hover'); });
			td.addEventListener('mouseleave', function() { td.classList.remove('hover'); });
			td.addEventListener('click', function(e)
			{
				e.preventDefault();
				e.stopPropagation();
				self.selectColor(window.getComputedStyle(td).getPropertyValue('background-color'));
			});

			tr.appendChild(td);
		});

		body.appendChild(tr);
	}

	showPanel()
	{
		var rect    = this.button.getBoundingClientRect();
		var scrollX = window.scrollX || window.pageXOffset;
		var scrollY = window.scrollY || window.pageYOffset;

		Object.assign(this.panel.style,
		{
			top:     (rect.bottom + scrollY) + 'px',
			left:    (rect.left   + scrollX) + 'px',
			display: 'block',
			opacity: '0'
		});

		this.panel.style.transition = 'opacity 200ms ease-in-out';
		var panel = this.panel;
		requestAnimationFrame(function() { panel.style.opacity = '1'; });
	}

	_hidePanel()
	{
		var panel = this.panel;
		panel.style.transition = 'opacity 200ms ease-in-out';
		panel.style.opacity    = '0';

		var finish = function()
		{
			panel.removeEventListener('transitionend', finish);
			panel.style.display    = 'none';
			panel.style.transition = '';
		};
		panel.addEventListener('transitionend', finish);
	}

	updateSwatch()
	{
		this.swatch.style.backgroundColor = this.valueField.value;
	}

	selectColor(color)
	{
		this.valueField.value = color;
		this.updateSwatch();
		this._hidePanel();
	}
}
