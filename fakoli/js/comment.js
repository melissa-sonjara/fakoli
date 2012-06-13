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
			this.dialog = modalPopup('Comment', '/action/comment/comment_form?comment_id=' + comment_id + '&xref_class=' + xref_class + "&component=" + component, '520px', 'auto', true);
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