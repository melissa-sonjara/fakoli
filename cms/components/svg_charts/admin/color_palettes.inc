<?php
Fakoli::using("svg_charts");
$menu_item = "Color Palettes";

$palettes = Query::create('ColorPalette', "ORDER BY name")->execute();

$colorsTemplate = "";
for($i = 1; $i < 16; ++$i)
{
	$colorsTemplate .= "<span class='swatch' style='background-color: {data_series_$i}' title='{data_series_$i}'></span>";
}

$table = new DataListView($palettes, "color_palettes");
$table->column("Palette Name", "<a href='color_palette_form?palette_id={palette_id}'>{name}</a>")
	  ->column("Background", "<span class='swatch' style='background-color: {background}'></span> {background}", false, "text-align: center")
	  ->column("Stroke", "<span class='swatch' style='background-color: {stroke}'></span> {stroke}", false, "text-align: center")
	  ->column("Button", "<span class='swatch' style='background-color: {stroke}'></span> {stroke}", false, "text-align: center")
	  ->column("Data Series Colors", $colorsTemplate, false, "text-align: center");

$table->paginate = false;
$table->sortable = false;
$table->filter = false;
$table->emptyMessage = "No color palettes have been created";

$script .= $table->writeScript();
$table->drawView();
?>
<br/>
<a class='button' href='color_palette_form'>Add a Color Palette</a>