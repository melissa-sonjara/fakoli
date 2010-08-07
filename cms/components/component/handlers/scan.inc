<?php
Fakoli::assertRole("admin");
			
$mgr = new ComponentManager();
$mgr->scan();

redirect("/admin/components");

?>