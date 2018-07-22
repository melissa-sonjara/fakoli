var AjaxLoginManager = new Class({
	
	Implements: [Options],
	form: null,
	error: null,
	
	options:
	{
		onSuccess: function(redirect) { return true; },
		onError: function(error) { return true; }
	},
	
	initialize: function(form, error, options)
	{
		this.form = document.id(form);
		this.error = document.id(error);
		this.setOptions(options);
		
		this.form.iFrameFormRequest(
		{
			'onRequest': 	function() { return true; }, 
			'onComplete': 	function(response) { this.onLoginResponse(response); }.bind(this),
			'onFailure':	function(error) { alert(error); }
		});
	},
	
	onLoginResponse: function(response)
	{
		var params = response.split('|');
		if (params[0] == "OK")
		{
			if (this.options.onSuccess.attempt(params[1]))
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
			if (this.options.onError.attempt(params[1]))
			{
				this.error.set('html', params[1]).setStyle('display', 'block');
			}
		}
	}
});