var FeedbackPanel = (function()
{
	class FeedbackPanelSingleton
	{
		constructor()
		{
			this.openPanel = null;
			this.closedPanel = null;
			this.closePanelLink = null;
			this.sendLink = null;
			this.textarea = null;
			this.thanks = null;

			this.openPanel = document.getElementById('feedback_open');
			if (!this.openPanel) return;

			this.closedPanel = document.getElementById('feedback_closed');
			this.closePanelLink = document.getElementById('feedback_open_title');
			this.sendLink = document.getElementById('feedback_send_button');
			this.textarea = document.getElementById('feedback_textarea');
			this.thanks = document.getElementById('feedback_thanks');

			this.closePanelLink.addEventListener('click', (e) => { e.preventDefault(); e.stopPropagation(); this.closeFeedback(); });
			this.closedPanel.addEventListener('click', (e) => { e.preventDefault(); e.stopPropagation(); this.openFeedback(); });
			this.sendLink.addEventListener('click', (e) => { e.preventDefault(); e.stopPropagation(); this.sendFeedback(); });
		}

		closeFeedback()
		{
			this.openPanel.style['display'] = 'none';
			this.closedPanel.style['display'] = 'block';
		}

		openFeedback()
		{
			this.openPanel.style['display'] = 'block';
			this.closedPanel.style['display'] = 'none';
		}

		sendFeedback()
		{
			fetch("/action/activity_tracker/save",
			{
				method: 'POST',
				headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
				body: "feedback=" + encodeURIComponent(this.textarea.value)
			})
			.then(r => r.text())
			.then(() => { this.feedbackSent(); });
		}

		feedbackSent()
		{
			this.textarea.style.transition = 'opacity 300ms';
			this.textarea.style.opacity = '0';
			Object.assign(this.thanks.style, { 'display': 'block', 'opacity': '0' });
			this.thanks.style.transition = 'opacity 300ms';
			this.thanks.style.opacity = '1';
			setTimeout(() => { this.clearFeedback(); }, 2500);
		}

		clearFeedback()
		{
			this.closeFeedback();
			this.textarea.style.opacity = '1';
			Object.assign(this.thanks.style, { 'opacity': '0', 'display': 'none' });
			this.textarea.value = "";
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new FeedbackPanelSingleton();
	};

})();

document.addEventListener('DOMContentLoaded', function()
{
	new FeedbackPanel();
});
