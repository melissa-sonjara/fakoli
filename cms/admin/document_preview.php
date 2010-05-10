<?php
require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/document.inc";
require_once "../datamodel/journal.inc";
require_once "../../framework/auto_form.inc";
require_once "../components/publication_view.inc";

require_once "include/publication_tabs.inc";

$menu_item = "Publications";

$publication_id = checkNumeric($_GET["publication_id"]);

if (!$publication_id) redirect('index.php');

$publication = new Publication($publication_id);

$view = new PublicationView($publication);

$tabs = publicationTabs($publication_id);
if ($publication_id)
{
	$publication->load($publication_id);
	$title = "Preview Publication: {$publication->title}";
}

require_once admin_begin_page;

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$view->drawView();
?>
</div><br/>
<?
require_once admin_end_page;
?>
