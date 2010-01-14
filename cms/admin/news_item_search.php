<?
require_once "../../include/config.inc";
require_once "../datamodel/news_item.inc";
require_once "../framework/search_form.inc";


require_once "../../include/permissions.inc";


$newsitem = new NewsItem();
/*$filter = new InclusionFilter();
$filter->add("last_modified");

$newsitem->filter = $filter; */

$form = new SearchForm($newsitem, "GET", "ni_search_results.php");
$form->params->fromGET();

$form->setMatchingMode("range", "last_modified");
$form->setMatchingMode("like", "title", "message");


$title = "Search for News Items";

$script = $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
