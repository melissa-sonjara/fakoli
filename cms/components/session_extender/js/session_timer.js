/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/
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
			timer = this.showExtendSessionDialog.delay(this.options.timeout, this);
			
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