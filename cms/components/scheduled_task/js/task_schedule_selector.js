/**
 * 
 */
var TaskScheduleSelector = new Class(
{
	table: 		null,
	control:	null,
	
	adding: 	true,
	
	initialize: function(table, control)
	{
		this.table = document.id(table);
		this.control = document.id(control);
		
		this.status = this.table.getElement('th');
		
		this.table.getElements('td.period').each(function(period)
		{
			period.addEvent('click', function() { this.togglePeriod(period); }.bind(this));
			period.addEvent('touchstart', function(e) { new DOMEvent(e).stop(); this.startTouch(period); }.bind(this));
			period.addEvent('touchmove', function(e) { new DOMEvent(e).stop(); this.continueTouch(period, e); }.bind(this));
		}.bind(this));
	},
	
	togglePeriod: function(period)
	{
		if (period.hasClass('selected'))
		{
			period.removeClass('selected');
		}
		else
		{
			period.addClass('selected');
		}
		
		this.evaluateSchedule();
	},
	
	evaluateSchedule: function()
	{
		var schedule = [];
		
		this.table.getElements('td.period.selected').each(function(period)
		{
			schedule.push(period.get('data-period'));
		});
		
		this.control.value = schedule.join(',');
	},
	
	startTouch: function(period)
	{
		this.adding = !period.hasClass('selected');
		
		if (this.adding)
		{
			period.addClass('selected');
		}
		else
		{
			period.removeClass('selected');
		}
		
		this.evaluateSchedule();
	},
	
	continueTouch: function(period, event)
	{
		target = document.id(document.elementFromPoint(event.client.x, event.client.y));
		
		if (this.adding && !target.hasClass('selected'))
		{
			target.addClass('selected');
			this.evaluateSchedule();
		}
		else if (!this.adding && target.hasClass('selected'))
		{
			target.removeClass('selected');
			this.evaluateSchedule();
		}
	}
});