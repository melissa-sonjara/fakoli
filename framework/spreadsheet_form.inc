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

require_once realpath(dirname(__FILE__)."/composite_auto_form.inc");

/**
 * SpreadsheetForm
 * 
 * Provides input forms in a spreadsheet layout
 * where each row is a subform containing one dataitem
 * object's field renderers.
 * 
 * @param subform - an instance of AutoForm containing
 * the target DataItem object. The target should have 
 * the filters set as desired and those filters will
 * apply to all objects displayed in the spreadsheet.
 * The "template" form should have any field aliases,
 * hidden fields, and any other options set. The
 * template form will be used for all the items displayed. 
 * 
 * @param items - array of dataitem objects to be
 * displayed in an editable table row. If you wish 
 * to display only empty rows, send an empty array.
 *  
 * @param blanks - the number of blank rows desired,
 * 0 if only previously saved dataitem objects supplied
 * in the items array will be displayed.
 *   
 */
class SpreadsheetForm extends CompositeAutoForm
{	
	var $colCount = 0;
	var $subform;
	
	function SpreadsheetForm($subform, $items = array(), $blanks = 5, $method="POST", $action="", $id="")
	{
		$this->subform = $subform;
		
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
	
	function createSpreadsheetRowForms($items, $subform)
	{
		$subform->subordinate = true;
		//$subform->layout = AutoFormLayout::create("table_row", $subform);

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
		$row->data =& $item;
		$row->id = $this->id . "_$idx";
		$row->validator = $validator;
		$row->validator->id = $row->id;
		$fields = $row->data->getFields();
		// clone a new instance of each field renderer
		// created for the template subform.
		foreach($fields as $field => $type)
		{
			$r = $row->getRenderer($field);
			if($r)
			{
				$renderer = clone($r);
				$renderer->parent = $row;
				$row->override($field, "", $renderer);
			}		
		}
		//$form = SubordinateAutoForm::create($this, $row, true);
		$subform = new SubordinateAutoForm($this, $row, true);
		$subform->form->layout = AutoFormLayout::create("table_row", $subform);
	}
	
	/*
	 * Draw the table headings using the labels set by the
	 * renderers in the template form. 
	 * 
	 * Draw each form (row).
	 */
	function drawForm()
	{
		echo "<form id='composite_auto_form' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'";
		echo " onsubmit='return onCompositeFormSubmit(this);'";
		echo ">\n";
			
		$form = $this->subform;
		$obj =& $form->getData();
		$pk = $obj->getPrimaryKey();
	
		$filter = $obj->getFilter();
		$fields = $obj->getFields();
			
		if ($form->markRequiredFields && $form->validator->hasRequiredFields())
		{
			$this->layout->requiredFields($form->requiredFieldsText);
		}
				
		$this->layout->startUngrouped();
	
		// Draw headings
		echo "<thead><tr>\n";
		foreach($fields as $field => $type)
		{
			if ($field != $pk && !array_key_exists($field, $form->hidden) && !($filter && $filter->isExcluded($field)))
			{
				$renderer = $form->getRenderer($field);
				if($renderer)
				{
					$renderer->_printLabel($field, 1, $styles = "text-align: center");
					$this->colCount++;
				}
			}
		}	

		echo "</tr></thead>\n";		
		
		foreach($this->forms as $form)
		{
			ob_start();
			$form->drawForm();
			$output = ob_get_contents();
			ob_end_clean();
			
			$output = preg_replace("/\\bname=(['\"])([^'\"]*?)['\"]/", "name=$1{$form->id}__$2$1", $output);

			$form->form->layout->startUngrouped();
			echo $output;
			$form->form->layout->endUngrouped();
		}

		$this->layout->endUngrouped();
		$this->drawButtons();		
		echo "</form>\n";
	}
	
	/*
	 * Create a queryString containing all the
	 * keys of the objects that saved successfully
	 * to use to reload the page after save.
	 */
	function getQueryString()
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
				$qs .= $sep.urlencode($pk)."[".urlencode($name)."]=".urlencode($val);
				$sep = "&";
			}
		}
		return $qs;
	}
}

?>