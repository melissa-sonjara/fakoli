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

// Internal helper: sends a reorder GET request encoding new positions
function _sendReorderRequest(handler, key, pairs)
{
	var qs = (handler.indexOf("?") < 0) ? "?" : "&";
	var c = 1;
	pairs.forEach(function(pk)
	{
		if (pk) qs += encodeURIComponent(key + "[" + pk + "]") + "=" + c + "&";
		c++;
	});
	fetch(handler + qs);
}

/**
 * Makes the items of an unordered list reorderable via HTML5 drag-and-drop,
 * sending the new order to a server endpoint on completion.
 */
class DraggableList
{
	constructor(list, handler, key)
	{
		this.handler = handler;
		this.key     = key;
		this.list    = $el(list);

		this._setupSortable();
	}

	_setupSortable()
	{
		var self = this;
		var items = Array.from(this.list.querySelectorAll('li'));

		items.forEach(function(item)
		{
			item.draggable = true;

			item.addEventListener('dragstart', function(e)
			{
				item.classList.add('dragRow');
				e.dataTransfer.effectAllowed = 'move';
				e.dataTransfer.setData('text/plain', '');
				self._dragging = item;
			});

			item.addEventListener('dragend', function()
			{
				item.classList.remove('dragRow');
				self._dragging = null;
				self._onComplete();
			});

			item.addEventListener('dragover', function(e)
			{
				e.preventDefault();
				e.dataTransfer.dropEffect = 'move';
				if (self._dragging && self._dragging !== item)
				{
					var rect = item.getBoundingClientRect();
					var mid  = rect.top + rect.height / 2;
					if (e.clientY < mid)
					{
						self.list.insertBefore(self._dragging, item);
					}
					else
					{
						self.list.insertBefore(self._dragging, item.nextSibling);
					}
				}
			});
		});
	}

	_onComplete()
	{
		var self = this;
		var pks  = Array.from(this.list.querySelectorAll('li'))
			.map(function(item) { return item.getAttribute('data-pk'); });
		_sendReorderRequest(self.handler, self.key, pks);
	}
}


/**
 * Makes the rows of a table reorderable via HTML5 drag-and-drop,
 * sending the new row order to a server endpoint on completion.
 */
class DraggableTable
{
	constructor(list, handler, key)
	{
		this.handler = handler;
		this.key     = key;
		this.list    = list; // CSS selector or id

		this._setupSortable();
	}

	_setupSortable()
	{
		var self    = this;
		var table   = typeof this.list === 'string' ? document.querySelector(this.list) : this.list;
		var tbody   = table.querySelector('tbody') || table;
		var rows    = Array.from(tbody.querySelectorAll('tr'));
		var expr    = self.key + "_";

		rows.forEach(function(row)
		{
			if (!row.id) return;
			row.draggable = true;

			row.addEventListener('dragstart', function(e)
			{
				row.classList.add('dragRow');
				e.dataTransfer.effectAllowed = 'move';
				e.dataTransfer.setData('text/plain', '');
				self._dragging = row;
			});

			row.addEventListener('dragend', function()
			{
				row.classList.remove('dragRow');
				self._dragging = null;
				self._onComplete(tbody, expr);
			});

			row.addEventListener('dragover', function(e)
			{
				e.preventDefault();
				if (self._dragging && self._dragging !== row)
				{
					var rect = row.getBoundingClientRect();
					var mid  = rect.top + rect.height / 2;
					if (e.clientY < mid)
					{
						tbody.insertBefore(self._dragging, row);
					}
					else
					{
						tbody.insertBefore(self._dragging, row.nextSibling);
					}
				}
			});
		});
	}

	_onComplete(tbody, expr)
	{
		var self = this;
		var pks  = Array.from(tbody.querySelectorAll('tr'))
			.filter(function(tr) { return !!tr.id; })
			.map(function(tr)    { return tr.id.replace(expr, ""); });
		_sendReorderRequest(self.handler, self.key, pks);
	}
}


/**
 * Allows table columns to be reordered by dragging their header cells,
 * reordering both header and body cells in sync and firing a columnsReordered event.
 */
class DraggableColumnTable extends EventEmitter
{
	constructor(table, options)
	{
		super();
		this.table = $el(table);
		this.clone = null;
		this.options = Object.assign({
			dragDelay: 250,
			onColumnsReordered: function() {}
		}, options || {});

		this.elements = Array.from(this.table.querySelectorAll("thead th"));
		if (this.elements.length == 0) return;

		this.container = this.elements[0].parentElement;

		var self = this;
		this.elements.forEach(function(element)
		{
			element.addEventListener('mousedown', function(e)
			{
				e.preventDefault();
				self.mouseDown = true;
				setTimeout(function() { self.startDrag(element, e); }, self.options.dragDelay);
			});
			element.addEventListener('mouseup', function()
			{
				self.mouseDown = false;
			});
		});
	}

	_getOrCreateClone(element)
	{
		if (!this.clone)
		{
			this.clone = document.createElement('div');
			this.clone.innerHTML = "<table class='list'><thead><tr><th>" + element.textContent + "</th></tr></thead></table>";
			document.body.appendChild(this.clone);
		}
		else
		{
			var th = this.clone.querySelector("th");
			if (th) th.textContent = element.textContent;
			this.clone.style.opacity = '1';
			this.clone.style.display = 'block';
		}

		var rect = element.getBoundingClientRect();
		setStyles(this.clone, {
			width:    element.offsetWidth + 'px',
			cursor:   'pointer',
			position: 'absolute',
			display:  'block',
			opacity:  '1',
			top:      (rect.top  + window.pageYOffset) + 'px',
			left:     (rect.left + window.pageXOffset) + 'px'
		});
	}

	startDrag(element, event)
	{
		if (!this.mouseDown) return;

		this._getOrCreateClone(element);
		this.dragIndex = Array.from(this.container.children).indexOf(element);

		var self     = this;
		var startX   = event.pageX;
		var startY   = event.pageY;
		var origLeft = parseInt(this.clone.style.left) || 0;
		var origTop  = parseInt(this.clone.style.top)  || 0;

		var entered = null;

		function onMove(e)
		{
			self.clone.style.left = (origLeft + e.pageX - startX) + 'px';
			self.clone.style.top  = (origTop  + e.pageY - startY) + 'px';

			// Find which header we're over
			var hit = null;
			self.elements.forEach(function(el)
			{
				var r = el.getBoundingClientRect();
				if (e.clientX >= r.left && e.clientX <= r.right &&
				    e.clientY >= r.top  && e.clientY <= r.bottom)
				    {
					hit = el;
				}
			});

			if (hit !== entered)
			{
				if (entered) entered.style.opacity = '1';
				entered = hit;
				if (entered) entered.style.opacity = '0.3';
			}
		}

		function onUp(e)
		{
			document.removeEventListener('mousemove', onMove);
			document.removeEventListener('mouseup',   onUp);

			if (self.clone)
			{
				self.clone.style.opacity = '0';
				self.clone.style.display = 'none';
			}

			if (entered)
			{
				entered.style.opacity = '1';
				self.drop(element, entered);
			}
			entered = null;
		}

		document.addEventListener('mousemove', onMove);
		document.addEventListener('mouseup',   onUp);
	}

	drop(draggable, droppable)
	{
		if (!droppable) return;

		var kids     = Array.from(this.container.children);
		var dragIdx  = kids.indexOf(draggable);
		var dropIdx  = kids.indexOf(droppable);

		if (dragIdx == dropIdx) return;

		var position = dropIdx < dragIdx ? 'before' : 'after';

		if (position == 'before')
		{
			this.container.insertBefore(draggable, droppable);
		}
		else
		{
			this.container.insertBefore(draggable, droppable.nextSibling);
		}

		var self = this;
		Array.from(this.table.querySelectorAll('tbody tr')).forEach(function(tr)
		{
			var tds = Array.from(tr.children);
			if (position == 'before')
			{
				tr.insertBefore(tds[dragIdx], tds[dropIdx]);
			}
			else
			{
				tr.insertBefore(tds[dragIdx], tds[dropIdx] ? tds[dropIdx].nextSibling : null);
			}
		});

		var tfoot = this.table.querySelector('tfoot');
		if (tfoot)
		{
			Array.from(tfoot.querySelectorAll('tr')).forEach(function(tr)
			{
				var tds = Array.from(tr.children);
				if (position == 'before')
				{
					tr.insertBefore(tds[dragIdx], tds[dropIdx]);
				}
				else
				{
					tr.insertBefore(tds[dragIdx], tds[dropIdx] ? tds[dropIdx].nextSibling : null);
				}
			});
		}

		this.fireEvent('columnsReordered', [Array.from(this.container.children)]);
	}
}
