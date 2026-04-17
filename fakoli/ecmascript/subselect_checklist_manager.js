/**
 * Shows or hides checklist items based on the value of a parent select element,
 * unchecking hidden items automatically when the selection changes.
 */
class SubSelectChecklistManager
{
	constructor(select, subSelect)
	{
		this.select    = $el(select);
		this.subSelect = $el(subSelect);

		var self = this;
		this.select.addEventListener('change', function() { self.update(); });
		this.update();
	}

	update()
	{
		var value = this.select.value;

		Array.from(this.subSelect.querySelectorAll('[data-select]')).forEach(function(item)
		{
			var input = item.querySelector('input');
			if (item.getAttribute('data-select') == value)
			{
				item.style.display = 'block';
			}
			else
			{
				item.style.display = 'none';
				if (input) input.checked = false;
			}
		});
	}
}
