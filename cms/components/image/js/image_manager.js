var ImageManager = new Class({
});

ImageManager.rescanGallery = function(gallery_id)
{
	result = httpRequest("/action/image/rescan?gallery_id=" + gallery_id);
	if (result == "OK")
	{
		window.location.reload();
	}
	else
	{
		alert(result);
	}
};
