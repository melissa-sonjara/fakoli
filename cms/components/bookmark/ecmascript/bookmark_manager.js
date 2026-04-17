var BookmarkManager = {};

BookmarkManager.bookmarkPage = function(onComplete)
{
	var location = document.location.href;
	var urlObj = new URL(location);
	var query = urlObj.search;
	var uri = urlObj.pathname + query;

	var title = document.title;

	BookmarkManager.onComplete = onComplete;
	BookmarkManager.bookmark_id = null;
	BookmarkManager.popup = modalPopup("Bookmark this Page", "/action/bookmark/bookmark_popup?url=" + encodeURIComponent(uri) + "&title=" + encodeURIComponent(title), '550', 'auto', true);
};

BookmarkManager.handleResponse = function(response)
{
	if (response == "OK")
	{
		var module = document.getElementById("bookmark_link");
		if (module)
		{
			var div = module.querySelector('div');
			if (div) div.innerHTML = "You have bookmarked this page";
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
		var error = document.getElementById("Bookmark_Form__error");
		error.style['display'] = 'table-cell';
		error.textContent = response;
	}
};

BookmarkManager.editBookmark = function(bookmark_id, onComplete)
{
	BookmarkManager.bookmark_id = bookmark_id;
	BookmarkManager.onComplete = onComplete;
	BookmarkManager.popup = modalPopup("Edit Bookmark", "/action/bookmark/bookmark_popup?bookmark_id=" + bookmark_id, '550', 'auto', true);
};
