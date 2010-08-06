<?php
Fakoli::using("forum", "site");
Fakoli::usingFile("framework/auto_form.inc");

$forum_id = checkNumeric($_GET["forum_id"]);

if ($forum_id)
{
	$forum = new Forum($forum_id);
	$page->page_title = "Edit Forum: $forum->title";
}
else
{
	$forum = new Forum();
	$page->page_title = "Add a New Discussion Forum";
}


$form = new AutoForm($forum);
$form->hide("created_date", "owner_id");

if (!$site)
{
	$sites = query(Site, "ORDER BY site_name");
	
	$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Publish on Sites", $sites, "{site_name}", ForumSiteXref);
}

if ($method == "POST")
{
	if (!$forum_id)
	{
		trace("***** CREATING NEW FORUM ******: ".now(), 3);
		$_POST['owner_id'] = $user->user_id;
		$_POST['created_date'] = now();
	}	

	if ($form->save())
	{
		if (!$forum_id && $site)
		{
			$jn = new ForumSiteXref();
			$jn->forum_id = $form->data->forum_id;
			$jn->site_id = $site->site_id;
			$jn->save();
		}
		
		redirect("/admin/forums");
	}
}

$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>