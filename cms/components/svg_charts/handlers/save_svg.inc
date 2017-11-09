<?php

$image = $_POST["svg"];
$filename = checkIdentifier($_POST["filename"]);

trace($image, 3);

$image = str_replace(array(";", ","), array(":", ":"), $image);

list($protocol,$mimeType,$encoding,$data) = explode(":", $image);


if ($encoding == "base64")
{
	$data = base64_decode($data);
}

header("Pragma: private");
header("Content-Disposition: attachment;filename=\"{$filename}.svg\"");
header("Cache-Control: must-revalidate");
header("Content-Type: $mimeType");
echo $data;
?>