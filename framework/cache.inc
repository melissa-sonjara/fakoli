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
}