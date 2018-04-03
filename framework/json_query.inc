<?php

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

require_once realpath(dirname(__FILE__)."/query.inc");

class JSONQuery extends AbstractQuery
{
	var $groupBy;
	
	public function __construct($class, $constraints = "")
	{
		parent::__construct($class, $constraints);
		$this->groupBy = $groupBy;
	}
		
	function groupBy($groupBy)
	{
		$this->groupBy = $groupBy;
		return $this;
	}
	
	function execute()
	{
		$json = $this->executeRaw();
		if (!$json) return null;
		
		$json = json_decode($json);
	
		if (is_object($json)) $json = array($json);
		$items = array();
		foreach($json as $jsonItem)
		{
			$item = new $this->class;
			$item->filter = $this->filter;
			$item->fromJSON($jsonItem);
			
			$items[] = $item;
		}
		
		return $items;
	}
	
	function executeSingle()
	{
		$json = $this->executeRaw();
		if (!$json) return null;
		
		if (is_array($json))
		{
			throw new FakoliException("Ambiguous Singleton Query - JSON");
		}
	
		$item = new $this->class;
		$item->filter = $this->filter;
		$item->fromJSON($json);
	
		return $item;
	}
	
	function executeRaw()
	{
		$json = null;
		
		$query = $this->constraints;
		if (!startsWith($query, "SELECT ")) $query = "SELECT $query";
		
		try
		{
			$db = ConnectionManager::getConnection();
			
			$result = $db->prepare($query);
			$result->execute($this->params);

			trace($query, 1);
			
			$line = $result->fetch();
			trace(print_r($line, true), 1);
			
			if ($line)
			{
				$json = $line[0];
			}
		
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "JSONQuery failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		return $json;
	}
	
	static function create($class, $constraints = "")
	{
		return new JSONQuery($class, $constraints);
	}
}