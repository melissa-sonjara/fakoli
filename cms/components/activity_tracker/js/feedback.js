var FeedbackPanel = (function()
{
	var FeedbackPanelSingleton = new Class(
	{
		openPanel:	 null,
		closedPanel: null,
		closePanelLink: null,
		sendLink: null,
		textarea: null,
		thanks: null,
		
		initialize: function()
		{
			this.openPanel = document.id('feedback_open');
			if (!this.openPanel) return;
			
			this.closedPanel = document.id('feedback_closed');
			this.closePanelLink = document.id('feedback_open_title');
			this.sendLink = document.id('feedback_send_button');
			this.textarea = document.id('feedback_textarea');
			this.thanks = document.id('feedback_thanks');
			
			this.closePanelLink.addEvent('click', function(e) { new Event(e).stop(); this.closeFeedback(); }.bind(this));
			this.closedPanel.addEvent('click', function(e) { new Event(e).stop(); this.openFeedback(); }.bind(this));
			this.sendLink.addEvent('click', function(e) { new Event(e).stop(); this.sendFeedback(); }.bind(this));
		},
		
		closeFeedback: function()
		{
			this.openPanel.setStyle('display', 'none');
			this.closedPanel.setStyle('display', 'block');
		},
		
		openFeedback: function()
		{
			this.openPanel.setStyle('display', 'block');
			this.closedPanel.setStyle('display', 'none');
		},
		
		sendFeedback: function()
		{
    		var request = new Request.HTML(
    		{
    			evalScripts: false,
    			evalResponse: false,
    			method: 'post', 
    			url: "/action/activity_tracker/save", 
    			data: "feedback=" + encodeURIComponent(this.textarea.value),
    			onSuccess: function(tree, elements, html, script) 
    			{ 
    				this.feedbackSent();
    			}.bind(this)
    		});
    		request.send();
		},
		
		feedbackSent: function()
		{
			this.textarea.fade('out');
			this.thanks.setStyles({'display': 'block', 'opacity': 0});
			this.thanks.fade('in');
			this.clearFeedback.bind(this).delay(2500);
		},
		
		clearFeedback: function()
		{
			this.closeFeedback();
			this.textarea.setStyle('opacity', 1);
			this.thanks.setStyles({'opacity': 0, 'display': 'none'});
			this.textarea.value = "";
		}
	});

	var instance;
	return function()
	{
		return instance ? instance : instance = new FeedbackPanelSingleton();
	};
	
})();

window.addEvent('domready', function()
{
	new FeedbackPanel();
});