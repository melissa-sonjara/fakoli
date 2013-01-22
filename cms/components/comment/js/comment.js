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
				this.reloadCommentPanel();
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
									this.reloadCommentPanel();
								}
								else
									alert(response);
							}.bind(this)
						});
					    
					    request.send();
		},
		
		reloadCommentPanel: function()
		{
			var params = new URI().get('query');
			var request = new Request.HTML(
			{
				url: "/action/comment/comment_panel?" + params,
				evalScripts: false,
				evalResponse: false,
				method: 'get', 
				onSuccess: function(tree, elements, html, script) 
				{ 
					document.id('comment_panel').set('html', html);

					if (Interstitial.current) 
					{
						Interstitial.current.hide();
					}
				
					$exec(script);
					this.closeDialog();
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