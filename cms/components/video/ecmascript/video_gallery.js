class VideoGallery
{
	constructor(id)
	{
		this.id          = id;
		this.scrollLeft  = null;
		this.scrollRight = null;
		this.gallery     = null;
		this.listView    = null;
		this.moving      = false;
		this.position    = 0;

		this.scrollLeft  = document.getElementById(id + '_scroll_left');
		this.scrollRight = document.getElementById(id + '_scroll_right');

		this.scrollLeft.style.cursor = 'pointer';

		var self = this;

		this.scrollLeft.addEventListener('mouseover', function() { self.scrollLeft.src = "/components/video/images/scroll_left_hover.png"; });
		this.scrollLeft.addEventListener('mouseout',  function() { self.scrollLeft.src = "/components/video/images/scroll_left.png"; });
		this.scrollLeft.addEventListener('click', function(e)
		{
			e.preventDefault();
			e.stopPropagation();
			self.scroll(-1);
		});

		this.scrollRight.style.cursor = 'pointer';

		this.scrollRight.addEventListener('mouseover', function() { self.scrollRight.src = "/components/video/images/scroll_right_hover.png"; });
		this.scrollRight.addEventListener('mouseout',  function() { self.scrollRight.src = "/components/video/images/scroll_right.png"; });
		this.scrollRight.addEventListener('click', function(e)
		{
			e.preventDefault();
			e.stopPropagation();
			self.scroll(1);
		});

		this.gallery  = document.getElementById(id);
		this.listView = document.getElementById(id + "_listView");

		this.listView.style.transition = 'left 300ms ease-in-out';

		this.normalizePosition();
	}

	scroll(direction)
	{
		var distance = direction * 185;
		this.position += distance;
		this.normalizePosition();
		this.listView.style.left = (-this.position - 5) + 'px';
	}

	normalizePosition()
	{
		if (this.position <= 0)
		{
			this.position = 0;
			this.scrollLeft.style.display = 'none';
		}
		else
		{
			this.scrollLeft.style.display = 'block';

			if (this.position >= this.listView.offsetWidth - 560)
			{
				this.scrollRight.style.display = 'none';
			}
			else
			{
				this.scrollRight.style.display = 'block';
			}
		}
	}
}
