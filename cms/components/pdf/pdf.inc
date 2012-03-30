<?php
Fakoli::using("settings", "auto_login");

class PDF
{
	var $uri;
	var $ssl;
	
	function PDF($uri, $ssl = false, $useToken = true)
	{
		$this->uri = $uri;
		$this->ssl = $ssl;
		$this->useToken = $useToken;
	}

	function coverPage($url)
	{
		$this->cover = $url;
		return $this;
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
		
		$tmpName = sha1($url . now()) . ".pdf";
		
		
		$output = PdfManager::getTempDirectory() . DIRECTORY_SEPARATOR . $tmpName;
		$cmd = "$exe -q $url $output";
		
		trace($cmd, 3);
		
		array($out);
		
		exec($cmd, $out);
		
		trace(implode("\n", $out), 3);
		
		$content = file_get_contents($output);
		//unlink($output);
		
		return $content;
	}
}

?>