var ScrollingTabs = new Class(
{
	tabContainer: Class.Empty,
	tabList: Class.Empty,
	tabs: Class.Empty,
	
	tabsWidth: 0,
	tabsOffsets: [],
	tabIndex: 0,
	currentIndex: 0,
	containerWidth: 0,
	
	initialize: function(tabContainer)
	{
		this.tabContainer = $(tabContainer);
		this.tabList = this.tabContainer.getElement("ul");
		this.tabs = this.tabList.getChildren("li");
		
		this.containerWidth = this.tabContainer.getDimensions().width;
		
		var tabsSize = this.tabs[0].getCoordinates();
		
		this.tabsWidth = 0;
		
		for(var i = 0; i < this.tabs.length; ++i)
		{
			var coords = this.tabs[i].getDimensions();
			this.tabsOffsets.push(this.tabsWidth);
			this.tabsWidth += coords.width;
			
			if (this.tabs[i].hasClass('current')) this.currentIndex = i;
		}
		
		for(var i = 0; i < this.tabs.length; ++i)
		{
			if (this.tabsWidth - this.tabsOffsets[i] < this.containerWidth - 40) 
			{
				this.maxIndex = i;
				break;
			}
		}
		
		this.tabContainer.setStyles({position: 'relative', overflow: 'hidden', padding: 0, height: tabsSize.height + 2});
		this.tabList.setStyles({position: 'absolute', top: 0, left: 0, width: this.tabsWidth});
		
		this.tabContainer.tabScroller = this;
		
		if (this.tabsWidth > this.containerWidth)
		{
			this.buildControls();
			this.tabIndex = this.currentIndex > this.maxIndex ? this.maxIndex : this.currentIndex;
			
			this.tabList.setStyle('left', -this.tabsOffsets[this.tabIndex]);
		}
	},
	
	buildControls: function()
	{
		var div = new Element('div');
		div.setStyles({position: 'absolute', bottom: 0, right: 0, 'z-index': 255});
		
		var leftButton = new Element('img', {'src': '/fakoli/images/tab_left_button.gif'});
		leftButton.setStyles({'cursor': 'pointer'});
		
		leftButton.addEvents(
		{
			'mouseenter': function(e) { new Event(e).stop(); this.set('src', '/fakoli/images/tab_left_button_active.gif');},
			'mouseleave':  function(e) { new Event(e).stop(); this.set('src', '/fakoli/images/tab_left_button.gif');},
			'click': 	 function(e) { new Event(e).stop(); this.scroll(-1);}.bind(this)
		});
		
		var rightButton = new Element('img', {'src': '/fakoli/images/tab_right_button.gif'});
		rightButton.setStyles({'cursor': 'pointer'});
		
		rightButton.addEvents(
		{
			'mouseenter': function(e) { new Event(e).stop(); this.set('src', '/fakoli/images/tab_right_button_active.gif');},
			'mouseleave':  function(e) { new Event(e).stop(); this.set('src', '/fakoli/images/tab_right_button.gif');},
			'click': 	 function(e) { new Event(e).stop(); this.scroll(1);}.bind(this)
		});

		leftButton.inject(div);
		rightButton.inject(div);
		div.inject(this.tabContainer);
	},
	
	scroll: function(dir)
	{
		this.tabIndex += dir;

		if (this.tabIndex < 0) this.tabIndex = 0;
		if (this.tabIndex > this.maxIndex) this.tabIndex = this.maxIndex;
		this.tabList.tween('left', -this.tabsOffsets[this.tabIndex]);
	}
	
});