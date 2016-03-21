var Palette = new Class(
{
	name: '',
	backgroundColor: '',
	strokeColor: '',
	buttonColor: '',
	swatches: Class.Empty,
	namedColors: {},
	
	initialize: function(name, background, stroke, button, palette, namedColors)
	{
		this.name = name;
		this.backgroundColor = background;
		this.strokeColor = stroke;
		this.buttonColor = button;
		this.swatches = palette;
		if (namedColors) this.namedColors = namedColors;
		Palette.palettes[name] = this;
	},
	
	getColor: function(color)
	{
		if (this.namedColors && color in this.namedColors)
		{
			return this.namedColors[color];
		}
		
		return this.swatches[color];
	},
	
	getFontColor: function(color, dark, light)
	{
		if (!dark) dark = "#000";
		if (!light) light = "#fff";
		
		var background = new Color(this.getColor(color));
		
		var intensity = background.rgb[0] *  0.299 + background.rgb[1] * 0.587 + background.rgb[2] * 0.114;
		return (intensity < 96) ? light : dark;
	}
});

Palette.palettes = {};

new Palette('standard', '#fff', '#000', '#ddd', ['#f00', '#0f0', '#00f', '#ff0', '#f0f', '#0ff', '#800', '#080', '#008', '#880', '#808', '#088', '#888']);
