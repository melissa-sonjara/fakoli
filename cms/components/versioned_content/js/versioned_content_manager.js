var VersionedContentManager = new Class({});

VersionedContentManager.approve = function(clazz, id)
{
	var request = new Request(
	{
		method: 'get',
		url: '/action/versioned_content/approve?target=' + clazz + "&item_id=" + id,
		onSuccess: function(response) 
		{ 
			if (response == "OK")
			{
				var url = new URI();
				url.setData("version", null);
				location.href = url.toString();
			}
			else
			{
				notification(response);
			}
		}.bind(this)
	});

	request.send();
};