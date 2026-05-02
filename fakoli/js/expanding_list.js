var ExpandingList = new Class({
    Implements: [Options],

    options: {
            expandedClass: 'expanded',
            scrollOnExpand: false,
            scrollOffset: -20,
            mode: 'accordion'
    },

    initialize: function(list, options)
    {
        this.list = $el(list);
        this.setOptions(options);

        self = this;

        this.list.querySelectorAll('dt').forEach(function(dt)
        {
            dt.addEventListener('click', function()
            {
                self.toggleDD(this);
            });
        });

        this.list.querySelectorAll("dd.expanded[data-url]").forEach(function(panel)
		{
			panel.reload();
			panel.removeClass('expanded');
		});
    },

    toggleDD: function(elt)
    {
        self = this;

        elt = $el(elt);
        var body = elt.getNext("dd");
        
        if (elt.hasClass(this.options.expandedClass))
        {
			body.dissolve();
			elt.removeClass("expanded");
		}
		else
		{
            if (this.options.mode == 'accordion')
            {
                this.list.querySelectorAll("dt." + this.options.expandedClass).forEach(function(e)
                {
			        var b = e.getNext("dd"); 
                    b.dissolve(); 
                    e.removeClass(self.options.expandedClass); 
                });
			}

			if (body.get('data-url') && !body.get('html'))
			{
				var cursor = elt.getStyle('cursor');
				elt.setStyle('cursor', 'progress');
				
				body.reload(function() 
				{ 
					body.reveal();
					elt.addClass("expanded");
					elt.setStyle('cursor', cursor);

                    if (self.options.scrollOnExpand)
			        {
    					window.scrollToElement(elt, self.options.scrollOffset);
        			}
				});
			}
			else
			{
				body.reveal();
				elt.addClass("expanded");

                if (self.options.scrollOnExpand)
    			{
				    window.scrollToElement(elt, self.options.scrollOffset);
			    }
			}
		}	
	}
});
