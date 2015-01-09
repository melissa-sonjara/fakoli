var BookmarkManager = new Class();

BookmarkManager.bookmarkPage = function(onComplete)
{
	var url = new URI(document.location.href);
	var query = url.get('query');
	if (query) query = "?" + query;
	
	var uri = url.get('directory') + url.get('file') + query;
	
	var title = document.title;
	
	BookmarkManager.onComplete = onComplete;
	BookmarkManager.bookmark_id = null;
	BookmarkManager.popup = modalPopup("Bookmark this Page", "/action/bookmark/bookmark_popup?url=" + encodeURIComponent(uri) + "&title=" + encodeURIComponent(title), '550', 'auto', true);
};

BookmarkManager.handleResponse = function(response)
{
	if (response == "OK")
	{
		var module = document.id("bookmark_link");
		if (module)
		{
			var div = module.getElement('div');
			if (div) div.set('html', "You have bookmarked this page");
		}

		if (BookmarkManager.onComplete)
		{
			BookmarkManager.onComplete();
			BookmarkManager.onComplete = false;
		}
		
		if (!BookmarkManager.bookmark_id)
		{
			notification("This page has been added to your bookmarks");
			BookmarkManager.popup.hide();
		}
		else
		{
			document.location.reload();
		}
	}
	else
	{
		var error = document.id("Bookmark_Form__error");
		error.setStyle('display', 'table-cell');
		error.setText(response);
	}
};

BookmarkManager.editBookmark = function(bookmark_id, onComplete)
{
	BookmarkManager.bookmark_id = bookmark_id;
	BookmarkManager.onComplete = onComplete;
	BookmarkManager.popup = modalPopup("Edit Bookmark", "/action/bookmark/bookmark_popup?bookmark_id=" + bookmark_id, '550', 'auto', true);
};