/**
 * PageRating handler
 */

class PageRating
{
	constructor(container, url, rating, numberOfRatings, options)
	{
		this.container = document.getElementById(container);
		this.url = url;
		this.rating = rating;
		this.numberOfRatings = numberOfRatings;
		this.stars = [];

		this.options = Object.assign(
		{
			emptyIcon: 		"/components/rating/images/star_empty.png",
			halfIcon: 		"/components/rating/images/star_half.png",
			fullIcon:		"/components/rating/images/star_full.png",
			ratingMaximum: 	5,
			readOnly:		true,
			title:			"Rate this Page",
			readOnlyTitle:	"Page Rating",
			loginMessage:	"Log in to rate this Page"
		}, options);

		if (!this.container) return;

		this.buildRating();
	}

	buildRating()
	{
		var span = document.createElement('span');
		span.className = 'page_rating_title';
		span.textContent = this.options.readOnly ? this.options.readOnlyTitle : this.options.title;
		this.container.appendChild(span);

		for (var i = 0; i < this.options.ratingMaximum; ++i)
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

			var img = document.createElement('img');
			img.src = icon;
			img.alt = '';
			Object.assign(img.style, {'verticalAlign': 'middle', 'display': 'inline-block'});

			if (!this.options.readOnly)
			{
				img.rating = i + 1;
				img.originalIcon = icon;
				img.style.cursor = 'pointer';
				img.addEventListener('mouseover', () => { this.highlightRating(img); });
				img.addEventListener('mouseleave', () => { this.originalRating(); });
				img.addEventListener('click', () => { this.selectRating(img); });
			}

			this.container.appendChild(img);
			this.stars.push(img);
		}
	}

	highlightRating(img)
	{
		if (this.options.readOnly) return;

		var rating = img.rating;
		for (var i = 0; i < rating; ++i)
		{
			this.stars[i].src = this.options.fullIcon;
		}

		for (i = rating; i < this.options.ratingMaximum; ++i)
		{
			this.stars[i].src = this.options.emptyIcon;
		}
	}

	selectRating(img)
	{
		if (this.options.readOnly) return;

		var rating = img.rating;
		var url = encodeURIComponent(this.url);

		fetch("/action/rating/rate_page?url=" + url + "&rating=" + rating)
			.then(r => r.text())
			.then(response =>
			{
				var result = JSON.parse(response);

				if (result.url == this.url)
				{
					this.rating = result.average_rating;
					this.numberOfRatings = result.number_of_ratings;
					this.options.readOnly = true;
					this.container.innerHTML = '';
					this.buildRating();
				}
			});
	}

	originalRating()
	{
		if (this.options.readOnly) return;

		for (var i = 0; i < this.options.ratingMaximum; ++i)
		{
			this.stars[i].src = this.stars[i].originalIcon;
		}
	}
}
