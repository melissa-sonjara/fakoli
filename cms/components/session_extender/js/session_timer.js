var SessionTimer =  (function()
{
	var SessionTimerSingleton = new Class(
	{
		Implements: [Options],
		
		options:
		{
			message: "We haven't heard from you in a while. Do you want to extend your session?",
			timeout: 8000
		},
		
		dialog: null,
		timer: null,
		//timeout: null, 
			
		initialize: function(options)
		{
			this.setOptions(options);
		},
			
		showExtendSessionDialog: function()
		{
			if (confirm(this.options.message))
			{		
				var request = new Request(
				{
					'url': 		'/action/session_extender/extend_session', 
					 'method': 	'get',
					 'onSuccess': function(response) 
					 {
						 this.resetTimer();
					 }.bind(this),
				
					 'onFailure':	function() { alert("Failed to communicate with server");}
				 });
				
				request.send();				
			}	
			else 
			{
			 go('/logout');	
				
			}
		},
		
		resetTimer: function()
		{
			//clearTimeout(timer);
			timer = this.showExtendSessionDialog.delay(this.options.timeout);
			
		},
			
		closeDialog: function()
		{
			this.dialog.hide();
		}
		  
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new SessionTimerSingleton();
	};
	
})();