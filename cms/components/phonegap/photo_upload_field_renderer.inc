<?php
/**\addtogroup phonegap
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

Fakoli::using("image");

class PhotoUploadFieldRenderer extends FieldRenderer
{
	static function create($form, $field, $xrefClass)
	{
		if (PhonegapManager::hasPhonegap())
		{
			return new PhotoUploadFieldRenderer($form, $field, $xrefClass);
		}
	}
	
	var $gallery_id;
	var $imageSize = 300;
	var $takePhotoLabel = "Take a Photo";
	var $uploadPhotoLabel = "Choose a Photo";
	var $uploadRedirect = false;
	
	function __construct($form, $field, $label, $gallery_id)
	{
		parent::__construct($form);
		$this->gallery_id = $gallery_id;
		
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->overrides[$field]['label'] = $label;
		}	
	}
	
	function renderScript($field)
	{
		
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($this->field) || array_key_exists($field, $this->parent->hidden)) return "";	

		if (!$field) $field = $this->field;
		
		global $phonegap;
		
		ob_start();
		
		$redirect = $this->uploadRedirect ? "'{$this->uploadRedirect}'" : "false";
?>
	<script type="text/javascript" src="/components/phonegap/js/photo_uploader.js"></script>
	<script type="text/javascript">
	window.addEvent('load', function()
	{
		new PhotoUploader().setup('<?echo $this->parent->id?>_<?echo $field?>', '<?echo $this->parent->id?>_<?echo $field?>_thumbnail', '<?echo $this->parent->id?>_<?echo $field?>_status', '<?echo $this->gallery_id?>', <?echo $redirect?>);
	});
	</script>
<?
		$phonegap .= ob_get_contents();
		ob_end_clean();
		
	}
	
	function renderField($field = "")
	{
		global $user;
		
		if (!$field) $field = $this->field;
		
		$this->_startField($field);
		
		$id = $this->parent->data->get($field);
		
		if ($id) 
		{
			$img = "<img src='/action/image/thumbnail?image_id={$id}&size={$this->imageSize}' alt=''/>";
		}
?>
	<input type="hidden" value="<?echo $id?>" name="<?echo $field?>" id="<?echo $this->parent->id?>_<?echo $field?>"/>
	<div id="<?echo $this->parent->id?>_<?echo $field?>_thumbnail"><?echo $img?></div>
	<div id="<?echo $this->parent->id?>_<?echo $field?>_status"></div>
	<br/>
<?
		if (PhonegapManager::hasPhonegap())
		{
?>
	<a href="#" onclick="new PhotoUploader().capturePhoto(); return false" class="button"><i class='icon-camera'></i> <?echo $this->takePhotoLabel?></a>&nbsp;&nbsp;
	<a href="#" onclick="new PhotoUploader().choosePhoto(); return false" class="button"><i class='icon-picture'></i> <?echo $this->uploadPhotoLabel?></a>
<?
		}
		$this->_endField($field);
	}
	
	function renderReadOnly($field = "")
	{
		if (!$field) $field = $this->field;

		$id = $this->parent->data->get($field);
		$this->_startField($field);
		
		if ($id) 
		{
			echo "<img src='/action/image/thumbnail?image_id=".$this->parent->data->get($field)."&size=300' alt=''/>";
		}
		else
		{
			echo "No image uploaded";
		}
		
		$this->_endField($field);
	}

}

/**@}*/?>
