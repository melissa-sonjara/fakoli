// Requires ui.js for EventEmitter, $el, positionRelative, fadeIn, fadeOut.

/**
 * Wraps a checkbox list in a dropdown widget, showing selected item labels
 * in a collapsed summary link and firing a selectionChanged event on change.
 */
class MultiSelect extends EventEmitter
{
	constructor(container, options)
	{
		super();
		this.container = $el(container);
		this.dropdown  = null;
		this.mode      = "list";

		this.options = Object.assign({
			message:          "Click to Select",
			maxWidth:         "400px",
			onSelectionChanged: function() {}
		}, options || {});

		if (!this.container) return;

		if (this.container.classList.contains("tree"))
		{
			this.mode = "tree";
		}

		this.container.multiSelect = this;

		this.buildDropdown();
		this.selectionChanged();
	}

	buildDropdown()
	{
		this.dropdown = document.createElement('a');
		this.dropdown.className = 'multi_select_dropdown';
		this.dropdown.href = '#';
		this.dropdown.textContent = this.options.message;
		this.dropdown.style.maxWidth = this.options.maxWidth;

		this.container.parentNode.insertBefore(this.dropdown, this.container);

		setStyles(this.container, { display: 'none', opacity: '0' });

		var self = this;
		this.container.addEventListener('mouseleave', function() { fadeOut(self.container); });

		this.dropdown.addEventListener('click', function(e)
		{
			e.preventDefault();
			var hidden = self.container.style.display == 'none' ||
			             parseFloat(self.container.style.opacity) == 0;

			if (hidden)
			{
				document.querySelectorAll('.multi_select_dropdown_list').forEach(function(elt)
				{
					elt.style.display = 'none';
				});
				setStyles(self.container, { display: 'block', opacity: '0' });
				self.container.position({ relativeTo: self.dropdown, position: 'bottomLeft', edge: 'topLeft' });

				//positionRelative(self.container, { relativeTo: self.dropdown, position: 'bottomLeft', edge: 'topLeft' });
				fadeIn(self.container);
			}
			else
			{
				fadeOut(self.container);
			}
			return false;
		});

		this.connectCheckboxes();
	}

	connectCheckboxes()
	{
		var self = this;
		Array.from(this.container.querySelectorAll('input[type=checkbox]')).forEach(function(cbox)
		{
			cbox.addEventListener('click', function() { self.selectionChanged(); });
		});
	}

	getCheckboxes()
	{
		return Array.from(this.container.querySelectorAll("input[type=checkbox]"));
	}

	addCheckbox(text, value)
	{
		var id   = "cbox_" + uniqueID();
		var cbox = document.createElement('input');
		cbox.type  = 'checkbox';
		cbox.id    = id;
		cbox.value = value;

		var label = document.createElement('label');
		label.setAttribute('for', id);
		label.insertBefore(cbox, label.firstChild);
		label.appendChild(document.createTextNode(text));
		this.container.appendChild(label);

		var self = this;
		cbox.addEventListener('click', function() { self.selectionChanged(); });
	}

	selectionChanged()
	{
		var selected = this.getCheckboxes();
		var message  = [];
		var self     = this;

		selected.forEach(function(cbox)
		{
			if (!cbox.checked) return;
			var label;
			if (self.mode == "list")
			{
				label = self.container.querySelector('label[for="' + cbox.id + '"]');
			}
			else
			{
				label = cbox.nextElementSibling;
			}
			if (label) message.push(label.textContent);
		});

		this.dropdown.textContent = (message.length == 0) ? this.options.message : message.join(", ");
		this.fireEvent("selectionChanged");
	}
}
