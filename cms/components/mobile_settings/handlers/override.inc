<?php
Fakoli::using("mobile_settings");

$mobile = checkNumeric($_GET["mobile"]);
$redirect = $_GET["redirect"];

$_SESSION["useMobile"] = $mobile;
trace("Overriding Mobile mode: $mobile", 3);

redirect($redirect);
?>