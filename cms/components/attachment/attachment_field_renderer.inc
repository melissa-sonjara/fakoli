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

Fakoli::usingFile("framework/field_renderers.inc");

class AttachmentFieldRenderer extends FieldRenderer
{
	var $colspan = 2;
	var $xrefClass;
	var $field;
	var $label;
	
	function AttachmentFieldRenderer(&$form, $field, $xrefClass)
	{
		$this->xrefClass = $xrefClass;
		$this->field = $field;	
		
		$this->FieldRenderer($form);
		$this->label = $form->prettifyFieldName($field);
		$form->add($this);
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

	function renderScript($field)
	{
		global $dialogs;
		if (!$field) $field = $this->field;
		
		ob_start();
?>
<div class="dialog" id="attachmentDialog" style="width: 400px">
 <div class="dialog_header" id="attachmentDialogHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeAttachmentDialog'">Close &times;</a></div>
   <span"style="font-weight: bold">Add an Attachment</span>
  </div>	
 </div>
 <div class="dialog_body">
  <p id="attachmentDialogMessage"></p>
  <form id="attachmentForm" method="POST" enctype="multipart/form-data" action="/cms/components/attachment/attachment_upload.php" >
   <br/>
   <label>Attachment File</label>
   <input type="file" name="attachmentFile"/>
   <br/>
   <br/><input type="submit" name="submit" value="Upload Attachment" style="float: right; margin-bottom: 4px"/>
  </form>
 </div>
</div>
<?
		$dialogs .= ob_get_contents();
		ob_end_clean();
?>
	<script type="text/javascript" src="/cms/js/iFrameFormRequest.js"></script>
	<script type="text/javascript" src="/cms/components/attachment/attachment_uploader.js"></script>
	<script type="text/javascript">
	window.addEvent('load', function()
	{
		new AttachmentUploader().setup('attachmentForm', '<?echo $this->parent->id?>_<?echo $field?>_list', '<?echo $this->parent->id?>_<?echo $field?>');
	});
	</script>
<?
	}
	
	function renderField($field = "")
	{
		if (!$field) $field = $this->field;
		
		$xref = new $this->xrefClass;
		
		$this->_startField($field);
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
				
		$ids = displayFieldAsList($attachments, "attachment_id");
?>
	<input type="hidden" value="<?echo $ids?>" name="<?echo $field?>" id="<?echo $this->parent->id?>_<?echo $field?>"/>
	<div id="<?echo $this->parent->id?>_<?echo $field?>_list">
	<?

 		foreach($attachments as $attachment)
 		{
 			$icon = getDocIcon($attachment->filename);
?>
	<img src="<?echo $icon?>" alt="Icon" style="display:inline-block;vertical-align:middle"/>&nbsp;<?echo $attachment->filename?>&nbsp;(<?echo $attachment->file_size?>)
	<br/>	
<?	
	}
	?>
	</div>
	<a href="#" onclick="new AttachmentUploader().addAttachment(); return false"> Add an Attachment </a>
<?
		$this->_endField($field);
	}
	
	function renderReadOnly($field = "")
	{

	}
	
	function postProcess($field = "")
	{
		if (!$field) $field = $this->field;
		
		$xref = new $this->xrefClass;
		
		$obj = $this->parent->data;
		$pk =  $this->getMatchingPK($xref, $obj);
		
		// Clear out current relations
		
		$xref = new $this->xrefClass;
		$xref->delete("WHERE {$pk}=".$obj->get($pk));
		
		if (isset($_POST[$field]))
		{
			$ids = explode(",", $_POST[$field]);
			
			foreach($ids as $id)
			{
				$xref = new $this->xrefClass;
				$xref->set($pk, $obj->get($pk));
				$xref->set("attachment_id", $id);
				$xref->save();
			}
		}
	}
}

?>