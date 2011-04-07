<?php
Fakoli::usingFeature("cache");

$last_modified = Cache::get("script_cache_timestamp");

header("Last-Modified: ".date('r', $last_modified));
header("Cache-Control: max-age=300, must-revalidate");
header("Content-Type: text/javascript");
header("Pragma: private");
header("Expires: " . date(DATE_RFC822,strtotime(" 2 day")));
		
if(array_key_exists("HTTP_IF_MODIFIED_SINCE",$_SERVER))
{
    $date = $_SERVER["HTTP_IF_MODIFIED_SINCE"];
    $if_modified_since=strtotime($date);
     
    if($if_modified_since >= $last_modified)
    {
        header("HTTP/1.0 304 Not Modified");
        exit;
    }
}

$content = Cache::get("script_cache");
header("Content-Length: ".strlen($content));

echo $content;
?>