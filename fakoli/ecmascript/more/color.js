/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above
 copyright holders shall not be used in advertising or otherwise
 to promote the sale, use or other dealings in this Software
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/**
 * Color class for creating and manipulating colors in JavaScript.
 * Supports RGB ↔ HSB conversion and mixing.
 */

function _rgbToHsb(r, g, b)
{
	var hue = 0;
	var max = Math.max(r, g, b);
	var min = Math.min(r, g, b);
	var delta = max - min;
	var brightness = max / 255;
	var saturation = (max !== 0) ? delta / max : 0;

	if (saturation !== 0)
	{
		var rr = (max - r) / delta;
		var gr = (max - g) / delta;
		var br = (max - b) / delta;
		if (r === max)      hue = br - gr;
		else if (g === max) hue = 2 + rr - br;
		else                hue = 4 + gr - rr;
		hue /= 6;
		if (hue < 0) hue++;
	}
	return [
		Math.round(hue * 360),
		Math.round(saturation * 100),
		Math.round(brightness * 100)
	];
}

function _hsbToRgb(h, s, b)
{
	var br = Math.round(b / 100 * 255);
	if (s === 0)
	{
		return [br, br, br];
	}
	var hue = h % 360;
	var f = hue % 60;
	var p = Math.round((b * (100 - s)) / 10000 * 255);
	var q = Math.round((b * (6000 - s * f)) / 600000 * 255);
	var t = Math.round((b * (6000 - s * (60 - f))) / 600000 * 255);
	switch (Math.floor(hue / 60))
	{
		case 0: return [br, t, p];
		case 1: return [q, br, p];
		case 2: return [p, br, t];
		case 3: return [p, q, br];
		case 4: return [t, p, br];
		case 5: return [br, p, q];
	}
	return [0, 0, 0];
}

function _hexToRgb(hex)
{
	hex = hex.replace(/^#/, '');
	if (hex.length === 3)
	{
		hex = hex[0] + hex[0] + hex[1] + hex[1] + hex[2] + hex[2];
	}
	var n = parseInt(hex, 16);
	return [(n >> 16) & 0xff, (n >> 8) & 0xff, n & 0xff];
}

function _rgbToHex(r, g, b)
{
	return '#' + [r, g, b].map(function(v)
	{
		return ('0' + v.toString(16)).slice(-2);
	}).join('');
}

class Color
{
	/**
	 * @param {string|number[]|{r,g,b}} color  Hex string, [r,g,b], {r,g,b}, or individual r argument.
	 * @param {'rgb'|'hsb'|'hex'} [type]
	 */
	constructor(color, type)
	{
		var rgb;

		if (typeof color === 'string')
		{
			if (/^rgb/i.test(color))
			{
				var parts = color.match(/\d+/g).map(Number);
				rgb = [parts[0], parts[1], parts[2]];
			}
			else
			{
				rgb = _hexToRgb(color);
			}
			type = type || 'rgb';
		}
		else if (Array.isArray(color))
		{
			type = type || 'rgb';
			if (type === 'hsb')
			{
				this.hsb = color.slice();
				rgb = _hsbToRgb(color[0], color[1], color[2]);
			}
			else if (type === 'hex')
			{
				rgb = _hexToRgb(color.join(''));
			}
			else
			{
				rgb = color.slice(0, 3).map(Number);
			}
		}
		else
		{
			rgb = [0, 0, 0];
		}

		this.rgb = rgb.slice(0, 3);
		this.hsb = this.hsb || _rgbToHsb(this.rgb[0], this.rgb[1], this.rgb[2]);
		this.hex = _rgbToHex(this.rgb[0], this.rgb[1], this.rgb[2]);
	}

	/**
	 * Mix this color with one or more other colors.
	 * @param {...Color|string|number[]} colors  Colors to mix in.
	 * @param {number} [alpha=50]  Mix percentage (0–100).
	 */
	mix()
	{
		var args = Array.from(arguments);
		var alpha = (typeof args[args.length - 1] === 'number') ? args.pop() : 50;
		var rgb = this.rgb.slice();
		args.forEach(function(c)
		{
			c = new Color(c);
			for (var i = 0; i < 3; i++)
			{
				rgb[i] = Math.round((rgb[i] / 100 * (100 - alpha)) + (c.rgb[i] / 100 * alpha));
			}
		});
		return new Color(rgb, 'rgb');
	}

	/** Returns a new Color that is the inverse of this one. */
	invert()
	{
		return new Color(this.rgb.map(function(v) { return 255 - v; }), 'rgb');
	}

	/** Returns a new Color with the hue component replaced. */
	setHue(value)
	{
		return new Color([value, this.hsb[1], this.hsb[2]], 'hsb');
	}

	/** Returns a new Color with the saturation component replaced. */
	setSaturation(percent)
	{
		return new Color([this.hsb[0], percent, this.hsb[2]], 'hsb');
	}

	/** Returns a new Color with the brightness component replaced. */
	setBrightness(percent)
	{
		return new Color([this.hsb[0], this.hsb[1], percent], 'hsb');
	}

	/** Returns the hex string representation. */
	toString()
	{
		return this.hex;
	}

	/** Returns an 'rgb(r,g,b)' CSS string. */
	toRGB()
	{
		return 'rgb(' + this.rgb.join(', ') + ')';
	}

	/** Returns the [h, s, b] array. */
	toHSB()
	{
		return this.hsb.slice();
	}

	/** Static factory: create from RGB values. */
	static fromRGB(r, g, b)
	{
		return new Color([r, g, b], 'rgb');
	}

	/** Static factory: create from HSB values. */
	static fromHSB(h, s, b)
	{
		return new Color([h, s, b], 'hsb');
	}

	/** Static factory: create from a hex string. */
	static fromHex(hex)
	{
		return new Color(hex);
	}

	/** Convert RGB to HSB. Returns [h, s, b]. */
	static RGBtoHSB(r, g, b)
	{
		return _rgbToHsb(r, g, b);
	}

	/** Convert HSB to RGB. Returns [r, g, b]. */
	static HSBtoRGB(h, s, b)
	{
		return _hsbToRgb(h, s, b);
	}
}

// Array prototype helpers

Array.prototype.rgbToHsb = function()
{
	return _rgbToHsb(this[0], this[1], this[2]);
};

Array.prototype.hsbToRgb = function()
{
	return _hsbToRgb(this[0], this[1], this[2]);
};

Array.prototype.rgbToHex = function()
{
	return _rgbToHex(this[0], this[1], this[2]);
};

Array.prototype.hexToRgb = function()
{
	return _hexToRgb(this.join(''));
};

// Factory shorthand functions

function $RGB(r, g, b)
{
	return new Color([r, g, b], 'rgb');
}

function $HSB(h, s, b)
{
	return new Color([h, s, b], 'hsb');
}

function $HEX(hex)
{
	return new Color(hex);
}
