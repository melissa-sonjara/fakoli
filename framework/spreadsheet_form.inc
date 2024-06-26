<?php
/**\addtogroup autoform
 * @{
 */
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

require_once realpath(dirname(__FILE__)."/composite_auto_form.inc");

/**
 * SpreadsheetForm
 * 
 * Provides input forms in a spreadsheet layout
 * where each row is a subform containing one dataitem
 * object's field renderers.
 * 
 *   
 */
class SpreadsheetForm extends CompositeAutoForm
{	
	var $colCount = 0;
	var $subform;					///< The prototype subform used to model each row in the spreadsheet.
	var $columnWidths = array(); 	///< Array of column widths, used to control relative cell sizes in the table output.
	var $msg;						///< Text to appear below buttons at top and above table
	var $onStartRow = null; 		///< Callback hook to determine row style or suppression of row.
	var $isRowSelected = null;  	///< Callback hook to determine whether a given row is selected. Assigning a callback to this hook turns on selection mode. Only selected sub-forms are saved.
	var $isRowEmpty = null;			///< Callback hook to determine if a given row should be saved, or skipped because it is an empty row
	var $rowLabelFormat = null;
	var $rowLabelTitle = null;
	var $rowLabelDefaultText = "";
	var $emptyMessage = "The spreadsheet is empty"; ///< The message to display if the item list is empty and the number of blank rows is set to zero
	var $onFormChanged = null; ///< Javascript function to be called when any value is changed within the spreadsheet
	var $ajaxSubmitHandler = false;		///< JavaScript Callback for AJAX Submit mode - called on success
	var $ajaxFailureHandler = false;	///< JavaScript Callback for AJAX Submit mode - called on failure
	
	/**
	 * Creates a new SpreadsheetForm
	
	 * @param AutoForm $subform an instance of AutoForm containing
	 * the target DataItem object. The target should have 
	 * the filters set as desired and those filters will
	 * apply to all objects displayed in the spreadsheet.
	 * The "template" form should have any field aliases,
	 * hidden fields, and any other options set. The
	 * template form will be used for all the items displayed. 
	 * 
	 * @param array $items array of dataitem objects to be
	 * displayed in an editable table row. If you wish 
	 * to display only empty rows, send an empty array.
	
	 * @param number $blanks the number of blank rows desired,
	 * 0 if only previously saved dataitem objects supplied
	 * in the items array will be displayed.
	 * 
	 * @param string $method the form submission method (i.e. GET or POST)
	 * @param string $action the target action URI (leave blank for default post-back processing)
	 * @param string $id the DOM id for the spreadsheet 
	 * @param string $onStartRow callback to be called before generating each row
	 * @param string $isRowSelected callback to determine the selection status for each row
	 */
	function SpreadsheetForm($subform, $items = array(), $blanks = 5, $method="POST", $action="", $id="", $onStartRow = null, $isRowSelected = null)
	{
		$this->subform = $subform;
		$this->onStartRow = $onStartRow;
		$this->isRowSelected = $isRowSelected;
		
		// Create one instance of AutoForm to apply certain setting
		// for all forms, e.g., aliases and hiddens
		parent::CompositeAutoForm($method, $action);
		$this->formCSS = "list spreadsheet";
		$this->layout = AutoFormLayout::create("spreadsheet", $this);

		$target = $subform->data;
		
		$this->id = get_class($target)."_form";
		
		for($i=0; $i < $blanks; $i++)
		{
			$items[] = clone($target);
		}		

		$this->createSpreadsheetRowForms($items, $subform);
	}	
	
	function showRowLabels($columnTitle, $format, $defaultText)
	{
		$this->rowLabelTitle = $columnTitle;
		$this->rowLabelFormat = $format;
		$this->rowLabelDefaultText = $defaultText;
	}
	
	function createSpreadsheetRowForms($items, $subform)
	{
		$subform->subordinate = true;
		$subform->layout = AutoFormLayout::create("table_row", $subform);

		// Don't allow HTMLEditor in spreadsheet form,
		// override with Text Editor
		$fields = $subform->data->getFields();
		foreach($fields as $field => $type)
		{
			$renderer = $subform->getRenderer($field);
			if(get_class($renderer) == "HTMLFieldRenderer")
			{
				$subform->override($field, "", new TextFieldRenderer($subform));
			}		
		}
		
		$idx = 1;
		foreach($items as $item)
		{
			$this->createOneSpreadsheetRow($item, clone($subform), clone($subform->validator), $idx);			
			$idx++;
		}
	}
	
	function createOneSpreadsheetRow($item, $row, $validator, $idx)
	{
		$layout = "table_row";
		
		if ($this->onStartRow) 
		{
			$ret = call_user_func($this->onStartRow, $item);
		}
		
		// If the row callback explicitly returns false, omit this row
		if ($ret === false) return;
		
		$row->data =& $item;
		$row->id = $this->id . "_$idx";
		$row->validator = $validator;
		$row->validator->id = $row->id;
		if ($ret) $row->formCSS = $ret;
		
		
		$fields = $row->data->getFields();
		// clone a new instance of each field renderer
		// created for the template subform.
		foreach($fields as $field => $type)
		{
			if ($item->filter && $item->filter->isExcluded($field)) continue;
			
			$r = $row->getRenderer($field);
			if($r)
			{
				$renderer = clone($r);
				$renderer->parent = $row;
				$row->override($field, "", $renderer);
			}		
		}
		
		$additional = array();
		foreach($row->additional as $r)
		{
			$renderer = $r['renderer'];
			// JDG 5/24/11 - fix get field name
			$r2 = clone($renderer);
			$r2->parent = &$row;
			$r['renderer'] = $r2;
			
			$additional[] = $r;
			
			trace("#@#@#@ ID: {$r2->parent->id}", 3);
			$row->override($r['field'], $r['label'], $r2);
		}
		
		$row->additional = $additional;
				
		trace(print_r($row->additional, true), 3);
		
		//$form = SubordinateAutoForm::create($this, $row, true);
		$subform = new SubordinateAutoForm($this, $row, true);

		if ($this->isRowSelected)
		{
			$layout = "selectable_table_row";
			$subform->isSelected = call_user_func($this->isRowSelected, $item);
		}	
		
		$subform->form->layout = AutoFormLayout::create($layout, $subform);

		if ($ret)
		{
			$subform->containerClass = trim($this->containerClass. " " . $ret);
		}
	}
	
	/**
	 * Set the td width for the table.
	 * 
	 * @param string $field
	 * @param number $width
	 */
	function setColumnWidth($field, $width)
	{
		$this->columnWidths[$field] = $width;
	}

	/**
	 * Put the form into AJAX submission mode. You must supply a Javascript function to call
	 * once the submission is complete.
	 * @param string $success the Javascript callback function for success
	 * @param string $failure the Javascript callback function for failure
	 */
	function ajaxSubmit($success, $failure = null)
	{
		$id = $this->id ? $this->id : "composite_auto_form";
		if ($failure == null)
		{
			$failure = "function() {document.id('{$id}_error').set('text','Failed to communicate with server'); }";
		}
		
		$this->ajaxSubmitHandler = $success;
		$this->ajaxFailureHandler = $failure;
	}
	
	/**
	 * Generates the Javascript for all the subforms, and creates a SpreadsheetFormManager object
	 */
	function writeScript()
	{
		if (!count($this->forms)) return;
		
		$script = parent::writeScript();
		
		$id = $this->id ? $this->id : "composite_auto_form";
		
		if ($this->onFormChanged) $options = "onFormChanged: {$this->onFormChanged}";
		
		$script .= <<<ENDSCRIPT
		<script type='text/javascript'>
		window.addEvent('domready', function()
		{
			document.id('{$id}').manager = new SpreadsheetFormManager('{$id}', {{$options}});			
		});
		</script>
ENDSCRIPT;
			
		if ($this->ajaxSubmitHandler && !$this->readOnlyForm)
		{
			$script .= <<<ENDSCRIPT
	<script>
	document.id('{$id}').iFrameFormRequest(
	{
		'onRequest': function() { return onCompositeFormSubmit(document.id('{$id}')); }, 
		'onComplete': {$this->ajaxSubmitHandler}, 
		'onFailure': {$this->ajaxFailureHandler}
	});
	</script>
ENDSCRIPT;
		}
			
		return $script;
	}
	
	/**
	 * Draws the spreadsheet. Each row is a subordinate form instance with the renderers 
	 * cloned from the prototype row form supplied in the constructor.
	 */
	function drawForm()
	{	
		if (!count($this->forms))
		{
			echo $this->emptyMessage;
			return;
		}
		
		$id = $this->id ? $this->id : "composite_auto_form";
			
		echo "<form id='$id' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'";
		echo " onsubmit='return onCompositeFormSubmit(this);'";
		echo ">\n";
			
		$layout = ($this->isRowSelected) ? "selectable_table_header" : "table_header";
		$form = $this->subform;
		$form->layout = AutoFormLayout::create($layout, $form);
		$obj =& $form->getData();
		$pk = $obj->getPrimaryKey();
	
		$filter = $obj->getFilter();
		$fields = $obj->getFields();

		
		if ($this->layout->externalErrorBox) $this->layout->errorBox();
		
		if ($this->buttons_at_top) 
		{
			echo "<p>";
			$this->drawButtons();
			echo "</p>";
		}
		
		if (!$this->layout->externalErrorBox) $this->layout->errorBox();
		
		if ($form->markRequiredFields && $form->validator->hasRequiredFields())
		{
			$this->layout->requiredFields($form->requiredFieldsText);
		}
				
		$this->layout->startUngrouped();
	
		// Draw headings
		echo "<thead><tr>\n";
		
		if ($this->isRowSelected)
		{
			echo "<th>&nbsp;</th>";
		}
		
		if ($this->rowLabelTitle)
		{
			echo "<th>{$this->rowLabelTitle}</th>";
		}
		
		foreach($fields as $field => $type)
		{
			if ($field != $pk && !array_key_exists($field, $form->hidden) && !($filter && $filter->isExcluded($field)))
			{
				$renderer = $form->getRenderer($field);
				if($renderer)
				{
					$styles = "text-align: center;";
					if(array_key_exists($field, $this->columnWidths))
					{
						$width = $this->columnWidths[$field];
						if ($width)
						{
							if (is_numeric($width))
							{
								$styles .= " width: {$width}px";
							}
							else
							{
								$styles .= "width: {$width}";
							}
						}
					}
					$renderer->_printLabel($field, 1, $styles);
					$this->colCount++;
				}
			}
		}	

		foreach($form->additional as $r)
		{
			$renderer = $r['renderer'];
			// JDG 5/24/11 - fix get field name
			$field = $r['field'];
			if(!$field)
				$field = $renderer->field;
		
			if(array_key_exists($field, $form->hidden))
				continue;
					
			$styles = "text-align: center;";
			if(array_key_exists($field, $this->columnWidths))
			{
				$width = $this->columnWidths[$field];
				if ($width)
				{
					if (is_numeric($width))
					{
						$styles .= " width: {$width}px";
					}
					else
					{
						$styles .= "width: {$width}";
					}
				}
			}
			$renderer->_printLabel($field, 1, $styles);
			$this->colCount++;
		}		
		echo "</tr></thead>\n";		
		
		foreach($this->forms as $form)
		{
			ob_start();
			$form->drawForm();
			$output = ob_get_contents();
			ob_end_clean();
			
			$output = preg_replace("/\\bname=(['\"])([^'\"]*?)['\"]/", "name=$1{$form->id}__$2$1", $output);

			if ($this->rowLabelTitle)
			{
				$output = preg_replace("/<tr id=(.*?)>/", "<tr id=$1><td>".($form->getData()->format($this->rowLabelFormat))."</td>", $output);
			}
			
			//$form->form->layout->startUngrouped();
			echo $output;
			//$form->form->layout->endUngrouped();
		}

		$this->layout->endUngrouped();
		$this->drawButtons();		
		echo "</form>\n";
	}
	
	/**
	 * Overrides CompositeAutoForm::save() to provide support for selectable spreadsheet forms.
	 * @see CompositeAutoForm::save()
	 */
	function save()
	{
		global $method;
	
		// Do magic
	
		$valid = true;

		
		foreach($this->forms as $form)
		{
			if ($this->isRowSelected != null)
			{
				// Selectable mode - skip form save if not selected

				if (!$_POST[$form->id."__#selected"]) 					
				{
					trace("SKIPPING {$form->id}", 4);
					continue;
				}
			}
			
			foreach($_POST as $name => $value)
			{
				if (strpos($name, "__") === false)
				{
					unset($_POST[$name]);
				}
			}

			$prefix = $form->id."__";
			$len = strlen($prefix);
				
			foreach($_POST as $name => $value)
			{
				if (!strncmp($name, $prefix, $len))
				{
					$_POST[substr($name, $len)] = $value;
				}
			}
			
			if (is_callable($this->isRowEmpty))
			{
				if (call_user_func($this->isRowEmpty)) continue;
			}
			
			if (!$form->save()) $valid = false;
		}
	
		return $valid;
	}
	/*
	 * Create a queryString containing all the
	 * keys of the objects that saved successfully
	 * to use to reload the page after save.
	 */
	function getQueryString($qs = "")
	{
		$values = array();
		
		foreach($this->forms as $form)
		{
			$obj = $form->getData();
			$pk = $obj->getPrimaryKey();
			$val = $obj->$pk;
			if($val)
			{
				$name = "{$form->id}_$pk}";
				$params .= $sep.urlencode($pk)."[".urlencode($name)."]=".urlencode($val);
				$sep = "&";
			}
		}
		
		if($params && $qs)
			$params = appendToQueryString($qs, $params);
		
		return $params;
	}
}

/**@}*/?>