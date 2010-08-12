<?php
Fakoli::usingFile("framework/field_renderers.inc");

/**
 * Constructs a folder selection tree for the specified document library
 */
class FolderSelectFieldRenderer extends FieldRenderer
{
	var $library;
	var $docMgr;
	var $tree;
	var $field;
	var $onPreProcess;
	var $onPostProcess;
	
	function FolderSelectFieldRenderer(&$form, $field, $label, $library)
	{
		global $config;
		
		$this->library = $library;
		$this->docMgr = new DocumentManager($library);
		
		$this->tree = new DirectoryTreeControl("folder");
		$this->tree->width = 320;
		$this->tree->height = 150;
		//$this->tree->onSelectionChanged = "checkFile";
		$this->tree->permissionCallback = array($this->docMgr, folderPermission);

		$root = $this->library->getLibraryDirectory();
		
		$this->tree->buildFolderTree($config["uploaddir"], $root);
		
		$this->onPreProcess = null;
		$this->onPostProcess = null;	
			
		$this->FieldRenderer($form);
		$this->field = $field;
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);	
		}
	}
	
	function renderScript($field)
	{
		//Not necessary - tree control writes script automatically
		//$this->tree->writeScript();
	}
	
	function renderSearchField($field, $mode = "")
	{
		$this->renderField($field);
	}
	
	function renderField($field = "")
	{
		if (!$field) $field = $this->field;
		
		$this->_startField($field);
		$this->tree->writeHTML();
		$this->_endField($field);
	}

	function renderReadOnly($field)
	{
		//TODO: Implement this
	}
	
	function preProcess($field = "")
	{
		if ($this->onPreProcess != null)
		{
			$callback = $this->onPreProcess;			
			call_user_func($callback, $this, $field);
		}
	}
	
	function postProcess($field = "")
	{
		if ($this->onPostProcess != null)
		{
			$callback = $this->onPostProcess;
			call_user_func($callback, $this, $field);
		}
	}
}
?>