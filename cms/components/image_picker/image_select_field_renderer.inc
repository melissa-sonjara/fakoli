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

Fakoli::using("image");
Fakoli::usingFile("framework/field_renderers.inc");
Fakoli::usingFeature("auto_form");

class ImageSelectFieldRenderer extends FieldRenderer
{
	var $tree;
	var $field;
	var $onPreProcess;
	var $onPostProcess;
	var $size;
	
	function ImageSelectFieldRenderer(&$form, $field = "", $required = false)
	{
		$this->FieldRenderer($form);
				
		if (!$field) return;
		
		$this->field = $field;
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this);	
		}
		
		if($required)
			$form->validator->add(new RequiredValidator($field, "Select an Image", "", false));
	}
	
	function renderScript($field)
	{

	}
	
	function renderField($field = "", $readonly = false)
	{
		if (!$field) $field = $this->field;
		
		$this->_startField($field);

		$key = $this->parent->data->get($field);

		if ($key)
		{
			$item = new ImageRecord($key);
			$src = $item->getThumbnail($this->size);
		}
		else
		{
			$src = "/fakoli/images/noimage.gif";
		}
?>
<input type='text' name='<?echo $field ?>' id='<?echo $field ?>' value='<?echo $key ?>' style='display: none'>
<table style='border: solid 1px #000'>
 <tr>
  <td><img id='<?echo $field ?>_preview' src='<?echo $src ?>'/></td>
 </tr>
 <?php 
 		if(!$readonly)
 		{
 ?>
 <tr>
  <td><button class='button' style='width: 100%' onclick="new ImagePicker().showSelectImageDialog('<?echo urlencode($field) ?>'); return false;"> Select Image </button></td>
 </tr>
 <?php 
 		}
 ?>
</table>
<?
		$this->_endField($field);
	}
	
	function renderReadOnly($field = "")
	{
		$this->renderField($field, true);
	}	
	
	function preProcess($field = "")
	{
		if ($this->parent->data->get($field) === "")
		{
			$this->parent->data->set($field, 0);
		}
		// Nothing to do
	}
	
	function postProcess($field = "")
	{
		// Nothing to do
	}
}

// Register the field renderer
AutoForm::registerFieldRendererClass(Image, ImageSelectFieldRenderer);
?>