<?
class SvgChartsManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "svg_charts";
		$component->description = "Implements charts and graphs using Raphael.js";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	var $allow_sessionless_handlers = true;
	
	static function getAdminMenu()
	{
		return array
		(
				"Site Configuration"	=> array
				(
						
						"Color Palettes"	=>	array(	"page"	=>	"/admin/color_palettes",
								"role"	=>	"admin",
								"weight"	=>	11,
								"icon"	=>	"palette")
				)
		);
	}	
	
	static function getScripts()
	{
		return array(
				"/fakoli/js/tooltip.js",
				"/components/svg_charts/js/snap.svg.js",
				"/components/svg_charts/js/snap.svg-plugins.js",
				"/components/svg_charts/js/canvg.js",
				"/components/svg_charts/js/rgbcolor.js",
				"/components/svg_charts/js/palette.js",
				"/components/svg_charts/js/chart.js",
				"/components/svg_charts/js/shape.js",
				"/components/svg_charts/js/pie_chart.js",
				"/components/svg_charts/js/histogram.js",
				"/components/svg_charts/js/morphing_histogram.js",
				"/components/svg_charts/js/radar_chart.js",
				"/components/svg_charts/js/dial.js"
		);
	}

	static function subscribeToEvents()
	{
		return array("upgradeComponent"			=>	array(SvgChartsManager, upgradeComponent),
					 "ComponentScanComplete"	=>	array(SvgChartsManager, setDefaults),
					 "PostProcessScript"		=>	array(SvgChartsManager, outputColorPalettes)
		);
	}
}

?>