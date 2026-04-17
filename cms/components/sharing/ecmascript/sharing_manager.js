/**
 *
 */
class SharingManager
{

}

SharingManager.toggleStatus = function(link)
{
	link = document.getElementById(link);
	var id = link.getAttribute("data-share_id");

	fetch('/action/sharing/toggle_share?share_id=' + id + "&work_area_id=" + SharingManager.work_area_id)
		.then(r => r.text())
		.then(response =>
		{
			if (response == "OK")
			{
				if (link.classList.contains('share_enabled'))
				{
					link.classList.remove("share_enabled");
					link.classList.add("share_disabled");
					link.innerHTML = "<i class='icon-cancel'></i> Disabled";
				}
				else
				{
					link.classList.remove("share_disabled");
					link.classList.add("share_enabled");
					link.innerHTML = "<i class='icon-ok'></i> Enabled";
				}
			}
			else
			{
				notification(response);
			}
		});
};
