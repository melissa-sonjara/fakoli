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
 * Displays a floating tooltip populated from a remote URL or inline text,
 * with configurable positioning, width, delay, and optional fade animation.
 */
class ToolTip
{
	constructor(id, options)
	{
		this.id = id;
		this.div = null;
		this.handlerURL = '';
		this.options = Object.assign(
			{},
			ToolTip.globalOptions,
			{
				css:     'tooltip_box',
				width:   'auto',
				loading: "Loading...",
				delay:   0,
				fade:    false
			},
			options || {}
		);
	}

	show(link, evt, handlerURL)
	{
		ToolTip.currentLink = $el(link);
		evt.preventDefault();
		evt.stopPropagation();

		var tip   = this;
		var event = evt;

		setTimeout(function()
		{
			if (ToolTip.currentLink !== $el(link)) return;

			if (tip.div == null)
			{
				tip.create();
				tip.position(event);
			}
			else if (tip.handlerURL == handlerURL)
			{
				tip.position(event);
				return;
			}

			tip.handlerURL = handlerURL;
			var cursor = window.getComputedStyle(ToolTip.currentLink).cursor;
			ToolTip.currentLink.style.cursor = 'progress';

			fetchHTML(handlerURL, function(html, scripts)
			{
				tip.div.innerHTML = html;
				tip.position(event);
				if (ToolTip.currentLink) ToolTip.currentLink.style.cursor = cursor;
				execScripts(scripts);
			});
		}, this.options.delay);
	}

	showText(link, evt, text)
	{
		ToolTip.currentLink = $el(link);
		evt.preventDefault();
		evt.stopPropagation();

		var tip   = this;
		var event = evt;

		setTimeout(function()
		{
			if (tip.div == null) tip.create();
			tip.position(event);
			tip.div.innerHTML = text;
		}, this.options.delay);
	}

	create()
	{
		this.div = document.createElement('div');
		this.div.id        = this.id;
		this.div.className = this.options.css;
		this.div.innerHTML = this.options.loading;
		var doc = document.body || document.documentElement;
		doc.insertBefore(this.div, doc.firstChild);
	}

	position(event)
	{
		var z = window.getZIndex(ToolTip.currentLink);
		++z;

		if (!this.options.position)
		{
			setStyles(this.div, {
				display:  'block',
				opacity:  '0',
				left:     (event.pageX + 10) + 'px',
				top:      (event.pageY - 20) + 'px',
				position: 'absolute',
				width:    this.options.width,
				'z-index': String(z)
			});
		}
		else
		{
			positionRelative(this.div, { relativeTo: ToolTip.currentLink, position: this.options.position, edge: this.options.edge });
			setStyles(this.div, {
				display:  'block',
				opacity:  '0',
				position: 'absolute',
				width:    this.options.width,
				'z-index': String(z)
			});
		}

		if (this.options.fade)
		{
			fadeIn(this.div);
		}
		else
		{
			setStyles(this.div, { display: 'block', visibility: 'visible', opacity: '1' });
		}
	}

	hide()
	{
		ToolTip.currentLink = null;
		if (this.div == null) return;

		if (this.options.fade)
		{
			fadeOut(this.div);
		}
		else
		{
			setStyles(this.div, { display: 'none', visibility: 'hidden', opacity: '0' });
		}
	}
}

ToolTip.globalOptions = {};
ToolTip.tooltips      = {};
ToolTip.currentLink   = null;

ToolTip.getToolTip = function(id, options)
{
	if (!ToolTip.tooltips[id])
	{
		ToolTip.tooltips[id] = new ToolTip(id, options);
	}
	return ToolTip.tooltips[id];
};

function showToolTip(link, evt, id, handlerURL, width)
{
	var tip = ToolTip.getToolTip(id);
	if (width) tip.options.width = width;
	tip.show(link, evt, handlerURL);
}

function showTextToolTip(link, evt, id, text, width, position, edge)
{
	var tip = ToolTip.getToolTip(id);
	if (width)    tip.options.width    = width;
	if (position) tip.options.position = position;
	if (edge)     tip.options.edge     = edge;
	tip.showText(link, evt, text);
}

function hideToolTip(id)
{
	var tip = ToolTip.getToolTip(id);
	tip.hide();
}
