<?
/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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
 * The Cache class provides a simple caching interface. If APC is installed and
 * enabled, caching will be done through APC and will be available across page
 * accesses. If APC is not available, a simple in-memory hash is used.
 * 
 * @author Andy Green
 */
class Cache
{
	static $instance;
	static $app_name;
	
	static function getInstance()
	{
		global $config;
		
		trace(get_class(Cache::$instance), 5);
		
		if (Cache::$instance) return Cache::$instance;
		
		if (array_key_exists("file_backed_cache_directory", $config))
		{
			Cache::$instance = new FileBackedCache();
		}
		else if (ini_get("apc.enabled") && PHP_SAPI !== 'cli')
		{
			if (PHP_MAJOR_VERSION >= 7)
			{
				Cache::$instance = new APCUCache();
			}
			else
			{
				Cache::$instance = new APCCache();
			}
		}
		else if (ini_get("wincache.ucenabled") && PHP_SAPI !== 'cli')
		{
			Cache::$instance = new WCCache();
		}
		else
		{
			Cache::$instance = new SimpleMemoryCache();
		}

		Cache::$app_name = codify($config['sitename']);
		
		return Cache::$instance;
	}
	
	/**
	 * Retrieve the specified object from the cache
	 * @param string $key the key identifying the object
	 * @return mixed the associated object, or null if no match was found
	 */
	static function get($key)
	{
		return Cache::getInstance()->get(ConnectionManager::$dsn."_".Cache::$app_name."_".$key);
	}
	
	/**
	 * Store the specified object in the cache at the specified key. If APC
	 * cacheing is enabled then this value will be available to subsequent script
	 * invocations, and the $ttl parameter can optionally be used to specify the 
	 * time-to-live of the cache record. If APC is enabled and the TTL is not specified
	 * then the item will stay in the cache until it is invalidated. If APC is not
	 * enabled then the item will only remain in the cache for the rest of this
	 * script invocation.
	 * 
	 * @param string $key the key that the value will be cached under.
	 * @param mixed $obj the object or value to be cached
	 * @param integer $ttl the time-to-live for the cache entry, in seconds
	 */
	static function put($key, $obj, $ttl = 0)
	{
		return Cache::getInstance()->put(ConnectionManager::$dsn."_".Cache::$app_name."_".$key, $obj, $ttl);
	}
	
	/**
	 * Invalidates the specifed entry in the cache. This can be used to 
	 * flag items in the cache that have potentially been updated by other
	 * actions. These items are removed from the cache upon invalidation.
	 * If the item has not been cached, no action is taken.
	 * 
	 * @param string $key the entry to be invalidated
	 */
	static function invalidate($key)
	{
		return Cache::getInstance()->invalidate(ConnectionManager::$dsn."_".Cache::$app_name."_".$key);
	}

	/**
	 * Invalidate all entries in the cache that match a specific pattern. 
	 * This can be used to 
	 * flag items in the cache that have potentially been updated by other
	 * actions. These items are removed from the cache upon invalidation.
	 * If the item has not been cached, no action is taken.
	 * 
	 * @param string $pattern PCRE describing the matching keys
	 */
	static function invalidateMatching($pattern)
	{
		return Cache::getInstance()->invalidateMatching("^".ConnectionManager::$dsn."_".Cache::$app_name."_".$pattern);
	}
	
	/**
	 * Clear the cache, deleting all records.
	 */
	static function clear()
	{
		return Cache::getInstance()->clear();
	}
	
	/**
	 * Output a dump of the cache for debugging purposes
	 */
	static function dump()
	{
		return Cache::getInstance()->dump();
	}
}


/**
 * Implements a simple in-memory cache when other cacheing mechanisms
 * are not available.
 * 
 * @author Andy Green
 */
class SimpleMemoryCache
{
	var $cache;
	
	function SimpleMemoryCache()
	{
		trace("Creating Simple In-memory Cache", 4);
		$this->cache = array();
	}
	
	function get($key)
	{
		return $this->cache[$key];
	}
	
	function put($key, $obj, $ttl = 0)
	{
		$this->cache[$key] = $obj;
		return true;
		// NOTE - ttl is ignored if using in-memory caching.
	}
	
	function invalidate($key)
	{
		unset($this->cache[$key]);
		return true;
	}
	
	function invalidateMatching($pattern)
	{
		$this->pattern = $pattern;
		$this->cache = arrayFilterKey($this->cache, array($this, filterKey));
		unset($this->pattern);
	}
	
	/**
	 * Internal callback function that supports the invalidateMatching function.
	 * @param string $key the key to be filtered.
	 */
	function filterKey($key)
	{
		return !preg_match("/".$this->pattern."/", $key);
	}
	
	function clear()
	{
		$this->cache = array();
	}
	
	function dump()
	{
		throw new FakoliException("Not implemented");
	}
}
	

/**
 * Provides a simple cacheing mechanism using the Alternative PHP Cache extension.
 * 
 * @author Andy Green
 */
class APCCache
{	
	function APCCache()
	{
		trace("Creating APC Cache", 4);
	}
	
	function get($key)
	{
		$success = true;
		
		$s = apc_fetch($key, $success);
		if (!$success) return null;
		
		return unserialize($s);
	}
	
	function put($key, $obj, $ttl = 0)
	{
		return apc_store($key, serialize($obj), $ttl);
	}
	
	function invalidate($key)
	{
		return apc_delete($key);
	}

	function invalidateMatching($pattern)
	{
		trace("invalidateMatching: $pattern", 3);
		//TODO - Not working on zither - investigate
		/*$iterator = new APCIterator("user", $pattern);
		
		while($key = $iterator->key())
		{
			trace($key, 3);
			Cache::invalidate($key);
			$iterator->next();
		}*/
	}	
	
	function clear()
	{
		apc_clear_cache('user');
	}
	
	function dump()
	{
		echo "<table class='list'><thead><tr><th>Key</th><th>Value</th></tr></thead><tbody>\n";
		
		$prefix = ConnectionManager::$dsn."_".Cache::$app_name."_";
			
		foreach (new APCIterator('user', '/.*/') as $kvp) 
		{
			if (!startsWith($kvp['key'], $prefix)) continue;
			$key = str_replace($prefix, "", $kvp['key']);
			$value = htmlSafe($kvp['value']);
			
			echo "<tr><td><b>$key</b></td><td>{$value}</td></tr>\n";
		}
		echo "</tbody></table>";
	}
}



/**
 * Provides a simple cacheing mechanism using the Alternative PHP Cache extension.
 * 
 * @author Andy Green
 */
class APCUCache
{	
	function __construct()
	{
		trace("Creating APCU Cache", 4);
	}
	
	function get($key)
	{
		$success = true;
		
		$s = apcu_fetch($key, $success);
		if (!$success) return null;
		
		return unserialize($s);
	}
	
	function put($key, $obj, $ttl = 0)
	{
		return apcu_store($key, serialize($obj), $ttl);
	}
	
	function invalidate($key)
	{
		return apcu_delete($key);
	}

	function invalidateMatching($pattern)
	{
		trace("invalidateMatching: $pattern", 3);
		//TODO - Not working on zither - investigate
		/*$iterator = new APCUIterator("user", $pattern);
		
		while($key = $iterator->key())
		{
			trace($key, 3);
			Cache::invalidate($key);
			$iterator->next();
		}*/
	}	
	
	function clear()
	{
		apcu_clear_cache('user');
	}
	
	function dump()
	{
		echo "<table class='list'><thead><tr><th>Key</th><th>Value</th></tr></thead><tbody>\n";
		
		$prefix = ConnectionManager::$dsn."_".Cache::$app_name."_";
			
		foreach (new APCUIterator('user', '/.*/') as $kvp) 
		{
			if (!startsWith($kvp['key'], $prefix)) continue;
			$key = str_replace($prefix, "", $kvp['key']);
			$value = htmlSafe($kvp['value']);
			
			echo "<tr><td><b>$key</b></td><td>{$value}</td></tr>\n";
		}
		echo "</tbody></table>";
	}
}

/**
 * Provides a simple cacheing mechanism using the WinCache PHP extension (for use under IIS).
 */
class WCCache
{
	function WCCache()
	{
		trace("Creating WinCache cache", 4);
	}
	
	function get($key)
	{
		$value = wincache_ucache_get($key, $success);
		if (!$success) return null;
		
		return unserialize($value);
	}
	
	function put($key, $obj, $ttl = 0)
	{
		return wincache_ucache_set($key, serialize($obj), $ttl);
	}
	
	function invalidate($key)
	{
		return wincache_ucache_delete($key);
	}

	function invalidateMatching($pattern)
	{
		$this->pattern = $pattern;
		
		$info = wincache_ucache_info();
		
		$keys = array();
		
		foreach($info['ucache_entries'] as $entry)
		{
			$keys[] = $entry['key_name'];
		}
		$doomedKeys = array_filter($keys, array($this, filterKey));
		
		foreach($doomedKeys as $key)
		{
			$this->invalidate($key);
		}
		
		unset($this->pattern);
	}
	
	/**
	 * Internal callback function that supports the invalidateMatching function.
	 * @param string $key the key to be filtered.
	 */
	function filterKey($key)
	{
		return preg_match("/".$this->pattern."/", $key);
	}
	
	function clear()
	{
		wincache_ucache_clear();
	}
	
	function dump()
	{
		$info = wincache_ucache_info();
		$prefix = ConnectionManager::$dsn."_".Cache::$app_name."_";
		echo "<table class='list'><thead><tr><th>Key</th><th>Value</th></tr></thead><tbody>\n";
		foreach ($info['ucache_entries'] as $kvp)
		{
			if (!startsWith($kvp['key_name'], $prefix)) continue;
			$key = str_replace($prefix, "", $kvp['key_name']);
			
			echo "<tr><td><b>{$key}</b></td><td>".htmlSafe(wincache_ucache_get($kvp['key_name']))."</td></tr>\n";
		}
		echo "</tbody></table>";
	}		
}

/**
 * Provides a simple File-backed caching implementation for when
 * no shared memory caching extension is present, but the application
 * has access to a writeable folder. This is NOT recommended for 
 * high-volume sites, but may be your only option on a shared server.
 */
class FileBackedCache
{
	var $cache;
	var $cacheDir;
	
	function FileBackedCache()
	{
		global $config;
		
		trace("Creating File-backed Cache", 4);
		
		$this->cacheDir = $config["file_backed_cache_directory"];
		$this->cache = array();
		
		trace("Cache Directory: {$this->cacheDir}", 3);
	}

	function keyToFile($key)
	{
		return $this->cacheDir . DIRECTORY_SEPARATOR . $key . ".txt";
	}
	
	function get($key)
	{
		if (array_key_exists($key, $this->cache)) return $this->cache[$key];
		$file = $this->keyToFile($key);
		if (file_exists($file))
		{
			$c = unserialize(file_get_contents($file));
			$this->cache[$key] = $c;
			return $c;
		}
		else return null;
	}
	
	function put($key, $obj, $ttl = 0)
	{
		$this->cache[$key] = $obj;
		
		$file = $this->keyToFile($key);
		
		if (!file_exists($file))
		{
			$fp = fopen($file, "w");
			fwrite($fp, serialize($obj));
			fclose($fp);
		}
		else
		{
			$fp = fopen($file, "w+");
	
			if (flock($fp, LOCK_EX)) 
			{
			    ftruncate($fp, 0);
			    fwrite($fp, serialize($obj));
			    flock($fp, LOCK_UN);
			} 
			else 
			{
			    echo "Couldn't get the lock!";
			}
		}
		fclose($fp);
		
		return true;
		// NOTE - ttl is ignored if using in-memory caching.
	}
	
	function invalidate($key)
	{
		$file = $this->keyToFile($key);

		if (file_exists($file)) unlink($file);
		unset($this->cache[$key]);
		return true;
	}
	
	function invalidateMatching($pattern)
	{
		throw new FakoliException("FileBackedCache currently does not support the invalidateMatching() method");
	}
	
	function clear()
	{
		foreach(glob($this->cacheDir . DIRECTORY_SEPARATOR . "*.txt") as $doomed)
		{
			unlink($doomed);
		} 
	}
}