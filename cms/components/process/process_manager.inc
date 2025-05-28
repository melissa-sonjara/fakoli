<?php
/**\addtogroup process
 * @{
 */


/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the process component.
 */

class ProcessManager
{
	var $id;
	var $cache;
		
	function __construct($id = "")
	{
		global $config;
		
		$this->id = $id;
		
		if (!$config["file_backed_cache_directory"])
		{
			$cacheDir = $config["uploadbase"] . DIRECTORY_SEPARATOR . "cache";
			if (!is_dir($cacheDir))
			{
				mkdir($cacheDir);
			}

			$config["file_backed_cache_directory"] = $cacheDir;
			
			$this->cache = new FileBackedCache();
		}
	}

	/**
	 * Spawns the specified action as a background process under the current user's credentials
	 * @param string $action the action handler to use to handle the request
	 * @param array $params (optional) array of parameters to pass to the spawned process
	 * @return string unique ID for the spawned process
	 */
	function run($action, $params = array())
	{
		global $config;
		global $user;
		
		$isWindows = Fakoli::detectWindowsOS();
		$this->id = sha1(now() . $action);
		foreach($params as $key => $value)
		{
			$this->put($key, $value);
		}
		
		$launch  = $isWindows ? "start " : "";
		$bg = $isWindows ? "" : " &";
		$run = Settings::getValue("process", "PHP_executable")." -d include_path='".
			get_include_path()."' ".
			sanitizePath(Fakoli::getComponent("process")->component_path) . DIRECTORY_SEPARATOR . "run.php";
		$home = $config['homedir'];
		
		$userPK = $user ? $user->get($user->primary_key) : "";
		
		$cmd = "$launch$run ".escapeshellarg($home)." ".escapeshellarg($this->id)." ".escapeshellarg($action)." ".$userPK." ".$bg;
		
		$this->put("status", "Starting");
		$this->put("error", "");
		
		trace($cmd, 3);
		
		pclose(popen($cmd, "r")); 
		
		return $this->id;
	}
	
	function put($key, $value)
	{
		$this->cache->put("{$this->id}_{$key}", $value);
	}
	
	function get($key)
	{
		return $this->cache->get("{$this->id}_{$key}");
	}
	
	function setProgress($status, $message, $percentage)
	{
		$this->put("status", $status);
		$this->put("message", $message);
		$this->put("percentage", number_format($percentage, 0));
		
		trace("{$this->id} $status: $message ($percentage%)", 3);
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("process", "PHP_executable", "/usr/bin/php", 'String', "This is the path to the PHP command line executable");
		Settings::setDefaultValue("process", "background_process_log_file", "", 'String', "Optionally specify a separate log file for background processes");
	}

	static function upgradeComponent($version)
	{
		$mgr = new ProcessUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>
