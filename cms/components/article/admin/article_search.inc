<?
Fakoli::using("article");

$article = new Article();

$form = new SearchForm($article, "GET", "/admin/article_search_results");
$form->params->fromGET();

$form->setMatchingMode("range", "last_modified");
$form->setMatchingMode("like", "title", "message");


$title = "Search for Articles";

$script = $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
