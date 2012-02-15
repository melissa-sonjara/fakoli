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
		$doc->loadXML($xml);
		
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
?>
	<form method="POST" action="<?echo $this->action?>">
<?
		foreach(SerializationManager::$map as $component => $handler)
		{
?>
	<input type="checkbox" value="<?echo $component?>" name="content_type[]"/>&nbsp;<?echo $handler['title'];?><br/>
<?
		}
?>
	<br/>
	<input type="submit" class="button" value="<?echo $this->mode?> Configuration"/>
	</form>
<?
	}
}

?>