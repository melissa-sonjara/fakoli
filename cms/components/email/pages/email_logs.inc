<?php
Fakoli::using("admin");
$view = new AdminPageProxyView();
ManagementDashboard::$menu_item = "Email Logs";
?>
<h2><i class='fas fa-archive'></i> Course Management</h2>
<?
$view->drawView();