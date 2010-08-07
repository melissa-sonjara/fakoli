<?php

require_once "../include/config.inc";

require_once "../include/permissions.inc";

if (!checkRole("admin"))
{
	redirect("login.php");
}

$title = "Site Administration";


?>
<b>Welcome <?echo $user->first_name?> <?echo $user->last_name?>!</b>
<br/>
<br/>
<?

?>