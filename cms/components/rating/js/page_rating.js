/**
 * PageRating handler
 */

var PageRating = new Class(
{
	container: 		 Class.Empty,
	url: 			 "",
	rating: 		 0,
	numberOfRatings: 0,
	
	Implements: Options,
	
	stars:	[],
	
	options:
	{
		emptyIcon: 		"/components/rating/images/star_empty.png",
		halfIcon: 		"/components/rating/images/star_half.png",
		fullIcon:		"/components/rating/images/star_full.png",
		ratingMaximum: 	5,
		readOnly:		true,
		title:			"Rate this Page",
		readOnlyTitle:	"Page Rating",
		loginMessage:	"Log in to rate this Page"
	},
	
	initialize: function(container, url, rating, numberOfRatings, options)
	{
		this.setOptions(options);
		this.container = document.id(container);
		
		this.url = url;
		this.rating = rating;
		this.numberOfRatings = numberOfRatings;
		
		if (!this.container) return;

		this.buildRating();
	},
	
	buildRating: function()
	{
		var span = new Element('span', {'class': 'page_rating_title'});
		span.set('text', this.options.readOnly ? this.options.readOnlyTitle : this.options.title);
		span.inject(this.container);
		
		for(var i = 0; i < this.options.ratingMaximum; ++i)
		{
			var icon;

			if (this.rating - i >= 1.0)
			{
				icon = this.options.fullIcon;
			}
			else if (this.rating - i >= 0.5)
			{
				icon = this.options.halfIcon;
			}
			else
			{
				icon = this.options.emptyIcon;
			}
			
			var self = this;
			
			var img = new Element('img', {'src': icon, 'alt': ''});
			img.setStyles({'vertical-align': 'middle', 'display': 'inline-block'});
			if (!this.options.readOnly)
			{
				img.rating = i + 1;
				img.originalIcon = icon;
				img.setStyle('cursor', 'pointer');
				img.addEvent('mouseover', function() { self.highlightRating(this); });
				img.addEvent('mouseleave', function() { self.originalRating(); });
				img.addEvent('click', function() { self.selectRating(this); });
			}
			img.inject(this.container);
			this.stars.push(img);
		}
	},
	
	highlightRating: function(img)
	{
		if (this.options.readOnly) return;
		
		var rating = img.rating;
		for(var i = 0; i < rating; ++i)
		{
			this.stars[i].src = this.options.fullIcon;
		}
	
		for(i = rating; i < this.options.ratingMaximum; ++i)
		{
			this.stars[i].src = this.options.emptyIcon;
		}
	},
	
	selectRating: function(img)
	{
		if (this.options.readOnly) return;
		
		var rating = img.rating;
		var url = encodeURIComponent(this.url);
		
		var request = new Request(
		{
			method: 'get', 
			url: "/action/rating/rate_page?url=" + url + "&rating=" + rating, 
			onSuccess: function(response) 
			{ 
				var result = JSON.decode(response);
				
				if (result.url == this.url)
				{
					this.rating = result.average_rating;
					this.numberOfRatings = result.number_of_ratings;
					this.options.readOnly = true;
					this.container.set('html', '');
					this.buildRating();
				}
			}.bind(this)
		});

		request.send();
	},
	
	originalRating: function()
	{
		if (this.options.readOnly) return;
		
		for(var i = 0; i < this.options.ratingMaximum; ++i)
		{
			this.stars[i].src = this.stars[i].originalIcon;
		}
	}
});