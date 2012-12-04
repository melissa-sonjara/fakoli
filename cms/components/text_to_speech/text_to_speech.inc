<?php
Fakoli::using("settings", "auto_login");

class TextToSpeech
{
	var $uri;
	var $ssl;
	var $output;
	var $footer;
	
	function TextToSpeech($uri, $ssl = false, $useToken = true, $output = null)
	{
		$this->uri = $uri;
		$this->ssl = $ssl;
		$this->useToken = $useToken;
		$this->output = $output;
	}
	
	function generate()
	{
		global $config;
		
		$exe = Settings::getValue("text_to_speech", "texttowav_path");
		
		$uri = $this->uri;
		
		if ($this->useToken)
		{
			$token = AutoLoginManager::createOneTimeToken($this->uri);
			$uri = "/action/auto_login/authenticate?token=$token";
		}
		
		$url = "http".($this->ssl?"s":"")."://".$config["http_host"].$uri;
			
		$tmpName = sha1($url . now()) . ".pdf";
	
		$output = $this->output ? $this->output : (TextToSpeechManager::getTempDirectory() . DIRECTORY_SEPARATOR . $tmpName);
		$cmd = "$exe $url -o $output";
		
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