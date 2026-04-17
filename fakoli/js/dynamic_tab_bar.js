var DynamicTabBar = new Class(
{
	Implements: [Options, Events],

	options:
	{
		dynamicLoadHandler: null,
		loadFirstTabOnLoad: false,
		tabLinkSelector:    'a'
	},

	initialize: function(tabBar, panel, options)
	{
		this.tabBar = document.id(tabBar);
		this.panel  = document.id(panel);
		this.setOptions(options);

		// Add load/reload methods to the panel element if not already present,
		// equivalent to addReloadHandler() in the ES version.
		var p = this.panel;
		if (!p.load)
		{
			p.load = function(url, onComplete)
			{
				new Request.HTML(
				{
					url:        url,
					update:     p,
					onComplete: onComplete || function(){}
				}).get();
			};
		}
		if (!p.reload)
		{
			p.reload = function(onComplete)
			{
				p.load(p.get('data-url'), onComplete);
			};
		}

		var self = this;
		this.tabBar.getElements(this.options.tabLinkSelector).each(function(a)
		{
			a.loadTab = function(override)
			{
				if (self.options.dynamicLoadHandler)
				{
					self.options.dynamicLoadHandler(override ? override : a.get('href'));
				}
				else
				{
					self.panel.load(override ? override : a.get('href'));
				}
			};

			a.addEvent('click', function(e)
			{
				e.stop();
				self.tabBar.getElements('li').each(function(l) { l.removeClass('current'); });
				a.getParent().addClass('current');
				a.loadTab();
			});
		});

		if (this.options.loadFirstTabOnLoad)
		{
			this.tabBar.getElement('li').addClass('current');
			this.tabBar.getElement('a').loadTab();
		}
	}
});
