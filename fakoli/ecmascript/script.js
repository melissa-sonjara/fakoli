function toggleContextHelp(eltName)
{
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	elt.style.display = (elt.style.display == 'block') ? 'none' : 'block';
}

function showContextHelp(eltName)
{
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	elt.style.display = 'block';
}

function hideContextHelp(eltName)
{
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	elt.style.display = 'none';
}

function toggleArticlePublished(img, id)
{
	var publish = (img.alt == "Published") ? 0 : 1;

	fetch("/action/article/update?article_id=" + id + "&published=" + publish)
	.then(function(r) { return r.text(); })
	.then(function(response)
	{
		if (response == 1)
		{
			img.src = publish ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
			img.alt = publish ? "Published" : "Unpublished";
		}
	});
}

function toggleGalleryPublished(img, id)
{
	var publish = (img.alt == "Published") ? 0 : 1;

	fetch("/action/image/update?gallery_id=" + id + "&published=" + publish)
	.then(function(r) { return r.text(); })
	.then(function(response)
	{
		if (response == 1)
		{
			img.src = publish ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
			img.alt = publish ? "Published" : "Unpublished";
		}
	});
}

function toggleBlogPublished(img, id)
{
	var publish = (img.alt == "Published") ? 0 : 1;

	fetch("/action/blog/update?blog_id=" + id + "&published=" + publish)
	.then(function(r) { return r.text(); })
	.then(function(response)
	{
		if (response == 1)
		{
			img.src = publish ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
			img.alt = publish ? "Published" : "Unpublished";
		}
	});
}
