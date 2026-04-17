// Requires ui.js for EventEmitter, $el helpers.

/**
 * Coordinates one or more facet filter handlers, applying their combined
 * criteria to DOM elements and firing filterChanged/filterCleared events.
 */
class FacetManager extends EventEmitter
{
	constructor(container, options)
	{
		super();
		this.container = $el(container);
		this.handlers  = [];
		this.options = Object.assign({
			onFilterChanged: function() {},
			onFilterCleared: function() {}
		}, options || {});

		if (this.container) this.container.facetManager = this;
	}

	filterChanged()
	{
		var clear = true;
		for (var i = 0; i < this.handlers.length; ++i)
		{
			if (!this.handlers[i].isClear()) { clear = false; break; }
		}
		this.fireEvent(clear ? 'filterCleared' : 'filterChanged');
	}

	registerHandler(handler)
	{
		this.handlers.push(handler);
	}

	preprocess(element)
	{
		var self = this;
		this.handlers.forEach(function(handler)
		{
			var id = element.id;
			if (!id)
			{
				id = uniqueID();
				element.id = id;
			}
			handler.preprocess(element);
		});
	}

	preprocessComplete()
	{
		this.handlers.forEach(function(handler)
		{
			handler.preprocessComplete();
		});
	}

	filter(element)
	{
		var clear = true;
		var found = true;
		for (var i = 0; i < this.handlers.length; ++i)
		{
			if (!this.handlers[i].isClear())
			{
				clear = false;
				found = found & this.handlers[i].filter(element);
			}
		}
		return clear || found;
	}

	getSelectedValues()
	{
		var values = {};
		this.handlers.forEach(function(handler)
		{
			values[handler.getName()] = handler.getSelectedValue();
		});
		return values;
	}

	getQueryString()
	{
		var values = this.getSelectedValues();
		var url = new URL(window.location.href);
		Object.keys(values).forEach(function(k)
		{
			url.searchParams.set(k, values[k]);
		});
		return url.searchParams.toString();
	}
}


/**
 * Facet handler that filters elements by matching a multi-select (checkbox list)
 * widget against a data attribute on each element.
 */
class MultiSelectFacetHandler
{
	constructor(id, select, manager)
	{
		this.id        = id;
		this.select    = select;
		this.checkboxes = this.select.getCheckboxes();
		this.manager   = manager;
		this.manager.registerHandler(this);
		this.termLookup = {};

		var self = this;
		this.select.addEvent('selectionChanged', function()
		{
			self.manager.filterChanged();
		});
	}

	getName() { return this.id; }

	preprocess(item)
	{
		var term = item.getAttribute("data-" + this.id);
		if (term) this.termLookup[term] = true;
	}

	preprocessComplete()
	{
		var terms = Object.keys(this.termLookup).sort();
		terms.forEach(function(term)
		{
			this.select.addCheckbox(term, term);
		}.bind(this));
		this.checkboxes = this.select.getCheckboxes();
	}

	filter(item)
	{
		var term = item.getAttribute("data-" + this.id);
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked && term == this.checkboxes[i].value) return true;
		}
		return false;
	}

	isClear()
	{
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked) return false;
		}
		return true;
	}

	getSelectedValue()
	{
		var values = [];
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked) values.push(this.checkboxes[i].value);
		}
		return values.join(",");
	}
}


/**
 * Facet handler that filters elements by substring-matching a text input
 * against a data attribute on each element.
 */
class StringFacetHandler
{
	constructor(id, textField, manager)
	{
		this.id           = id;
		this.textField    = $el(textField);
		this.manager      = manager;
		this.currentValue = "";
		this.manager.registerHandler(this);

		var self = this;
		this.textField.addEventListener('input', function()
		{
			var value = self.textField.value.toLowerCase();
			if (value == self.currentValue) return;
			self.currentValue = value;
			self.manager.filterChanged();
		});
	}

	getName() { return this.id; }
	preprocess(item) {}
	preprocessComplete() {}

	filter(item)
	{
		var term = item.getAttribute("data-" + this.id) || '';
		return term.indexOf(this.currentValue) != -1;
	}

	isClear() { return this.textField.value.length < 3; }

	getSelectedValue() { return this.textField.value; }
}


/**
 * Facet handler that filters elements by matching a select dropdown value
 * against a data attribute on each element.
 */
class SelectFacetHandler
{
	constructor(id, textField, manager)
	{
		this.id           = id;
		this.select       = $el(textField);
		this.manager      = manager;
		this.currentValue = "";
		this.manager.registerHandler(this);

		var self = this;
		this.select.addEventListener('change', function()
		{
			var value = self.select.value;
			if (value == self.currentValue) return;
			self.currentValue = value;
			self.manager.filterChanged();
		});
	}

	getName() { return this.id; }
	preprocess(item) {}
	preprocessComplete() {}

	filter(item)
	{
		var term = item.getAttribute("data-" + this.id) || '';
		return term.indexOf(this.currentValue) != -1;
	}

	isClear() { return this.select.selectedIndex == -1; }

	getSelectedValue() { return this.select.value; }
}


/**
 * Facet handler for many-to-many cross-reference filtering: matches elements
 * whose comma-separated data attribute contains any of the selected checkbox values.
 */
class CrossReferenceFacetHandler
{
	constructor(name, select, manager)
	{
		this.name       = name;
		this.select     = select;
		this.checkboxes = this.select.getCheckboxes();
		this.manager    = manager;
		this.termLookup = {};
		this.manager.registerHandler(this);

		var self = this;
		this.select.addEvent('selectionChanged', function()
		{
			self.manager.filterChanged();
		});
	}

	getName() { return this.name; }

	preprocess(item)
	{
		var terms = item.getAttribute("data-" + this.name);
		var id    = item.id;
		if (terms)
		{
			terms.split(",").forEach(function(term)
			{
				this.termLookup[id + ":" + term] = true;
			}.bind(this));
		}
	}

	preprocessComplete() {}

	filter(item)
	{
		var id = item.id;
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked && this.termLookup[id + ":" + this.checkboxes[i].value]) return true;
		}
		return false;
	}

	isClear()
	{
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked) return false;
		}
		return true;
	}

	getSelectedValue()
	{
		var values = [];
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked) values.push(this.checkboxes[i].value);
		}
		return values.join(",");
	}
}


/**
 * Facet handler that filters elements by matching a checklist widget against
 * comma-separated values stored in a data attribute on each element.
 */
class CheckListFacetHandler
{
	constructor(name, select, manager)
	{
		this.name       = name;
		this.select     = select;
		this.checkboxes = this.select.getCheckboxes();
		this.manager    = manager;
		this.termLookup = {};
		this.manager.registerHandler(this);

		var self = this;
		this.select.addEvent('selectionChanged', function()
		{
			self.manager.filterChanged();
		});
	}

	getName() { return this.name; }

	preprocess(item)
	{
		var terms = item.getAttribute("data-" + this.name);
		var id    = item.id;
		if (terms)
		{
			terms.split(",").forEach(function(term)
			{
				this.termLookup[id + ":" + term] = true;
			}.bind(this));
		}
	}

	preprocessComplete() {}

	filter(item)
	{
		var id = item.id;
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked && this.termLookup[id + ":" + this.checkboxes[i].value]) return true;
		}
		return false;
	}

	isClear()
	{
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked) return false;
		}
		return true;
	}

	getSelectedValue()
	{
		var values = [];
		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked) values.push(this.checkboxes[i].value);
		}
		return values.join(",");
	}
}
