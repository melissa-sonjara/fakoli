<?php
/**\defgroup pdf Pdf
 * @{
 */


class PdfManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "pdf";
		$component->description = "PDF Generation via wkhtmltopdf";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(PdfManager, setDefaults),
					"PreProcessPage"		=>	array(PdfManager, preProcessTemplate)
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>