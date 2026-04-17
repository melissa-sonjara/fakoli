/**
 *
 */
class TaskScheduleSelector
{
	constructor(table, control)
	{
		this.table = document.getElementById(table);
		this.control = document.getElementById(control);
		this.adding = true;

		this.status = this.table.querySelector('th');

		this.table.querySelectorAll('td.period').forEach(period =>
		{
			period.addEventListener('click', () => { this.togglePeriod(period); });
			period.addEventListener('touchstart', e => { e.stopPropagation(); e.preventDefault(); this.startTouch(period); });
			period.addEventListener('touchmove', e => { e.stopPropagation(); e.preventDefault(); this.continueTouch(period, e); });
		});
	}

	togglePeriod(period)
	{
		if (period.classList.contains('selected'))
		{
			period.classList.remove('selected');
		}
		else
		{
			period.classList.add('selected');
		}

		this.evaluateSchedule();
	}

	evaluateSchedule()
	{
		var schedule = [];

		this.table.querySelectorAll('td.period.selected').forEach(period =>
		{
			schedule.push(period.getAttribute('data-period'));
		});

		this.control.value = schedule.join(',');
	}

	startTouch(period)
	{
		this.adding = !period.classList.contains('selected');

		if (this.adding)
		{
			period.classList.add('selected');
		}
		else
		{
			period.classList.remove('selected');
		}

		this.evaluateSchedule();
	}

	continueTouch(period, event)
	{
		var touch = event.touches[0];
		var target = document.elementFromPoint(touch.clientX, touch.clientY);

		if (this.adding && !target.classList.contains('selected'))
		{
			target.classList.add('selected');
			this.evaluateSchedule();
		}
		else if (!this.adding && target.classList.contains('selected'))
		{
			target.classList.remove('selected');
			this.evaluateSchedule();
		}
	}
}
