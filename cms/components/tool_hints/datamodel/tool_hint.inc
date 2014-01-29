<?php
/**************************************************************

Copyright (c) 2014 Sonjara, Inc

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

class ToolHint extends DataItem
{
	var $table = "tool_hint";
	var $primary_key = "hint_id";
	
	var $fields = array("hint_id"		=>	Number,
						"code"			=>	String,
						"title"			=>	String,
						"hint"			=>	HTML);

	static function findToolHint($code)
	{
		return Query::create(ToolHint, "WHERE code=:c")
					->bind(":c", $code)
					->executeSingle();
	}
	
	function isHidden()
	{
		global $user;
		
		try
		{
			$status = Query::create(ToolHintUserStatus, "WHERE hint_id=:h AND user_id=:u")
					       ->bind(":h", $this->hint_id, ":u", $user->get($user->getPrimaryKey()))
					       ->executeSingle();
			
			return $status->hidden;
		}
		catch(DataNotFoundException $e)
		{
			return false;
		}
	}
	
	function setHidden($hidden = true)
	{
		global $user;
		$status = null;
		
		try
		{
			$status = Query::create(ToolHintUserStatus, "WHERE hint_id=:h AND user_id=:u")
			->bind(":h", $this->hint_id, ":u", $user->get($user->getPrimaryKey()))
			->executeSingle();
		}
		catch(DataNotFoundException $e)
		{
			$status = new ToolHintUserStatus();
			$status->hint_id = $this->hint_id;
			$status->user_id = $user->get($user->getPrimaryKey());
		}

		$status->hidden = $hidden;
		$status->save();
	}
}
	
class ToolHintUserStatus extends DataItem
{
	var $table = "tool_hint_user_status";
	var $primary_key = "status_id";
	
	var $fields = array("status_id"	=>	Number,
						"hint_id"	=>	Number,
						"user_id"	=>	Number,
						"hidden"	=>	Boolean);
	
	function Hint()
	{
		return $this->getRelated(ToolHint);
	}
}
	