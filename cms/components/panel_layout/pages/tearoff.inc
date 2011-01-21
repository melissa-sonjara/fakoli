<?php
Fakoli::using("panel_layout");

$uri = $_GET["uri"];
$default = $_GET["defaultPanel"];

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

	$('tearoff').setStyles({width: cw, height: ch});
	layout.calculateLayout(); 
}

window.addEvent('domready', function()
{
	layout = new PanelLayout();
	layout.setup('tearoff', {stretch: true, replace: true, 
		onLoad: function()
		{
			var titleField = $('tearoff').getElement(".tearoff_title");
			if (titleField != null) document.title = titleField.get('text');	
		} 
	});
	
	layout.addSlots('.slot');	
	stretch();
	
	var panel = layout.dockAndLoad('main', '{$uri}', 'slot1', {stretch: true});
	window.addEvent('resize', stretch);
	layout.options.defaultPanel = '$default';
	
});
</script>
ENDSCRIPT;
?>
<div id="tearoff" style="width: 100%">
 <div id="slot1" class="slot">
 </div>
</div> 
