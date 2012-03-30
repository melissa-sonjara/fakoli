<?php
Fakoli::using("pdf");

$uri = $_GET["uri"];

$file = basename(preg_replace("/\\?.*/", "", $uri)) . ".pdf";

$pdf = new PDF($uri, false, isset($user));
$output = $pdf->generate();
$size = strlen($output);

header("Content-Type: application/pdf");
header("Content-Disposition: attachment;filename=\"" . $file . "\"");
header("Content-Length: $size");
echo $output;
?>