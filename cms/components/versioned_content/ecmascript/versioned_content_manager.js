class VersionedContentManager
{
}

VersionedContentManager.approve = function(clazz, id)
{
	fetch('/action/versioned_content/approve?target=' + clazz + "&item_id=" + id)
		.then(function(r) { return r.text(); })
		.then(function(response)
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
		});
};

VersionedContentManager.revert = function(clazz, id)
{
	if (!confirm("Are you sure you want to discard your draft edits and revert to the live version?")) return;

	fetch('/action/versioned_content/revert?target=' + clazz + "&item_id=" + id)
		.then(function(r) { return r.text(); })
		.then(function(response)
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
		});
};
