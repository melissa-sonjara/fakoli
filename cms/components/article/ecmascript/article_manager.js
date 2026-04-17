var ArticleManager = (function()
{
	class ArticleManagerSingleton
	{
		constructor()
		{
			this.dialog = null;
		}

		editArticle(article_id, blog_id)
		{
			this.dialog = modalPopup('Edit Article Details', '/action/article/edit?article_id=' + article_id + "&blog_id=" + blog_id, '900px', 'auto', true);
		}

		editResult(result)
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
				var el = document.getElementById('Article_form__error');
				el.textContent = result;
				el.style['display'] = 'table-cell';
			}
		}

		closeDialog()
		{
			this.dialog.hide();
		}
	}

	var instance = null;
	return function()
	{
		return instance ? instance : instance = new ArticleManagerSingleton();
	};

})();
