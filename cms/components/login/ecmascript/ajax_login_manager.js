class AjaxLoginManager
{
	constructor(form, error, options)
	{
		this.form = null;
		this.error = null;
		this.options =
		{
			onSuccess: function(redirect) { return true; },
			onError: function(error) { return true; }
		};

		this.form = document.getElementById(form);
		this.error = document.getElementById(error);
		Object.assign(this.options, options);

		this.form.iFrameFormRequest(
		{
			'onRequest': function() { return true; },
			'onComplete': (response) => { this.onLoginResponse(response); },
			'onFailure': function(error) { alert(error); }
		});
	}

	onLoginResponse(response)
	{
		var params = response.split('|');
		if (params[0] == "OK")
		{
			if (this.options.onSuccess(params[1]))
			{
				go(params[1]);
			}
		}
		else if (params[0] == "REDIRECT")
		{
			go(params[1]);
		}
		else
		{
			if (this.options.onError(params[1]))
			{
				this.error.innerHTML = params[1];
				this.error.style['display'] = 'block';
			}
		}
	}
}
