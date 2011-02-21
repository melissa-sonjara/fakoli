<?php
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

Fakoli::using("settings");

class SessionManager
{
	static $lifeTime;
	static $hash;
	
	static function createDefaultSettings()
	{
		Settings::setDefaultValue("session_persistance", "enable_database_persistance", "");
	}
	
	static function onInitialize()
	{
		if (!Settings::getValue("session_persistance", "enable_database_persistance")) return;
		
		SessionManager::$lifeTime = get_cfg_var("session.gc_maxlifetime");
		SessionManager::$hash = "";
		
		session_set_save_handler(
	        array(SessionManager, "open"), 
	        array(SessionManager, "close"),
	        array(SessionManager, "read"),
	        array(SessionManager, "write"),
	        array(SessionManager, "destroy"),
	        array(SessionManager, "gc")
	      );
	}
	
	static function open($save_path, $session_name)
	{
		return true;
	}
	
	static function close()
	{
		return true;
	}
	
	static function read($id)
	{
		trace("Session Read $id", 3);
		
		try
		{
			$session = PHPSession::read($id);
			if ($session->expires < time()) return null;
			return $session->data;
		}
		catch(Exception $e)
		{
			return null;
		}
	}
	
	static function write($id, $data)
	{
		trace("Session Write $id", 3);
		
		$session = PHPSession::read($id);
		if (!$session) $session = new PHPSession();

		$expiry = time() + SessionManager::$lifeTime;
		
		if ($session->session_id && ($data == $session->data))
		{
			$session->filter = new ExclusionFilter("data", "id");
		}
		else
		{
			$session->filter = null;
		}
		
		$session->id = $id;
		$session->data = $data;
		$session->expires = $expiry;
		$session->save();
	}
	
	static function destroy($id)
	{
		trace("Session Destroy $id", 3);
		
		try
		{
			$session = PHPSession::read($id);
			if ($session) $session->delete();
		}
		catch(Exception $e)
		{
		}
	}
	
	static function gc()
	{
		$session = new PHPSession();
		$session->delete("WHERE expires < ".time());
	}
}
?>