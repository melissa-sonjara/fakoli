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

 
var DraggableTable = new Class({
		
	sortables: Class.Empty,
	options: {clone: true, revert: true, opacity: 1},
	handler: "",
	key: "",
	
	initialize: function(list, handler, key)
	{
		this.handler = handler;
		this.key = key;
		
		this.options.onStart = function(element, clone)
		{
			etds = element.getElements("td");
			ctds = clone.getElements("td");
			
			etds.each(function(td, i)
			{
				ctds[i].setStyle('width', td.getSize().x);
			});
			
			clone.addClass("dragRow");
		};
		
		this.options.onComplete = function()
		{
			var trs = $$(list).getElements("tr").flatten();
			var qs = (this.handler.indexOf("?") < 0) ? "?" : "&";
			
			var expr = this.key + "_";
			
			var c = 1;
			
			trs.each(function(tr, i)
			{
				if (tr.id)
				{
					pk = tr.id.replace(expr, "");
					qs += this.key + "[" + pk + "]=" + c + "&";
					++c;
				}
			}.bind(this));
			
    		var request = new Request.HTML(
    		{
    			evalScripts: false,
    			evalResponse: false,
    			method: 'get', 
    			url: handler + qs
    		});
    		request.send();
    		
    		return true;
		}.bind(this);
		
		this.sortables = new Sortables(list, this.options);
	}
});

var DraggableColumnTable = new Class({
	
	Implements: [Options, Events],
	elements: [],
	table: Class.Empty,
	clone: Class.Empty,
	container: Class.Empty,
	options:
	{
		dragDelay: 250,
		onColumnsReordered: Class.Empty
	},
	
	initialize: function(table, options)
	{
		this.table = document.id(table);
		this.setOptions(options);
		this.elements = this.table.getElements("thead th");
		if (this.elements.length == 0) return;
		
		this.container = this.elements[0].getParent();
		
		this.elements.each(function(element)
		{
			element.addEvent('mousedown', function(e) { new DOMEvent(e).stop(); this.mouseDown = true; this.startDrag.delay(this.options.dragDelay, this, [element, e]); }.bind(this));
			element.addEvent('mouseup', function(e) { this.mouseDown = false; }.bind(this));
		}.bind(this));	
	},
	
	getClone: function(element)
	{
		if (this.clone)
		{
			this.clone.getElement("th").set('text', element.get('text'));
			this.clone.fade('show');
		}
		else
		{
			this.clone = new Element('div');
			this.clone.set('html', "<table class='list'><thead><tr><th>" + element.get('text') + "</th></tr></thead></table>");
			this.clone.inject(document.body);
		}
		
		this.clone.setStyles({width: element.getWidth(), cursor: 'pointer', position: 'absolute', display: 'block', 'opacity': 1, top: element.getTop(), left: element.getLeft()});
	},
	
	startDrag: function(element, event)
	{
		if (!this.mouseDown) return;
		
		this.getClone(element);
		this.dragIndex = this.container.getChildren().indexOf(element);
		
		this.drag = new Drag.Move(this.clone, 
		{
			droppables: this.elements,
			onEnter:	this.onEnter.bind(this),
			onLeave:	this.onLeave.bind(this),
			onCancel:	this.cancel.bind(this),
			onDrop:		this.drop.bind(this)
		});
		
		this.drag.start(event);
	},
	
	onEnter: function(draggable, droppable)
	{
		this.borderColor = droppable.getStyle('border-color');
		droppable.setStyles({'opacity': 0.3});
	},
	
	onLeave: function(draggable, droppable)
	{
		droppable.setStyles({'opacity': 1});
	},
	
	cancel: function()
	{
		this.drag.detach();
		this.clone.fade('out');
	},
	
	drop: function(draggable, droppable)
	{
		if (!droppable) 
		{
			this.cancel();
			return;
		}
		
		droppable.setStyles({'opacity': 1});
		this.drag.detach();
		this.clone.fade('out');
		
		var kids =  this.container.getChildren();
		this.dropIndex = kids.indexOf(droppable);
		
		if (this.dragIndex == this.dropIndex) return;
		
		position = this.dropIndex < this.dragIndex ? 'before' : 'after';
		kids[this.dragIndex].inject(droppable, position);
		
		this.table.getElement('tbody').getChildren('tr').each(function(tr)
		{
			kids = tr.getChildren();
			kids[this.dragIndex].inject(kids[this.dropIndex], position);
		}.bind(this));
		
		this.table.getElement('tfoot').getChildren('tr').each(function(tr)
		{
			kids = tr.getChildren();
			kids[this.dragIndex].inject(kids[this.dropIndex], position);
		}.bind(this));
		
		this.fireEvent('columnsReordered', [this.container.getChildren()]);
	}
});