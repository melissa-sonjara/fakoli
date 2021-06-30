<?php
Fakoli::using("pdf");

$uri = $_GET["uri"];

$file = basename(preg_replace("/\\?.*/", "", $uri)) . ".png";

$https = Fakoli::checkSSL();

$pdf = new Screenshot($uri, $https, isset($user));
$output = $pdf->generate();
$size = strlen($output);

header("Content-Type: image/png");
header("Content-Disposition: attachment;filename=\"" . $file . "\"");
header("Content-Length: $size");
echo $output;
?>