/**
 * 
 */
var SharingManager = new Class({});

SharingManager.toggleStatus = function(link)
{
	link = document.id(link);
	id = link.get("data-share_id");
	
	var request = new Request(
	{
		url: '/action/sharing/toggle_share?share_id=' + id + "&work_area_id=" + SharingManager.work_area_id,
		method:'get',

		onComplete: function(response)
		{
			if (response == "OK")
			{
				if (link.hasClass('share_enabled'))
				{
					link.removeClass("share_enabled");
					link.addClass("share_disabled");
					link.set('html', "<i class='icon-cancel'></i> Disabled");
				}
				else
				{
					link.removeClass("share_disabled");
					link.addClass("share_enabled");
					link.set('html', "<i class='icon-ok'></i> Enabled");
				}
			}
			else
			{
				notification(response);
			}
		}
	});
	request.send();
};