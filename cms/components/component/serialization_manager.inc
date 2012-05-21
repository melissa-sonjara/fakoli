<?php
Fakoli::using("component");

interface SerializationHandler
{
	function export();
	function import($doc, $tx);
}

class SerializationManager
{
	static $map;
	
	function SerializationManager()
	{
		if (!SerializationManager::$map)
		{
			SerializationManager::$map = array();
			ComponentManager::fireEvent("RegisterSerializationHandler");
		}
	}
	
	function registerHandler($component, $title, $handler)
	{
		SerializationManager::$map[$component] = array('title' => $title, 'handler' => $handler);
	}
	
	function export($components)
	{
		$components = explode(",", $components);
		
		$xml = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>";
		$xml .= "\n<Fakoli>";
		
		foreach($components as $component)
		{
			$handler = SerializationManager::$map[$component]['handler'];
			$xml .= $handler->export();
		}
		
		$xml .= "\n</Fakoli>";
		return $xml;
	}
	
	function exportAll()
	{
		$components = implode(",", array_keys(SerializationManager::$map));
		return $this->export($components);
	}
	
	function import($xml, $components)
	{
		$components = explode(",", $components);
		
		$doc = new DOMDocument();
		if (!$doc->loadXML($xml))
		{
		 	throw new FakoliException("Failed to load XML data file");
		};
		
		$tx = new DataTransaction();

		try
		{
			foreach($components as $component)
			{
				$handler = SerializationManager::$map[$component]['handler'];
				$xml .= $handler->import($doc, $tx);				
			}
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
	}
	
	function importAll($xml)
	{
		$components = implode(",", array_keys(SerializationManager::$map));
		return $this->import($xml, $components);
	}
	
	
	static function unserialize($class, $doc, $tx)
	{
		trace("== Instantiating $class Records", 3);
		
		$nodes = $doc->getElementsByTagName($class);
	
		$objects = array();
		
		foreach($nodes as $node)
		{
			$object = new $class;
			$object->fromXML($node);
			$objects[] = $object;
		}
				
		$o = new $class;
		$o->joinTransaction();
		$o->deleteAll();
		$o->disablePrimaryKey();
		
		foreach($objects as $object)
		{
			$object->_disablePK = true;
			$object->joinTransaction($tx);
			$object->save();
		}
			
		$o->enablePrimaryKey();
	}
	
	static function serialize($class, $constraint = "")
	{
		$objects = Query::create($class, $constraint)->execute();
		return toXML($class."List", $objects, "");
	}
	
}

class SerializationForm
{
	var $mgr;
	var $mode;
	var $action;
	
	function SerializationForm($mode, $action = "")
	{
		$this->mode = $mode;
		$this->action = $action;
		$this->mgr = new SerializationManager();
	}
	
	function getSelected()
	{
		global $_POST;
		return implode(",", $_POST["content_type"]);
	}
	
	function writeScript()
	{
	}
	
	function drawForm()
	{
		global $config;
		
		if ($this->mode == "Import")
		{
			$onsubmit = " onsubmit=\"return confirm('WARNING! Importing site content WILL overwrite existing content in your database.\\nThis CANNOT be undone.\\nDo you wish to continue?');\"";
		}
		
?>
	<form method="POST" action="<?echo $this->action?>" enctype="multipart/form-data"<?echo $onsubmit?>>
<?
		foreach(SerializationManager::$map as $component => $handler)
		{
?>
	<input type="checkbox" value="<?echo $component?>" name="content_type[]"/>&nbsp;<?echo $handler['title'];?><br/>
<?
		}
?>
	<br/>
<?
		if ($this->mode == "Import")
		{
?>
	<label for="import_file">Import File </label><input type="file" name="import_file"/><br/><br/>
<?
		}
		else
		{
			$file = codify($config["sitename"]."_".date("Ymd")).".xml";
?>
	<label for="export_file">Export File Name </label><input type="text" size="50" name="export_file" value="<?echo $file?>"/><br/><br/>
<?
		}	
?>
	<input type="submit" class="button" value="<?echo $this->mode?> Configuration"/>
	</form>
<?
	}
}

class SimpleSerializationHandler
{
	var $class;
	var $constraint;
	
	function SimpleSerializationHandler($class, $constraint = "")
	{
		$this->class = $class;
		
		if ($constraint)
		{
			$this->constraint = $constraint;
		}
		else
		{
			$obj = new $class;
			$this->constraint = "ORDER BY ".$obj->getPrimaryKey();
		}
	}
	
	function export()
	{
		return SerializationManager::serialize($this->class, $this->constraint);
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize($this->class, $doc, $tx);
	}		
}
	
?>