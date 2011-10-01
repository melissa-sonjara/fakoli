var VideoGallery = new Class(
{
	id: 			"",
	scrollLeft: 	Class.Empty,
	scrollRight: 	Class.Empty,
	gallery:		Class.Empty,
	listView:		Class.Empty,
	moving:			false,
	position:		0,
	
	initialize: function(id)
	{
		this.id = id;
		this.scrollLeft = $(id + '_scroll_left');
		this.scrollRight = $(id + '_scroll_right');
	
		this.scrollLeft.setStyle('cursor', 'pointer');
		this.scrollLeft.addEvents(
		{
			'mouseover': function(e) { this.scrollLeft.src = "/fakoli/image_gallery/scroll_left_hover.png"; }.bind(this),
			'mouseout': function(e) { this.scrollLeft.src = "/fakoli/image_gallery/scroll_left.png"; }.bind(this),
			'click': function(e) { new Event(e).stop(); this.scroll(-1); }.bind(this)
		});
		
		this.scrollRight.setStyle('cursor', 'pointer');
		this.scrollRight.addEvents(
		{
			'mouseover': function(e) { this.scrollRight.src = "/fakoli/image_gallery/scroll_right_hover.png"; }.bind(this),
			'mouseout': function(e) { this.scrollRight.src = "/fakoli/image_gallery/scroll_right.png"; }.bind(this),
			'click': function(e) { new Event(e).stop(); this.scroll(1); }.bind(this)
		});
		
		this.gallery = $(id);
		this.listView = $(id + "_listView");
		this.normalizePosition();
	},
	
	scroll: function(direction)
	{				
		var distance = direction * 185;
	
		this.position += distance;
		
		this.normalizePosition();
		
		this.listView.tween('left', -this.position - 5);
	},
	
	normalizePosition: function()
	{
		if (this.position <= 0)
		{
			this.position = 0;
			this.scrollLeft.tween('display', 'none');
		}
		else
		{
			this.scrollLeft.setStyle('display', 'block');
			
			if (this.position >= this.listView.getWidth() - 560)
			{
				this.scrollRight.setStyle('display', 'none');
			}
			else
			{
				this.scrollRight.setStyle('display', 'block');
			}
		}
	}	

});
