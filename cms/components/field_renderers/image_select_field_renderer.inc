<?php

require_once realpath(dirname(__FILE__))."/../../datamodel/image.inc";
require_once realpath(dirname(__FILE__))."/../../../framework/field_renderers.inc";

class ImageSelectFieldRenderer extends FieldRenderer
{
	var $tree;
	var $field;
	var $onPreProcess;
	var $onPostProcess;
	
	function ImageSelectFieldRenderer(&$form, $field, $required = false)
	{
		$this->FieldRenderer($form);
				
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
	
	function renderField($field = "")
	{
		if (!$field) $field = $this->field;
		
		$this->_startField($field);

			$key = $this->parent->data->$field;
		if ($key)
		{
			$item = new Image($key);
			$src = $item->getThumbnail();
		}
		else
		{
			$src = "/cms/images/noimage.gif";
		}
?>
<input type='text' name='<?echo $field ?>' id='<?echo $field ?>' value='<?echo $key ?>' style='display: none'>
<table style='border: solid 1px #000'>
 <tr>
  <td><img id='<?echo $field ?>_preview' src='<?echo $src ?>'/></td>
 </tr>
 <tr>
  <td><button class='button' style='width: 100%' onclick="popup('/cms/components/image_picker/image_picker.php?Mode=select&Field=<?echo urlencode($field) ?>&Preview=<?echo urlencode($field."_preview") ?>', 'source_picker', 500, 600); return false;"> Select Image </button></td>
 </tr>
</table>
<?
		$this->_endField($field);
	}
	
	function preProcess($field = "")
	{
		// Nothing to do
	}
	
	function postProcess($field = "")
	{
		// Nothing to do
	}
}

?>