<?php

require_once "../include/config.inc";

require_once "../include/permissions.inc";

if (!checkRole("admin"))
{
	redirect("login.php");
}

$title = "Site Administration";

require_once admin_begin_page;
?>
<b>Welcome <?echo $user->first_name?> <?echo $user->last_name?>!</b>
<br/>
<br/>
<?
require_once admin_end_page;
?>