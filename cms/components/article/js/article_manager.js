var ArticleManager = (function()
{
	var ArticleManagerSingleton = new Class(
	{
		dialog: null,
		
		initialize: function()
		{
		},
		
		editArticle: function(article_id, blog_id)
		{
			this.dialog = modalPopup('Edit Article Details', '/action/article/edit?article_id=' + article_id + "&blog_id=" + blog_id, '900px', 'auto', true);
		},
		
		editResult: function(result)
		{
			if (result == "OK")
			{
				this.closeDialog();
				window.location.reload();
			}
			else if (result == "DELETED")
			{
				var url = window.location.href.replace(/\?.*$/, "");
				window.location.href = url;
			}
			else
			{
				$('Article_form__error').set({'text': result, 'display': 'table-cell'});
			}
		},
		
		closeDialog: function()
		{
			this.dialog.hide();		
		},
		
	});
	
	var instance = null;
	return function()
	{
		return instance ? instance : instance = new ArticleManagerSingleton();
	};
	
})();
