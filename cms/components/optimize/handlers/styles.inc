<?php
Fakoli::usingFeature("cache");

$last_modified = Cache::get("css_cache_timestamp");

header("Last-Modified: ".date('r', $last_modified));
header("Cache-Control: max-age=300, must-revalidate");
header("Content-Type: text/css");
header("Pragma: private");
header("Expires: " . date(DATE_RFC822,strtotime(" 2 day")));
		
if(array_key_exists("HTTP_IF_MODIFIED_SINCE",$_SERVER))
{
    
    $if_modified_since=strtotime($_SERVER["HTTP_IF_MODIFIED_SINCE"]);
        
    if ($if_modified_since >= $last_modified)
    {
        header("HTTP/1.0 304 Not Modified");
        exit();
    }
}

$content = Cache::get("css_cache");
header("Content-Length: ".strlen($content));

echo $content;
?>