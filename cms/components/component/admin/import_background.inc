<?php
Fakoli::using("component");

$menu_item = "Import Configuration";
$title = "Importing Configuration";
?>
<script>
window.addEvent('load', function()
{
	new BackgroundProcess("Importing Configuration", "/action/component/import_background");
});
</script>