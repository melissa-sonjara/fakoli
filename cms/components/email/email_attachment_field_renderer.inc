<?php
/**\addtogroup email
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

Fakoli::using("attachment", "document");
Fakoli::usingFile("framework/field_renderers.inc");


/**
 * Used on email forms to attach one or more files to an 
 * email. Does not store the selected documents so no documents 
 * are retrieved. The object displayed in the AutoForm would be 
 * an email form that is not retrieved or stored.
 *
 * @author janice
 *
 */
class EmailAttachmentFieldRenderer extends FieldRenderer
{
	var $colspan = 2;
	var $field;
	var $label;
	var $cssClass = "attachment";
	var $deleteIcon = "/fakoli/images/delete.gif";
	
	function __construct(&$form, $field, $label)
	{
		$this->field = $field;
	
		parent::__construct($form);
		$this->label = ($label) ? $label : $form->prettifyFieldName($field);

		if($form->hasAdditional($field))
		{
			die("field $field already exists in form");
		}
		$form->add($this);
	
		if (isset($form->attachment_css_class)) $this->cssClass = $form->attachment_css_class;
		if (isset($form->attachment_delete_icon)) $this->deleteIcon = $form->attachment_delete_icon;
	}
	
	function renderField($field = "")
	{
		if (!$field) $field = $this->field;

		$this->_startField($field);

		echo "<input id='{$this->parent->id}_{$field}' type='hidden' name='{$field}' value='' />\n";
		echo "<ul id='{$this->parent->id}_{$field}_list' class='{$this->cssClass}' style='display: table-cell'>\n</ul>\n";
		echo "<a href='#' onclick='emailAttachmentMgr.showAddAttachmentDialog(); return false'>Add an Attachment</a>\n";

		$this->_endField($field);
	}

	function renderScript($field)
	{
		if (!$field) $field = $this->field;

		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
	
		global $dialogs;
	
		ob_start();
		?>
	<div class="dialog" id="attachmentDialog" style="width: 260px">
	 <div class="dialog_header" id="attachmentDialogHeader">
	  <div style="padding: 4px;">
	   <div style="float: right">&nbsp;<a id='closeAttachmentDialog'>Close &times;</a></div>
	   <span"style="font-weight: bold" id="attachmentDialogTitle">Add an Attachment</span>
	  </div>	
	 </div>
	 <div class="dialog_body">
	  <p id="attachmentDialogMessage"></p>
	  <form id="attachmentForm" method="POST" enctype="multipart/form-data" action="/action/attachment/upload" >
	   <br/>
	   <label>Attachment File</label><br/>
	   <input type="file" name="attachmentFile"/>
	   <br/>
	   <br/><input type="submit" class="button" name="submit" value="Upload Attachment" style="float: right; margin-bottom: 4px"/>
	  </form>
	 </div>
	</div>
	<?
		$dialogs .= ob_get_contents();
		ob_end_clean();

		echo "<script type='text/javascript'>\n";
		echo "var emailAttachmentMgr;\n";
		echo "window.addEvent('domready', function()\n";
		echo "{\n";
		echo "\temailAttachmentMgr = new EmailAttachmentManager('{$this->parent->id}_{$field}_list', '{$this->parent->id}_{$field}', '{$this->cssClass}', '{$this->deleteIcon}');\n";
		echo "});\n";
		echo "</script>\n";
	
	}
	
	/**
	 * Setting the attachments is done in preprocess b/c no object is
	 * saved for email and b/c a call to AutoForm's custom save hander
	 * (where email is sent) would be done before postProcess.
	 *
	 * @see FieldRenderer::preProcess()
	 */
	function preProcess($field = "")
	{
		if (!$field) $field = $this->field;
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
	
		if (isset($_POST[$field]) && $_POST[$field] != "")
		{
			$ids = explode(",", $_POST[$field]);
	
			foreach($ids as $id)
			{
				$attachment = new Attachment($id);
				$file = sanitizePath($this->formatFilePath($attachment));
				$attachments[$attachment->filename] = $file;
			}
			$this->parent->data->set($field, $attachments);
		}
	}


	/**
	 * Once we have sent the email, we can delete the attachment
	 * records.
	 *
	 * (non-PHPdoc)
	 * @see FieldRenderer::postProcess()
	 */
	function postProcess($field = "")
	{
		if (!$field) $field = $this->field;
	
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
		
		if (isset($_POST[$field]) && $_POST[$field] != "")
		{
			$ids = explode(",", $_POST[$field]);
			
			foreach($ids as $id)
			{
				$attachment = new Attachment($id);			
				$file = $this->formatFilePath($attachment);
				if (file_exists($file))
				{
					unlink($file);
				}
				$attachment->delete();	
			}
		}
	}	
	
	function formatFilePath($attachment)
	{
		global $config;
		
		$dir = $config["attachmentdir"];
		$base = $config['uploadbase'];
			
		$file = "$base/$dir{$attachment->local_filename}";
		
		return $file;	
	}
} // end EmailAttachmentFieldRenderer

/**
 * Used on email forms to attach documents from document
 * libraries to an email. Does not store the selected
 * documents so no documents are retrieved. The object
 * displayed in the AutoForm would be an email form that 
 * is not retrieved or stored.
 * 
 * @author janice
 *
 */
class EmailLibraryAttachmentFieldRenderer extends FieldRenderer
{	
	var $colspan = 2;
	var $field;
	var $label;
	var $cssClass = "attachment";
	var $deleteIcon = "/fakoli/images/delete.gif";
	
	function __construct(&$form, $field, $label)
	{
		$this->field = $field;
	
		parent::__construct($form);
		$this->label = ($label) ? $label : $form->prettifyFieldName($field);

		if($form->hasAdditional($field))
		{
			die("field $field already exists in form");
		}
		$form->add($this);
	
		if (isset($form->attachment_css_class)) $this->cssClass = $form->attachment_css_class;
		if (isset($form->attachment_delete_icon)) $this->deleteIcon = $form->attachment_delete_icon;
	} 
	

	function getAttachmentType()
	{
		return "document";
	}
	
	function renderField($field = "")
	{
		if (!$field) $field = $this->field;
	
		$this->_startField($field);
 
		echo "<input id='{$this->parent->id}_{$field}' type='hidden' name='{$field}' value='' />\n";
		echo "<ul id='{$this->parent->id}_{$field}_list' class='{$this->cssClass}' style='display: table-cell'>\n</ul>\n";
		echo "<a href='#' onclick='emailDocAttachmentMgr.showDocumentSelectDialog(); return false'>Add an Attachment</a>\n";
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field = "")
	{
		// nothing to do
	}
	
	/**
	 * Setting the attachments is done in preprocess b/c no object is
	 * saved for email and b/c a call to AutoForm's custom save hander
	 * (where email is sent) would be done before postProcess.
	 *
	 * @see FieldRenderer::preProcess()
	 */
	function preProcess($field = "")
	{
		if (!$field) $field = $this->field;
	
		trace("EmailAttachmentFieldRenderer preProcess field $field value ". $_POST[$field], 3);
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
	
		if (isset($_POST[$field]) && $_POST[$field] != "")
		{
			$ids = explode(",", $_POST[$field]);
	
			foreach($ids as $id)
			{
				$document = new Document($id);
				$library = $document->DocumentLibrary();
				$file = sanitizePath($library->getLibraryDirectory() . DIRECTORY_SEPARATOR . $document->file);
				$attachments[basename($document->file)] = $file;			
			}
			$this->parent->data->set($field, $attachments);
		}
	}
	

	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
	
		if (!$field) $field = $this->field;
	
		echo "<script type='text/javascript' src='/components/email/js/email_attachment_manager.js'></script>\n";
		echo "<script type='text/javascript'>\n";
		echo "var emailDocAttachmentMgr;\n";
		echo "window.addEvent('load', function()\n";
		echo "{\n";
		echo "\temailDocAttachmentMgr = new EmailDocumentAttachmentManager('{$this->parent->id}_{$field}_list', '{$this->parent->id}_{$field}', '{$this->cssClass}', '{$this->deleteIcon}');\n";
		echo "});\n";
		echo "</script>\n";
	}
		
	
	/**
	 * Called by document_select_dialog.inc
	 */
	static function getLibraryOptions()
	{
		$libraries = Query::create('DocumentLibrary', "ORDER BY name")->execute();
		$libraries = filterByRole($libraries, "allow_access");
	
		$options = array();
		if (count($libraries) > 0)
		{
			foreach($libraries as $library)
			{
				$options[$library->document_library_id] = $library->name;
			}
		}
		return $options;
	}	
	
	/**
	 * Called by document_select_dialog.inc
	 */
	static function getDocumentOptions()
	{
		$documents = Query::create('Document', "ORDER BY title")->execute();
		$documents = filterByRole($documents, "allow_access");
				
		if (count($documents) > 0)
		{
			$documents = regroupList($documents, "document_library_id");
		}
		return $documents;
	}
}

/**@}*/?>
