<?php
Fakoli::using("settings", "auto_login");

interface PDFConvertor
{
	function coverPage($url);
	function footer($text, $position = "center", $size = 11);
	function generate();
	function preprocessTemplate($template);
};
	
class WkhtmltoPDFConvertor implements PDFConvertor
{
	private $pdf;
	private $footer;
	
	function __construct($pdf)
	{
		$this->pdf = $pdf;
	}
	
	/**
	 * Provides a separate URL to use as the cover page of the PDF
	 * @param string $url the URL used to generate the cover page
	 * @return PDF
	 */
	function coverPage($url)
	{
		$this->cover = $url;
		return $this;
	}
	
	/**
	 *
	 * @param $text - text of the footer
	 * @param $position - valid positions are left, right, and center
	 * @param $size - font size for the footer ?? doesn't work
	 */
	function footer($text, $position = "center", $size = 11)
	{
		$this->footer = "--footer-font-size {$size} --footer-{$position} \"{$text}\"";
	}
	
	function generate()
	{
		global $config;
		
		$exe = Settings::getValue("pdf", "wkhtmltopdf_path");
		if (!$exe) throw new FakoliException("Convertor path not specified");
		
		$uri = $this->pdf->uri;
		
		if ($this->pdf->useToken)
		{
			$token = AutoLoginManager::createOneTimeToken($this->pdf->uri);
			$uri = "/action/auto_login/authenticate?token=$token";
		}
		
		$url = "http".($this->pdf->ssl?"s":"")."://".$config["http_host"].$uri;
		
		$flags = "";
		if ($this->pdf->landscape)
		{
			$flags = "-O landscape ";
		}
		
		$tmpName = sha1($url . now()) . ".pdf";
		
		$footer = ($this->footer) ?  $this->footer : "";
		
		$output = $this->pdf->output ? $this->pdf->output : (PdfManager::getTempDirectory() . DIRECTORY_SEPARATOR . $tmpName);
		$cmd = "{$exe} {$footer} {$flags}-q \"{$url}\" {$output}";
		
		trace($cmd, 3);
		
		$out = [];
		
		exec($cmd, $out);
		
		trace(implode("\n", $out), 3);
		
		$content = file_get_contents($output);
		if (!$this->pdf->output) unlink($output);
		
		return $content;
	}
	
	function preProcessTemplate($template)
	{
		return $template;
	}
}

class ChromePDFConvertor implements PDFConvertor
{
	private $pdf;
	private $footer;
	
	function __construct($pdf)
	{
		$this->pdf = $pdf;
	}
	
	/**
	 * Provides a separate URL to use as the cover page of the PDF
	 * @param string $url the URL used to generate the cover page
	 * @return PDF
	 */
	function coverPage($url)
	{
		$this->cover = $url;
		return $this;
	}
	
	/**
	 *
	 * @param $text - text of the footer
	 * @param $position - valid positions are left, right, and center
	 * @param $size - font size for the footer ?? doesn't work
	 */
	function footer($text, $position = "center", $size = 11)
	{
		$this->footer = "--footer-font-size {$size} --footer-{$position} \"{$text}\"";
	}
	
	function generate()
	{
		global $config;
		
		$exe = Settings::getValue("pdf", "chrome_path");
		
		if (!$exe) throw new FakoliException("Convertor path not specified");
		
		$uri = $this->pdf->uri;
		$uri = appendToQueryString($uri, "__pdfoutput=1");
		if ($this->pdf->landscape)
		{
			$uri = appendToQueryString($uri, "__pdflayout=landscape");
		}
		
		if ($this->pdf->useToken)
		{
			$token = AutoLoginManager::createOneTimeToken($uri);
			$uri = "/action/auto_login/authenticate?token=$token";
		}
		
		$url = "http".($this->pdf->ssl?"s":"")."://".$config["http_host"].$uri;
		
		$flags = "--headless --disable-gpu --no-margins ";
		
		$tmpName = sha1($url . now()) . ".pdf";
				
		$output = $this->pdf->output ? $this->pdf->output : (PdfManager::getTempDirectory() . DIRECTORY_SEPARATOR . $tmpName);
		$cmd = "{$exe} {$flags} {$url} --print-to-pdf={$output}";
		
		trace($cmd, 3);
		
		$out = [];
		
		exec($cmd, $out);
		
		trace(implode("\n", $out), 3);
		
		$content = file_get_contents($output);
		if (!$this->pdf->output) unlink($output);
		
		return $content;
	}
	
	
	function preProcessTemplate($template)
	{
		if (!isset($_GET["__pdfoutput"])) return $template;
		$layout = ($_GET["__pdflayout"] == "landscape") ? " @page { size: landscape; }" : "";
		$styles = "<style type='text/css' media='print'>{$layout} @page {margin:0;} body { margin: 1.5cm }</style></head>";
		
		$template = preg_replace("/<\\/head>/i", $styles, $template);
		return $template;
	}
}

class PDF
{
	var $uri;
	var $ssl;
	var $output;
	var $footer;
	var $landscape = false;
	
	/**
	 * Creates a new PDF object configured to generate a PDF from the supplied URI on the
	 * local web application.
	 * @param string $uri the URI that will generate the content for the PDF
	 * @param boolean $ssl whether to use SSL to access the URI
	 * @param boolean $useToken set to true to use an AutoLogin token to use the credentials of the currently logged in user to generate the PDF content
	 * @param string $output (optional) a local output file path if the PDF output is to be saved to disk (for caching, or as an intermeediate step)
	 * @param boolean $landscape true for landscape orientated output, false for portrait oriented output (the default)
	 */
	function PDF($uri, $ssl = false, $useToken = true, $output = null, $landscape = false)
	{
		$this->uri = $uri;
		$this->ssl = $ssl;
		$this->useToken = $useToken;
		$this->output = $output;
		$this->landscape = $landscape;
		$this->convertor = PdfManager::createConvertor($this);
	}

	
	/**
	 * Provides a separate URL to use as the cover page of the PDF
	 * @param string $url the URL used to generate the cover page
	 * @return PDF
	 */
	function coverPage($url)
	{
		$this->convertor->corverPage($url);
	}
	
	/**
	 * 
	 * @param $text - text of the footer
	 * @param $position - valid positions are left, right, and center
	 * @param $size - font size for the footer ?? doesn't work
	 */
	function footer($text, $position = "center", $size = 11)
	{
		$this->convertor->footer($text, $position, $size);
	}
	
	function generate()
	{
		return $this->convertor->generate();
	}
}

?>