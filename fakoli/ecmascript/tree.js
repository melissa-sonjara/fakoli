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

var Tree = {};

Tree.toggleFolder = function(id, openStyle, closedStyle)
{
	var div  = $el(id + "_contents");
	var link = $el(id);

	if (div.style.display == "none" || div.style.display == "")
	{
		div.style.display = "block";
		link.className = openStyle;
	}
	else
	{
		div.style.display = "none";
		link.className = closedStyle;
	}
};

Tree.loadOnDemand = function(id, fragmentURL, force)
{
	var link = $el(id);
	var div  = $el(id + "_contents");
	var cursor = window.getComputedStyle(link).cursor;

	if (!div._loaded || force)
	{
		if (force) div.textContent = '';

		link.style.cursor = 'progress';

		fetchHTML(fragmentURL, function(html, scripts)
		{
			div.textContent = '';
			div.innerHTML = html;
			link.style.cursor = cursor;
			execScripts(scripts);
		});

		div._loaded = true;
	}
};

Tree.clearCheckBoxes = function(id, except)
{
	var div = $el(id + "_table");
	Array.from(div.querySelectorAll("input")).forEach(function(box)
	{
		if (box != except)
		{
			box.checked = false;
		}
	});

	var val = $el(id);
	if (val) val.value = except.value;
};

Tree.toggleCheckbox = function(link, id, mode)
{
	var parent = link.parentElement;
	var cbx = parent.querySelector("input[type=checkbox]");
	if (mode == 'single') Tree.clearCheckBoxes(id, cbx);
	if (cbx) cbx.checked = !cbx.checked;
};

Tree.selectCheckbox = function(link, id, mode)
{
	var parent = link.parentElement;
	var cbx = parent.querySelector("input[type=checkbox]");
	if (mode == 'single') Tree.clearCheckBoxes(id, cbx);
	if (mode == 'context' && !cbx.checked) Tree.clearCheckBoxes(id, cbx);
	if (cbx) cbx.checked = true;
};

Tree.selectedValues = function(id)
{
	var values = [];
	document.querySelectorAll("#" + id + " input:checked").forEach(function(input)
	{
		values.push(input.value);
	});
	return values;
};
