<?php
/**\addtogroup pdf
 * @{
 */


Fakoli::using("settings");

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the pdf component.
 */

class PdfManager
{
	function __construct()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("pdf", "wkhtmltopdf_path", "wkhtmltopdf", 'String', "The path to the wkhtmltopdf executable on your system.", "PDF Settings", null, 2);
		Settings::setDefaultValue("pdf", "PDF_convertor", "wkhtmltopdf", 'String', "Specifies the method used to convert HTML to PDF", "PDF Settings", "wkhtmltopdf\nchrome\nexternal", 1);
		Settings::setDefaultValue("pdf", "chrome_path", "chrome", 'String', "The path to the chrome (or chromium) executable on your system", "PDF Settings", null, 3);
		Settings::setDefaultValue("pdf", "chrome_screenshot_path", "", 'String', "Path to the screenshot script on your system", "PDF Settings", null, 4);
		Settings::setDefaultValue("pdf", "external_PDF_url", "", 'String', "External URL to web-based PDF generator", "PDF Settings", null, 5);
		Settings::setDefaultValue("pdf", "external_PNG_url", "", 'String', "External URL to web-based PNG generator", "PDF Settings", null, 5);
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
	
	static function preprocessTemplate($template)
	{
		$convertor = PdfManager::createConvertor(null);
		return $convertor->preProcessTemplate($template);
	}
	
	static function createConvertor($pdf)
	{
		$mode = Settings::getValue("pdf", "PDF_convertor");
		switch($mode)
		{
			case "external":
				return new ExternalPDFConvertor($pdf);
				
			case "chrome":
				return new ChromePDFConvertor($pdf);
				
			case "whtmktopdf":
			default:
				return new WkhtmltoPDFConvertor($pdf);
		}
	}
	
}

/**@}*/?>
