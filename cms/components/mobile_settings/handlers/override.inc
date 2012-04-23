<?php
Fakoli::using("mobile_settings");

$mobile = checkNumeric($_GET["mobile"]);
$redirect = $_GET["redirect"];

$_SESSION["useMobile"] = $mobile;
redirect($redirect);
?>