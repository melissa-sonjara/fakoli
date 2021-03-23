<?php
/**\addtogroup svg_charts
 * @{
 */



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
	
	static function generatePalettes()
	{
		$paletteScript = Cache::get("palette_script");
		
		if ($paletteScript === null)
		{
			$paletteScript = "";
			$palettes = Query::create('ColorPalette')->execute();
			if (count($palettes) > 0)
			{
				$paletteScript = "<script>\n";
				foreach($palettes as $palette)
				{
					$paletteScript .= $palette->format("new Palette(\"{name:jsSafe}\", '{background}', '{stroke}', '{button}', ".
									"['{data_series_1}', '{data_series_2}', '{data_series_3}', '{data_series_4}', '{data_series_5}', '{data_series_6}', '{data_series_7}', ".
									"'{data_series_8}', '{data_series_9}', '{data_series_10}', '{data_series_11}', '{data_series_12}', '{data_series_13}', '{data_series_14}', ".
									"'{data_series_15}']);\n\n");
				}
				$paletteScript .= "</script>\n";
			}
			Cache::put("palette_script", $paletteScript);
		}
		return $paletteScript;
	}
}
/**@}*/?>