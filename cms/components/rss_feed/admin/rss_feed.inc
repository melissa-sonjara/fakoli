<?php
Fakoli::using("rss_feed");
Fakoli::usingFeature("data_view");

$menu_item="RSS Feeds";

$rssFeeds = Query::create(RSSFeed, "ORDER BY rss_feed_id")->execute();

$table = new DataListView($rssFeeds, "rss_feeds");
$table->column("Feed Title", "<a href='/admin/rss_feed_form?rss_feed_id={rss_feed_id}'>{rss_title}</a>", true)
	->column("Articles", "<a href='/admin/rss_feed_articles?rss_feed_id={rss_feed_id}'>View Stored Articles</a>", true)
	->column("Last Updated", "{last_updated}", true)
	;

$table->emptyMessage = "No RSS feeds defined.";
$table->enableDragReorder("/action/rss_feed/reorder_rss_feeds");
$table->dragText = "<span style='font-size: 10px'>Click and drag to change the order of the feeds</span>";

$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a href='/admin/rss_feed_form' class='button'>New RSS Feed</a>
<a href='/admin/pull_feeds' class='button'>Update Feeds</a>