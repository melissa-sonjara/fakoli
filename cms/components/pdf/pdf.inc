<?php
Fakoli::using("settings", "auto_login");

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
		
		$uri = $this->uri;
		
		if ($this->useToken)
		{
			$token = AutoLoginManager::createOneTimeToken($this->uri);
			$uri = "/action/auto_login/authenticate?token=$token";
		}
		
		$url = "http".($this->ssl?"s":"")."://".$config["http_host"].$uri;
			
		$flags = "";
		if ($this->landscape)
		{
			$flags = "-O landscape ";
		}
		
		$tmpName = sha1($url . now()) . ".pdf";
		
		$footer = ($this->footer) ?  $this->footer : "";
	
		$output = $this->output ? $this->output : (PdfManager::getTempDirectory() . DIRECTORY_SEPARATOR . $tmpName);
		$cmd = "{$exe} {$footer} {$flags}-q \"{$url}\" {$output}";
		
		trace($cmd, 3);
		
		array($out);
		
		exec($cmd, $out);
		
		trace(implode("\n", $out), 3);
		
		$content = file_get_contents($output);
		if (!$this->output) unlink($output);
		
		return $content;
	}
}

?>