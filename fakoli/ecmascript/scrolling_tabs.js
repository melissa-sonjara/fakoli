/**
 * Handles a tab bar that is wider than its container by adding left/right
 * scroll buttons and an optional dropdown list of all tabs.
 */
class ScrollingTabs
{
	constructor(tabContainer)
	{
		this.tabContainer  = $el(tabContainer);
		this.tabList       = this.tabContainer.querySelector("ul");
		this.tabs          = Array.from(this.tabList.children).filter(function(c) { return c.tagName == 'LI'; });

		this.tabsWidth     = 0;
		this.tabsOffsets   = [];
		this.tabIndex      = 0;
		this.currentIndex  = 0;
		this.maxIndex      = 0;
		this.useDropdown   = 0;
		this.dropdown      = null;
		this.dropdownButton = null;
		this.dropdownList  = null;

		this.containerWidth = this.tabContainer.offsetWidth;

		var tabsSize = this.tabs.length > 0 ? this.tabs[0].getBoundingClientRect() : { height: 0 };

		for (var i = 0; i < this.tabs.length; ++i)
		{
			this.tabsOffsets.push(this.tabsWidth);
			this.tabsWidth += this.tabs[i].offsetWidth;
			if (this.tabs[i].classList.contains('current')) this.currentIndex = i;
		}

		for (var i = 0; i < this.tabs.length; ++i)
		{
			if (this.tabsWidth - this.tabsOffsets[i] < this.containerWidth - 40)
			{
				this.maxIndex = i;
				break;
			}
		}

		this.tabContainer.tabScroller = this;

		if (this.tabsWidth > this.containerWidth)
		{
			if (this.tabsWidth - this.containerWidth > 200)
			{
				this.useDropdown = 1;
			}

			setStyles(this.tabContainer, {
				position: 'relative',
				overflow: 'hidden',
				padding:  '0',
				height:   (tabsSize.height + 2) + 'px'
			});
			setStyles(this.tabList, {
				position: 'absolute',
				top:      '0',
				left:     '0',
				width:    this.tabsWidth + 'px'
			});

			this.buildControls();
			this.tabIndex = this.currentIndex > this.maxIndex ? this.maxIndex : this.currentIndex;
			this.tabList.style.left = -this.tabsOffsets[this.tabIndex] + 'px';
		}
	}

	buildControls()
	{
		var div = document.createElement('div');
		setStyles(div, { position: 'absolute', bottom: '0', right: '0', 'z-index': '255' });

		var leftButton = document.createElement('img');
		leftButton.src = '/fakoli/images/tab_left_button.gif';
		leftButton.style.cursor = 'pointer';

		var self = this;
		leftButton.addEventListener('mouseenter', function() { leftButton.src = '/fakoli/images/tab_left_button_active.gif'; });
		leftButton.addEventListener('mouseleave', function() { leftButton.src = '/fakoli/images/tab_left_button.gif'; });
		leftButton.addEventListener('click', function(e) { e.preventDefault(); e.stopPropagation(); self.scroll(-1); });

		var rightButton = document.createElement('img');
		rightButton.src = '/fakoli/images/tab_right_button.gif';
		rightButton.style.cursor = 'pointer';

		rightButton.addEventListener('mouseenter', function() { rightButton.src = '/fakoli/images/tab_right_button_active.gif'; });
		rightButton.addEventListener('mouseleave', function() { rightButton.src = '/fakoli/images/tab_right_button.gif'; });
		rightButton.addEventListener('click', function(e) { e.preventDefault(); e.stopPropagation(); self.scroll(1); });

		div.appendChild(leftButton);
		div.appendChild(rightButton);

		if (this.useDropdown)
		{
			this.dropdownButton = document.createElement('img');
			this.dropdownButton.src = '/fakoli/images/tab_dropdown_button.gif';
			this.dropdownButton.style.cursor = 'pointer';

			this.dropdownButton.addEventListener('mouseenter', function() { self.dropdownButton.src = '/fakoli/images/tab_dropdown_button_active.gif'; });
			this.dropdownButton.addEventListener('mouseleave', function() { self.dropdownButton.src = '/fakoli/images/tab_dropdown_button.gif'; });
			this.dropdownButton.addEventListener('click', function(e) { e.preventDefault(); e.stopPropagation(); self.toggleDropdown(); });

			div.appendChild(this.dropdownButton);

			this.dropdown = document.createElement('div');
			this.dropdown.className = 'tabs_dropdown';
			this.dropdownList = this.tabList.cloneNode(true);
			this.dropdownList.removeAttribute('style');
			this.dropdown.appendChild(this.dropdownList);

			var doc = document.body || document.documentElement;
			doc.appendChild(this.dropdown);
		}

		this.tabContainer.appendChild(div);
	}

	scroll(dir)
	{
		this.tabIndex += dir;
		if (this.tabIndex < 0)           this.tabIndex = 0;
		if (this.tabIndex > this.maxIndex) this.tabIndex = this.maxIndex;

		// Smooth slide using CSS transition
		this.tabList.style.transition = 'left 200ms ease';
		this.tabList.style.left = -this.tabsOffsets[this.tabIndex] + 'px';
	}

	toggleDropdown()
	{
		if (this.dropdown.style.display == 'none' || this.dropdown.style.display == '')
		{
			positionRelative(this.dropdown, { relativeTo: this.dropdownButton, position: 'bottomRight', edge: 'topRight' });
			this.dropdown.style.display = 'block';
		}
		else
		{
			this.dropdown.style.display = 'none';
		}
	}
}
