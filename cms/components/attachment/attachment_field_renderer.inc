<?php
/**\addtogroup attachment
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

class AttachmentFieldRenderer extends FieldRenderer
{
	var $colspan = 2;
	var $xrefClass;
	var $field;
	var $label;
	var $cssClass = "attachment";
	var $deleteIcon = "/fakoli/images/delete.gif";
	var $useCamera = false;
	var $addLabel = "Add an Attachment";
	var $emptyMessage = "No files have been attached";
	
	function AttachmentFieldRenderer(&$form, $field, $xrefClass)
	{
		$this->xrefClass = $xrefClass;
		$this->field = $field;	
		
		$this->FieldRenderer($form);
		$this->label = $form->prettifyFieldName($field);
		$form->add($this);
		
		if (isset($form->attachment_css_class)) $this->cssClass = $form->attachment_css_class;
		if (isset($form->attachment_delete_icon)) $this->deleteIcon = $form->attachment_delete_icon;
	} 

	
	function getMatchingPK($xref, $obj)
	{
		$pks = $obj->getPrimaryKeyList();
		foreach($pks as $pk)
		{
			if ($xref->hasField($pk)) return $pk;
		}
		
		throw new DataItemException("No matching foreign key in xref table");
	}

	static function renderDialog($useCamera)
	{
		ob_start();
?>
<div class="dialog" id="attachmentDialog" style="width: 260px">
 <div class="dialog_header" id="attachmentDialogHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeAttachmentDialog'>Close &times;</a></div>
   <span style="font-weight: bold" id="attachmentDialogTitle">Add an Attachment</span>
  </div>	
 </div>
 <div class="dialog_body">
<?
		echo AttachmentFieldRenderer::renderDialogBody($useCamera);
?>
  </form>
 </div>
</div>
<?		
		$dialog = ob_get_contents();
		ob_end_clean();
		return $dialog;
	}
	
	static function renderDialogBody($useCamera)
	{
		ob_start();
?>
		<p id="attachmentDialogMessage"></p>
		<form id="attachmentForm" method="POST" enctype="multipart/form-data" action="/action/attachment/upload" >
		<br/>
		<label>Attachment File</label><br/>
		<input type="file" name="attachmentFile"<?if ($useCamera) { echo ' accept="image/*" capture="camera"'; }?>/>
   		<br/>
   		<br/><input type="submit" class="button" name="submit" value="Upload Attachment" style="float: right; margin-bottom: 4px"/>
<?			
		$body = ob_get_contents();
		ob_end_clean();
		return $body;
	}
	
	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
		
		global $dialogs;
		if (!$field) $field = $this->field;
				
		$dialogs .= AttachmentFieldRenderer::renderDialog($this->useCamera);
?>
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		new AttachmentUploader().setup('attachmentForm', '<?echo $this->parent->id?>_<?echo $field?>_list', '<?echo $this->parent->id?>_<?echo $field?>', '<?echo $this->cssClass?>', '<?echo $this->deleteIcon?>');
	});
	</script>
<?
	}
	
	function renderField($field = "")
	{
		if (!$field) $field = $this->field;
		
		$xref = new $this->xrefClass;
	
		$this->_startField($field);
		
		$attachments = $this->getAttachments();
					
		$ids = displayFieldAsList($attachments, "attachment_id");
?>
	<input type="hidden" value="<?echo $ids?>" name="<?echo $field?>" id="<?echo $this->parent->id?>_<?echo $field?>"/>
	<ul id="<?echo $this->parent->id?>_<?echo $field?>_list" class="<?echo $this->cssClass?>" style='display: table-cell'>
	<?

 		foreach($attachments as $attachment)
 		{
 ?>	<li id='attachment_<?echo $attachment->attachment_id?>' class='<?echo $this->cssClass?>'><span>
 <?php		
 			$this->drawOneAttachment($attachment);
 ?>
	&nbsp;
	<a href="#" onclick='new AttachmentUploader().deleteAttachment("<?echo $attachment->filename?>", <?echo $attachment->attachment_id?>); return false' title='Delete this Attachment'><img src='<?echo $this->deleteIcon?>' style='display:inline-block; vertical-align: middle' alt='Delete this Attachment'/></a>
	</span></li>
<?	
		}
	?>
	</ul>
	<a class='add_attachment_label' href="#" onclick="new AttachmentUploader().addAttachment(); return false"> <?echo $this->addLabel?> </a>
<?
		$this->_endField($field);
	}
	
	// JDG 5/24/11 - add readonly handling
	function renderReadOnly($field = "")
	{
		if (!$field) $field = $this->field;
	
		$attachments = $this->getAttachments();
		if(count($attachments) < 2) $this->colspan = 1;
		
		$this->_startField($field);
				
		if (count($attachments) == 0)
		{
			echo "<p>{$this->emptyMessage}</p>";
		}
		else if(count($attachments) == 1)
		{
			// Omit ul li list if only one
					
			$this->drawOneAttachment($attachments[0]);
		}
		else if(count($attachments) > 0)
		{
?>	
	<ul id="<?echo $this->parent->id?>_<?echo $field?>_list" class="<?echo $this->cssClass?>">
<?	
		foreach($attachments as $attachment)
		{
 ?>	<li id='attachment_<?echo $attachment->attachment_id?>' class='<?echo $this->cssClass?>'><span>
 <?php
 			$this->drawOneAttachment($attachment);
 ?>
	 </span></li>
<?
		}
?></ul><?php			
		}

		$this->_endField($field);
	}
	
	function getAttachments()
	{
		$xref = new $this->xrefClass;
		
		$pk = $this->getMatchingPK($xref, $this->parent->data);
		$pkv = $this->parent->data->get($pk);
		
		if ($pkv)
		{
			$attachments = query(Attachment, "WHERE attachment_id in (select attachment_id from {$xref->table} where $pk=$pkv) ORDER BY attachment_id");
		}
		else
		{
			$attachments = array();
		}
				
		return $attachments;
	}	
	
	function drawOneAttachment($attachment)
	{
 			$icon = getDocIcon($attachment->filename);
?>
	<img src="<?echo $icon?>" alt="Icon" style="display:inline-block;vertical-align:middle"/>&nbsp;
	<a href='/action/attachment/download?attachment_id=<?echo $attachment->attachment_id?>'><?echo $attachment->filename?></a>&nbsp;
	(<?echo $attachment->file_size?>)
<?			
	}
	
	static function formatAttachment($attachment)
	{
		$icon = getDocIcon($attachment->filename);
	
		return $attachment->format("<img src='{$icon}' alt='Icon' style='display:inline-block;vertical-align:middle'/>&nbsp;<a href='/action/attachment/download?attachment_id={attachment_id}'>{filename}</a>&nbsp;({file_size})");
	}
	
	function postProcess($field = "")
	{
		if (!$field) $field = $this->field;
		
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
		trace("AttachmentFieldRenderer postProcess field $field value ". $_POST[$field], 3);
					
		$xref = new $this->xrefClass;
		
		$obj = $this->parent->data;
		$pk =  $this->getMatchingPK($xref, $obj);
		
		// Clear out current relations
	
		$xref = new $this->xrefClass;
		$xref->delete("WHERE {$pk}=".$obj->get($pk));
		
		if (isset($_POST[$field]))
		{
			trace("AttachmentFieldRenderer postProcess _POST attachment field is ". $_POST[$field], 3);
			$ids = explode(",", $_POST[$field]);
			
			foreach($ids as $id)
			{
				$xref = new $this->xrefClass;
				$xref->set($pk, $obj->get($pk));
				$xref->set("attachment_id", $id);
				$xref->save();
			}
		}
		else
		{
			trace("AttachmentFieldRenderer postProcess _POST attachment field is empty", 3);			
		}	
	}
}

/**@}*/?>