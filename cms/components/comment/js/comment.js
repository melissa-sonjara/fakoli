var Comment =  (function()
{
	var CommentSingleton = new Class(
	{
		dialog: null,
		dialog2: null,	
		initialize: function()
		{
		},
	
		showCommentDialog: function(comment_id, xref_class, component)
		{
			this.dialog = modalPopup('Comment', '/action/comment/comment_form?comment_id=' + comment_id + '&xref_class=' + xref_class + "&xref_component=" + component, '520px', 'auto', true);
		},
			
		commentFormResult: function(response)
		{
			if (response == "OK")
			{
				window.location.reload();	
			}
			else
				$('EditComment_form__error').set('html', response);
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
									window.location.reload();
								}
								else
									alert(response);
							}.bind(this)
						});
					    
					    request.send();
		},
		
		closeDialog: function()
		{
			this.dialog.hide();
		}
		  
	});

	var instance;
	return function()
	{
		return instance ? instance : instance = new CommentSingleton();
	};
	
	
	
})();