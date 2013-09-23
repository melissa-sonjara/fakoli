var Comment =  (function()
{
	var CommentSingleton = new Class(
	{
		dialog: null,
		
		initialize: function()
		{
		},
	
		showCommentDialog: function(comment_id, xref_class, component)
		{
			this.dialog = modalPopup('Comment', '/action/comment/comment_form?comment_id=' + comment_id + '&xref_class=' + xref_class + "&xref_component=" + component, '520px', 'auto', true);
		},
			
		commentFormResult: function(response)
		{
			if (response.indexOf("OK") == 0)
			{
				var responseFields = response.split("|");

				this.reloadCommentPanel(responseFields[1]);
			}
			else
			{
				document.id('Comment_form__error').set('html', response).setStyle('display', 'table-cell');
			}
		},
	
		
		editCommentFormResult: function(response)
		{
			if (response.indexOf("OK") == 0)
			{
				var responseFields = response.split("|");

				this.reloadCommentPanel(responseFields[1]);
			}
			else
			{
				document.id('EditComment_form__error').set('html', response).setStyle('display', 'table-cell');
			}
		},
		
		commentPublish: function(comment_id)
		{
			  var request = new Request(
					    {
					     'url':   '/action/comment/publish_comment?comment_id=' + comment_id, 
					      'method':  'get',
					      onSuccess: function(response) 
							{ 
								if(response == "OK")
								{
									this.reloadCommentPanel("");
								}
								else
									alert(response);
							}.bind(this)
						});
					    
					    request.send();
		},
		
		reloadCommentPanel: function(confirmation_message)
		{
			document.id('comment_panel').reload(function() {this.closeDialog(); this.showConfirmationMessage(confirmation_message);}.bind(this));
		},
		
		showConfirmationMessage: function(confirmation_message)
		{
			if(!confirmation_message) return;
			notification(confirmation_message);
		},
		
		closeDialog: function()
		{
			if (this.dialog)
			{
				this.dialog.hide();
				this.dialog = null;
			}			
		}
		  
	});

	var instance;
	return function()
	{
		return instance ? instance : instance = new CommentSingleton();
	};
	
	
	
})();