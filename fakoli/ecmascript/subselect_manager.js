/**
 * Populates a dependent select element with options corresponding to the
 * current value of a parent select, showing or hiding it with a fade effect.
 */
class SubSelectManager
{
	constructor(select, subSelect, value)
	{
		this.select     = $el(select);
		this.subSelect  = $el(subSelect);
		this.subSelectLabel = $el(subSelect + "_label");
		this.value      = value || 0;
		this.selectOptions = {};

		this.select.manager = this;

		var container = this.subSelect.parentElement;
		this.subselectVisibility = container.classList.contains('subselect_container') ? container : this.subSelect;

		var self = this;
		this.select.addEventListener('change', function() { self.update(true); });
		this.subSelect.addEventListener('change', function() { self.value = self.subSelect.value; });
		this.update();
	}

	clear()
	{
		this.subSelect.innerHTML = '';
	}

	update(fade)
	{
		this.clear();
		var val  = this.select.value;
		var opts = this.selectOptions[val];

		if (typeof opts === 'undefined' || !Array.isArray(opts))
		{
			this.subselectVisibility.style.display = 'none';
			if (this.subSelectLabel) this.subSelectLabel.style.display = 'none';
			return;
		}

		var self = this;
		opts.forEach(function(opt)
		{
			var option = document.createElement('option');
			option.text     = opt.name;
			option.value    = opt.value;
			option.selected = (opt.value == self.value);
			self.subSelect.appendChild(option);
		});

		if (fade)
		{
			setStyles(this.subselectVisibility, { display: 'inline', opacity: '0' });
			if (this.subSelectLabel) setStyles(this.subSelectLabel, { display: 'inline', opacity: '0' });
			fadeIn(this.subselectVisibility);
			if (this.subSelectLabel) fadeIn(this.subSelectLabel);
		}
		else
		{
			setStyles(this.subselectVisibility, { display: 'inline', opacity: '1' });
			if (this.subSelectLabel) setStyles(this.subSelectLabel, { display: 'inline', opacity: '1' });
		}
	}

	addOptions(selectValue, options)
	{
		this.selectOptions[selectValue] = options;
	}

	validateRequired()
	{
		var selected    = this.select.value;
		var subSelected = this.subSelect.value;

		if (!selected) return false;

		var opts = this.selectOptions[selected];
		if ((!subSelected || subSelected === "0") && (typeof opts !== 'undefined')) return false;
		return true;
	}
}
