<?php
Fakoli::using("rss_feed");
Fakoli::usingFeature("auto_form");

$menu_item = "RSS Feeds";

$rss_feed_id = checkNumeric($_GET["rss_feed_id"]);

$rssFeed = new rssFeed();

if ($rss_feed_id)
{
	$rssFeed->load($rss_feed_id);
}
else
{
	//$rssFeed->sort_order = Query::create(rssFeed, "")->executeValue("MAX(sort_order)") + 1;
}

$form = new AutoForm($rssFeed);
$form->required("rss_url", "rss_title");
$form->hide("last_updated");

$form->allowDelete = true;
$form->button("Cancel", "/admin/rss_feed");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/rss_feed");
	}
}

$script .= $form->writeScript();

$form->drawForm();


?>