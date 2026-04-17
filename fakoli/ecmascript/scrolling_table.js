/**
 * Fixes column widths on a scrollable table so the header row stays aligned
 * with a vertically scrolling tbody, adjusting on window resize.
 */
class ScrollingTable
{
	constructor(table)
	{
		var t = $el(table);
		if (!t) return;

		this.table = t;
		window.addEventListener('resize', function() { this.resize(); }.bind(this));
		window.addEventListener('load',   function() { this.resize(); }.bind(this));

		this.resize();
	}

	resize()
	{
		var thead = this.table.querySelector('thead');
		var tbody = this.table.querySelector('tbody');

		var ths = Array.from(thead.querySelectorAll('th'));
		var widths = ths.map(function(th) { return th.offsetWidth; });

		// Adjust first column for scrollbar (non-mac only)
		var isMac = /mac/i.test(navigator.platform);
		widths[0] -= isMac ? 0 : 17;

		thead.style.display = 'block';
		var headerRow = thead.querySelector('tr');
		if (headerRow)
		{
			headerRow.style.display = 'block';
			headerRow.style.width   = '100%';
		}

		ths.forEach(function(th, idx)
		{
			var w = widths[idx];
			if (idx == widths.length - 1)
			{
				w += isMac ? 0 : 17;
			}
			th.style.width = w + 'px';
		});

		Array.from(tbody.querySelectorAll('tr')).forEach(function(tr)
		{
			var tds = Array.from(tr.querySelectorAll('td'));
			var idx = 0;
			tds.forEach(function(td)
			{
				var w = 0;
				var colspan = td.colSpan || 1;
				while (colspan--)
				{
					w += widths[idx++];
				}
				td.style.width = w + 'px';
			});
		});
	}
}
