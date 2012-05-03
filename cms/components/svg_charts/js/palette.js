var Palette = new Class(
{
	name: '',
	backgroundColor: '',
	strokeColor: '',
	swatches: Class.Empty,
	
	initialize: function(name, background, stroke, palette)
	{
		this.name = name;
		this.backgroundColor = background;
		this.strokeColor = stroke;
		this.swatches = palette;
		Palette.palettes[name] = this;
	}
});

Palette.palettes = {};

new Palette('standard', '#fff', '#000', ['#f00', '#0f0', '#00f', '#ff0', '#f0f', '#0ff']);
