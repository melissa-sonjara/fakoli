<?php
Fakoli::using("rss_feed");
Fakoli::usingFeature("auto_form");

$menu_item="rss Feeds";

$rss_article_id = checkNumeric($_GET["rss_article_id"]);
$feed_id = checkNumeric($_GET["feed_id"]);

$RSSFeedArticles = new RSSFeedArticles();

if ($rss_article_id)
{
	$RSSFeedArticles->load($rss_article_id);
}
else
{
	//$RSSFeedArticles->sort_order = Query::create(rssFeed, "")->executeValue("MAX(sort_order)") + 1;
}

$form = new AutoForm($RSSFeedArticles);
$form->required("rss_title");
$form->hide("last_updated","date_added","rss_article_id","rss_feed_id","rss_article_URL","rss_article_author");

$form->allowDelete = true;
$form->button("Cancel", "/admin/rss_feed_articles?rss_feed_id=$feed_id");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/rss_feed_articles?rss_feed_id=$feed_id");
	}
}

$script .= $form->writeScript();

$form->drawForm();


?>