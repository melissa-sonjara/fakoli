<?
/*
 * Allow users to download/view a file uploaded through
 * settings feature.
 */
Fakoli::using("settings");

$setting_id = checkNumeric($_GET["setting_id"]);

$setting = new Settings($setting_id);

$dir = $config['uploadbase'] . DIRECTORY_SEPARATOR;

trace("directory:dir:" . $dir, 3);

$filepath = sanitizePath($dir . $setting->value);
$mimeType = getMimeType($setting->value);

trace("attachment download: filepath:" . $filepath, 3);
$f = fopen($filepath, "r");
$size = filesize($filepath);
	
header("Content-Disposition: attachment;filename=\"" . basename($setting->value) . "\"");
header("Content-Type: $mimeType");
header("Content-Length: $size");
	
fpassthru($f);
fclose($f);

session_write_close();
?>