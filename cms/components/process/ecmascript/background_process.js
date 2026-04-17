class BackgroundProcess
{
	constructor(title, handler, options)
	{
		this.handler = handler;
		this.title = title;
		this.options = Object.assign(
			{
				width: 400,
				height: 'auto',
				period: 2000,
				onComplete: null,
				hideOnComplete: true,
				closeAction: function() {}
			},
			options
		);
		this.id = null;
		this.dialog = null;
		this.timer = 0;
		this._listeners = {};

		fetch(handler)
			.then(function(r) { return r.text(); })
			.then(function(result)
			{
				if (!result.match(/[0-9a-f]{32}/i))
				{
					alert(result);
					return;
				}

				this.id = result;

				this.dialog = modalPopup(this.title, null, this.options.width, this.options.height, true, false);
				this.dialog.addEvent('hide', this.options.closeAction);

				this.container = document.createElement('div');
				this.container.className = 'process_container';

				this.message = document.createElement('div');
				this.message.className = 'process_message';

				this.percentage = document.createElement('div');
				this.percentage.className = 'process_percentage';

				this.progressBar = document.createElement('div');
				this.progressBar.className = 'process_progress_bar';

				this.progressBar.style['width'] = 0;

				this.container.appendChild(this.message);
				this.container.appendChild(this.percentage);
				this.container.appendChild(this.progressBar);

				this.dialog.options.body.appendChild(this.container);
				this.dialog.center();
				//this.dialog.show();

				this.timer = setInterval(this.updateProgress.bind(this), this.options.period);

			}.bind(this));
	}

	updateProgress()
	{
		fetch('/action/process/check_progress?id=' + this.id)
			.then(function(r) { return r.json(); })
			.then(function(progress)
			{
				switch (progress.status)
				{
				case "Starting":

					break;

				case "Running":

					this.message.textContent = progress.message;

					var percent = progress.percentage + "%";

					this.progressBar.style['width'] = percent;
					this.percentage.textContent = percent;
					break;

				case "Completed":

					clearInterval(this.timer);
					this.message.textContent = progress.message;
					this.progressBar.classList.add('completed');
					this.percentage.textContent = '100%';

					if (this.options.hideOnComplete)
					{
						this.dialog.hide();
					}
					this.fireEvent("complete");
					break;

				case "Error":

					clearInterval(this.timer);
					this.progressBar.classList.add('error');
					this.message.innerHTML = progress.message;
					break;
				}
			}.bind(this));
	}

	addEvent(event, fn)
	{
		if (!this._listeners[event]) this._listeners[event] = [];
		this._listeners[event].push(fn);
	}

	fireEvent(event)
	{
		if (this._listeners[event])
		{
			this._listeners[event].forEach(function(fn) { fn(); });
		}
		if (event === 'complete' && this.options.onComplete)
		{
			this.options.onComplete();
		}
	}
}
