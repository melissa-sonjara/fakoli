/**
description: TextareaResizer — auto-growing drag handle for element elements.

license: MIT-style license.

copyright: Copyright (c) 2009 Joshua Partogi (http://scrum8.com/).

authors: Joshua Partogi (http://scrum8.com/)

The MIT License

Copyright (c) 2009 Joshua Partogi (http://scrum8.com/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

class ElementResizer
{
	constructor(element)
	{
		this.element = element;
	}

	resizable()
	{
		var staticOffset  = 0;
		var iLastMousePos = 0;
		var iMin          = 32;

		var element = this.element;
		if (element.classList.contains('fixed-size')) return;

		element.classList.add('processed');

		var div     = document.createElement('div');
		var span    = document.createElement('span');
		var grippie = document.createElement('div');

		div.classList.add('resizable-textarea');
		grippie.classList.add('grippie');

		// Build structure: div > span > (element + grippie)
		// replacing element in the original DOM position
		element.parentNode.insertBefore(div, element);
		div.appendChild(span);
		span.appendChild(element);
		span.appendChild(grippie);

		grippie.style.marginRight = (grippie.offsetWidth - element.offsetWidth) + 'px';
		grippie.style.width       = window.getComputedStyle(element).width;
		grippie.style.padding     = '0';
		grippie.style.margin      = '0';

		var mousePosition = function(e)
		{
			return {
				x: e.clientX + document.documentElement.scrollLeft,
				y: e.clientY + document.documentElement.scrollTop
			};
		};

		var endDrag = function(e)
		{
			document.removeEventListener('mousemove', performDrag);
			document.removeEventListener('mouseup',   endDrag);

			element.style.opacity = 1.0;
			element.focus();

			staticOffset  = 0;
			iLastMousePos = 0;
		};

		var performDrag = function(e)
		{
			var iThisMousePos = mousePosition(e).y;
			var iMousePos     = staticOffset + iThisMousePos;

			if (iLastMousePos >= iThisMousePos)
			{
				iMousePos -= 5;
			}

			iLastMousePos = iThisMousePos;
			iMousePos     = Math.max(iMin, iMousePos);

			element.style.height = iMousePos + 'px';

			if (iMousePos < iMin)
			{
				endDrag(e);
			}
		};

		var startDrag = function(e)
		{
			element.blur();

			iLastMousePos = mousePosition(e).y;
			staticOffset  = element.offsetHeight - iLastMousePos;
			element.style.opacity = 0.25;

			document.addEventListener('mousemove', performDrag);
			document.addEventListener('mouseup',   endDrag);
		};

		grippie.addEventListener('mousedown', startDrag);
	}
}

Element.prototype.resizable = function()
{
	var resizer = new ElementResizer(this);
	resizer.resizable();
};

window.addEvent('domready', function()
{
	document.querySelectorAll('.resizable').forEach(function(el)
	{
		el.resizable();
	});
});
