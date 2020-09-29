<?php
Fakoli::using("svg_charts");

$palette_id = checkNumeric($_GET["palette_id"]);

$palette = new ColorPalette();
if ($palette_id)
{
	$palette->load($palette_id);
}

$form = new AutoForm($palette);
$form->required("name");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("color_palettes");
	}
}

$script .= $form->writeScript();
$form->drawForm();