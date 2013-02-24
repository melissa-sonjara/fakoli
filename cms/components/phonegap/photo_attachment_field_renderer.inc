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

Fakoli::using("attachment");

class PhotoAttachmentFieldRenderer extends AttachmentFieldRenderer
{
	static function create($form, $field, $xrefClass)
	{
		if (PhonegapManager::hasPhonegap())
		{
			return new PhotoAttachmentFieldRenderer($form, $field, $xrefClass);
		}
		else
		{
			return new AttachmentFieldRenderer($form, $field, $xrefClass);
		}
	}
	
	function __construct($form, $field, $xrefClass)
	{
		parent::__construct($form, $field, $xrefClass);
	}
	
	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";		
?>
	<script type="text/javascript" src="/components/phonegap/js/photo_attachment_uploader.js"></script>
	<script type="text/javascript">
	window.addEvent('load', function()
	{
		new PhotoAttachmentUploader().setup('<?echo $this->parent->id?>_<?echo $field?>_list', '<?echo $this->parent->id?>_<?echo $field?>', '<?echo $this->cssClass?>', '<?echo $this->deleteIcon?>');
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
	<a href="#" onclick='new PhotoAttachmentUploader().deleteAttachment("<?echo $attachment->filename?>", <?echo $attachment->attachment_id?>); return false' title='Delete this Attachment'><img src='<?echo $this->deleteIcon?>' style='display:inline-block; vertical-align: middle' alt='Delete this Attachment'/></a>
	</span></li>
<?	
		}
	?>
	</ul>
	<br/>
	<a href="#" onclick="new PhotoAttachmentUploader().capturePhoto(); return false" class="button"> Take a Photo </a>&nbsp;&nbsp;
	<a href="#" onclick="new PhotoAttachmentUploader().choosePhoto(); return false" class="button">Upload a Photo</a>
<?
		$this->_endField($field);
	}
	
	// JDG 5/24/11 - add readonly handling
	function renderReadOnly($field = "")
	{
		if (!$field) $field = $this->field;
	
		$attachments = $this->getAttachments();
		if(count($attachments) < 2) $this->colspan = 1;
		
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">";		
		
		// Omit ul li list if only one
		if(count($attachments) == 1)
		{
			$this->drawOneAttachment($attachments[0]);
		}
		elseif(count($attachments) > 0)
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

		echo "</td></tr>\n";
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
	
	function postProcess($field = "")
	{
		trace("AttachmentFieldRenderer postProcess field $field value ". $_POST[$field], 3);
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
		
		if (!$field) $field = $this->field;
		
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

?>