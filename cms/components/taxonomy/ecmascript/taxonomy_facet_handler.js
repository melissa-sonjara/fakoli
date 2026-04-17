class TaxonomyFacetHandler
{
	constructor(taxonomy, select, manager)
	{
		this.taxonomy = taxonomy;
		this.select = select;
		this.checkboxes = this.select.getCheckboxes();
		this.manager = manager;
		this.termLookup = {};

		this.manager.registerHandler(this);

		this.select.addEventListener('selectionChanged', function()
		{
			this.manager.filterChanged();
		}.bind(this));
	}

	getName()
	{
		return this.taxonomy;
	}

	preprocess(item)
	{
		var terms = item.getAttribute("data-taxonomy-" + this.taxonomy);
		var id = item.getAttribute("id");

		if (terms)
		{
			terms = terms.split(",");
			terms.forEach(function(term) { this.termLookup[id + ":" + term] = true; }.bind(this));
		}
	}

	preprocessComplete()
	{
	}

	filter(item)
	{
		var id = item.getAttribute('id');

		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked && this.termLookup[id + ":" + this.checkboxes[i].value])
			{
				return true;
			}
		}
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
		var values = "";

		for (var i = 0; i < this.checkboxes.length; ++i)
		{
			if (this.checkboxes[i].checked)
			{
				if (values != "")
				{
					values += ",";
				}
				values += this.checkboxes[i].value;
			}
		}

		return values;
	}
}
