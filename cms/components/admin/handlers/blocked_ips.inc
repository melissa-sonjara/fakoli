<?php
Fakoli::assertRole("admin");
$blockedIPs = Fakoli::getBlockedIPs();
echo "<h2>Currently Blocked IP Addresses</h2>\n";
echo "<p><b>";
echo pluralize(count($blockedIPs). " blocked address", count($blockedIPs));
echo "</b></p>\n";
echo "<p>";
foreach($blockedIPs as $ip => $blocked)
{
	echo "{$ip}<br/>\n";
}
echo "<p>";
Cache::dump();
?>