<?php
Fakoli::using("component");

/**
 * Defines the basic interface contract for a SerializationHandler.
 * @author Andy Green
 *
 */
interface SerializationHandler
{
	/**
	 * Export the serialized objects.
	 * @return string XML serialization of the exported objects
	 */
	function export();
	
	/**
	 * Import from a serialized XML document
	 * @param DOMDocument $doc The document to be used as the source of the import
	 * @param DataTransaction $tx The database transaction to join when creating imported objects
	 */
	function import($doc, $tx);
}

/**
 * SerializationManager handles import/export of DataItems via an XML file. This allows data to be 
 * transferred between Fakoli instances that provide the same data model (for instance, between development
 * and production versions of the same application).
 * 
 * To implement serialization for your Fakoli component, add a line to your manifest specifying that you
 * can recieve the RegisterSerializationHandler event. Your callback for this event you then use the 
 * SerializationManager::registerHandler method to register the serialization handler in charge of 
 * serializing your data model. Two standard serializers are provided - SimpleSerializationHandler will
 * serialize a single DataItem class. CompoundSerializationHandler serializes a set of classes grouped together
 * in the XML format under a common tag. You can provide custom serialization handlers for more complex
 * needs (should they arise) by implementing the SerializationHandler interface and providing your own
 * implementation.
 * 
 * @author Andy Green
 *
 */
class SerializationManager
{
	static $map;
	
	/**
	 * Creates a SerializationManager, building the serialization map from the registered components
	 * via the RegisterSerializationHandler event.
	 */
	function __construct()
	{
		if (!SerializationManager::$map)
		{
			SerializationManager::$map = array();
			ComponentManager::fireEvent("RegisterSerializationHandler");
			asort(SerializationManager::$map);
		}
	}
	
	/**
	 * Registers a serialization handler for a component.
	 * @param string $component the component for which the handler is being registered (i.e. "blog")
	 * @param string $title the display title describing the data that is serialized for this component (i.e. "Blogs and Articles")
	 * @param SerializationHandler $handler the serialization handler object providing the implementation
	 */
	function registerHandler($component, $title, $handler)
	{
		SerializationManager::$map[$component] = array('title' => $title, 'handler' => $handler);
	}
	
	/**
	 * Exports data for the specified components
	 * @param string $components a comma-separated list of the components for which data is to be exported
	 * @return string the serialized XML data representation
	 */
	function export($components)
	{
		global $config;
		
		$components = explode(",", $components);

		$default_charset = $config["default_charset"];
		if (!$default_charset) $default_charset = "iso-8859-1";
		
		$xml = "<?xml version=\"1.0\" encoding=\"{$default_charset}\"?>";
		$xml .= "\n<Fakoli>";
		
		foreach($components as $component)
		{
			$handler = SerializationManager::$map[$component]['handler'];
			$xml .= $handler->export();
		}
		
		$xml .= "\n</Fakoli>";
		return $xml;
	}
	
	/**
	 * Export data for all registered components
	 * @return string the serialized XML data representation
	 */
	function exportAll()
	{
		$components = implode(",", array_keys(SerializationManager::$map));
		return $this->export($components);
	}
	
	/**
	 * Imports data from the specified XML document for the specified components. Any other data
	 * in the XML document is ignored.
	 * @param string $xml the file path of the XML document containing the serialized data
	 * @param string $components a comma-separated list of the components for which data is to be exported
	 * @throws FakoliException
	 * @throws Exception
	 */
	function import($xml, $components)
	{
		$components = explode(",", $components);
		
		if (filesize($xml) > 10 * 1024 * 1024)
		{
			$_SESSION["xml_import_batch_path"] = $xml;
			$_SESSION["xml_import_components"] = $components;
			redirect("/admin/import_background");
		}
		
		$doc = new DOMDocument();
		if (!$doc->loadXML($xml))
		{
		 	throw new FakoliException("Failed to load XML data file");
		};
		
		$this->processImport($xml, $components);		
	}
	
	function importBackground()
	{
		global $process;
		if (!$process)
		{
			throw new FakoliException("Not running as a background process");
		}
		
		$xml = $_SESSION["xml_import_file_path"];
		$components = $_SESSION["xml_import_components"];
		
		$process->setProgress("Running", "Starting import", 0);
	
		$this->processImport($xml, $components);
		
	}
	
	private function processImport($xml, $components)
	{
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
	
	/**
	 * Import data for all registered components
     * @param string $xml the file path of the XML document containing the serialized data
	 */
	function importAll($xml)
	{
		$components = implode(",", array_keys(SerializationManager::$map));
		return $this->import($xml, $components);
	}
	
	/**
	 * Instantiates DataItems from the supplied XML document and stores them in the database. 
	 * This utility method is provided to simplify the implementation of SerializationHandlers.
	 * 
	 * @param string $class the target DataItem class
	 * @param DOMDocument $doc the source XML document
	 * @param DataTransaction $tx The database transaction to join when creating imported objects
	 * @param bool $save true to save to the database, false to just return the reconstituted objects
	 */
	static function unserialize($class, $doc, $tx, $save = true)
	{
		global $process;
		trace("== Instantiating $class Records", 3);
		
		// Check to see if there are any list tags for the class present.
		// If not, the class was not serialized in this document, so return
		// without processing. If there is a list tag (even if it is empty)
		// we continue processing. This allows for correct deletion of items
		// as part of the import process.
		
		$lists = $doc->getElementsByTagName($class . "List");
		if ($lists->length == 0) return;
		
		$nodes = $doc->getElementsByTagName($class);
	
		$count = count($nodes);
		
		
		if ($process)
		{
			$process->setProgress("Running", pluralize("Reading $count files {$class}", $count), 0);
		}
		
		$objects = array();
		
		foreach($nodes as $node)
		{
			$object = new $class;
			$object->fromXML($node);
			$objects[] = $object;
		}
				
		if ($save)
		{
			$o = new $class;
			$o->joinTransaction($tx);
			$o->deleteAll();
			
			$c = 0;
			
			foreach($objects as $object)
			{
				$object->_disablePK = true;
				$object->joinTransaction($tx);
				$object->save();
				if ($process)
				{
					$process->setProgress("Running", pluralize("Imported $c of $count {$class}", $count), intval($c/$count));
				}
			}
		}
		
		return $objects;
	}
	
	/**
	 * Serializes the specified DataItems to XML.
	 * This utility method is provided to simplify the implementation of SerializationHandlers.
	 * @param string $class the target DataItem class
	 * @param string $constraint optional database constraint for the objects to be serialized
	 * @return string serialized XML representation of the DataItems
	 */
	static function serialize($class, $constraint = "")
	{
		$objects = IteratedQuery::create($class, $constraint)->execute();
		return toXML($class."List", $objects, "");
	}
	
}

/**
 * SerializationForm implements the custom form used in the Fakoli admin section import and export pages.
 * @author Andy Green
 */
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

/**
 * Provides a simple implementation of a SerializationHandler that can serialize a single DataItem class.
 * 
 * @author Andy Green
 *
 */
class SimpleSerializationHandler
{
	var $class;
	var $constraint;
	
	/**
	 * Creates a new SimpleSerializationHandler 
	 * @param string $class the target DataItem class
	 * @param string $constraint optional database constraint to use when exporting
	 */
	function __construct($class, $constraint = "")
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
	
	/**
	 * Exports the DataItems to XML
	 * @return string the serialized XML representation
	 */
	function export()
	{
		return SerializationManager::serialize($this->class, $this->constraint);
	}
	
	/**
	 * Imports the DataItems from XML
	 * @param DOMDocument $doc the source XML document
	 * @param DataTransaction $tx The database transaction to join when creating imported objects
	 */
	function import($doc, $tx)
	{
		global $process;
		SerializationManager::unserialize($this->class, $doc, $tx);
		if ($process)
		{
			$process->setProgress("Completed", "Import successful", 100);
		}
	}		
}

/**
 * Provides serialization for a number of DataItem classes, grouped under a common tag in the
 * serialized XML. Use this when your component needs to serialize a number of related objects
 * under the same transaction.
 * 
 * To serialize two or more items, you would create the handler like this:
 * new ComponentSerializationHandler("GroupTag", Class1, Class2)
 * 
 * You can provide database constraints for export for each class by passing an array containing
 * the class name and the constraint:
 * 
 * new CompoundSerializationHandler("GroupTag", array(Class1, "WHERE active=1"), Class2)
 * 
 * @author Andy Green
 */
class CompoundSerializationHandler
{
	var $groupTag;
	var $classes = array();
	var $constraints = array();
	
	/**
	 * Creates the CompoundSerializationHandler
	 * @param string $groupTag the tag name to be used to group the XML output
	 * @throws FakoliException
	 */
	function __construct($groupTag)
	{
		if (!$groupTag)
		{
			throw new FakoliException("No group tag specified for compound serialization handler");
		}
		
		$this->groupTag = $groupTag;
		for($i = 1; $i < func_num_args(); ++$i)
		{
			$arg = func_get_arg($i);
			if (is_array($arg))
			{
				$this->classes[] = $arg;
			}
			else
			{
				$this->classes[] = array($arg, "");
			}
		}
	}

	/**
	 * Add a target DataItem with optional constraint
	 * @param string $class the target DataItem class
	 * @param string $constraint optional database constraint to use for export
	 */
	function addTarget($class, $constraint = "")
	{
		$this->classes[] = array($class, $constraint);
	}
	
	/**
	 * Exports the target DataItems to XML
	 * @return string the XML serialization of the objects
	 */
	function export()
	{
		$xml  = "\n<{$this->groupTag}>";
		foreach($this->classes as $class)
		{
			$xml.= SerializationManager::serialize($class[0], $class[1]);
		}
		
		$xml .= "\n</{$this->groupTag}>";
		return $xml;
	}

	/**
	 * Import from a serialized XML document
	 * @param DOMDocument $doc The document to be used as the source of the import
	 * @param DataTransaction $tx The database transaction to join when creating imported objects
	 */
	function import($doc, $tx)
	{
		global $process;
		
		foreach($this->classes as $class)
		{
			SerializationManager::unserialize($class[0], $doc, $tx);
		}
		
		if ($process)
		{
			$process->setProgress("Completed", "Import successful", 100);
		}
	}
	
	/**
	 * Retrieves serialized objects from the provided XML document, but does not store them in the database.
	 * Instead they are returned as an array so that further processing can be carried out.
	 * @param DOMDocument $doc The document to be used as the source of the retrieval
	 * @param string $class The class of objects to retrieve
	 * @return array an array of DataItems
	 */
	function retrieve($doc, $class)
	{
		return SerializationManager::unserialize($class, $doc, null, false);
	}
}
?>