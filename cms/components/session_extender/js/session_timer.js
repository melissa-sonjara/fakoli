var SessionTimer =  (function()
{
	var SessionTimerSingleton = new Class(
	{
		dialog: null,
		timer: null,
		//timeout: null, 
			
		initialize: function()
		{
		  timer = 8000;
		},
			
		showExtendSessionDialog: function()
		{
			var msg = "You have been inactive, do you want to extend your session?";
			
			if (confirm(msg))
			{		
				var request = new Request(
				{
					'url': 		'/action/session_extender/extend_session', 
					 'method': 	'get',
					 'onSuccess': function(response) 
					{ 
					if (response == "OK") 
					{
						window.location.reload();
					}
				 },
				
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
			timer = this.showExtendSessionDialog.delay(8000);
			
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