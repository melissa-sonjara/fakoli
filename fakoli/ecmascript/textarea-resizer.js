/**
 * TextareaResizer
 *
 * Original author: Joshua Partogi (MIT license)
 * Converted to plain ECMAScript.
 */

/**
 * Adds a draggable grippie handle below a textarea element, allowing the
 * user to resize its height by dragging.
 */
class TextareaResizer
{
	constructor(element)
	{
		this.textarea = element;
		this.element  = element;
	}

	resizable()
	{
		var textarea = this.element;
		if (textarea.classList.contains("fixed-size")) return;

		textarea.classList.add('processed');

		var div = document.createElement('div');
		div.classList.add('resizable-textarea');

		var span = document.createElement('span');
		textarea.parentNode.insertBefore(div, textarea);
		span.appendChild(textarea);
		div.appendChild(span);

		var grippie = document.createElement('div');
		grippie.classList.add('grippie');
		span.appendChild(grippie);
		grippie.style.marginRight = (grippie.offsetWidth - textarea.offsetWidth) + 'px';
		grippie.style.width = window.getComputedStyle(textarea).width;
		grippie.style.padding = '0';
		grippie.style.margin  = '0';

		var staticOffset  = 0;
		var iLastMousePos = 0;
		var iMin = 32;

		function mousePosition(e)
		{
			return {
				x: e.clientX + document.documentElement.scrollLeft,
				y: e.clientY + document.documentElement.scrollTop
			};
		}

		function endDrag(e)
		{
			document.removeEventListener('mousemove', performDrag);
			document.removeEventListener('mouseup',   endDrag);

			textarea.style.opacity = '1';
			textarea.focus();

			staticOffset  = 0;
			iLastMousePos = 0;
		}

		function performDrag(e)
		{
			var iThisMousePos = mousePosition(e).y;
			var iMousePos = staticOffset + iThisMousePos;

			if (iLastMousePos >= iThisMousePos)
			{
				iMousePos -= 5;
			}

			iLastMousePos = iThisMousePos;
			iMousePos = Math.max(iMin, iMousePos);

			textarea.style.height = iMousePos + 'px';

			if (iMousePos < iMin)
			{
				endDrag(e);
			}
		}

		function startDrag(e)
		{
			textarea.blur();

			iLastMousePos = mousePosition(e).y;
			staticOffset  = textarea.offsetHeight - iLastMousePos;
			textarea.style.opacity = '0.25';

			document.addEventListener('mousemove', performDrag);
			document.addEventListener('mouseup',   endDrag);
		}

		grippie.addEventListener('mousedown', startDrag);
	}
}

document.addEventListener('DOMContentLoaded', function()
{
	document.querySelectorAll('.resizable').forEach(function(el)
	{
		new TextareaResizer(el).resizable();
	});
});
