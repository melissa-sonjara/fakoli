var ForumSubscriptionManager = new Class(
{
	forum_id: null,
	topic_id: null,
	forum_subscription_id: null,
	dialog_title: null,
	
	initialize: function(forum_id, topic_id, forum_subscription_id, dialog_title)
	{
		this.forum_id = forum_id;
		this.topic_id = topic_id;
		this.forum_subscription_id = forum_subscription_id;
		this.dialog_title = dialog_title;
	},
	
	showSubscriptionDialog: function()
	{
		this.dialog = modalPopup(this.dialog_title, '/action/forum/subscription_dialog?forum_id=' + this.forum_id + '&topic_id=' + this.topic_id + '&forum_subscription_id=' + this.forum_subscription_id,  '600px', 'auto', true);
	},
	
	subscriptionDialogResult: function(response)
	{
		if (response == "OK")
		{
			window.location.reload();
		}
		else
		{
			$('ForumSubscription_form__error').set('html', response);		
			$('ForumSubscription_form__error').setStyle('display', 'table-cell');		
		}
	},
	
	closeDialog: function()
	{
		this.dialog.hide();		
	}
	
});