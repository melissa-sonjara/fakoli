<?php
Fakoli::using("rss_feed_articles");
Fakoli::usingFeature("data_view");

$feed_id = checkNumeric($_GET["rss_feed_id"]);

$rssFeeds = Query::create(RSSFeedArticles, "WHERE rss_feed_id=:f")
	->bind(":f",$feed_id)
	->execute();

$table = new DataListView($rssFeeds, "rss_feeds");
$table->column("Feed Title", "{rss_title}", true)
	->column("Author", "{rss_article_author}", true)
	->column("Date Added", "{date_added}", true)
	;

$table->emptyMessage = "No Articles found.";
$table->enableDragReorder("/action/rss_feed/reorder_rss_feeds");
$table->dragText = "<span style='font-size: 10px'>Click and drag to change the order of the feeds</span>";

$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a href='/admin/rss_feed' class='button'>View RSS Feeds</a>
<a href='/admin/pull_feeds' class='button'>Update Feeds</a>