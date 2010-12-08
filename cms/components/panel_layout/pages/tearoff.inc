<?php
Fakoli::using("panel_layout");

$uri = $_GET["uri"];

$script .= <<<ENDSCRIPT
<script type="text/javascript">
var testSplitter;
var layout;

function toggleSplit()
{
	var orientation = (testSplitter.options.orientation == "horizontal") ? "vertical" : "horizontal";
	testSplitter.setOrientation(orientation);
}
	
window.addEvent('domready', function()
{
	layout = new PanelLayout();
	layout.setup('tearoff', {stretch: true, replace: true});
	layout.addSlots('.slot');	
	var panel = layout.dockAndLoad('main', '{$uri}', 'slot1', {stretch: true});
	panel.addEvent('close', function() { window.close(); });
});
</script>
ENDSCRIPT;
?>
<div id="tearoff" style="width: 100%">
 <div id="slot1" class="slot">
 </div>
</div> 
