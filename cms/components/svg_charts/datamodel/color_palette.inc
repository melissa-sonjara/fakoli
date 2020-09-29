<?php

class ColorPalette extends DataItem
{
	var $table = "color_palette";
	var $primary_key = "palette_id";
	
	var $fields = array("palette_id"		=>	'Number',
						"name"				=>	'String',
						"background"		=>	'Color',
						"stroke"			=>	'Color',
						"button"			=>	'Color',
						"data_series_1"		=>	'Color',
						"data_series_2"		=>	'Color',
						"data_series_3"		=>	'Color',
						"data_series_4"		=>	'Color',
						"data_series_5"		=>	'Color',
						"data_series_6"		=>	'Color',
						"data_series_7"		=>	'Color',
						"data_series_8"		=>	'Color',
						"data_series_9"		=>	'Color',
						"data_series_10"	=>	'Color',
						"data_series_11"	=>	'Color',
						"data_series_12"	=>	'Color',
						"data_series_13"	=>	'Color',
						"data_series_14"	=>	'Color',
						"data_series_15"	=>	'Color');
}