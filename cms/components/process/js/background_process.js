var BackgroundProcess = new Class(
{
	Implements: [Options, Events],
	
	handler: Class.Empty,
	title:   Class.Empty,
	id:		 Class.Empty,
	dialog:	 Class.Empty,
	options:
	{
		width:  400,
		height: 'auto',
		period:	2000,
		onComplete: Class.Empty,
		hideOnComplete: true
	},
	timer:	0,
	
	initialize: function(title, handler, options)
	{
		this.handler = handler;
		this.title = title;
		this.setOptions(options);
		
		var result = httpRequest(handler);
		
		if (!result.match(/[0-9a-f]{32}/i))
		{
			alert(result);
			return;
		}
		
		this.id = result;
		
		this.dialog = modalPopup(this.title, null, this.options.width, this.options.height, true, false);
		
		this.container = new Element('div', {'class': 'process_container'});
		this.message = new Element('div', {'class': 'process_message'});
		this.percentage = new Element('div', {'class': 'process_percentage'});
		this.progressBar = new Element('div', {'class': 'process_progress_bar'});
		
		this.progressBar.setStyle('width', 0);
		
		this.container.adopt(this.message);
		this.container.adopt(this.percentage);
		this.container.adopt(this.progressBar);
		
		this.dialog.options.body.adopt(this.container);
		
		this.timer = this.updateProgress.periodical(this.options.period, this);
	},
	
	updateProgress: function()
	{
		var request = new Request.JSON(
		{
			url: '/action/process/check_progress?id=' + this.id,
			onSuccess: function(progress)
			{
				switch(progress.status)
				{
				case "Starting":
					
					break;
				
				case "Running":
					
					this.message.set('text', progress.message);
					
					percent = progress.percentage + "%";
					
					this.progressBar.setStyle('width', percent);
					this.percentage.set('text', percent);
					break;
				
				case "Completed":
					
					clearInterval(this.timer);
					if (this.options.hideOnComplete)
					{
						this.dialog.hide();
					}
					this.fireEvent("complete");
					break;
					
				case "Error":

					clearInterval(this.timer);
					this.dialog.hide();
					messagePopup("An Error Ocurred", progress.message);
					break;
				}
			}.bind(this)
		});	
		
		request.send();
	}
	
});