<?php
/**\addtogroup document
 * @{
 */


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
	
	function __construct(&$form, $field, $label, $library)
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
			
		parent::__construct($form);
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
/**@}*/?>
