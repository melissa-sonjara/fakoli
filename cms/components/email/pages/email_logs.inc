<?php
Fakoli::using("admin");
$view = new AdminPageProxyView();
?>
<h2><i class='fas fa-archive'></i> Email Logs</h2>
<?
$view->drawView();