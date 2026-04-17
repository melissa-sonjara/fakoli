class DynamicTabBar extends EventEmitter
{
    constructor(tabBar, panel, options)
    {
        super();
        this.tabBar = $el(tabBar);
        this.panel = $el(panel);
        addReloadHandler(this.panel);

        this.options = Object.assign({
			dynamicLoadHandler: null,
            loadFirstTabOnLoad: false,
            tabLinkSelector:    "a"
		}, options || {});

        self = this;
        this.tabBar.querySelectorAll(this.options.tabLinkSelector).forEach(function(a)
		{
			a.loadTab = function(override)
            {
                if (self.options.dynamicLoadHandler)
                {
                    self.options.dynamicLoadHandler(override ? override : this.href);
                }
                else
                {
                    self.panel.load(override ? override : this.href);
                }
            };

			a.addEventListener('click', function(e) 
			{
				new DOMEvent(e).stop();
				var url = a.href;
				var parent = a.getParent();
				self.tabBar.querySelectorAll("li").forEach(function(l) { l.removeClass("current"); });
				parent.addClass('current');
				
				a.loadTab();			
			});
		});

        if (this.options.loadFirstTabOnLoad)
        {
            this.tabBar.querySelector("li").addClass('current');
            this.tabBar.querySelector("a").loadTab();
        }
    }
}