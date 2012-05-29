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

	static function getScripts()
	{
		return array(
				"/fakoli/js/tooltip.js",
				"/components/svg_charts/js/raphael.js",
				"/components/svg_charts/js/raphael.dropshadow.js",
				"/components/svg_charts/js/raphael.emboss.js",
				"/components/svg_charts/js/palette.js",
				"/components/svg_charts/js/chart.js",
				"/components/svg_charts/js/pie_chart.js",
				"/components/svg_charts/js/histogram.js",
				"/components/svg_charts/js/morphing_histogram.js"
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(SvgChartsManager, setDefaults)
		);
	}
}

?>