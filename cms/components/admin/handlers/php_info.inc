<?php
Fakoli::assertRole("admin");

ob_start();

phpinfo();

$info = ob_get_contents();
ob_end_clean();

$info = preg_replace("/.*?<body>(.*?)<\/body>/s", "$1", $info);

echo $info;
?>