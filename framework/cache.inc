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
	
	static function getInstance()
	{
		if (Cache::$instance) return Cache::$instance;
		
		if (ini_get("apc.enabled"))
		{
			Cache::$instance = new APCCache();
		}
		else if (ini_get("wincache.ucenabled"))
		{
			Cache::$instance = new WCCache();
		}
		else
		{
			Cache::$instance = new SimpleMemoryCache();
		}
		
		return Cache::$instance;
	}
	
	/**
	 * Retrieve the specified object from the cache
	 * @param string $key the key identifying the object
	 * @return mixed the associated object, or null if no match was found
	 */
	function get($key)
	{
		return Cache::getInstance()->get($key);
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
	function put($key, $obj, $ttl = 0)
	{
		return Cache::getInstance()->put($key, $obj, $ttl);
	}
	
	/**
	 * Invalidates the specifed entry in the cache. This can be used to 
	 * flag items in the cache that have potentially been updated by other
	 * actions. These items are removed from the cache upon invalidation.
	 * If the item has not been cached, no action is taken.
	 * 
	 * @param string $key the entry to be invalidated
	 */
	function invalidate($key)
	{
		return Cache::getInstance()->invalidate($key);
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
		return Cache::getInstance()->invalidateMatch($pattern);
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
		trace("Creating Simple In-memory Cache", 3);
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
		return !preg_match($key, $this->pattern);
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
		trace("Creating APC Cache", 3);
	}
	
	function get($key)
	{
		return unserialize(apc_fetch($key));
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
		$iterator = new APCIterator("user", $pattern);
		
		while($key = $iterator->key())
		{
			Cache::invalidate($key);
			$iterator->next();
		}
	}	
}

/**
 * Provides a simple cacheing mechanism using the WinCache PHP extension (for use under IIS).
 */
class WCCache
{
	function WCCache()
	{
		trace("Creating WinCache cache", 3);
	}
	
	function get($key)
	{
		return unserialize(wincache_ucache_get($key));
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
		$info = wincache_ucache_info();
		
		$keys = array_keys($info['ucache_entries']);
		$doomedKeys = arrayKeyFilter($keys, array($this, filterKey));
		
		foreach($doomedKeys as $key)
		{
			$this->invalidate($key);
		}
	}
	
	/**
	 * Internal callback function that supports the invalidateMatching function.
	 * @param string $key the key to be filtered.
	 */
	function filterKey($key)
	{
		return preg_match($key, $this->pattern);
	}
}