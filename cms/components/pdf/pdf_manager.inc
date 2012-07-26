<?php
Fakoli::using("settings");

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the pdf component.
 */

class PdfManager
{
	function PdfManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("pdf", "wkhtmltopdf_path", "wkhtmltopdf", String, "The path to the wkhtmltopdf executable on your system.");
		
	}

	static function getTempDirectory()
	{
		global $config;
		
		$dir = $config["uploadbase"] . DIRECTORY_SEPARATOR . "pdf_tmp";
		
		if (!file_exists($dir)) 
		{
			mkdir($dir);
			chmod($dir, 0777);
		}

		return $dir;
	}
}

?>