class ImageGrid
{
	constructor(grid, size, options)
	{
		this.grid = null;
		this.size = 200;
		this.options =
		{
			puff: true,
		};

		Object.assign(this.options, options);

		this.size = size;
		this.grid = document.getElementById(grid);

		if (!this.grid) return;

		if (this.options.puff)
		{
			this.grid.querySelectorAll('li').forEach((elt) =>
			{
				elt.addEventListener('mouseover', (e) => { e.preventDefault(); e.stopPropagation(); this.puff(elt); });
				elt.addEventListener('mouseout', (e) => { e.preventDefault(); e.stopPropagation(); this.unpuff(elt); });
			});
		}

		if (this.grid.facetManager)
		{
			this.grid.facetManager.addEventListener('filterChanged', () => { this.filterChanged(); });
			this.grid.facetManager.addEventListener('filterCleared', () => { this.filterCleared(); });
			this.preprocessFacets();
		}
	}

	puff(elt)
	{
		var div = elt.querySelector('div');
		var img = elt.querySelector('img');
		Object.assign(elt.style, {'width': (this.size + 30) + 'px', 'height': (this.size + 60) + 'px', 'margin': '0'});
		Object.assign(div.style, {'width': (this.size + 20) + 'px', 'height': (this.size + 60) + 'px'});
		Object.assign(img.style, {'width': (this.size + 20) + 'px', 'height': (this.size + 20) + 'px'});
	}

	unpuff(elt)
	{
		var div = elt.querySelector('div');
		var img = elt.querySelector('img');
		Object.assign(elt.style, {'width': (this.size + 10) + 'px', 'height': (this.size + 40) + 'px', 'margin': '10px'});
		Object.assign(div.style, {'width': this.size + 'px', 'height': (this.size + 40) + 'px'});
		Object.assign(img.style, {'width': this.size + 'px', 'height': this.size + 'px'});
	}

	preprocessFacets()
	{
		this.grid.querySelectorAll('li').forEach((elt) =>
		{
			this.grid.facetManager.preprocess(elt);
		});
	}

	filterChanged()
	{
		this.grid.querySelectorAll('li').forEach((elt) =>
		{
			elt.classList.remove("filtered");
			elt.classList.remove("filtermatch");

			var match = this.grid.facetManager.filter(elt);

			if (match)
			{
				elt.classList.add('filtermatch');
				elt.style['display'] = 'block';
				elt.style.transition = 'opacity 0.3s';
				elt.style.opacity = 1;
			}
			else
			{
				elt.classList.add('filtered');
				elt.style.transition = 'opacity 0.3s';
				elt.style.opacity = 0;
				elt.addEventListener('transitionend', function handler()
				{
					elt.style['display'] = 'none';
					elt.removeEventListener('transitionend', handler);
				});
			}
		});
	}

	filterCleared()
	{
		this.grid.querySelectorAll('li').forEach((elt) =>
		{
			elt.classList.remove("filtered");
			elt.classList.remove("filtermatch");
			elt.style['display'] = 'block';
			elt.style.transition = 'opacity 0.3s';
			elt.style.opacity = 1;
		});
	}
}
