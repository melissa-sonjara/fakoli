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
 * @author andy
 */
class Cache
{
	static $cache;
	
	/**
	 * Retrieve the specified object from the cache
	 * @param string $key the key identifying the object
	 * @return mixed the associated object, or null if no match was found
	 */
	static function get($key)
	{
		if (ini_get("apc.enabled"))
		{
			return unserialize(apc_fetch($key));
		}
		else
		{
			return Cache::$cache[$key];
			return true;
		}
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
		if (ini_get("apc.enabled"))
		{
			return apc_store($key, serialize($obj), $ttl);
		}
		else
		{
			Cache::$cache[$key] = $obj;
			return true;
			// NOTE - ttl is ignored if using in-memory caching.
		}
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
		if (ini_get("apc.enabled"))
		{
			return apc_delete($key);
		}
		else
		{
			unset(Cache::$cache[$key]);
			return true;
		}
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
		if(ini_get("apc.enabled"))
		{
			$iterator = new APCIterator("user", $pattern);
			
			while($key = $iterator->key())
			{
				Cache::invalidate($key);
				$iterator->next();
			}
		}
		else
		{
			Cache::$pattern = $pattern;
			Cache::$cache = arrayFilterKey(Cache::$cache, array(Cache, filterKey));
		}	
	}
	
	/**
	 * Internal callback function that supports the invalidateMatching function.
	 * @param string $key the key to be filtered.
	 */
	static function filterKey($key)
	{
		return preg_match($key, Cache::$pattern);
	}
	
}