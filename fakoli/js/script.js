
function toggleContextHelp(eltName)
{
	if (!document.getElementById) return null;
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	if (elt.style.display == 'block')
	{
		elt.style.display = 'none';
	}
	else
	{
		elt.style.display = 'block';
	}
}

function showContextHelp(eltName)
{
	if (!document.getElementById) return null;
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	elt.style.display = 'block';
}

function hideContextHelp(eltName)
{
	if (!document.getElementById) return null;
	var elt = document.getElementById(eltName);
	if (elt == null) return;
	elt.style.display = 'none';
}


function toggleArticlePublished(img, id)
{
	var enable;
	if (img.alt == "Published")
	{
		publish = 0;
	}
	else
	{
		publish = 1;
	}
	
	var request = new Request({
									url: "/action/article/update?article_id=" + id +"&published=" + publish,
									method: 'get',
									onSuccess: function (response) 
									{ 
										if (response == 1) 
										{
											img.src = publish ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
											img.alt = publish ? "Published" : "Unpublished";
										}
									},
									onFailure: function () {},
									async: true
								});
	request.send();
}



function toggleGalleryPublished(img, id)
{
	var enable;
	if (img.alt == "Published")
	{
		publish = 0;
	}
	else
	{
		publish = 1;
	}
	
	var request = new Request({
									url: "/action/image/update?gallery_id=" + id +"&published=" + publish,
									method: 'get',
									onSuccess: function (response) 
									{ 
										if (response == 1) 
										{
											img.src = publish ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
											img.alt = publish ? "Published" : "Unpublished";
										}
									},
									onFailure: function () {},
									async: true
								});
	request.send();
}


function toggleBlogPublished(img, id)
{
	var enable;
	if (img.alt == "Published")
	{
		publish = 0;
	}
	else
	{
		publish = 1;
	}
	
	var request = new Request({
									url: "/action/blog/update?blog_id=" + id +"&published=" + publish,
									method: 'get',
									onSuccess: function (response) 
									{ 
										if (response == 1) 
										{
											img.src = publish ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
											img.alt = publish ? "Published" : "Unpublished";
										}
									},
									onFailure: function () {},
									async: true
								});
	request.send();
}