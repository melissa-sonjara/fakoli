<?php
Fakoli::using("panel_layout");

$uri = $_GET["uri"];

$script .= <<<ENDSCRIPT
<script type="text/javascript">
var layout;

function stretch()
{
	var cw = window.innerWidth == undefined ?
			((document.documentElement.clientWidth == 0) ? document.body.clientWidth : document.documentElement.clientWidth)
											 : window.innerWidth;
	var ch = window.innerHeight == undefined ? 
			((document.documentElement.clientHeight == 0) ? document.body.clientHeight : document.documentElement.clientHeight)
											 : window.innerHeight;

	$('slot1').setStyles({width: cw, height: ch}); 
}

window.addEvent('domready', function()
{
	layout = new PanelLayout();
	layout.setup('tearoff', {stretch: true, replace: true});
	layout.addSlots('.slot');	
	var panel = layout.dockAndLoad('main', '{$uri}', 'slot1', {stretch: true});
	panel.addEvent('close', function() { window.close(); });
	
	stretch();
	window.addEvent('resize', stretch);
	
});
</script>
ENDSCRIPT;
?>
<div id="tearoff" style="width: 100%">
 <div id="slot1" class="slot">
 </div>
</div> 
