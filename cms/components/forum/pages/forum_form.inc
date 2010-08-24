<?php
Fakoli::using("forum");
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

if ($method == "POST")
{
	if (!$forum_id)
	{
		trace("***** CREATING NEW FORUM ******: ".now(), 3);
		$_POST['owner_id'] = $user->site_user_id;
		$_POST['created_date'] = now();
	}	

	if ($form->save())
	{
		if (!$forum_id)
		{
			$jn = new ForumSiteXref();
			$jn->forum_id = $form->data->forum_id;
			$jn->site_id = $site->site_id;
			$jn->save();
		}
		
		redirect("/forum_list");
	}
}

$script .= $form->writeScript();

$form->drawForm();
?>