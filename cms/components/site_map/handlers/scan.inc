<?php
Fakoli::assertRole("admin");
Fakoli::using("site_map");

$mgr = new SiteMapManager();
$mgr->scan();

redirect("/admin/site_map");

?>